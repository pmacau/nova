# team 12

| #   | Category             | Task                                                                                  | Description                                                                                                                                                                                                      | Code |
| :-- | :------------------- | :------------------------------------------------------------------------------------ | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :--- |
| 1   | Rendering            | Textured geometry                                                                     | All sprites are rendered as quad geometries, textured mapped from a spritesheet. Our drawing order layers entities such that entities further to the camera (i.e. a lower y-value), are drawn first.             |      |
| 2   | Rendering            | Basic 2D transformations                                                              | Through a motion component, every rendered entity has an associated position (translation), angle (rotation), and scale (size). These values are used to properly transform the sprites when rendered on-screen. |      |
| 3   | Rendering            | Key-frame/state interpolation                                                         |                                                                                                                                                                                                                  |      |
| 4   | Gameplay             | Keyboard/mouse control                                                                |                                                                                                                                                                                                                  |      |
| 5   | Gameplay             | Random/coded action                                                                   |                                                                                                                                                                                                                  |      |
| 6   | Gameplay             | Well-defined game-space boundaries                                                    |                                                                                                                                                                                                                  |      |
| 7   | Gameplay             | Simple collision detection & resolution                                               |                                                                                                                                                                                                                  |      |
| 8   | Stability            | Stable frame rate and minimal game lag                                                |                                                                                                                                                                                                                  |      |
| 9   | Stability            | No crashes, glitches, or unpredictable behaviour                                      |                                                                                                                                                                                                                  |      |
| 10  | Software Engineering | Test plan - a list of player or game actions and their excepted outcomes              |                                                                                                                                                                                                                  |      |
| 11  | Reporting            | Bug list (preliminary) - Google Sheets or Microsoft Excel spreadsheet                 |                                                                                                                                                                                                                  |      |
| 12  | Reporting            | Demonstration video (3 min. max) showcasing assignment required and creative features |                                                                                                                                                                                                                  |      |
| 13  | Creative Element     |

## Smooth Interpolation Implementation

To meet requirement [3] on the milestone (smooth interpolation between at least two values of a property), we implemented a vignette effect in the fragment shader. The implementation uses linear interpolation (`mix` function in GLSL) to smoothly transition between the original screen color and a red tint based on the `vignetteFactor`, which is the distance from the center of the screen, and a uniform `darken_screen_factor`. The point of this interpolation is to show how hurt the player is with the vignette getting more and more red the more damage the player takes.

### Code Location

The logic for this is located in the fragment shader `vignette.fs.glsl`.

### Properties Interpolated

- **Color**: The shader interpolates between the original screen color and a red vignette color based on the `vignetteFactor` and `darken_screen_factor`.
  - The `darken_screen_factor` is what is being increased as the player gets damaged.
