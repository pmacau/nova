# Controls

With each milestone, we will add additional controls as necessary

- Movement:
  - Move up: press "w" or up-arrow
  - Move down: press "s" or down-arrow
  - Move left: press "a" or left-arrow
  - Move right: press "d" or right-arrow
  - Dash: press "space" to dash in the current direction
- Combat:
  - Aim with the mouse
  - Shoot projectiles at the enemy with left-mouse click
  - Melee: press "v" to issue a melee attack
- Gameplay:
  - When near the ship, press "f" to toggle the ship UI
  - Left click items in the inventory to consume them
  - Press "tab" to see your full inventory
  - Arrange your items with right-mouse click (RMC)
  - Holding CRTL+RMC picks up a whole stack, LSHIFT+RMC picks up half of the stack, and ALT+RMC picks up at most 5 items from a stack.
  - While moving an item in your inventory, left click it outside of the inventory to drop it on the ground.
- Utility:
  - Press "r" to restart the game
  - Press "esc" to pause the game and view the title screeen
  - Press "enter" to skip through tutorial dialogue

# Milestone 4

## Actions

_All of the below actions from previous milestones and..._

- **Traverse the island**:

  - Explore the 5 unique biomes. See their differences in weather, and explore the variance in the trees within a biome.
  - Find house and castle tower structures.
  - Notice that enemies now try to avoid obstacles like trees and houses or towers.

- **Fight the enemies**:

  - Find the far corners of the island to view the new boss sprites.
  - Fight the new ranged enemies that will strategically keep distance from the player.
  - Collect many copper and iron items for upgrades.

- **UI**
  - Use your collected resources to upgrade the ship, the player's stats, and the player's weapons
  - Completely upgrade the ship to see the end-dialogue, and finish the game!


# Milestone 3

## Actions

_All of the below actions from previous milestones and..._

- **Traverse the island**:

  - Explore the 5 unique biomes. See their differences in tileset, and the different types of tress that grow in these biomes.
  - Notice the differences in music track for each of the biomes.
  - If you have explored long enough, notice the screen darkens to simulate a day-night cycle
  - Find bosses in the far corners of each biome

- **Fight the enemies**:

  - Walk close to enemies and lure enemies through complex terrain (e.g., water). Observe how they avoid walking into unwalkable tiles, correctly navigating around it using the A\* pathfinding system.
  - Notice that enemies walk through trees and land obstacles intentionally: this is a design choice to make enemy movement feel more fluid and intense. Listen for a new sound effect triggered when enemies pass through trees (a quick wooden click sound).
  - Lure enemies to chase you and control the player to constantly moving around, and observe how enemies recalculate their paths dynamically when the player moves out of range.
  - Move around the island and stay away from enemies. Note how enemy patrol routes and spawn locations vary based on map layout avoiding unwalkable map structures.
  - Kill enemies to see two new drops: iron and copper. Notice that not every enemy drops an item, and the amount and identity of an item dropped is randomly generated. Notice that boss enemies (found deep in each biome) drop a larger quantity of items.

- **Performance testing**:

  - View the FPS in the window title caption
  - Traverse the now 16x larger map, thanks to the improved quad-tree performance acceleration. Notice that, to the user, collisions and rendering appears unchanged, but the performance is much better, allowing for much smoother gameplay.

- **UI**
  - Follow the instructions in the tutorial to learn how to play the game, the main objective, and some backstory about the setting
  - Collect items around the map by defeating enemies (boss enemies drop a large amount of items) to upgrade the ship through the ship upgrade UI (press "f" near the ship to access it). Notice how the ship visually changes appearance when upgrading the fire rate, blasters, or health.
  - Press "tab" to see your full inventory, and arrange your items with right-mouse click (RMC). Holding CRTL+RMC picks up a whole stack, LSHIFT+RMC picks up half of the stack, and ALT+RMC picks up at most 5 items from a stack. While moving an item in your inventory, left click it outside of the inventory to drop it on the ground.
  - Use keys 1-5 to interact with items in your hotbar items (consumes a health potion, or swaps to a different weapon; resource items cannot be interacted with this way).

# Milestone 2

## Actions

_All of the below actions from previous milestones and..._

- **Traverse the island**:

  - Notice the new trees and shrubbery. Trees spawn on grass tiles, and cacti spawn on sand tiles
  - Run around to see different enemy types including blue goblins (periodically spawn), and boss enemies (found at the far corners of the map)
  - Walk into different obstacles including water, trees/cacti, and the ship to see the collision detection

- **Fight the enemy**:

  - Experiment with running to and away the blue goblins to see their AI state transitions
  - Observe that enemies spawn at set intervals randomly around the player, up to a certain mob cap
  - Notice that enemies can spawn in groups/hoardes instead of individually
  - Travel to all corners of the map to find and defeat the 4 boss enemies

- **Performance testing**:

  - View the FPS in the window title caption
  - Shoot many projectiles, and observe as the FPS will stay relatively constant
  - Leave the game on for an extended period of time (i.e. 5 minutes). Observe that the performance does not degrade

- **UI**:
  - Follow the prompts on screen during the tutorial to learn how to play the game
  - Notice that the tutorial changes state after succesfully completing a stage
  - Press "f" near the ship to view the ship upgrade UI (ship upgrades coming in a future milestone)
  - Press "esc" to pause the game and view the title screen UI. Load and save functionality coming in a future milestone.

# Milestone 1

## Actions

- **Traverse the island**:

  - Move the player around the island to observe water, sand, and grass
  - Collide with the rocketship, and with water, to see barrier collisions
  - Move up, down, left, and right to observe the player's walking animation
  - Observe no movement animation when the player is standing still
  - Walk behind the rocketship to observe the player being drawn before the ship. This is due to the sorted draw order required for our pseudo-3D effect

- **Fight the enemy**:

  - Run away from the enemy and observe the primitive path-finding as they run towards you (to be expanded upon in later milestones)
  - Draw the enemy near the ship, and watch as the ship will fire projectiles automatically when the enemy is within range
  - Fire projectiles with left-mouse click at the enemy, and watch as their health lowers
  - Defeat the enemy by lowering their health to zero, observe the enemy disappearing, and see a health potion item dropped
  - Take damage from the enemy by walking into them, and observe being knocked back from the collision. Notice that this knockback will not allow you to pass through barriers. You will also see your health lower (in the green health bar), and the red vignette begin to grow (indicating you are on lower-than-full health)
  - Take damage from the enemy until you die, in which you will respawn at the spawnpoint

- **Interact with the inventory**:

  - Walk over a dropped health potion to pick it up.
  - Left click the health potion in its inventory slot to consume and replenish health. If not already at full health, notice that the vignette will lighten from this, and the green healthbar will grow.

- **Performance testing**:
  - View the FPS in the window title caption
  - Shoot many projectiles, and observe as the FPS will stay relatively constant
  - Leave the game on for an extended period of time (i.e. 5 minutes). Observe that the performance does not degrade
