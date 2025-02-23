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
- Utility:
  - Press "r" to restart the game

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
