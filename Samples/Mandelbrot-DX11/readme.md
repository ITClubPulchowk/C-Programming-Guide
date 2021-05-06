# [Mandelbrot Set](https://en.wikipedia.org/wiki/Mandelbrot_set)

## Usage
* This program only runs on Windows 10
* Build by running the `build.bat` (requires visual studio or msvc compiler)
* The executables are generated in `bin/<compiler>` directory
* In the same directroy as the executables there exits `mandelbrot.hlsl` file
* You can edit and save the `mandelbrot.hlsl` file and it will automatically be reloaded by the program
* You can use the mouse wheel to zoom in and out, and drag the screen for panning
* Press [F5] to capture the screen, the images will be saved in `Captures` directory

## Variables
*Note: The following variables are present in mandelbrot.frag file*
- The `ps_main` function can be modified to call different coloring methods
- The available methods are: `Mandelbrot_SimpleColoring`, `Mandelbrot_SmoothColoring`, `Mandelbrot_WaveColoring` and `Mandelbrot_WaveColoringAnimated`
- At the top of each of the coloring functions, constansts are defined which can be tweaked as required

## Screeenshot

![Mandelbrot Diagram](screenshot.png)
