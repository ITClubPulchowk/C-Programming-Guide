/*
* File Navigation
* 
* [Helper Macros]
* [Utility Structs & Functions]
* [Context]
* [Font]
* [Mesh]
* [Rendering]
* [Code]
* [Panel]
* [Actor]
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "glfw/include/GLFW/glfw3.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#if defined(_MSC_VER)
#pragma comment(lib, "Opengl32.lib")
#endif

//
// Helper Macros
//

#define ARRAY_COUNT(A) (sizeof(A)/sizeof((A)[0]))

#define MINIMUM(A, B)			(((A) < (B)) ? (A) : (B))
#define MAXIMUM(A, B)			(((A) > (B)) ? (A) : (B))
#define CLAMP(A, B, V)      MINIMUM(B, MAXIMUM(A, V))
#define CLAMP01(V)				CLAMP(0.0F, 1.0F, V)

//
// Utility Structs & Functions
//

float lerp(float a, float b, float t) { return (1.0f - t) * a + t * b; }

typedef struct { float x, y; } V2;
typedef struct { float x, y, z, w; } V4;

V2 v2(float x, float y) { return (V2) { x, y }; }
V2 v2add(V2 a, V2 b) { return (V2) { a.x + b.x, a.y + b.y }; }
V2 v2sub(V2 a, V2 b) { return (V2) { a.x - b.x, a.y - b.y }; }
V2 v2mul(V2 a, float b) { return (V2) { a.x *b, a.y *b }; }
float v2dot(V2 a, V2 b) { return a.x * b.x + a.y * b.y; }
V2 v2lerp(V2 a, V2 b, float t) { return v2add(v2mul(a, 1.0f - t), v2mul(b, t)); }

V4 v4(float x, float y, float z, float w) { return (V4) { x, y, z, w }; }
V4 v4add(V4 a, V4 b) { return (V4) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
V4 v4sub(V4 a, V4 b) { return (V4) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
V4 v4mul(V4 a, float b) { return (V4) { a.x *b, a.y *b, a.z *b, a.w *b }; }
float v4dot(V4 a, V4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
V4 v4lerp(V4 a, V4 b, float t) { return v4add(v4mul(a, 1.0f - t), v4mul(b, t)); }

bool point_inside_rect(V2 p, V2 ra, V2 rb) { return (p.x > ra.x && p.x < rb.x) && (p.y > ra.y && p.y < rb.y); }

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

typedef struct {
	size_t length;
	char *data;
} String;

//
// Context
//

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

void glfw_error(int error, const char *description) {
	fprintf(stderr, "Error(%d): %s\n", error, description);
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

//
// Font
//

typedef struct {
	GLuint id;
	int width, height;
} Texture;

// ASCII 32..126 is 95 glyphs
#define FONT_PACKED_MIN_CODEPOINT 32
#define FONT_PACKED_MAX_CODEPOINT 126
#define FONT_PACKED_CODEPOINT_COUNT (FONT_PACKED_MAX_CODEPOINT - FONT_PACKED_MIN_CODEPOINT + 1)

typedef struct {
	Texture texture;
	float size;
	stbtt_packedchar cdata[FONT_PACKED_CODEPOINT_COUNT];
} Font;

void stbttEx_GetPackedQuad(const stbtt_packedchar *chardata, int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q) {
	float ipw = 1.0f / pw, iph = 1.0f / ph;
	const stbtt_packedchar *b = chardata + char_index;

	float x = (float)STBTT_ifloor((*xpos + b->xoff) + 0.5f);
	float y = (float)STBTT_ifloor((*ypos - b->yoff2) + 0.5f);
	q->x0 = x;
	q->y0 = y;
	q->x1 = x + b->xoff2 - b->xoff;
	q->y1 = y + b->yoff2 - b->yoff;

	q->s0 = b->x0 * ipw;
	q->t0 = b->y0 * iph;
	q->s1 = b->x1 * ipw;
	q->t1 = b->y1 * iph;

	*xpos += b->xadvance;
}

size_t stbttEx_FindCursorOffset(Font *font, V2 pos, float c, const char *text, size_t len) {
	stbtt_aligned_quad q;
	const char *first = text;
	const char *last = text + len;

	while (text != last) {
		if (*text >= 32 && *text < 126) {
			float prev_x = pos.x;
			stbttEx_GetPackedQuad(font->cdata, font->texture.width, font->texture.height, *text - 32, &pos.x, &pos.y, &q);
			if (c >= prev_x && c <= pos.x) {
				if (c - prev_x < pos.x - c) {
					return text - first;
				} else {
					return text - first + 1;
				}
			}
		}
		++text;
	}

	return len;
}

bool font_load(const char *file, float font_size, int bitmap_w, int bitmap_h, Font *font) {
	char *data = read_entire_file(file);
	if (!data) return false;

	stbtt_fontinfo info;

	int offset = stbtt_GetFontOffsetForIndex(data, 0);
	if (!stbtt_InitFont(&info, data, offset)) {
		free(data);
		return false;
	}

	unsigned char *pixels = malloc(bitmap_w * bitmap_h);

	stbtt_pack_context context;

	stbtt_PackBegin(&context, pixels, bitmap_w, bitmap_h, 0, 1, NULL);
	stbtt_PackSetOversampling(&context, 1, 1);
	stbtt_PackFontRange(&context, data, 0, font_size, FONT_PACKED_MIN_CODEPOINT, FONT_PACKED_CODEPOINT_COUNT, font->cdata);
	stbtt_PackEnd(&context);

	font->texture.width = bitmap_w;
	font->texture.height = bitmap_h;
	font->size = font_size;

	glGenTextures(1, &font->texture.id);
	glBindTexture(GL_TEXTURE_2D, font->texture.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, bitmap_w, bitmap_h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(pixels);
	free(data);
	return true;
}

//
// Rendering
//

void render_rect(V2 pos, V2 dim, V4 color) {
	glColor4f(color.x, color.y, color.z, color.w);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.x, pos.y + dim.y);
	glVertex2f(pos.x + dim.x, pos.y + dim.y);
	glVertex2f(pos.x + dim.x, pos.y);
}

float render_font(Font *font, V2 pos, V4 color, const char *text, size_t len) {
	stbtt_aligned_quad q;
	glColor4f(color.x, color.y, color.z, color.w);
	const char *last = text + len;
	while (text != last) {
		if (*text >= 32 && *text < 126) {
			stbttEx_GetPackedQuad(font->cdata, font->texture.width, font->texture.height, *text - 32, &pos.x, &pos.y, &q);
			glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y0);
			glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y0);
			glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y1);
			glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y1);
		}
		++text;
	}
	return pos.x;
}

float render_font_stub(Font *font, V2 pos, const char *text, size_t len) {
	stbtt_aligned_quad q;
	const char *last = text + len;
	while (text != last) {
		if (*text >= 32 && *text < 126) {
			stbttEx_GetPackedQuad(font->cdata, font->texture.width, font->texture.height, *text - 32, &pos.x, &pos.y, &q);
		}
		++text;
	}
	return pos.x;
}

//
// Code
//

typedef enum {
	TOKEN_KIND_ERROR,
	TOKEN_KIND_EOF,

	TOKEN_KIND_NUMBER_LITERAL,

	TOKEN_KIND_PLUS,
	TOKEN_KIND_MINUS,
	TOKEN_KIND_EQUALS,
	TOKEN_KIND_BRACKET_OPEN,
	TOKEN_KIND_BRACKET_CLOSE,
	TOKEN_KIND_MUL,
	TOKEN_KIND_DIV,
	TOKEN_KIND_PERIOD,

	TOKEN_KIND_IDENTIFIER,

	_TOKEN_KIND_COUNT
} Token_Kind;

typedef struct {
	Token_Kind	kind;
	String		string;
} Token;

typedef struct {
	char *first;
	char *token_start;
	char *current;

	double number;

	char *error;

	Token token;

	char scratch[512];

	bool tokenizing;
} Lexer;

void lexer_init(Lexer *l, char *first) {
	l->first = first;
	l->token_start = first;
	l->current = first;

	l->number = 0;

	l->tokenizing = (*first != 0);
}

void _lexer_consume_character(Lexer *l) {
	if (l->tokenizing) {
		l->current += 1;
		if (*l->current == 0)
			l->tokenizing = false;
	}
}

void _lexer_consume_characters(Lexer *l, size_t count) {
	for (size_t i = 0; i < count; ++i)
		_lexer_consume_character(l);
}

void _lexer_set_token_start(Lexer *l) {
	l->token_start = l->current;
}

void _lexer_make_token(Lexer *l, Token_Kind kind) {
	l->token.kind = kind;
	l->token.string.data = l->token_start;
	l->token.string.length = l->current - l->token_start;
	l->token_start = l->current;
}

void _lexer_make_error_token(Lexer *l, char *error) {
	_lexer_make_token(l, TOKEN_KIND_ERROR);
	l->error = error;
}

void lexer_advance_token(Lexer *l) {
	while (l->tokenizing) {
		char a = *l->current;
		char b = *(l->current + 1);

		if (isspace(a)) {
			_lexer_consume_character(l);

			while (l->tokenizing && isspace(*l->current)) {
				_lexer_consume_character(l);
			}

			_lexer_set_token_start(l);
			continue;
		}

		switch (a) {
			case '+': _lexer_consume_character(l); _lexer_make_token(l, TOKEN_KIND_PLUS); return;
			case '-': _lexer_consume_character(l); _lexer_make_token(l, TOKEN_KIND_MINUS); return;
			case '*': _lexer_consume_character(l); _lexer_make_token(l, TOKEN_KIND_MUL); return;
			case '/': _lexer_consume_character(l); _lexer_make_token(l, TOKEN_KIND_DIV); return;
			case '(': _lexer_consume_character(l); _lexer_make_token(l, TOKEN_KIND_BRACKET_OPEN); return;
			case ')': _lexer_consume_character(l); _lexer_make_token(l, TOKEN_KIND_BRACKET_CLOSE); return;

			case '.': {
				if (isdigit(b)) {
					char *endptr = NULL;
					double value = strtod(l->current, &endptr);
					if (l->current == endptr) {
						_lexer_consume_character(l);
						_lexer_make_token(l, TOKEN_KIND_PERIOD);
					} else {
						_lexer_consume_characters(l, endptr - l->current);
						if (errno == ERANGE) {
							_lexer_make_error_token(l, "Number literal out of range");
						} else {
							l->number = value;
							_lexer_make_token(l, TOKEN_KIND_NUMBER_LITERAL);
						}
					}
				} else {
					_lexer_consume_character(l);
					_lexer_make_token(l, TOKEN_KIND_PERIOD);
				}
				return;
			} break;

			default: {
				if (isdigit(a)) {
					char *endptr = NULL;
					double value = strtod(l->current, &endptr);
					if (l->current != endptr) {
						_lexer_consume_characters(l, endptr - l->current);
						if (errno == ERANGE) {
							_lexer_make_error_token(l, "Number literal out of range");
						} else {
							l->number = value;
							_lexer_make_token(l, TOKEN_KIND_NUMBER_LITERAL);
						}
						return;
					}
				}

				if (!isalnum(a) && a != '_') {
					_lexer_consume_character(l);
					_lexer_make_error_token(l, "Invalid character");
					return;
				}

				while (l->tokenizing) {
					_lexer_consume_character(l);
					if (!isalnum(*l->current) && a != '_') break;
				}

				_lexer_make_token(l, TOKEN_KIND_IDENTIFIER);
				return;
			} break;
		}
	}

	_lexer_make_token(l, TOKEN_KIND_EOF);
}

//
// Panel
//

typedef enum {
	PANEL_COLOR_BACKGROUND,
	PANEL_COLOR_INPUT_INDICATOR,

	PANEL_COLOR_CURSOR0,
	PANEL_COLOR_CURSOR1,
	PANEL_COLOR_CURSOR_NO_TYPE,

	PANEL_COLOR_TEXT_INPUT_PLACEHOLDER,

	PANEL_COLOR_CODE_GENERAL,
	PANEL_COLOR_CODE_ERROR,
	PANEL_COLOR_CODE_NUMBER_LITERAL,
	PANEL_COLOR_CODE_IDENTIFIER,
	PANEL_COLOR_CODE_OPERATORS,
	
	_PANEL_COLOR_COUNT,
} Panel_Color;

typedef struct {
	Font font;
	float height;
	float indicator_size;
	float cursor_blink_time;
	float cursor_blink_rate;
	double cursor_dposition;
	double cursor_dsize;
	V2 cursor_size[2];
	V4 colors[_PANEL_COLOR_COUNT];
} Panel_Style;

#define PANEL_TEXT_INPUT_BUFFER_SIZE 256

typedef struct {
	char buffer[PANEL_TEXT_INPUT_BUFFER_SIZE + 1]; // extra 1 for null-terminator
	size_t count;
	size_t cursor;
} Panel_Text_Input;

typedef enum {
	PANEL_STATE_IDEL,
	PANEL_STATE_TYPING,
} Panel_State;

typedef struct {
	Panel_Style style;
	Panel_Text_Input text_input;
	Lexer lexer;

	Panel_State state;
	float text_position_x_offset;
	float cursor_t;
	float cursor_position;
	float cursor_position_target;
	V2 cursor_size;
	bool hovering;
} Panel;

typedef bool(*Panel_Styler)(Panel_Style *style);

bool panel_default_styler(Panel_Style *style) {
	const char *font_file = "Stanberry.ttf";
	const float font_size = 16;
	const int font_bitmap_w = 512;
	const int font_bitmap_h = 512;

	if (!font_load(font_file, font_size, font_bitmap_w, font_bitmap_h, &style->font)) {
		fprintf(stderr, "Failed to load font: %s\n", font_file);
		return false;
	}

	style->height = font_size + 20;
	style->indicator_size = 20;
	style->cursor_blink_rate = 1.5f;
	style->cursor_blink_time = 1.5f;
	style->cursor_dposition = 0.000000000000000001;
	style->cursor_dsize = 0.00000001;
	style->cursor_size[0] = v2(2.0f, 0.7f * style->height);
	style->cursor_size[1] = v2(0.5f * font_size, 0.7f * style->height);

	style->colors[PANEL_COLOR_BACKGROUND] = v4(0.04f, 0.04f, 0.04f, 1.0f);
	style->colors[PANEL_COLOR_INPUT_INDICATOR] = v4(0.2f, 0.6f, 0.6f, 1.0f);
	style->colors[PANEL_COLOR_CURSOR0] = v4(0.2f, 0.8f, 0.2f, 1.f);
	style->colors[PANEL_COLOR_CURSOR1] = v4(0.2f, 0.6f, 0.6f, 1.0f);
	style->colors[PANEL_COLOR_CURSOR_NO_TYPE] = v4(4.0f, 0.0f, 0.0f, 1.0f);
	style->colors[PANEL_COLOR_TEXT_INPUT_PLACEHOLDER] = v4(0.5, 0.5f, 0.5f, 1.0f);

	style->colors[PANEL_COLOR_CODE_GENERAL] = v4(.8f, .8f, .9f, 1.f);
	style->colors[PANEL_COLOR_CODE_ERROR] = v4(1.f, .3f, .3f, 1.f);
	style->colors[PANEL_COLOR_CODE_NUMBER_LITERAL] = v4(.3f, .3f, .8f, 1.f);
	style->colors[PANEL_COLOR_CODE_IDENTIFIER] = v4(.3f, .9f, .3f, 1.f);
	style->colors[PANEL_COLOR_CODE_OPERATORS] = v4(.8f, .8f, .3f, 1.f);

	return true;
}

String panel_text(Panel *panel) {
	String string;
	string.length = panel->text_input.count;
	string.data = panel->text_input.buffer;
	return string;
}

void panel_input_character(Panel *panel, char c) {
	size_t index = panel->text_input.cursor;
	size_t count = panel->text_input.count;
	if (index <= count && count + 1 <= PANEL_TEXT_INPUT_BUFFER_SIZE) {
		memmove(panel->text_input.buffer + index + 1, panel->text_input.buffer + index, count - index);
		panel->text_input.buffer[index] = c;
		panel->text_input.count += 1;
		panel->text_input.cursor += 1;
	}
	panel->cursor_t = 0;
}

void panel_delete_character(Panel *panel, bool backspace) {
	size_t index = panel->text_input.cursor - (backspace != 0);
	size_t count = panel->text_input.count;
	if (index < count && count) {
		memmove(panel->text_input.buffer + index, panel->text_input.buffer + index + 1, (count - index - 1));
		panel->text_input.count -= 1;
		panel->text_input.cursor -= (backspace != 0);
	}
	panel->cursor_t = 0;
}

void panel_set_cursor(Panel *panel, size_t cursor) {
	if (cursor <= panel->text_input.count) {
		panel->text_input.cursor = cursor;
		panel->cursor_t = 0;
	}
}

V2 panel_text_render_position(Panel *panel) {
	float mid_y = (panel->style.height - panel->style.font.size) * 0.5f;
	return v2(panel->style.indicator_size - panel->text_position_x_offset, mid_y);
}

void panel_set_cursor_at_position(Panel *panel, float xpos) {
	V2 pos = panel_text_render_position(panel);
	String text = panel_text(panel);
	size_t cursor = stbttEx_FindCursorOffset(&panel->style.font, pos, xpos, text.data, text.length);
	panel_set_cursor(panel, cursor);
}

void panel_start_typing(Panel *panel) {
	panel->state = PANEL_STATE_TYPING;
	panel->cursor_t = 0;
}

void panel_stop_typing(Panel *panel) {
	panel->state = PANEL_STATE_IDEL;
}

void panel_on_cursor_pos_changed(GLFWwindow *window, double x, double y) {
	Panel *panel = glfwGetWindowUserPointer(window);

	y = (double)context.window_h - y;
	V2 cursor = v2((float)x, (float)y);

	V2 panel_rect_min = v2(panel->style.indicator_size, 0);
	V2 panel_rect_max = v2((float)context.framebuffer_w, panel->style.height);

	bool hovered = panel->hovering;
	panel->hovering = point_inside_rect(cursor, panel_rect_min, panel_rect_max);

	if (panel->hovering) {
		if (!hovered)
			glfwSetCursor(window, context.cursors[CURSOR_KIND_IBEAM]);
	} else {
		if (hovered)
			glfwSetCursor(window, context.cursors[CURSOR_KIND_ARROW]);
	}
}

void panel_on_mouse_input(GLFWwindow *window, int button, int action, int mods) {
	Panel *panel = glfwGetWindowUserPointer(window);

	switch (panel->state) {
		case PANEL_STATE_IDEL: {
			if (panel->hovering && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				panel_start_typing(panel);

				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);
				panel_set_cursor_at_position(panel, (float)xpos);
			}
		} break;

		case PANEL_STATE_TYPING: {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				if (panel->hovering) {
					double xpos, ypos;
					glfwGetCursorPos(window, &xpos, &ypos);
					panel_set_cursor_at_position(panel, (float)xpos);
				} else {
					panel_stop_typing(panel);
				}
			}
		} break;
	}
}

void panel_on_key_input(GLFWwindow *window, int key, int scancode, int action, int mods) {
	Panel *panel = glfwGetWindowUserPointer(window);

	switch (panel->state) {
		case PANEL_STATE_TYPING: {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				panel_stop_typing(panel);
			} else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
				switch (key) {
					case GLFW_KEY_BACKSPACE: panel_delete_character(panel, true); break;
					case GLFW_KEY_DELETE: panel_delete_character(panel, false); break;
					case GLFW_KEY_RIGHT: panel_set_cursor(panel, panel->text_input.cursor + 1); break;
					case GLFW_KEY_LEFT: panel_set_cursor(panel, panel->text_input.cursor ? panel->text_input.cursor - 1 : 0); break;
					case GLFW_KEY_HOME: panel_set_cursor(panel, 0); break;
					case GLFW_KEY_END: panel_set_cursor(panel, panel->text_input.count); break;
				}
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
			if (codepoint >= FONT_PACKED_MIN_CODEPOINT && codepoint < FONT_PACKED_MAX_CODEPOINT)
				panel_input_character(panel, (char)codepoint);
		} break;
	}
}

bool panel_create(Panel_Styler styler, Panel *panel) {
	if (styler == NULL)
		styler = panel_default_styler;

	if (!styler(&panel->style)) {
		return false;
	}

	memset(&panel->text_input, 0, sizeof(panel->text_input));

	panel->state = PANEL_STATE_IDEL;
	panel->text_position_x_offset = false;
	panel->cursor_t = 0;
	panel->cursor_position = panel->style.indicator_size;
	panel->cursor_position_target = panel->style.indicator_size;
	panel->cursor_size = panel->style.cursor_size[1];
	panel->hovering = false;

	return true;
}

void panel_update(Panel *panel, float dt) {
	panel->cursor_t += dt * panel->style.cursor_blink_rate;
	if (panel->cursor_t > panel->style.cursor_blink_time) panel->cursor_t = 0;

	panel->cursor_position = lerp(panel->cursor_position, panel->cursor_position_target, (float)(1.0 - pow(panel->style.cursor_dposition, (double)dt)));

	V2 cursor_target_size = ((panel->text_input.count != panel->text_input.cursor) ? panel->style.cursor_size[0] : panel->style.cursor_size[1]);
	panel->cursor_size = v2lerp(panel->cursor_size, cursor_target_size, (float)(1.0f - pow(panel->style.cursor_dsize, dt)));
}

void panel_render(Panel *panel) {
	float off_x = panel->style.indicator_size - 1;

	glLoadIdentity();
	glOrtho(0, context.framebuffer_w, 0, context.framebuffer_h, -1, 1);

	glBegin(GL_QUADS);
	render_rect(v2(0, 0), v2((float)context.framebuffer_w, panel->style.height), panel->style.colors[PANEL_COLOR_BACKGROUND]);
	if (off_x > 0) {
		render_rect(v2(0, 0), v2(off_x - 1, panel->style.height), panel->style.colors[PANEL_COLOR_INPUT_INDICATOR]);
	}
	glEnd();

	String text = panel_text(panel);
	size_t cursor = panel->text_input.cursor;

	float cursor_w = panel->cursor_size.x;
	float cursor_h = panel->cursor_size.y;

	V2 text_pos = panel_text_render_position(panel);
	float cursor_render_x = render_font_stub(&panel->style.font, text_pos, text.data, cursor);

	if (cursor_render_x < panel->style.indicator_size) {
		panel->text_position_x_offset -= (panel->style.indicator_size - cursor_render_x);
		text_pos = panel_text_render_position(panel);
		cursor_render_x = render_font_stub(&panel->style.font, text_pos, text.data, cursor);
	} else if (cursor_render_x > (context.framebuffer_w - cursor_w - panel->style.font.size)) {
		panel->text_position_x_offset += (cursor_render_x - context.framebuffer_w + cursor_w + panel->style.font.size);
		text_pos = panel_text_render_position(panel);
		cursor_render_x = render_font_stub(&panel->style.font, text_pos, text.data, cursor);
	}

	glEnable(GL_SCISSOR_TEST);
	glScissor((GLint)panel->style.indicator_size, 0, context.framebuffer_w, (GLsizei)panel->style.height);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, panel->style.font.texture.id);
	glBegin(GL_QUADS);

	if (panel->state == PANEL_STATE_TYPING || text.length != 0) {
		Panel_Style *style = &panel->style;
		
		panel->text_input.buffer[panel->text_input.count] = 0;
		lexer_init(&panel->lexer, panel->text_input.buffer);

		lexer_advance_token(&panel->lexer);

		V4 text_color;
		char *text_start = panel->text_input.buffer;
		while (panel->lexer.token.kind != TOKEN_KIND_EOF) {
			Token *token = &panel->lexer.token;
			switch (token->kind) {
				case TOKEN_KIND_ERROR: text_color = style->colors[PANEL_COLOR_CODE_ERROR]; break;
				case TOKEN_KIND_NUMBER_LITERAL: text_color = style->colors[PANEL_COLOR_CODE_NUMBER_LITERAL]; break;
				case TOKEN_KIND_IDENTIFIER: text_color = style->colors[PANEL_COLOR_CODE_IDENTIFIER]; break;

				case TOKEN_KIND_PLUS:
				case TOKEN_KIND_MINUS:
				case TOKEN_KIND_EQUALS:
				case TOKEN_KIND_MUL:
				case TOKEN_KIND_DIV:
				case TOKEN_KIND_PERIOD:
					text_color = style->colors[PANEL_COLOR_CODE_OPERATORS];
					break;

				default: text_color = style->colors[PANEL_COLOR_CODE_GENERAL]; break;
			}

			text_pos.x = render_font(&panel->style.font, text_pos, style->colors[PANEL_COLOR_CODE_GENERAL], text_start, token->string.data - text_start);
			text_pos.x = render_font(&panel->style.font, text_pos, text_color, token->string.data, token->string.length);
			text_start = token->string.data + token->string.length;
			lexer_advance_token(&panel->lexer);
		}

		char *text_end = panel->text_input.buffer + panel->text_input.count;
		text_pos.x = render_font(&panel->style.font, text_pos, style->colors[PANEL_COLOR_CODE_GENERAL], text_start, text_end - text_start);
	} else {
		const char msg[] = "Enter Code...";
		render_font(&panel->style.font, v2(panel->style.indicator_size, text_pos.y), panel->style.colors[PANEL_COLOR_TEXT_INPUT_PLACEHOLDER], msg, sizeof(msg) - 1);
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);

	if (panel->state == PANEL_STATE_TYPING) {
		float t = panel->cursor_t;
		if (t > 1) t = 1;
		V4 cursor_color; 
		
		if (text.length != PANEL_TEXT_INPUT_BUFFER_SIZE)
			cursor_color = v4lerp(panel->style.colors[PANEL_COLOR_CURSOR0], panel->style.colors[PANEL_COLOR_CURSOR1], t);
		else
			cursor_color = panel->style.colors[PANEL_COLOR_CURSOR_NO_TYPE];

		float mid_y = (panel->style.height - cursor_h) * 0.5f;
		panel->cursor_position_target = cursor_render_x;

		glBegin(GL_QUADS);
		render_rect(v2(panel->cursor_position, mid_y), v2(cursor_w, cursor_h), cursor_color);
		glEnd();
	}

	glDisable(GL_SCISSOR_TEST);

	glPopMatrix();
}

//
// Actor
//

typedef struct {
	V2 position;
	float rotation;
	V2 scale;
	V4 color;
} Actor;

void actor_render(Actor *actor) {
	glPushMatrix();
	glTranslatef(actor->position.x, actor->position.y, 0);
	glRotatef(actor->rotation, 0, 0, -1);
	glScalef(actor->scale.x, actor->scale.y, 1);

	glColor4f(actor->color.x, actor->color.y, actor->color.z, actor->color.w);

	glBegin(GL_TRIANGLES);
	glVertex3f(-1, -1, 0);
	glVertex3f(0, 1, 0);
	glVertex3f(1, -1, 0);
	glEnd();
}

//
// Michi
//

typedef struct {
	float size;
	Actor actor;
} Michi;

void michi_create(float size, Michi *michi) {
	michi->size = size;

	michi->actor.position = v2(0, 0);
	michi->actor.rotation = 0;
	michi->actor.scale = v2(4, 4);
	michi->actor.color = v4(0, 1, 1, 1);
}

void michi_render(Michi *michi) {
	glLoadIdentity();

	float aspect_ratio = (float)context.framebuffer_w / (float)context.framebuffer_h;
	float half_height = michi->size;
	float half_width = half_height * aspect_ratio;

	glOrtho(-half_width, half_width, -half_height, half_height, -1, 1);

	actor_render(&michi->actor);

	glPopMatrix();
}

int main(int argc, char *argv[]) {
	if (!context_create()) {
		return -1;
	}

	Panel panel;
	if (!panel_create(NULL, &panel)) {
		fprintf(stderr, "Panel failed to create!\n");
		return -1;
	}

	Michi michi;
	michi_create(100, &michi);

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

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glViewport(0, 0, context.framebuffer_w, context.framebuffer_h);

		michi_render(&michi);

		panel_render(&panel);

		glfwSwapBuffers(context.window);

		uint64_t new_counter = glfwGetTimerValue();
		uint64_t counts = new_counter - counter;
		counter = new_counter;
		dt = ((1000000.0f * (float)counts) / (float)frequency) / 1000000.0f;
	}

	context_destory();

	return 0;
}
