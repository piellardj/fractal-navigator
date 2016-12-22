# fractal-navigator

Program for navigating Mandelbrot and Julia fractals with keyboard and mouse.


# Usage

When launched this program opens two windows. The one on the left is the Mandelbrot fractal, the other one is the Julia fractal.

In each window the user can navigate in the fractal using mouse left button for moving and mouse middle button for zooming in or out.

To choose the seed used in the Julia fractal, use mouse right button in the Mandelbrot window.
Notice how when you choose a seed that is in the Mandelbrot fractal the Julia fractal is connected, and how it's not when you choose an external seed.

The user can choose the precision of the render ("max iteration" variable) by pressing A and E keys.
The deeper you are in the fractal the more precision you need to see things clearly.

The color palette used for rendering is customizable and placed in the resource folder.

Two render modes are implemented for the Mandelbrot fractal:
- one that shows the number of iterations, resulting in visible color steps
- one that is smoothed

To choose a render mode, rename the appropriate shader in the shaders folder.


# Screenshots

![alt text](screenshots/mandelbrot01.png "Raw (top) and smooth (bottom) render")
![alt text](screenshots/julia03.png")
![alt text](screenshots/julia02.png")
![alt text](screenshots/b.png" "Close up view of the south-east area of the Mandelbrot fractal")
