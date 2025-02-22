# team 12

## Smooth Interpolation Implementation
To meet requirement [3] on the milestone (smooth interpolation between at least two values of a property), we implemented a vignette effect in the fragment shader. The implementation uses linear interpolation (`mix` function in GLSL) to smoothly transition between the original screen color and a red tint based on the `vignetteFactor`, which is the distance from the center of the screen, and a uniform `darken_screen_factor`. The point of this interpolation is to show how hurt the player is with the vignette getting more and more red the more damage the player takes.

### Code Location
The logic for this is located in the fragment shader `vignette.fs.glsl`.

### Properties Interpolated
- **Color**: The shader interpolates between the original screen color and a red vignette color based on the `vignetteFactor` and `darken_screen_factor`.
    - The `darken_screen_factor` is what is being increased as the player gets damaged.
