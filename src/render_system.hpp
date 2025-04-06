#pragma once

#include <array>
#include <utility>
#include <entt.hpp>
#include "common.hpp"
#include "tinyECS/components.hpp"
#include "quadtree/quadtree.hpp"
#include "render/shader.h"

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
		textures_path("ship/Ship-Full-HP.png"),
		textures_path("ship/Ship-Slightly-Damaged.png"),
		textures_path("ship/Ship-Damaged.png"),
		textures_path("ship/Ship-Very-Damaged.png"),		
		//textures_path("player/astronaut.png"), // might have to look at for conflict
		textures_path("ship/weapon/ship-blaster-spritesheet.png"),
		textures_path("ship/weapon/ship-missles-spritesheet.png"),
		textures_path("ship/weapon/ship-railgun-spritesheet.png"),
		textures_path("ship/weapon/ship-smg-spritesheet.png"),
		textures_path("ship/engine/ship-blaster-engine.png"),
		textures_path("ship/engine/ship-missle-engine.png"),
		textures_path("ship/engine/ship-railgun-engine.png"),
		textures_path("ship/engine/ship-smg-engine.png"),
        textures_path("mob/demoMob.png"),
		textures_path("tile/tileset.png"),
		map_path("textured_map.png"),
		textures_path("projectiles/gold_bubble.png"),
		textures_path("projectiles/ship/blaster-projectile.png"),
		textures_path("projectiles/ship/missle-projectile.png"),
		textures_path("projectiles/ship/railgun-projectile.png"),
		textures_path("projectiles/ship/smg-projectile.png"),
		textures_path("projectiles/shotgun_projectile.png"),
		textures_path("weapons/default.png"),
		textures_path("weapons/homing-missile.png"),
		textures_path("weapons/shotgun.png"),
		textures_path("healthbar/red.png"), 
		textures_path("healthbar/player-health-inner.png"), 
		textures_path("healthbar/player-health-outer.png"),
		textures_path("items/potion.png"), 
		textures_path("items/grave.png"),
		textures_path("items/iron.png"),
		textures_path("items/copper.png"),
		textures_path("inventory/inventory-slot.png"),
		textures_path("inventory/inventory-slot-active.png"),
		textures_path("terrain/tree.png"),
		textures_path("mob/goblin_torch_blue.png"), 
		textures_path("title/screen.png"), 
		textures_path("textBackground/textbox.png"),
		textures_path("ui/selection_button.png"),
		textures_path("ui/green-btn-active.png"),
		textures_path("ui/green-btn-pressed.png"),
		textures_path("ui/red-btn-active.png"),
		textures_path("ui/red-btn-pressed.png"),
		map_path("biome_map.png"),
		textures_path("text/text.png")
	};

	std::array<GLuint, effect_count> effects;

	std::unordered_map<std::string, Shader> shaders;

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("textured"),
		shader_path("vignette"),
		shader_path("coloured"),
		shader_path("debug"),
		shader_path("text"),
		shader_path("line"),
		shader_path("snow"),
		shader_path("fog"),
		shader_path("heat"),
		shader_path("rain"),
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
	RenderSystem(entt::registry& reg, QuadTree& quadTree);


	// Initialize the window
	bool init(GLFWwindow* window);
	bool initFreetype();
	void renderTitle();
	bool debugModeEnabled;
	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);
	//void initTree(); 
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

	//entt::entity get_screen_state_entity() { return screen_state_entity; }

private:
	entt::registry& registry;
	QuadTree& quadTree;
	// Internal drawing functions for each entity type
	void drawTexturedMesh(entt::entity entity, const mat3& projection);
	void drawToScreen(bool vignette);
	void renderGamePlay();
	void renderUpgradeUI();
	void renderShipUI();

	void renderEndScreen();
	void drawDebugHitBoxes(const glm::mat3& projection);
	void drawLine(vec2 start, vec2 end, vec3 color, float thickness, const mat3& projection);

	// helpers
	float getScaledWidth(float percentage);
	float getScaledHeight(float percentage);
	vec2 getScaledPosition(float xPercentage, float yPercentage);
	vec2 getScaledSize(float widthPercentage, float heightPercentage);


	// void drawDebugHitBoxes(const glm::mat3& projection, const glm::mat3& transform);

	void drawDebugPoint(mat3 projection, mat3 transform, vec3 color);
	// Window handle
	GLFWwindow* window;
	GLuint defaultVAO;
	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;
	//entt::entity screen_state_entity;
	entt::entity screen_entity;

	// text based stuff
	std::map<char, Character> Characters;
	GLuint textShaderProgram;
	GLuint textVAO = 0;
	GLuint textVBO = 0;

	mat3 shipUITransform = mat3(1.0f);


	bool shipFullyUpgraded = false;
	float shipUpgradeTime = -1.0f;
	bool endScreenTriggered = false;
	
	// void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color, const mat3& projection);
	int getTextWidth(const std::string& text, int scale);
	void renderText(const std::string& text, float x, float y, int scale, glm::vec3 color, const mat3& projection, bool wrap=false);
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
