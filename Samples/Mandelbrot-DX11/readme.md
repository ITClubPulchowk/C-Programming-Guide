# [Mandelbrot Set](https://en.wikipedia.org/wiki/Mandelbrot_set)

## Usage
* This program only runs on Windows 10
* Build by running the `build.bat` (requires visual studio or msvc compiler)
* The executables are generated in `bin/<compiler>` directory
* In the same directroy as the executables there exits `mandelbrot.hlsl` file
* You can edit and save the `mandelbrot.hlsl` file and it will automatically be reloaded by the program
* You can use the mouse wheel to zoom in and out, and drag the screen for panning

## Variables
*Note: The following variables are present in mandelbrot.frag file*
- `Radius` : Change the value and observe the result
- `ColorWeight` : Using this variable, control the output color of the Mandelbrot
- `MaxIterations` : Change the number of iterations made in the calculation and observe the result

## Screeenshot

![Mandelbrot Diagram](screenshot.png)
