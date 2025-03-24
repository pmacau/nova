# Controls

With each milestone, we will add additional controls as necessary

- Movement:
  - Move up: press "w" or up-arrow
  - Move down: press "s" or down-arrow
  - Move left: press "a" or left-arrow
  - Move right: press "d" or right-arrow
- Combat:
  - Aim with the mouse
  - Shoot projectiles at the enemy with left-mouse click
- Gameplay:
  - When near the ship, press "f" to toggle the ship UI
  - Left click items in the inventory to consume them
  - @PRANAV todo include controls on how to use the inventory
- Utility:
  - Press "r" to restart the game
  - Press "esc" to pause the game and view the title screeen
  - Press "enter" to skip the tutorial

# Milestone 3

## Actions

_All of the below actions from previous milestones and..._

- **Traverse the island**:

  - Explore the 5 unique biomes. See their differences in tileset, and the different types of tress that grow in these biomes.
  - Notice the differences in music track for each of the biomes.
  - If you have explored long enough, notice the screen darkens to simulate a day-night cycle

- **Fight the enemies**:

  - @STEVE todo add something about noticing how enemies dont go in water, how they still (intentionally) pass through trees for gameplay sake (and how that makes a noise now)
  - @STEVE todo also talk about the a\* path finding of course
  - @PRANAV/FRANK todo talk about the new items dropped by the enemies, at different rates, and how boss drops many items

- **Performance testing**:

  - View the FPS in the window title caption
  - @PHILLIP todo talk about how the FPS is good because of quadtree. talk about how no matter the entities on the screen, we only render whats on screen/process stuff nearby in the quadtree, so we have good FPS

- **UI**
  - Follow the instructions in the tutorial to learn how to play the game, the main objective, and some backstory about the setting
  - @FRANK todo talk about ship upgrades and how it costs materials to do them
  - @PRANAV todo talk about the inventory system, how to stack items and whatnot, and how to open the inventory

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
