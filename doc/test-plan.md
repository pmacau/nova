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
- Utility:
  - Press "r" to restart the game
  - Press buttons 1-6 to change the background music track
  - Press "esc" to pause the game and view the title screeen 

# Milestone 1

# Actions

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
 
# Milestone 2

# Actions

_All of the above actions from previous milestones and..._

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
