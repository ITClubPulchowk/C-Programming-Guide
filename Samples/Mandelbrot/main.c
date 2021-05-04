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
#include <stdlib.h>


//change the maximum amount of iteration here, more the iteration higher the quality but slower
#define maxIter 100

#define minimum(a, b)			(((a) < (b)) ? (a) : (b))

/*
	To change the colors, change the values here
*/
float redWeight = 5.0f;
float greenWeight = 2.0f;
float blueWeight = 3.0f;



typedef enum bool{
	false,true
}bool;


//complex number
typedef struct Complex {
	float real;
	float imag;
}Complex;

Complex start, end;


//slider for color weights
typedef enum Color {
	red = 0, green = 1, blue = 2
}Color;

typedef struct Slider {
	float x, y, w, h;
	float maxValue, value, minValue;
	float pixel[3];
	Color color;
	bool isClicked;
}Slider;


//screen parameters
typedef struct Screen {
	float* colors;
	int width, height;
}Screen;


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
	result.imag = c1.real * c2.imag + c1.imag * c2.real;

	return result;
}

float absolute(Complex c) {
	double real = (double)c.real;
	double imag = (double)c.imag;
	float magnitude = sqrt(real * real + imag * imag);
	return magnitude;
}


int doesDiverge(Complex* c, float radius) {
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


void MandelbrotSet(int width, int height, float* colors) {
	const float radius = 4.0f;

	for (int y = 0; y < height; y++) {
		float imag = start.imag + ((float)y / height) * (end.imag - start.imag);
		for (int x = 0; x < width; x++) {
			float real = start.real + ((float)x / width) * (end.real - start.real);

			Complex z = initComplex(real, imag);

			int nIter = doesDiverge(&z, radius);

			colors[x + y * width] = (float)((nIter - log2(absolute(z) / radius)) / maxIter) * 255;
		}
	}
}


void rendermandelbrot(float* colors, int width, int height) {
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);
	glBegin(GL_POINTS);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			const float color = colors[x + y * width];

			const float r = minimum(255.0f, redWeight * color) / 255;
			const float g = minimum(255.0f, greenWeight * color) / 255;
			const float b = minimum(255.0f, blueWeight * color) / 255;

			glColor3f(r, g, b);
			glVertex2f((GLfloat)x, (GLfloat)y);
		}
	}
	glEnd();
}


void renderSlider(Slider *slider) {
	glLoadIdentity();
	int frameWidth, frameHeight;
	glBegin(GL_QUADS);
	glColor3f(slider->pixel[0], slider->pixel[1], slider->pixel[2]);
	glVertex2f(slider->x, slider->y);
	glVertex2f(slider->x + slider->value / slider->maxValue * slider->w, slider->y);
	glVertex2f(slider->x + slider->value / slider->maxValue * slider->w, slider->y + slider->h);
	glVertex2f(slider->x, slider->y + slider->h);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(slider->pixel[0] / 2, slider->pixel[1] / 2, slider->pixel[2] / 2);
	glVertex2f(slider->x + slider->value / slider->maxValue * slider->w, slider->y);
	glVertex2f(slider->x + slider->w, slider->y);
	glVertex2f(slider->x + slider->w, slider->y + slider->h);
	glVertex2f(slider->x + slider->value / slider->maxValue * slider->w, slider->y + slider->h);
	glEnd();
}

void handleSliderEvent(GLFWwindow* window, Slider* slider) {
	int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (mouseState == GLFW_PRESS) {
		double xCoord, yCoord;
		glfwGetCursorPos(window, &xCoord, &yCoord);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		float xPos, yPos;
		xPos = (xCoord - (width / 2)) / (float)(width / 2);
		yPos = ((height / 2) - yCoord) / ((float)height / 2);
		if (xPos > slider->x && xPos < slider->x + slider->w && yPos > slider->y && yPos < slider->y + slider->h) {
			slider->isClicked = true;
		}
	}

	if (slider->isClicked) {
		slider->isClicked = false;
		double xCoord, yCoord;
		glfwGetCursorPos(window, &xCoord, &yCoord); int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		float xPos, yPos;
		xPos = (xCoord - (width / 2)) / (float)(width / 2);
		yPos = ((height / 2) - yCoord) / ((float)height / 2);
		if (slider->color == red) {
			redWeight = minimum(slider->minValue + ((xPos - slider->x) / slider->w) * slider->maxValue, slider->maxValue);
			slider->value = redWeight;
		}
		else if (slider->color == green) {
			greenWeight = minimum(slider->minValue + ((xPos - slider->x) / slider->w) * slider->maxValue, slider->maxValue);
			slider->value = greenWeight;
		}
		else if (slider->color == blue) {
			blueWeight = minimum(slider->minValue + ((xPos - slider->x) / slider->w) * slider->maxValue, slider->maxValue);
			slider->value = blueWeight;
		}
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
	Screen* screen = (Screen*)glfwGetWindowUserPointer(window);
	glfwGetFramebufferSize(window, &screen->width, &screen->height);
	screen->colors = (float *)realloc(screen->colors, screen->width * screen->height * sizeof(float));
	MandelbrotSet(width, height, screen->colors);
	glfwSetWindowUserPointer(window, screen);
}


float MapRange(float from_x1, float from_x2, float to_x1, float to_x2, float x) {
	return (to_x2 - to_x1) / (from_x2 - from_x1) * (x - from_x1) + to_x1;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	double xCoord, yCoord;
	glfwGetCursorPos(window, &xCoord, &yCoord);
	float zoomSize = 2.0f;
	Screen* screen = (Screen*)glfwGetWindowUserPointer(window);

	float cx = MapRange(0, (float)screen->width, start.real, end.real, (float)xCoord);
	float cy = MapRange(0, (float)screen->height, start.imag, end.imag, screen->width - (float)yCoord);

	start.real -= cx;
	end.real -= cx;
	start.imag -= cy;
	end.imag -= cy;

	float factor = yoffset > 0 ? 0.9f : 1.1f;

	start.real *= factor;
	end.real *= factor;
	start.imag *= factor;
	end.imag *= factor;

	start.real += cx;
	end.real += cx;
	start.imag += cy;
	end.imag += cy;

	MandelbrotSet(screen->width, screen->height, screen->colors);
}



int main(int argc, char* argv[]) {
	Screen screen;

	//width and height
	int width = 800;
	int height = 800;

	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(width, height, "Mandelbrot Set", NULL, NULL);

	screen.colors = (float*)malloc(width * height * sizeof(float));

	if (!window) {
		glfwTerminate();
		return -1;
	}
	screen.width = width;
	screen.height = height;

	glfwSetWindowUserPointer(window, &screen);
	glfwSetWindowSizeCallback(window, framebuffer_size_callback);

	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);

	start = initComplex(-2.5f, -2);
	end = initComplex(1.0f, 2.0f);

	MandelbrotSet(screen.width, screen.height, screen.colors);

	Slider sliders[3];
	for (int i = 0; i < 3; i++) {
		sliders[i].w = 0.2f;
		sliders[i].h = 0.05f;
		sliders[i].x = -1.0f;
		sliders[i].y = 0.95 - sliders[i].h * i - 0.02 * i;
		sliders[i].pixel[i] = 1.0f;
		sliders[i].maxValue = 8.0f;
		sliders[i].minValue = 1.0f;
		sliders[i].color = i;
		sliders[i].isClicked = false;
		if ((sliders + i)->color == red) {
			(sliders + i)->value = redWeight;
		}
		else if ((sliders + i)->color == green) {
			(sliders + i)->value = greenWeight;
		}
		else if ((sliders + i)->color == blue) {
			(sliders + i)->value = blueWeight;
		}
	}

	glfwSetScrollCallback(window, scroll_callback);


	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		rendermandelbrot(screen.colors, screen.width, screen.height);

		for (int i = 0; i < 3; i++) {
			renderSlider(&sliders[i]);
		}

		glfwSwapBuffers(window);

		glfwPollEvents();

		for (int i = 0; i < 3; i++) {
			handleSliderEvent(window, &sliders[i]);
		}
	}

	glfwTerminate();
	return 0;
}
