#pragma once

#include <array>
#include <utility>
#include <entt.hpp>
#include "common.hpp"
#include "tinyECS/components.hpp"


// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count>  texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators (see TEXTURE_ASSET_ID).
	const std::array<std::string, texture_count> texture_paths = {
		textures_path("player/astronaut-spritesheet.png"),
		textures_path("ship/Ship1.png"),
		textures_path("ship/Ship2.png"),
		textures_path("ship/Ship3.png"),
		textures_path("ship/Ship4.png"),
		textures_path("ship/Ship5.png"),
		textures_path("ship/Ship6.png"),		
		//textures_path("player/astronaut.png"), // might have to look at for conflict
        textures_path("mob/demoMob.png"),
		textures_path("tile/tileset.png"),
		map_path("textured_map.png"),
		textures_path("projectiles/gold_bubble.png"),
		textures_path("healthbar/green.png"), 
		textures_path("healthbar/red.png"), 
		textures_path("items/potion.png"), 
		textures_path("inventory/inventory-slot.png"),
		textures_path("terrain/tree.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("textured"),
		shader_path("vignette"),
		shader_path("coloured"),
		shader_path("debug"),
		shader_path("text"),
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;

	// std::vector<std::vector<TileRender>> tileMap;

	// for texts
	struct Character {
		GLuint     TextureID;  // ID handle of the glyph texture
		glm::ivec2 Size;       // Size of glyph
		glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
		GLuint     Advance;    // Horizontal offset to advance to next glyph
	};

public:
	RenderSystem(entt::registry& reg);

	// Initialize the window
	bool init(GLFWwindow* window);
	bool initFreetype();
	bool debugModeEnabled;
	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlGeometryBuffers();

	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the vignette shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createUIProjectionMatrix();

	mat3 createProjectionMatrix();

	entt::entity get_screen_state_entity() { return screen_state_entity; }

private:
	entt::registry& registry;

	// Internal drawing functions for each entity type
	void drawTexturedMesh(entt::entity entity, const mat3& projection);
	void drawToScreen();

	void renderGamePlay();
	void renderShipUI();

	void drawBackground(const mat3& projection);
	void drawDebugHitBoxes(const glm::mat3& projection, const glm::mat3& transform);

	void drawDebugPoint(mat3 projection, mat3 transform, vec3 color);
	// Window handle
	GLFWwindow* window;
	GLuint defaultVAO;
	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;
	entt::entity screen_state_entity;
	entt::entity screen_entity;

	// text based stuff
	std::map<char, Character> Characters;
	GLuint textShaderProgram;
	GLuint textVAO = 0;
	GLuint textVBO = 0;

	mat3 shipUITransform = mat3(1.0f);

	GLuint createShader(const std::string& vertexPath, const std::string& fragmentPath);
	
	void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color, const mat3& projection);
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
