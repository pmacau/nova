#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/trigonometric.hpp>
#include <iostream>
// internal
#include "render_system.hpp"
#include "tinyECS/components.hpp"
#include <glm/gtc/type_ptr.hpp>
// for the text rendering
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>
#include <filesystem>
#include <sstream>

RenderSystem::RenderSystem(entt::registry& reg) :
	registry(reg)
{
	screen_state_entity = registry.create();
	screen_entity = registry.view<ScreenState>().front();
}

bool RenderSystem::initFreetype() {
    // Initialize FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }

    // Load font
    FT_Face face;
    // Modify this path to point to your font file
	std::filesystem::path basePath = std::filesystem::path(__FILE__).parent_path().parent_path();
	std::filesystem::path fullPath = basePath / "fonts" / "Oxanium.ttf";

    if (FT_New_Face(ft, fullPath.string().c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return false;
    }

    // Set size to load glyphs
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free FreeType resources
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VAO/VBO for text quads
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

	// Check if VAO/VBO are created successfully
	if (gl_has_errors()) {
		std::cerr << "ERROR: OpenGL encountered an issue creating text VAO/VBO" << std::endl;
		return false;
	}


	if (textVAO == 0) {
        std::cerr << "ERROR: Failed to generate text VAO" << std::endl;
        return false;
    }
    if (textVBO == 0) {
        std::cerr << "ERROR: Failed to generate text VBO" << std::endl;
        return false;
    }

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void RenderSystem::renderText(const std::string& text, float x, float y, float scale, glm::vec3 color, const mat3& projection) {
    // Activate corresponding render state	
    glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::TEXT]);
    gl_has_errors();

    // Validate VAO and VBO
    if (textVAO == 0 || textVBO == 0) {
        std::cerr << "ERROR: Text VAO or VBO not initialized" << std::endl;
        return;
    }

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set projection
    GLuint projLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::TEXT], "projection");
    glUniformMatrix3fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Set text color
    GLuint colorLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::TEXT], "textColor");
    glUniform3f(colorLoc, color.x, color.y, color.z);
    gl_has_errors();

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // Iterate through all characters
    for (char c : text) {
        if (Characters.find(c) == Characters.end()) {
            std::cerr << "Character not found in font map: " << c << std::endl;
            continue;
        }

        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        
        // Update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        if (gl_has_errors()) {
            std::cerr << "Error binding texture for character" << std::endl;
            continue;
        }
        
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (gl_has_errors()) {
            std::cerr << "Error updating VBO data" << std::endl;
            continue;
        }

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        if (gl_has_errors()) {
            std::cerr << "Error drawing text quad" << std::endl;
        }

        // Advance cursor for next glyph
        x += (ch.Advance >> 6) * scale;
    }
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    gl_has_errors();
}

static std::vector<glm::vec2> generateCircleVertices(float centerX, float centerY, float radius, int segments = 32) {
	std::vector<glm::vec2> vertices;
	for (int i = 0; i < segments; i++) {
		float theta = 2.0f * 3.14159f * float(i) / float(segments);
		float x = centerX + radius * cos(theta);
		float y = centerY + radius * sin(theta);
		vertices.emplace_back(x, y);
	}
	return vertices;
}

std::vector<glm::vec2> generateRectVertices(float centerX, float centerY, float width, float height) {
	float halfW = width / 2.0f;
	float halfH = height / 2.0f;
	return {
		{centerX - halfW, centerY - halfH},
		{centerX + halfW, centerY - halfH},
		{centerX + halfW, centerY + halfH},
		{centerX - halfW, centerY + halfH}
	};
}

void RenderSystem::drawDebugHitBoxes(const glm::mat3& projection, const glm::mat3& transform) {
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::DEBUG]);
	gl_has_errors();
	GLint projLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::DEBUG], "projection");
	GLint transLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::DEBUG], "transform");
	GLint colorLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::DEBUG], "debugColor");
	glUniformMatrix3fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix3fv(transLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // Draw in red
	gl_has_errors();
	auto view = registry.view<HitBox, Motion>(); //goes through every moving thing with a hitbox
	for (auto entity : view) {
		std::vector<glm::vec2> vertices;
		float posX = registry.get<Motion>(entity).position.x;
		float posY = registry.get<Motion>(entity).position.y;
		if (registry.get<HitBox>(entity).type == HITBOX_CIRCLE) { //generates corresponding hitbox
			vertices = generateCircleVertices(posX, posY, registry.get<HitBox>(entity).shape.circle.radius); 
		}
		else if (registry.get<HitBox>(entity).type == HITBOX_RECT) {
			vertices = generateRectVertices(posX, posY,
				registry.get<HitBox>(entity).shape.rect.width,
				registry.get<HitBox>(entity).shape.rect.height);
		}
		GLuint debugVAO, debugVBO;
		glGenVertexArrays(1, &debugVAO);
		glGenBuffers(1, &debugVBO);

		glBindVertexArray(debugVAO);
		glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

		glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glDeleteBuffers(1, &debugVBO);
		glDeleteVertexArrays(1, &debugVAO);
	}

	glBindVertexArray(defaultVAO);
}

void RenderSystem::drawTexturedMesh(entt::entity entity,
									const mat3 &projection)
{
	glBindVertexArray(defaultVAO);
	auto& motion = registry.get<Motion>(entity);

	auto camera_entity = registry.view<Camera>().front(); // TODO: make this more robust
	auto& camera = registry.get<Camera>(camera_entity);


	vec2 centre_position = motion.position;
	Transform model_transform;
	model_transform.translate(centre_position);
	model_transform.scale(motion.scale);
	model_transform.rotate(radians(motion.angle));

	Transform camera_transform;
	// camera_transform.translate(vec2(WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX / 2) - camera.offset);
	// Only apply camera transform for non-UI elements
	if (!registry.any_of<FixedUI>(entity)) {
		camera_transform.translate(-camera.offset);
	}

	assert(registry.any_of<RenderRequest>(entity));
	const auto& render_request = registry.get<RenderRequest>(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];
	// Setting shaders
	glUseProgram(program);
	gl_has_errors();
	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];
	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// texture-mapped entities - use data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);
		glEnableVertexAttribArray(in_position_loc);
		gl_has_errors();
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();
		assert(registry.any_of<RenderRequest>(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.get<RenderRequest>(entity).used_texture];
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.any_of<vec3>(entity) ? registry.get<vec3>(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	// Setting uniform values to the currently bound program

	// model transform
	GLuint model_transform_loc = glGetUniformLocation(currProgram, "model_transform");
	glUniformMatrix3fv(model_transform_loc, 1, GL_FALSE, (float *)&model_transform.mat);
	gl_has_errors();

	// camera transform
	GLuint camera_transform_loc = glGetUniformLocation(currProgram, "camera_transform");
	glUniformMatrix3fv(camera_transform_loc, 1, GL_FALSE, (float *)&camera_transform.mat);
	gl_has_errors();

	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();

	// TODO: update this with registry
	assert(registry.any_of<Sprite>(entity) && "Textured entities must have a sprite component");

	GLuint spriteData_loc = glGetUniformLocation(currProgram, "spriteData");
	GLuint sheetDims_loc = glGetUniformLocation(currProgram, "sheetDims");
	const auto& s = registry.get<Sprite>(entity);

	glUniform4f(spriteData_loc, s.coord.row, s.coord.col, s.dims.x, s.dims.y);
	glUniform2f(sheetDims_loc, s.sheet_dims.x, s.sheet_dims.y);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();

}

// first draw to an intermediate texture,
// apply the "vignette" texture, when requested
// then draw the intermediate texture
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the vignette texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::VIGNETTE]);
	gl_has_errors();

	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	// glDepthRange(0, 10);
	glDepthRange(0.0, 1.0);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // Debug: disable depth test

	// glEnable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	// add the "vignette" effect
	const GLuint vignette_program = effects[(GLuint)EFFECT_ASSET_ID::VIGNETTE];

	// set clock
	GLuint time_uloc       = glGetUniformLocation(vignette_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(vignette_program, "darken_screen_factor");

	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	
	auto& screen = registry.get<ScreenState>(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();

	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(vignette_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();

	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

void RenderSystem::renderGamePlay()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// clear backbuffer
	glViewport(0, 0, w, h);
	// glDepthRange(0.00001, 10);
	glDepthRange(0.0, 1.0);
	// water-colored background
	glClearColor(0.0f, 0.58431373f, 0.91372549f, 1.0f);

	// Debug: claer depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Debug
	// glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();

	mat3 projection_2D = createProjectionMatrix();
	mat3 ui_projection_2D = createUIProjectionMatrix();

	// Render huge background texture
	auto background = registry.view<Background>().front();
	drawTexturedMesh(background, projection_2D);

	// Render main entities
	registry.sort<Motion>([](const Motion& lhs, const Motion& rhs) {
        return (lhs.position.y + lhs.offset_to_ground.y) < (rhs.position.y + rhs.offset_to_ground.y);
    });
    auto spriteRenders = registry.view<Motion, RenderRequest>(entt::exclude<UI, Background>);
    spriteRenders.use<Motion>();
    for (auto entity : spriteRenders) {
        drawTexturedMesh(entity, projection_2D);
    }

	// Render dynamic UI
	for (auto entity : registry.view<UI, Motion, RenderRequest>(entt::exclude<UIShip, FixedUI>)) {
		drawTexturedMesh(entity, projection_2D);
	}

	// Render static UI
	for (auto entity: registry.view<FixedUI, Motion, RenderRequest>(entt::exclude<UIShip, Item>)) {
		drawTexturedMesh(entity, ui_projection_2D);
	}
	
	// Render items on static UI
	for (auto entity: registry.view<FixedUI, Motion, Item, RenderRequest>(entt::exclude<UIShip>)) {
		drawTexturedMesh(entity, ui_projection_2D);
	}

	// draw framebuffer to screen
	// adding "vignette" effect when applied
	drawToScreen();
	// DEBUG
	auto debugView = registry.view<Debug>();
	if (!debugView.empty()) {
		glm::mat3 projection = createProjectionMatrix();
		glm::mat3 transform = glm::mat3(1.0f);
		drawDebugHitBoxes(projection, transform);
	}
	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();

}

void RenderSystem::renderShipUI() 
{
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Framebuffer is not complete! Status: " << status << std::endl;
        return;
    }

	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	// clear backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.0, 10);

	// black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat3 projection_2D = createProjectionMatrix();
	mat3 ui_projection_2D = createUIProjectionMatrix();

	// render ship
	std::vector<entt::entity> PlayerMobsRenderEntities;
	auto UIShips = registry.view<UIShip, Motion, RenderRequest>();

	for (auto entity : UIShips) {
		PlayerMobsRenderEntities.push_back(entity);
	}

	for (auto entity : PlayerMobsRenderEntities) {
		drawTexturedMesh(entity, ui_projection_2D);
	}

	drawToScreen();

	mat3 flippedProjection = projection_2D;
	flippedProjection[1][1] *= -1.0f; 
	renderText("SHIP UPGRADES", -125.0f, 225.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);
	renderText("Current ship", -348.0f, -35.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);

	// current ship
	// renderText("Damage: 10", -220.0f, 40.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);
	// renderText("Atk Spd: 10", -220.0f, 28.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);
	// renderText("HP: 500", -220.0f, 18.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);
	// renderText("Range: 250", -220.0f, 8.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);

	// ship 1
	renderText("Ship 1", -40.0f, 100.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);
	// renderText("Damage: 40", 50.0f, 160.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);
	// renderText("Atk Spd: 30", 50.0f, 148.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);
	// renderText("HP: 300", 50.0f, 138.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);
	// renderText("Range: 400", 50.0f, 128.0f, 0.2f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);

	// ship 2
	renderText("Ship 2", -35.0f, -151.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);

	// ship 3
	renderText("Ship 3", 220.0f, 90.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);

	// ship 4
	renderText("Ship 4", 225.0f, -70.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);

	// ship 5
	renderText("Ship 5", 225.0f, -245.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), flippedProjection);

	glfwSwapBuffers(window);
    gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	auto& screen_state = registry.get<ScreenState>(screen_entity);
	// auto& screen_state = registry.get<ScreenState>(screens.front());

    switch (screen_state.current_screen) {
		case ScreenState::ScreenType::SHIP_UPGRADE_UI:
            renderShipUI();
            break;
        case ScreenState::ScreenType::GAMEPLAY:
            renderGamePlay();
            break;
    }
}

mat3 RenderSystem::createUIProjectionMatrix() {
	/*float left = 0.f;
	float top = 0.f;
	float right = (float)WINDOW_WIDTH_PX;
	float bottom = (float)WINDOW_HEIGHT_PX;

	float near_plane = -1.0f;
	float far_plane = 1.0f;

	return glm::ortho(left, right, bottom, top, near_plane, far_plane);*/
	float left = 0.f;
	float top = 0.f;
	float right = (float)WINDOW_WIDTH_PX;
	float bottom = (float)WINDOW_HEIGHT_PX;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);

	return {
		{ sx, 0.f, 0.f},
		{0.f,  sy, 0.f},
		{ tx,  ty, 1.f}
	};
}

mat3 RenderSystem::createProjectionMatrix()
{
	// fake projection matrix, scaled to window coordinates
	float left   = 0.f;
	float top    = 0.f;
	float right  = (float) WINDOW_WIDTH_PX;
	float bottom = (float) WINDOW_HEIGHT_PX;

	// float sx = 2.f / (right - left);
	// float sy = 2.f / (top - bottom);
	// float tx = -(right + left) / (right - left);
	// float ty = -(top + bottom) / (top - bottom);

	// return {
	// 	{ sx, 0.f, 0.f},
	// 	{0.f,  sy, 0.f},
	// 	{ tx,  ty, 1.f}
	// };

	float near_plane = -1.0f; // Near clipping plane
    float far_plane = 1.0f;   // Far clipping plane

	return glm::ortho(left, right, bottom, top, near_plane, far_plane);
}

void RenderSystem::drawDebugPoint(mat3 projection, mat3 transform, vec3 color)
{
	const GLuint program = (GLuint)effects[(GLuint)EFFECT_ASSET_ID::COLOURED];
	glUseProgram(program);
	gl_has_errors();

    const GLuint vbo = vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::DEBUG_POINT];
	const GLuint ibo = index_buffers[(GLuint)GEOMETRY_BUFFER_ID::DEBUG_POINT];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// position
	GLint in_position_loc = glGetAttribLocation(program, "in_position");
	gl_has_errors();
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
		sizeof(ColoredVertex), (void*)0);
	gl_has_errors();


	// set uniform: color, transform, projection
	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	GLint color_uloc = glGetUniformLocation(currProgram, "in_color");
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	GLint transform_uloc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	gl_has_errors();

	GLint projection_uloc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	// Temporarily disable depth test to check if it's blocking rendering
    glDisable(GL_DEPTH_TEST);

	// Check if OpenGL has errors before drawing
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error BEFORE drawing debug point: " << error << std::endl;
    }

	// draw
	glPointSize(10.0f);
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();

	error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error AFTER drawing debug point: " << error << std::endl;
    }


	// Re-enable depth test
    glEnable(GL_DEPTH_TEST);
} 
