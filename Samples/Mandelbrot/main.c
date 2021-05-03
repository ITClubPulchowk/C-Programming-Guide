/*
  Libraries:
	glfw(https://www.glfw.org/documentation.html)
*/

//Author: Sanskar Amgain(@Imsanskar)

/*
The Mandelbrot set is the set of complex numbers c for which the function f(z) = z^2 + c does not diverge when iterated from z = 0 ,remains bounded in absolute value.

For further information follow this link:
https://en.wikipedia.org/wiki/Mandelbrot_set

Or you can watch this numberphile video
https://youtu.be/FFftmWSzgmk
*/

//TODO: build for linux
//TODO: Fix gcc build

#include <stdio.h>
#include <math.h>
#include "glfw/include/GLFW/glfw3.h"
#include <gl/GL.h>

#define maxIter 200

/*
	To change the colors, change the values here
*/
const float redWeight = 2.0f;
const float greenWeight = 3.0f;
const float blueWeight = 5.0f;


//width and height
const int width = 1300;
const int height = 1000;


typedef enum bool{
	false,true
}bool;


//complex number
typedef struct Complex {
	float real;
	float imag;
}Complex;

Complex initComplex(float real, float imag) {
	Complex c;
	c.real = real;
	c.imag = imag;

	return c;
}


Complex add(Complex c1, Complex c2) {
	Complex result;
	result.real = c1.real + c2.real;
	result.imag = c1.imag + c2.imag;

	return result;
}

Complex multiply(Complex c1, Complex c2) {
	Complex result;
	result.real = c1.real * c2.real - c1.imag * c2.imag;
	result.imag = c1.real * c2.imag + c1.imag * c1.real;

	return result;
}

float absolute(Complex c) {
	double real = (double)c.real;
	double imag = (double)c.imag;
	float magnitude = sqrt(real * real + imag * imag);
	return magnitude;
}


int doesDiverge(Complex *c, float radius) {
	Complex z = initComplex(0.0f, 0.0f);
	int iter = 0;

	while (absolute(z) <= radius && iter < maxIter) {
		z = add(multiply(z, z), *c);
		iter += 1;
	}

	//for returning complex number
	*c = z;

	return iter;
}

float min(float a, float b) {
	if (a < b)
		return a;
	return b;
}


void drawMandelbrotSet(Complex start, Complex end, GLFWwindow *window) {
	const int maxIteration = 100;
	const float radius = 4.0f;

	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);
	glBegin(GL_POINTS);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			float real = start.real + ((float)x / width) * (end.real - start.real);
			float imag = start.imag + ((float)y / height) * (end.imag - start.imag);

			Complex z = initComplex(real, imag);

			int nIter = doesDiverge(&z, radius);

			const float color = ((nIter - log2(absolute(z) / radius)) / maxIter) * 255;

			const float r = min(255.0f, redWeight * color) / 255;
			const float g = min(255.0f, greenWeight * color) / 255;
			const float b = min(255.0f, blueWeight * color) / 255;

			float xCoord = ((float)(x - (width / 2))) / (width / 2);/*
			float yCoord = ((float)((height / 2) - y)) / (height / 2)*/;

			glColor3f(r, g, b);
			glVertex2f(x, y);
		}
	}
	glEnd();
	glPopMatrix();
}


int main(int argc, char *argv[]) {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	if (!window){
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	int count = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		Complex start = initComplex(-2.5f, -2);
		Complex end = initComplex(1.0f, 2.0f);

		if (count == 0) {
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			drawMandelbrotSet(start, end, window);
			printf("Completed\n");
			/* Swap front and back buffers */
			glfwSwapBuffers(window);
		}

		count++;


		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}