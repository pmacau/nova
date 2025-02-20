#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/trigonometric.hpp>
#include <iostream>
// internal
#include "render_system.hpp"
#include "tinyECS/components.hpp"
#include <glm/gtc/type_ptr.hpp>

RenderSystem::RenderSystem(entt::registry& reg) :
	registry(reg)
{
	screen_state_entity = registry.create();
}


// TODO: can refactor this, and possibly speed it up by not binding the same texture again and again
void RenderSystem::drawBackground(const mat3& projection) {
	entt::entity player_entity = *registry.view<Player>().begin();
	auto& p_motion = registry.get<Motion>(player_entity);

	int p_row = int(p_motion.position.y / 16); 
	int p_col = int(p_motion.position.x / 16);
	int screen_tile_width = WINDOW_WIDTH_PX / 16;
	int screen_tile_height = WINDOW_WIDTH_PX / 16;

	int min_x = max(0, p_col - screen_tile_width);
	int max_x = min(199, p_col + screen_tile_width);
	int min_y = max(0, p_row - screen_tile_height);
	int max_y = min(199, p_row + screen_tile_height);

	auto camera_entity = registry.view<Camera>().front(); // TODO: make this more robust
	auto& camera = registry.get<Camera>(camera_entity);

	Transform camera_transform;
	camera_transform.translate(- camera.offset);

	for (int i = min_y; i < max_y; i++) {
		for (int j = min_x; j < max_x; j++) {
			
			// Create transforms
			Transform model_transform;
			model_transform.translate(vec2(j * 16, i * 16));
			model_transform.scale(vec2(16.f, 16.f));

			RenderRequest render_request = tileMap[i][j].request;

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

			GLint in_position_loc = glGetAttribLocation(program, "in_position");
			GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
			gl_has_errors();
			assert(in_texcoord_loc >= 0);

			glEnableVertexAttribArray(in_position_loc);
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

			GLuint texture_id =
				texture_gl_handles[(GLuint)render_request.used_texture];

			glBindTexture(GL_TEXTURE_2D, texture_id);
			gl_has_errors();

			// Getting uniform locations for glUniform* calls
			GLint color_uloc = glGetUniformLocation(program, "fcolor");
			const vec3 color = vec3(1);
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


			GLuint spriteData_loc = glGetUniformLocation(currProgram, "spriteData");
			GLuint sheetDims_loc = glGetUniformLocation(currProgram, "sheetDims");

			vec2& coord = tileMap[i][j].coord;
			glUniform4f(spriteData_loc, coord.x, coord.y, 16.f, 16.f);
			glUniform2f(sheetDims_loc, 112.f, 112.f);
			gl_has_errors();

			// Drawing of num_indices/3 triangles specified in the index buffer
			glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
			gl_has_errors();
		}
	}
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
	//std::cout << projLoc << std::endl;
	GLint transLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::DEBUG], "transform");
	//std::cout << transLoc << std::endl;
	GLint colorLoc = glGetUniformLocation(effects[(GLuint)EFFECT_ASSET_ID::DEBUG], "debugColor");
	//std::cout << colorLoc << std::endl;
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
	camera_transform.translate(- camera.offset);

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
	//std::cout << "Drawing entity with texture: " << (int)render_request.used_texture << std::endl;
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

	glUniform4f(spriteData_loc, s.coord.x, s.coord.y, s.dims.x, s.dims.y);
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
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glDisable(GL_DEPTH_TEST); // Debug: disable depth test

	glEnable(GL_DEPTH_TEST);

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

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// clear backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	// white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Debug: claer depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Debug
	// glClearDepth(10.f);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Debug
	glEnable(GL_DEPTH_TEST);
	// glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();

	mat3 projection_2D = createProjectionMatrix();

	// render players and mobs
	std::vector<entt::entity> PlayerMobsRenderEntities;
	// get all mob and player entities with motion and render request components
	auto mobs = registry.view<Mob, Motion, RenderRequest>();
	auto players = registry.view<Player, Motion, RenderRequest>();
	auto ships = registry.view<Ship, Motion, RenderRequest>();

	for (auto entity : mobs) {
		PlayerMobsRenderEntities.push_back(entity);
	}

	for (auto entity : players) {
		PlayerMobsRenderEntities.push_back(entity);
	}

	for (auto entity : ships) {
		PlayerMobsRenderEntities.push_back(entity);
	}

	// Sort entities based on Y position of the "ground offset"
	std::sort(PlayerMobsRenderEntities.begin(), PlayerMobsRenderEntities.end(), [this](entt::entity a, entt::entity b) {
		auto& motionA = registry.get<Motion>(a);
		auto& motionB = registry.get<Motion>(b);
		vec2 groundA = motionA.position + motionA.offset_to_ground;
		vec2 groundB = motionB.position + motionB.offset_to_ground;

		// if y vlue the same, check if either is a player
		if (groundA.y == groundB.y) {
			if (registry.any_of<Player>(a)) {
				return true;
			}
			if (registry.any_of<Player>(b)) {
				return false;
			}

			// else sort by x
			return groundA.x > groundB.x;
		}

		return groundA.y > groundB.y; // Lower Y should render later (on top)
	});

	// Render entities in sorted order
	for (auto entity : PlayerMobsRenderEntities) {
		drawTexturedMesh(entity, projection_2D);
	}

	// render projectiles
	std::vector<entt::entity> ProjectileRenderEntities;
	auto projectiles = registry.view<Projectile, Motion, RenderRequest>();

	for (auto entity : projectiles) {
		ProjectileRenderEntities.push_back(entity);
	}

	for (auto entity : ProjectileRenderEntities) {
		drawTexturedMesh(entity, projection_2D);
	}

	// Draw background first
	drawBackground(projection_2D);

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
	// vec3 transformedPos = projection * transform * vec3(0.0f, 0.0f, 1.0f);

    // std::cout << "[DEBUG] Drawing point at screen position: (" 
    //           << transformedPos.x << ", " << transformedPos.y << ")" << std::endl;

	const GLuint program = (GLuint)effects[(GLuint)EFFECT_ASSET_ID::COLOURED];
	glUseProgram(program);
	gl_has_errors();

	// std::cout << "Program: " << program << std::endl;	


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

	// std::cout << "currProgram: " << currProgram << std::endl;	


	GLint color_uloc = glGetUniformLocation(currProgram, "in_color");
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	// std::cout << "color_uloc: " << color_uloc << std::endl;

	GLint transform_uloc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	gl_has_errors();

	// std::cout << "transform_uloc: " << transform_uloc << std::endl;


	GLint projection_uloc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	// std::cout << "projection_uloc: " << projection_uloc << std::endl;


	// Temporarily disable depth test to check if it's blocking rendering
    glDisable(GL_DEPTH_TEST);

	// Check if OpenGL has errors before drawing
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error BEFORE drawing debug point: " << error << std::endl;
    }

	// draw
	glPointSize(10.0f);
	// std::cout << "Rendering Debug Point..." << std::endl;
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();

	error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error AFTER drawing debug point: " << error << std::endl;
    }


	// Re-enable depth test
    glEnable(GL_DEPTH_TEST);
}