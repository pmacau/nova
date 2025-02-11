class SaveAndLoad {
	public: 
		static void save(entt::registry& registry);
		static void load(entt::registry& registry, entt::entity& player_entity);
};