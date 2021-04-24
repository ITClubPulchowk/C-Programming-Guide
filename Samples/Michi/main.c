#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "glfw/include/GLFW/glfw3.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#pragma comment(lib, "Opengl32.lib")

#define ARRAY_COUNT(A) (sizeof(A)/sizeof((A)[0]))

typedef struct {
	float x, y;
} V2;
typedef struct {
	float x, y, z, w;
} V4;

float lerp(float a, float b, float t) { return (1.0f - t) * a + t * b; }

V2 v2(float x, float y) { return (V2){ x, y }; }
V2 v2add(V2 a, V2 b) { return (V2) { a.x + b.x, a.y + b.y }; }
V2 v2sub(V2 a, V2 b) { return (V2) { a.x - b.x, a.y - b.y }; }
V2 v2mul(V2 a, float b) { return (V2) { a.x * b, a.y * b }; }
float v2dot(V2 a, V2 b) { return a.x * b.x + a.y * b.y; }

V4 v4(float x, float y, float z, float w) { return (V4){ x, y, z, w }; }
V4 v4add(V4 a, V4 b) { return (V4) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
V4 v4sub(V4 a, V4 b) { return (V4) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
V4 v4mul(V4 a, float b) { return (V4) { a.x * b, a.y * b, a.z * b, a.w * b }; }
float v4dot(V4 a, V4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
V4 v4lerp(V4 a, V4 b, float t) { return v4add(v4mul(a, 1.0f - t), v4mul(b, t)); }

bool point_inside_rect(V2 p, V2 ra, V2 rb) {
	return (p.x > ra.x && p.x < rb.x) && (p.y > ra.y && p.y < rb.y);
}

typedef struct {
	GLuint texture;
	int width, height;
	float size;
	stbtt_packedchar cdata[96];  // ASCII 32..126 is 95 glyphs
} Font;

char *read_entire_file(const char *file) {
	FILE *f = fopen(file, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *string = malloc(fsize + 1);
	fread(string, 1, fsize, f);
	fclose(f);

	string[fsize] = 0;
	return string;
}

bool font_load(const char *file, float font_size, Font *font) {
	char *data = read_entire_file(file);
	if (!data) return false;

	stbtt_fontinfo info;

	int offset = stbtt_GetFontOffsetForIndex(data, 0);
	if (!stbtt_InitFont(&info, data, offset)) {
		free(data);
		return false;
	}

	int width = 512;
	int height = 512;
	unsigned char *pixels = malloc(width * height);

	stbtt_pack_context context;

	stbtt_PackBegin(&context, pixels, width, height, 0, 1, NULL);
	stbtt_PackSetOversampling(&context, 1, 1);
	stbtt_PackFontRange(&context, data, 0, font_size, 32, ARRAY_COUNT(font->cdata), font->cdata);
	stbtt_PackEnd(&context);

	font->width = width;
	font->height = height;
	font->size = font_size;

	glGenTextures(1, &font->texture);
	glBindTexture(GL_TEXTURE_2D, font->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(pixels);
	free(data);
	return true;
}

STBTT_DEF void ex_stbtt_GetPackedQuad(const stbtt_packedchar *chardata, int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q, int align_to_integer) {
	float ipw = 1.0f / pw, iph = 1.0f / ph;
	const stbtt_packedchar *b = chardata + char_index;

	if (align_to_integer) {
		float x = (float)STBTT_ifloor((*xpos + b->xoff) + 0.5f);
		float y = (float)STBTT_ifloor((*ypos - b->yoff2) + 0.5f);
		q->x0 = x;
		q->y0 = y;
		q->x1 = x + b->xoff2 - b->xoff;
		q->y1 = y + b->yoff2 - b->yoff;
	} else {
		q->x0 = *xpos + b->xoff;
		q->y0 = *ypos - b->yoff2;
		q->x1 = *xpos + b->xoff2;
		q->y1 = *ypos - b->yoff;
	}

	q->s0 = b->x0 * ipw;
	q->t0 = b->y0 * iph;
	q->s1 = b->x1 * ipw;
	q->t1 = b->y1 * iph;

	*xpos += b->xadvance;
}

void render_rect(V2 pos, V2 dim, V4 color) {
	glColor4f(color.x, color.y, color.z, color.w);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.x, pos.y + dim.y);
	glVertex2f(pos.x + dim.x, pos.y + dim.y);
	glVertex2f(pos.x + dim.x, pos.y);
}

float render_font(Font *font, V2 pos, V4 color, const char *text) {	
	glColor4f(color.x, color.y, color.z, color.w);
	while (*text) {
		if (*text >= 32 && *text < 126) {
			stbtt_aligned_quad q;
			ex_stbtt_GetPackedQuad(font->cdata, font->width, font->height, *text - 32, &pos.x, &pos.y, &q, 1);
			glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y0);
			glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y0);
			glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y1);
			glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y1);
		}
		++text;
	}
	return pos.x;
}

void glfw_error(int error, const char *description) {
	fprintf(stderr, "Error(%d): %s\n", error, description);
}

typedef struct {
	V2 p;
	V2 d;
	V4 color[2];
} Panel_Background;

typedef struct {
	float offset;
	Font font;
	char text[255];
	int cursor;
	bool hovered;

	float cursor_blink_t;
	V4 cursor_colors[2];
} Panel_Text_Input;

typedef enum {
	PANEL_STATE_IDEL,
	PANEL_STATE_TYPING,
} Panel_State;

typedef struct {
	Panel_State state;
	Panel_Text_Input input;
	Panel_Background background;
} Panel;

typedef enum {
	CURSOR_KIND_ARROW,
	CURSOR_KIND_IBEAM,

	_CURSOR_KIND_COUNT
} Cursor_Kind;

typedef struct {
	int framebuffer_w, framebuffer_h;
	int window_w, window_h;
	GLFWwindow *window;
	GLFWcursor *cursors[_CURSOR_KIND_COUNT];
} Context;

static Context context;

bool panel_create(Panel *panel, const char *font, float font_size, const float offset) {
	if (!font_load(font, font_size, &panel->input.font)) {
		fprintf(stderr, "Failed to load font: %s\n", font);
		return false;
	}

	panel->input.offset = offset;

	panel->background.p = v2(0, 0);
	panel->background.d = v2(0, font_size + 20);
	panel->background.color[0] = v4(0.04f, 0.04f, 0.04f, 1.0f);
	panel->background.color[1] = v4(0.2f, 0.6f, 0.6f, 1.0f);

	panel->state = PANEL_STATE_IDEL;

	memset(panel->input.text, 0, sizeof(panel->input.text));
	panel->input.cursor = 0;
	panel->input.hovered = false;

	panel->input.cursor_colors[0] = v4(.2f, .8f, .2f, 1.f);
	panel->input.cursor_colors[1] = v4(1.f, 1.f, 1.f, 1.f);
	panel->input.cursor_blink_t = 0;

	return true;
}

void panel_start_typing(Panel *panel) {
	panel->state = PANEL_STATE_TYPING;
	panel->input.cursor_blink_t = 0;
}

void panel_stop_typing(Panel *panel) {
	panel->state = PANEL_STATE_IDEL;
}

void panel_on_cursor_pos_changed(GLFWwindow *window, double x, double y) {
	Panel *panel = glfwGetWindowUserPointer(window);

	y = (double)context.window_h - y;
	V2 cursor = v2((float)x, (float)y);

	V2 rect_min = panel->background.p;
	rect_min.x += panel->input.offset;
	V2 rect_max = v2add(rect_min, v2((float)context.framebuffer_w, panel->background.d.y));

	bool previous_hovered = panel->input.hovered;
	panel->input.hovered = point_inside_rect(cursor, rect_min, rect_max);

	if (panel->input.hovered) {
		if (!previous_hovered) {
			glfwSetCursor(context.window, context.cursors[CURSOR_KIND_IBEAM]);
		}
	} else {
		glfwSetCursor(context.window, context.cursors[CURSOR_KIND_ARROW]);
	}
}

void panel_on_mouse_input(GLFWwindow *window, int button, int action, int mods) {
	Panel *panel = glfwGetWindowUserPointer(window);

	switch (panel->state) {
		case PANEL_STATE_IDEL: {
			if (panel->input.hovered && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				panel_start_typing(panel);
			}
		} break;

		case PANEL_STATE_TYPING: {
			if (!panel->input.hovered && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				panel_stop_typing(panel);
			}
		} break;
	}
}

void panel_on_key_input(GLFWwindow *window, int key, int scancode, int action, int mods) {
	Panel *panel = glfwGetWindowUserPointer(window);

	switch (panel->state) {
		case PANEL_STATE_TYPING: {
			if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
				int len = (int)strlen(panel->input.text);
				if (len > 0) {
					panel->input.text[len - 1] = 0;
					panel->input.cursor -= 1;
					panel->input.cursor_blink_t = 0;
				}
			} else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				panel_stop_typing(panel);
			}
		} break;

		case PANEL_STATE_IDEL: {
			if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
				panel_start_typing(panel);
			}
		} break;
	}
}

void panel_on_text_input(GLFWwindow *window, unsigned int codepoint) {
	Panel *panel = glfwGetWindowUserPointer(window);

	switch (panel->state) {
		case PANEL_STATE_TYPING: {
			int len = (int)strlen(panel->input.text);
			if (len != sizeof(panel->input.text)) {
				if (codepoint >= 32 && codepoint < 126) {
					panel->input.text[len] = (char)codepoint;
					panel->input.cursor += 1;
					panel->input.cursor_blink_t = 0;
				}
			}
		} break;
	}
}

void panel_update(Panel *panel, float dt) {
	panel->background.d.x = (float)context.framebuffer_w;

	panel->input.cursor_blink_t += dt * 1.5f;
	if (panel->input.cursor_blink_t > 1.5f) panel->input.cursor_blink_t = 0;
}

void panel_render(Panel *panel) {
	float mid_y = (panel->background.d.y - panel->input.font.size) * 0.5f + panel->background.p.y;
	float off_x = panel->input.offset - 1;

	glBegin(GL_QUADS);
	render_rect(panel->background.p, panel->background.d, panel->background.color[0]);
	if (off_x > 0) {
		render_rect(panel->background.p, v2(off_x - 1, panel->background.d.y), panel->background.color[1]);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, panel->input.font.texture);
	glBegin(GL_QUADS);

	const char *text = "Enter Command...";
	if (panel->state == PANEL_STATE_TYPING) {
		text = panel->input.text;
	} else if (panel->input.text[0] != 0) {
		text = panel->input.text;
	}

	V4 color = v4(.5f, .5f, .5f, 1.f);
	float x = render_font(&panel->input.font, v2(panel->input.offset, mid_y), color, text);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	if (panel->state == PANEL_STATE_TYPING) {
		float t = panel->input.cursor_blink_t;
		if (t > 1) t = 1;
		V4 cursor_color = v4lerp(panel->input.cursor_colors[0], panel->input.cursor_colors[1], t);

		float cursor_height = panel->background.d.y * 0.8f;
		float cursor_width = 0.6f * panel->input.font.size;
		mid_y = (panel->background.d.y - cursor_height) * 0.5f + panel->background.p.y;

		glBegin(GL_QUADS);
		render_rect(v2(x, mid_y), v2(cursor_width, cursor_height), cursor_color);
		glEnd();
	}
}

bool context_create() {
	if (!glfwInit()) {
		fprintf(stderr, "glfwInit() failed!\n");
		return false;
	}

	glfwSetErrorCallback(glfw_error);

	context.window = glfwCreateWindow(800, 600, "Michi", NULL, NULL);
	if (!context.window) {
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(context.window);

	context.cursors[CURSOR_KIND_ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	context.cursors[CURSOR_KIND_IBEAM] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);

	return true;
}

void context_destory() {
	for (int i = 0; i < _CURSOR_KIND_COUNT; ++i)
		glfwDestroyCursor(context.cursors[i]);
	glfwDestroyWindow(context.window);
	glfwTerminate();
}

int main(int argc, char *argv[]) {
	if (!context_create()) {
		return -1;
	}

	Panel panel;
	if (!panel_create(&panel, "Stanberry.ttf", 32, 20)) {
		fprintf(stderr, "Panel failed to create!\n");
		return -1;
	}

	glfwSetWindowUserPointer(context.window, &panel);
	glfwSetCursorPosCallback(context.window, panel_on_cursor_pos_changed);
	glfwSetCharCallback(context.window, panel_on_text_input);
	glfwSetKeyCallback(context.window, panel_on_key_input);
	glfwSetMouseButtonCallback(context.window, panel_on_mouse_input);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSwapInterval(1);

	uint64_t frequency = glfwGetTimerFrequency();
	uint64_t counter = glfwGetTimerValue();
	float dt = 1.0f / 60.0f;

	while (!glfwWindowShouldClose(context.window)) {
		glfwPollEvents();

		glfwGetFramebufferSize(context.window, &context.framebuffer_w, &context.framebuffer_h);
		glfwGetWindowSize(context.window, &context.window_w, &context.window_h);

		panel_update(&panel, dt);

		glViewport(0, 0, context.framebuffer_w, context.framebuffer_h);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		glOrtho(0, context.framebuffer_w, 0, context.framebuffer_h, -1, 1);

		panel_render(&panel);

		glPopMatrix();

		glfwSwapBuffers(context.window);

		uint64_t new_counter = glfwGetTimerValue();
		uint64_t counts = new_counter - counter;
		counter = new_counter;
		dt = ((1000000.0f * (float)counts) / (float)frequency) / 1000000.0f;
	}

	context_destory();

	return 0;
}
