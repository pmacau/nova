#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/trigonometric.hpp>
#include <iostream>
// internal
#include "render_system.hpp"
#include "tinyECS/components.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "collision/hitbox.hpp"
#include "quadtree/quadtree.hpp"
// for the text rendering
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>
#include <filesystem>
#include <sstream>
#include <cctype>



RenderSystem::RenderSystem(entt::registry& reg, QuadTree& quadTree):
	registry(reg), 
	quadTree(quadTree)

{
	// screen_state_entity = registry.create();
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

ivec2 get_char_coords(const unsigned char& glyph) {
	unsigned char upper = std::toupper(glyph);
	int ascii_value = static_cast<int>(upper);

	if (std::isalpha(glyph)) {
		ascii_value -= 65; // ASCII for 'A'
		return ivec2(0, ascii_value);
	}
	else if (std::isdigit(glyph)) {
		ascii_value -= 48; // ASCII for '0'
		return ivec2(1, ascii_value);
	}
	else {
		int val = 10;
		switch (upper) {
			case '!':  {val += 0; break;}
			case '?':  {val += 1; break;}
			case '.':  {val += 2; break;}
			case ',':  {val += 3; break;}
			case ';':  {val += 4; break;}
			case '\'': {val += 5; break;}
			case '#':  {val += 6; break;}
			case '$':  {val += 7; break;}
			case '%':  {val += 8; break;}
			case '&':  {val += 9; break;}
			case '*':  {val += 10; break;}
			case ' ':  {val += 11; break;}
			default:   {val += 0; break;}
		}
		return ivec2(1, val);
	}
}

entt::entity createGlyph(
	entt::registry& reg, const unsigned char& c,
	float x, float y, int scale, vec3 color
) {
	ivec2 coord = get_char_coords(c);
	auto e = reg.create();

	reg.emplace<Glyph>(e);
	reg.emplace<FixedUI>(e);

	auto& motion = reg.emplace<Motion>(e);
	motion.scale = float(scale) * vec2(5, 7);
	motion.position = {x, y};

	auto& sprite = reg.emplace<Sprite>(e);
	sprite.coord.row = coord.x;
	sprite.coord.col = coord.y;
	
	sprite.dims = {5, 7};
	sprite.sheet_dims = {130, 14};

	auto& render = reg.emplace<RenderRequest>(e);
	render.used_effect = EFFECT_ASSET_ID::TEXTURED;
	render.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
	render.used_texture = TEXTURE_ASSET_ID::TEXT;

	auto& tColor = reg.emplace<vec3>(e);
	tColor = color;

	return e;
}

const std::unordered_map<char, vec3> color_codes = {
	// Biome codes
	{'I', {155.f/255.f, 226.f/255.f, 255.f/255.f}},
	{'S', {120.f/255.f, 138.f/255.f, 50.f/255.f}},
	{'J', {17.f/255.f,  91.f/255.f,  40.f/255.f}},
	{'B', {255.f/255.f, 220.f/255.f, 85.f/255.f}},
	{'F', {62.f/255.f,  137.f/255.f, 72.f/255.f}},
	{'W', {0.f/255.f,   149.f/255.f, 233.f/255.f}},

	{'0', {124.f/255.f, 109.f/255.f, 162.f/255.f}},
	{'1', {0, 0, 1}},
	{'2', {1, 0, 0}},
};

const int x_char_space = (5 + 1);

void RenderSystem::renderText(
	const std::string& text,
	float x, float y, int scale,
	glm::vec3 color, const mat3& projection,
	bool wrap
) {
    std::istringstream iss(text);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }

	int x_offset = 0, y_offset = 0;

	vec3 tColor = color;
	bool changeColor = false;
	
	// TODO: make this a parameter
	int max_width = WINDOW_WIDTH_PX / 3 - (WINDOW_WIDTH_PX / 15);

	for (const auto& word: words) {
		if (wrap &&
			(x + x_offset + getTextWidth(word, scale) > max_width)
		) {
			y_offset += scale * (7 + 3);
			x_offset = 0;
		}
		for (const char& c: word) {
			if (c == '{') {changeColor = true; continue;}
			if (c == '}') {tColor = color;     continue;}
			if (changeColor) {
				auto it = color_codes.find(c);
				if (it != color_codes.end()) {
					tColor = it->second;
				} else {
					tColor = color;
				}
				changeColor = false;
				continue;
			}
	
			auto entity = createGlyph(registry, c, x + x_offset, y + y_offset, scale, tColor);
			drawTexturedMesh(entity, projection);
			x_offset += scale * x_char_space;
		}
		// Add space for a ' ' character
		x_offset += scale * x_char_space;
	}
}

int RenderSystem::getTextWidth(const std::string& text, int scale) {
	return (text.length() * scale * x_char_space);
}

void RenderSystem::drawLine(vec2 start, vec2 end, vec3 color, float thickness, const mat3& projection) {
    // Use the existing shader program or create a new one for simple colored lines
    GLuint program = effects[5];
	if (program == 0) {
		std::cerr << "Error: Shader program at index 5 (line) is invalid!" << std::endl;
		return;
	}
	glUseProgram(program);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error after glUseProgram: " << error << std::endl;
	}
    
    // Create vertices for the line (two triangles forming a rectangle along the line)
    vec2 direction = normalize(end - start);
    vec2 perpendicular = vec2(-direction.y, direction.x) * (thickness / 2.0f);
    
    std::vector<vec2> vertices = {
        start + perpendicular, start - perpendicular,
        end + perpendicular, end - perpendicular
    };
    
    std::vector<uint16_t> indices = {0, 1, 2, 1, 2, 3};
    // Create and bind VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Create and bind VBO for vertices
    GLuint VBO;
    glGenBuffers(1, &VBO);
	if (VBO == 0) {
		std::cerr << "Error generating VBO!" << std::endl;
		return;
	}
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), vertices.data(), GL_STATIC_DRAW);
    
    // Create and bind EBO for indices
    GLuint EBO;
    glGenBuffers(1, &EBO);
	if (EBO == 0) {
		std::cerr << "Error generating EBO!" << std::endl;
		return;
	}
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
    
    // Set vertex attributes
    GLint posAttrib = glGetAttribLocation(program, "position");
	if (posAttrib == -1) {
		std::cerr << "Error: 'position' attribute not found in shader program!" << std::endl;
		return;
	}
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
    
    // Set color uniform
    GLint colorUniform = glGetUniformLocation(program, "color");
	if (colorUniform == -1) {
		std::cerr << "Error: 'color' uniform not found in shader program!" << std::endl;
		return;
	}
    glUniform3fv(colorUniform, 1, &color[0]);
    
    // Set projection matrix
    GLint projectionUniform = glGetUniformLocation(program, "projection");
	if (projectionUniform == -1) {
		std::cerr << "Error: 'projection' uniform not found in shader program!" << std::endl;
		return;
	}
    glUniformMatrix3fv(projectionUniform, 1, GL_FALSE, (float*)&projection);
    
    // Draw the line
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

	// Clean up
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    
    gl_has_errors();
}

// TODO: fix debug rendering later
/*
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
*/

void RenderSystem::drawDebugHitBoxes(const glm::mat3& projection) {
	// Skip if debug mode is not enabled
	//if (!debugModeEnabled) return;

	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::DEBUG]);
	gl_has_errors();

	// Set shader uniforms
	GLint projLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::DEBUG], "projection");
	glUniformMatrix3fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	gl_has_errors();

	// Get camera entity for offset
	auto camera_entity = registry.view<Camera>().front();
	auto& camera = registry.get<Camera>(camera_entity);

	// Process all entities with HitBox and Motion components
	auto view = registry.view<Hitbox, Motion>();
	for (auto entity : view) {
		auto& hitbox = registry.get<Hitbox>(entity);
		auto& motion = registry.get<Motion>(entity);

		// Skip drawing if entity is too far from camera view
		vec2 entityPos = motion.position - camera.offset;
		if (entityPos.x < -200 || entityPos.x > WINDOW_WIDTH_PX + 200 ||
			entityPos.y < -200 || entityPos.y > WINDOW_HEIGHT_PX + 200) {
			continue;
		}

		// Create transform matrix for this entity
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, glm::vec3(motion.position, 0.0f));
		transform = glm::rotate(transform, glm::radians(motion.angle), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(motion.scale, 1.0f));

		// Set color based on entity type
		vec3 color = { 1.0f, 0.0f, 0.0f }; // Default red
		if (registry.any_of<Player>(entity)) {
			color = { 0.0f, 1.0f, 0.0f }; // Green for player
		}
		else if (registry.any_of<Mob>(entity)) {
			color = { 1.0f, 0.5f, 0.0f }; // Orange for mobs
		}
		else if (registry.any_of<Projectile>(entity)) {
			color = { 0.0f, 0.0f, 1.0f }; // Blue for projectiles
		}

		GLint colorLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::DEBUG], "debugColor");
		glUniform3fv(colorLoc, 1, glm::value_ptr(color));
		gl_has_errors();

		// Create and bind VAO/VBO for this hitbox
		GLuint debugVAO, debugVBO;
		glGenVertexArrays(1, &debugVAO);
		glGenBuffers(1, &debugVBO);

		glBindVertexArray(debugVAO);
		glBindBuffer(GL_ARRAY_BUFFER, debugVBO);

		// Convert hitbox points to vertices
		std::vector<float> vertices;
		for (const auto& pt : hitbox.pts) {
			vertices.push_back(pt.x);
			vertices.push_back(pt.y);
		}

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		// Draw the hitbox as a line loop
		glDrawArrays(GL_LINE_LOOP, 0, hitbox.pts.size());

		// Clean up
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDeleteBuffers(1, &debugVBO);
		glDeleteVertexArrays(1, &debugVAO);
	}

	// Restore default VAO
	glBindVertexArray(defaultVAO);
	gl_has_errors();
}


void RenderSystem::drawTexturedMesh(entt::entity entity,
									const mat3 &projection)
{
	glBindVertexArray(defaultVAO);
	auto& motion = registry.get<Motion>(entity);

	auto camera_entity = registry.view<Camera>().front(); // TODO: make this more robust
	auto& camera = registry.get<Camera>(camera_entity);


	vec2 centre_position = motion.position;
	if (registry.any_of<Slash>(entity)) {
		auto player = registry.view<Player>().front(); 
		centre_position = registry.get<Motion>(player).position; 
	}
	Transform model_transform;
	model_transform.translate(centre_position);
	model_transform.rotate(radians(motion.angle));
	model_transform.scale(motion.scale);

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
void RenderSystem::drawToScreen(bool vignette)
{
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
	glDisable(GL_DEPTH_TEST); // Debug: disable depth test

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	auto& screen = registry.get<ScreenState>(screen_entity);

	Shader v = shaders.at("vignette");

    if (vignette) {
		if      (screen.curr_effect == EFFECT_ASSET_ID::E_FOG)  v = shaders.at("fog");
		else if (screen.curr_effect == EFFECT_ASSET_ID::E_SNOW) v = shaders.at("snow");
		else if (screen.curr_effect == EFFECT_ASSET_ID::E_HEAT) v = shaders.at("heat");
		else if (screen.curr_effect == EFFECT_ASSET_ID::E_RAIN) v = shaders.at("rain");
	}
	
	v.use();
	v.setFloat("time", vignette ? screen.time : (M_PI / 2 * 60.0));
	v.setVec2("resolution", vec2(w, h));
	v.setFloat("darken_screen_factor", vignette ? screen.darken_screen_factor : 0.f);
		
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(v.ID, "in_position");
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	mat3 flippedProjection = projection_2D;
	flippedProjection[1][1] *= -1.0f;
	mat3 flippedUIProjection = ui_projection_2D;
	flippedUIProjection[1][1] *= -1.0f;

	// Render huge background texture 
	auto background = registry.view<Background>().front();
	drawTexturedMesh(background, projection_2D); 

	auto playerView = registry.view<Player, Motion>();
	if (playerView.begin() == playerView.end()) {
		return; 
	}
	auto playerEntity = playerView.front();
	const auto& playerMotion = registry.get<Motion>(playerEntity);
	const float queryRange = WINDOW_WIDTH_PX * 1.25f; // Adjust based on your game's scale
	Quad rangeQuad(
		playerMotion.position.x,
		playerMotion.position.y,
		queryRange,
		queryRange
	);

	std::vector<entt::entity> nearbyEntities = quadTree.quadTree->queryRange(rangeQuad, registry);
	nearbyEntities.push_back(playerEntity); // never have to update player since all queries will be based off player anyways
	auto mobs = registry.view<Mob>();
	for (auto mob : mobs) {
		nearbyEntities.push_back(mob);
	}
	auto projectiles = registry.view<Projectile>();
	for (auto projectile : projectiles) {
		nearbyEntities.push_back(projectile);
	}
	auto slashes = registry.view<Slash>(); 
	for (auto slash : slashes) {
		nearbyEntities.push_back(slash); 
	}


	// render all the ship weapons/engine
	auto shipEngineRenders = registry.view<ShipEngine, Motion, RenderRequest>(entt::exclude<UI, Background, TextData, DeathItems, Button, UIIcon, UIShipWeapon, UIShipEngine, UpgradeButton>);
    shipEngineRenders.use<Motion>();
    for (auto entity : shipEngineRenders) {
        drawTexturedMesh(entity, projection_2D);
    }
	auto shipWeaponRenders = registry.view<ShipWeapon, Motion, RenderRequest>(entt::exclude<UI, Background, TextData, DeathItems, Button, UIIcon, UIShipWeapon, UIShipEngine, UpgradeButton>);
    shipWeaponRenders.use<Motion>();
    for (auto entity : shipWeaponRenders) {
        drawTexturedMesh(entity, projection_2D);
    }

	for (auto item : registry.view<Item>(entt::exclude<UI, DeathItems>)) {
		nearbyEntities.push_back(item);
	}

	std::sort(nearbyEntities.begin(), nearbyEntities.end(),
		[this](entt::entity lhs, entt::entity rhs) {
			const auto& lhsMotion = registry.get<Motion>(lhs);
			const auto& rhsMotion = registry.get<Motion>(rhs);
			return (lhsMotion.position.y + lhsMotion.offset_to_ground.y) <
				(rhsMotion.position.y + rhsMotion.offset_to_ground.y);
		});
	for (auto entity : nearbyEntities) {
		drawTexturedMesh(entity, projection_2D);
	}

	//auto uiMotions = registry.view<UI, Motion, RenderRequest>(entt::exclude<UIShip, FixedUI, TextData, Title>); 

	//std::sort(uiMotions.begin(), uiMotions.end(),
	//	[this](entt::entity lhs, entt::entity rhs) {
	//		const auto& lhsMotion = registry.get<Motion>(lhs);
	//		const auto& rhsMotion = registry.get<Motion>(rhs);
	//		return (lhsMotion.position.y + lhsMotion.offset_to_ground.y) <
	//			(rhsMotion.position.y + rhsMotion.offset_to_ground.y);
	//	});

	// draw all the debug tiles
	for (auto entity : registry.view<DebugTile>()) {
		drawTexturedMesh(entity, projection_2D);
	}


	for (auto entity : registry.view<UI, Motion, RenderRequest>(entt::exclude<UIShip, FixedUI, TextData, Title, Button, UIIcon, UIShipWeapon, UIShipEngine, UpgradeButton>)) {
		drawTexturedMesh(entity, projection_2D);
	}

	// draw framebuffer to screen
	// adding "vignette" effect when applied
	drawToScreen(true);
	
	std::vector<std::tuple<std::string, vec2, float, vec3, mat3, bool>> textsToRender;
	// Render static UI
	for (auto entity: registry.view<FixedUI, Motion, RenderRequest>(entt::exclude<UIShip, Item, Title, HiddenInventory, Button, UIIcon, UIShipWeapon, UIShipEngine, UpgradeButton>)) {
		if (registry.all_of<TextData>(entity)) {
			auto& textData = registry.get<TextData>(entity);
			if (textData.active) {
				drawTexturedMesh(entity, projection_2D);

				auto& motion = registry.get<Motion>(entity);				
				textsToRender.push_back(
					std::make_tuple(
						textData.content,
						vec2(
							motion.position.x - motion.scale.x / 2 + (motion.scale.x / 10.f),
							motion.position.y - motion.scale.y / 2 + (motion.scale.y / 4.f)
						),
						textData.scale,
						textData.color,
						projection_2D,
						true
					)
				);
			}
		}
		else {
			drawTexturedMesh(entity, ui_projection_2D);
		}
	}

	// Render items on static UI
	for (auto entity: registry.view<FixedUI, Motion, Item, RenderRequest>(entt::exclude<UIShip, TextData, Title, HiddenInventory, Button, UIIcon>)) {
		drawTexturedMesh(entity, ui_projection_2D);
	}

	
	// multiple quantity item on ground and on the inventory system should have a text next to it
	for (auto entity : registry.view<Item>(entt::exclude<DeathItems, HiddenInventory>)) {
		auto& motion = registry.get<Motion>(entity);
		auto& item = registry.get<Item>(entity);
		auto& camera = registry.get<Camera>(registry.view<Camera>().front());
		if (item.no == 1) continue;
		// TODO use ternary operator instead
		if (item.no >= 10) {
			if (registry.all_of<UI>(entity)) {
				textsToRender.push_back(
					std::make_tuple(
						std::to_string(item.no),
						vec2(
							motion.position.x + motion.scale.x / 2.f - 13.f, 
							motion.position.y + motion.scale.y / 2.f - 10.f
						),
						2,
						vec3({ 1.f, 1.f, 1.f }),
						ui_projection_2D,
						false
					)
				);
			}
			else {
				textsToRender.push_back(
					std::make_tuple(
						std::to_string(item.no),
						vec2(
							motion.position.x - camera.offset.x + motion.scale.x / 2.f - 13.f,
							motion.position.y - camera.offset.y - motion.scale.y / 2.f + 10.f),
						2,
						vec3({ 1.f, 1.f, 1.f }),
						projection_2D,
						false
					)
				);
			}
		}
		else {
			if (registry.any_of<UI>(entity)) {
				textsToRender.push_back(
					std::make_tuple(
						std::to_string(item.no),
						vec2(
							motion.position.x + motion.scale.x / 2.f - 8.f,
							motion.position.y + motion.scale.y / 2.f - 10.f),
						2,
						vec3({ 1.f, 1.f, 1.f }),
						ui_projection_2D,
						false
					)
				);
			}
			else {
				textsToRender.push_back(
					std::make_tuple(
						std::to_string(item.no),
						vec2(
							motion.position.x - camera.offset.x + motion.scale.x / 2.f - 8.f,
							motion.position.y - camera.offset.y - motion.scale.y / 2.f + 10.f
						),
						2,
						vec3({ 1.f, 1.f, 1.f }),
						projection_2D,
						false
					)
				);
			}
		}
	}

	// RENDERING ALL THE TEXT
	for (const auto& [content, position, scale, color, projection, wrap] : textsToRender) {
		renderText(content, position.x, position.y, scale, color, projection, wrap);
	}

	float x_ratio = (playerMotion.position.x / 16.f) / 500.f;
	float y_ratio = (playerMotion.position.y / 16.f) / 500.f;
	vec2 scale = vec2(499.f) / 3.f;
	vec2 offset = { WINDOW_WIDTH_PX - 175.f, 150.f };
	vec2 marker_pos = vec2(scale.x * x_ratio, scale.y * y_ratio) + offset - scale / 2.f;

	renderText("*", marker_pos.x, marker_pos.y, 1, vec3(0), ui_projection_2D);

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}


void RenderSystem::renderTitle()
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

	// clear backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.0, 10);

	// black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mat3 ui_projection_2D = createUIProjectionMatrix();

	auto title = registry.view<Title, Motion, RenderRequest>();

	for (auto entity : title) {
		drawTexturedMesh(entity, ui_projection_2D);
	}

	for (auto entity : registry.view<TitleOption>()) {
		auto& title_option = registry.get<TitleOption>(entity);
		if (title_option.hover) {
			renderText(
				title_option.text,
				title_option.position.x - title_option.size.x / 2,
				WINDOW_HEIGHT_PX - 35,
				2,
				glm::vec3(1.0f, 1.0f, 1.0f),
				ui_projection_2D
			);
		}
	}

	drawToScreen(false);

	glfwSwapBuffers(window);
	gl_has_errors();
}


void RenderSystem::renderUpgradeUI()
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

	// dark purple background
	glClearColor(0.2078f, 0.2078f, 0.2510f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// mat3 projection_2D = createProjectionMatrix();
	mat3 ui_projection_2D = createUIProjectionMatrix();

	// RENDER THE UI SCREEN
	auto buttons = registry.view<Button, Motion, RenderRequest>();

	// render the buttons
	for (auto entity : buttons) {
		drawTexturedMesh(entity, ui_projection_2D);
	}

	drawToScreen(false);
	
	// mat3 flippedProjection = ui_projection_2D;
	// flippedProjection[1][1] *= -1.0f;

	auto& screen_state = registry.get<ScreenState>(screen_entity);

	// render all the texts for the buttons
	for (auto entity : registry.view<Button>()) {
		auto& ui_option = registry.get<ButtonOption>(entity);
		if (ui_option.hover && screen_state.current_screen == ScreenState::ScreenType::UPGRADE_UI) {
			int textWidth = getTextWidth(ui_option.text, 4);
			float centeredX = ui_option.position.x - textWidth / 2.0f;
			float centeredY = ui_option.position.y + ui_option.size.y / 2.0f + 10.0f;
			
			renderText(
				ui_option.text, 
				centeredX, 
				centeredY, 
				4, 
				glm::vec3(1.0f, 1.0f, 1.0f), 
				ui_projection_2D
			);
		}
	}

	// render the icons for each button
	for (auto entity : registry.view<UIIcon, Motion, RenderRequest>()) {
		drawTexturedMesh(entity, ui_projection_2D);
	}

	renderText(
		"UPGRADES",  
		WINDOW_WIDTH_PX / 2 - getTextWidth("UPGRADES", 4)/2, 
		100.f,
		4, 
		glm::vec3(1.0f, 1.0f, 1.0f), 
		ui_projection_2D
	);

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

	// clear backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.0, 10);

	// dark purple background
	glClearColor(0.2078f, 0.2078f, 0.2510f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mat3 ui_projection_2D = createUIProjectionMatrix();

	// render upgrade buttons
	std::vector<entt::entity> buttonEntities;
	for (auto entity : registry.view<UpgradeButton, Motion, RenderRequest>()) {
		buttonEntities.push_back(entity);
		drawTexturedMesh(entity, ui_projection_2D);
	}

	// render ship weapons
	for (auto entity : registry.view<UIShipWeapon, Motion, RenderRequest>()) {
		auto& shipWeapon = registry.get<UIShipWeapon>(entity);
		if (shipWeapon.active) {
			drawTexturedMesh(entity, ui_projection_2D);
		}
	}

	// render ship engine
	for (auto entity : registry.view<UIShipEngine, Motion, RenderRequest>()) {
		auto& shipEngine = registry.get<UIShipEngine>(entity);
		if (shipEngine.active) {
			drawTexturedMesh(entity, ui_projection_2D);
		}
	}

	// render ship
	for (auto entity : registry.view<UIShip, Motion, RenderRequest>()) {
		drawTexturedMesh(entity, ui_projection_2D);
	}

	// CHANGE -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	drawToScreen(false);
	renderText(
		"SHIP UPGRADES", 
		WINDOW_WIDTH_PX / 2 - 180, 
		100.f,
		4, 
		vec3(1.0f, 1.0f, 1.0f), 
		ui_projection_2D
	);

	int width = 2 * WINDOW_WIDTH_PX, height = 2 * WINDOW_HEIGHT_PX;

	// Draw upgrade lines
    // positions of upgradeable parts (start)
    std::vector<std::pair<std::string, vec2>> upgradePoints = {
		// health line
        {"", vec2(width/4, height/4 - height/2*0.045f)},
        {"Health", vec2(width/8 + width/2*0.12f, height/8 + height/2*0.09f)},
        // weapon line
        {"", vec2(width/4 + width/2*0.09f, height/4)},
        {"Blaster", vec2(3*width/8 - width/2*0.09f, height/8 + height/2*0.13f)},
        // shield line
        {"", vec2(width/4 - width/2*0.035f, height/4 + height/2*0.13f)},
        {"Range", vec2(width/8 + width/2*0.12f, 3*height/8)},
        // fire rate line
        {"", vec2(width/4 + width/2*0.09f, height/4 + height/2*0.07f)},
        {"Fire Rate", vec2(3*width/8 - width/2*0.09f, 3*height/8 - height/2*0.045f)},
    };
    
    // where labels should be positioned (end)
    std::vector<vec3> labelPositions = {
		// health line
        vec3(width/8 + width/2*0.12f, height/8 + height/2*0.09f, 0.0f),
        vec3(width/8 + width/2*0.03f, height/8 + height/2*0.09f, width/2*0.09f),
        // weapon line
        vec3(3*width/8 - width/2*0.09f, height/8 + height/2*0.13f, 0.0f),
        vec3(3*width/8, height/8 + height/2*0.13f, 0.0f),
        // shield line
        vec3(width/8 + width/2*0.12f, 3*height/8, 0.0f),
        vec3(width/8 + width/2*0.03f, 3*height/8, width/2*0.08f),
        // fire rate line
        vec3(3*width/8 - width/2*0.09f, 3*height/8 - height/2*0.045f, 0.0f),
        vec3(3*width/8 + width/2*0.025f, 3*height/8 - height/2*0.045f, 0.0f),
    };
    
    // draw lines pointing to upgradeable parts
    for (int i = 0; i < upgradePoints.size(); i++) {
        drawLine(upgradePoints[i].second, labelPositions[i], vec3(0.49f, 0.43f, 0.63f), 2.0f, ui_projection_2D);
		
		// CHANGE -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // mat3 flippedProjection = ui_projection_2D;
        // flippedProjection[1][1] *= -1.0f;
        renderText(
			upgradePoints[i].first, 
			labelPositions[i].x - labelPositions[i].z + 20.0f, 
			labelPositions[i].y - height/2*0.01 + 10.0f, 
            4, 
			vec3(1.0f, 1.0f, 1.0f), 
			ui_projection_2D
		);
    }

	// mat3 flippedProjection = projection_2D;
	// flippedProjection[1][1] *= -1.0f; 

	// display all the text for the buttons
	for (auto& entity : buttonEntities) {
		auto& button = registry.get<UpgradeButton>(entity);
		auto& motion = registry.get<Motion>(entity);
		renderText(
			button.text, 
			motion.position.x - 35.0f, 
			motion.position.y, 
			2, 
			glm::vec3(1.0f, 1.0f, 1.0f),
			ui_projection_2D)
		;

		if (button.missingResources) {
			renderText(
				button.missingResourcesText, 
				motion.position.x - 65.0f,
				motion.position.y + 35.0f,
				2, 
				glm::vec3(1.0f, 0.0f, 0.0f), 
				ui_projection_2D
			);
		} else {
			renderText(
				button.missingResourcesText, 
				motion.position.x - 65.0f, 
				motion.position.y + 35.0f, 
				2, 
				glm::vec3(0.0f, 1.0f, 0.0f), 
				ui_projection_2D);
		}
	}
	
	renderText(
		"SHIP UPGRADES", 
		-width/2*0.15f,
		height/2*0.4f, 
		1, 
		glm::vec3(1.0f, 1.0f, 1.0f), 
		ui_projection_2D
	);



	glfwSwapBuffers(window);
    gl_has_errors();
}

void RenderSystem::renderEndScreen() {
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

	// clear backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.0, 10);

	// black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mat3 ui_projection_2D = createUIProjectionMatrix();
	int width = 2 * WINDOW_WIDTH_PX, height = 2 * WINDOW_HEIGHT_PX;

	drawToScreen(false);

	std::string end_0 = std::string("... you did it. against all odds, you fully repaired the ship. ");
	std::string end_1 = std::string("honestly? we didnt think you had it in you, but here we are. ");

	std::string end_2 = std::string("systems are online, engines are humming. all thats left is to press {1'F'} one last time and launch. ");
	std::string end_3 = std::string("say goodbye to planet {1Nova} and good riddance.");

	std::string end_4 = std::string("whats waiting for you back home? maybe a cold beer. maybe a debrief with some grumpy admiral. ");
	std::string end_5 = std::string("maybe just some damn peace and quiet for once.");

	std::string end_6 = std::string("whatever it is... you earned it. ");
	std::string end_7 = std::string("now get off this rock. and try not to crash the next one, yeah?");

	std::string end_8 = std::string("press {1'F'} to leave planet");

	float end_0_x = width/4 - getTextWidth(end_0, 2.0f)/2;
	float end_1_x = width/4 - getTextWidth(end_1, 2.0f)/2;
	float end_2_x = width/4 - getTextWidth(end_2, 2.0f)/2;
	float end_3_x = width/4 - getTextWidth(end_3, 2.0f)/2;
	float end_4_x = width/4 - getTextWidth(end_4, 2.0f)/2;
	float end_5_x = width/4 - getTextWidth(end_5, 2.0f)/2;
	float end_6_x = width/4 - getTextWidth(end_6, 2.0f)/2;
	float end_7_x = width/4 - getTextWidth(end_7, 2.0f)/2;
	float end_8_x = width/4 - getTextWidth(end_8, 4.0f)/2;

	renderText(end_0, end_0_x, height/4 - height/8, 2.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);
	renderText(end_1, end_1_x, height/4 - height/8 + 25.0f, 2.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);
	renderText(end_2, end_2_x, height/4 - height/8 + 50.0f, 2.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);
	renderText(end_3, end_3_x, height/4 - height/8 + 75.0f, 2.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);
	renderText(end_4, end_4_x, height/4 - height/8 + 100.0f, 2.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);
	renderText(end_5, end_5_x, height/4 - height/8 + 125.0f, 2.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);
	renderText(end_6, end_6_x, height/4 - height/8 + 150.0f, 2.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);
	renderText(end_7, end_7_x, height/4 - height/8 + 175.0f, 2.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);

	renderText(end_8, end_8_x, height/4 - height/8 + 450.0f, 4.0f, vec3(1.0f, 1.0f, 1.0f), ui_projection_2D);

	glfwSwapBuffers(window);
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	auto& screen_state = registry.get<ScreenState>(screen_entity);
	auto& ship = registry.get<Ship>(registry.view<Ship>().front());

	if (ship.maxHealth && ship.maxFireRate && ship.maxRange && ship.maxWeapon) {
		if (!shipFullyUpgraded) {
			shipFullyUpgraded = true;
			shipUpgradeTime = (float)glfwGetTime(); // mark the time of upgrade
		}
		float timeSinceUpgrade = (float)glfwGetTime() - shipUpgradeTime;
		if (timeSinceUpgrade >= 5.0f) {
			renderEndScreen();
			endScreenTriggered = true;
			screen_state.current_screen = ScreenState::ScreenType::END_SCREEN;
		}
	}

	if (endScreenTriggered) {
		return;
	}

	switch (screen_state.current_screen) {
		case ScreenState::ScreenType::TITLE:
			renderTitle();
			break;
		case ScreenState::ScreenType::UPGRADE_UI:
			renderUpgradeUI();
			break;
		case ScreenState::ScreenType::SHIP_UPGRADE_UI:
			renderShipUI();
			break;
		case ScreenState::ScreenType::PLAYER_UPGRADE_UI:
			// renderPlayerUI
			break;
		case ScreenState::ScreenType::WEAPON_UPGRADE_UI:
			// renderWeaponUI
			break;
		case ScreenState::ScreenType::GAMEPLAY:
			renderGamePlay();
			break;
	}

	auto glyphs = registry.view<Glyph>();

	if (glyphs.size() > 0) {
    	registry.destroy(glyphs.begin(), glyphs.end());
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

// helpers
float RenderSystem::getScaledWidth(float percentage) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    return w * percentage;
}

float RenderSystem::getScaledHeight(float percentage) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    return h * percentage;
}

vec2 RenderSystem::getScaledPosition(float xPercentage, float yPercentage) {
    return vec2(getScaledWidth(xPercentage), getScaledHeight(yPercentage));
}

vec2 RenderSystem::getScaledSize(float widthPercentage, float heightPercentage) {
    return vec2(getScaledWidth(widthPercentage), getScaledHeight(heightPercentage));
}
