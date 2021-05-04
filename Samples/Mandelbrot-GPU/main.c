#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

static const float g_Vertices[] = {
    -1, -1, -1, 1, 1, +1,
    -1, -1, +1, 1, 1, -1
};

static void ErrorCallback(int error, const char *description) {
    fprintf(stderr, "Error(%d): %s\n", error, description);
}

#ifdef _GLFW_WIN32
#ifdef APIENTRY
#undef APIENTRY
#endif
#include <Windows.h>
uint64_t GetFileModifiedTime(const char *file) {
    HANDLE handle = CreateFileA(file, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    uint64_t result = 0;

    FILETIME write_time;
    if (GetFileTime(handle, NULL, NULL, &write_time)) {
        ULARGE_INTEGER r;
        r.HighPart = write_time.dwHighDateTime;
        r.LowPart = write_time.dwLowDateTime;
        result = r.QuadPart;
    }

    CloseHandle(handle);

    return result;
}
#else
#include <sys/stat.h>
uint64_t GetFileModifiedTime(const char *file) {
    struct stat buf;
    if (stat(file, &buf)) {
        return buf.st_mtime;
    }
    return 0;
}
#endif

char *ReadEntireFile(const char *file, uint64_t *time) {
    *time = GetFileModifiedTime(file);

    FILE *f = fopen(file, "rb");
    if ( !f ){
      fprintf(stderr, "File not found %s\n", file );
      return NULL;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    if (!string) {
        fprintf(stderr, "Out of Memory, malloc failed\n");
        exit(0);
    }

    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

uint32_t FindLeastSignificantSetBit(uint32_t value) {
    for (uint32_t test = 0; test < 32; ++test) {
        if (value & (1 << test)) {
            return test;
            break;
        }
    }
    return 0;
}

unsigned char *LoadBMP(const char *file, int *w, int *h) {
    FILE *fp = fopen(file, "rb");
    if (!fp) return NULL;

    uint16_t file_type; uint32_t file_size; uint16_t reserved1; uint16_t reserved2; uint32_t bitmap_offset;
    if (fread(&file_type, sizeof(file_type), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&file_size, sizeof(file_size), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&reserved1, sizeof(reserved1), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&reserved2, sizeof(reserved2), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&bitmap_offset, sizeof(bitmap_offset), 1, fp) != 1) { fclose(fp); return NULL; }

    uint32_t size; int32_t width; int32_t height; uint16_t planes; uint16_t bits_per_pixel; uint32_t compression;
    if (fread(&size, sizeof(size), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&width, sizeof(width), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&height, sizeof(height), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&planes, sizeof(planes), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&bits_per_pixel, sizeof(bits_per_pixel), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&compression, sizeof(compression), 1, fp) != 1) { fclose(fp); return NULL; }

    uint32_t bitmap_size; int32_t h_res; int32_t v_res; uint32_t color_used; uint32_t colors_important;
    if (fread(&bitmap_size, sizeof(bitmap_size), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&h_res, sizeof(h_res), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&v_res, sizeof(v_res), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&color_used, sizeof(color_used), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&colors_important, sizeof(colors_important), 1, fp) != 1) { fclose(fp); return NULL; }

    uint32_t red_mask; uint32_t green_mask; uint32_t blue_mask;
    if (fread(&red_mask, sizeof(red_mask), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&green_mask, sizeof(green_mask), 1, fp) != 1) { fclose(fp); return NULL; }
    if (fread(&blue_mask, sizeof(blue_mask), 1, fp) != 1) { fclose(fp); return NULL; }

    uint32_t alpha_mask = ~(red_mask | green_mask | blue_mask);
    if (red_mask == 0 || green_mask == 0 || blue_mask == 0 || alpha_mask == 0) {
        fprintf(stderr, "Failed to load BMP(%s). Color format must be RGBA", file);
        fclose(fp);
        return NULL;
    }

    if (compression != 3) {
        fprintf(stderr, "Failed to load BMP(%s). Compression is not supported", file);
        fclose(fp);
        return NULL;
    }

    if (bits_per_pixel != 32) {
        fprintf(stderr, "Failed to load BMP(%s). Bits per pixel must be 32", file);
        fclose(fp);
        return NULL;
    }

    size_t pixels_size = sizeof(uint32_t) * width * height;
    unsigned char *pixels = malloc(pixels_size);
    if (pixels == NULL) {
        fprintf(stderr, "Failed to allocated memory, malloc failed!\n");
        exit(0);
    }

    if (fread(pixels, pixels_size, 1, fp) != 1) {
        fprintf(stderr, "Invalid BMP file");
        free(pixels);
        fclose(fp);
        return NULL;
    }

    fclose(fp);

    uint32_t red_shift = FindLeastSignificantSetBit(red_mask);
    uint32_t green_shift = FindLeastSignificantSetBit(green_mask);
    uint32_t blue_shift = FindLeastSignificantSetBit(blue_mask);
    uint32_t alpha_shift = FindLeastSignificantSetBit(alpha_mask);

    uint32_t *dest = (uint32_t *)pixels;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint32_t c = *dest;
            *dest = ((((c >> alpha_shift) & 0xff) << 24) | (((c >> blue_shift) & 0xff) << 16) |
                     (((c >> green_shift) & 0xff) << 8) | (((c >> red_shift) & 0xff) << 0));
            dest += 1;
        }
    }

    *w = width;
    *h = height;
    return pixels;
}

GLuint CompileShader(GLenum type, const char *code) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar message[512];
        glGetShaderInfoLog(shader, 512, NULL, message);
        char *str_type = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
        fprintf(stderr, "%s Shader Compilation failed: %s\n", str_type, message);
        return -1;
    }

    return shader;
}

typedef struct Shader {
    GLuint id;
    uint64_t v_tmod;
    uint64_t f_tmod;
} Shader;

static const char *VertexFile = "mandelbrot.vert";
static const char *FragmentFile = "mandelbrot.frag";

bool LoadShader(Shader *shader) {
    char *vertex_code = ReadEntireFile(VertexFile, &shader->v_tmod);
    char *fragment_code = ReadEntireFile(FragmentFile, &shader->f_tmod);

    if (vertex_code && fragment_code) {
        GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertex_code);
        GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragment_code);

        if (vertex == -1 || fragment == -1) {
            free(vertex_code);
            free(fragment_code);
            return false;
        }

        GLuint id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        
        GLint success;
        glGetProgramiv(id, GL_LINK_STATUS, &success); 
        if (!success) {
            GLchar message[512];
            glGetProgramInfoLog(id, 512, NULL, message);
            fprintf(stderr, "Linking Shader faliled: %s\n", message);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            glDeleteProgram(id);
            free(vertex_code);
            free(fragment_code);
            return false;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        free(vertex_code);
        free(fragment_code);

        shader->id = id;

        return true;
    }

    return false;
}

bool ReloadShaderIfFileChanged(Shader *shader) {
    uint64_t vtime = GetFileModifiedTime(VertexFile);
    uint64_t ftime = GetFileModifiedTime(FragmentFile);

    if (shader->v_tmod < vtime || shader->f_tmod < ftime) {
        Shader nshader;
        if (LoadShader(&nshader)) {
            printf("Hot reloaded shader!\n");
            *shader = nshader;
            return true;
        } else {
            shader->v_tmod = vtime;
            shader->f_tmod = ftime;
            return false;
        }
    }

    return false;
}

typedef struct Mandelbrot {
    Shader shader;
    GLint u_resolution;
    GLint u_rect_min;
    GLint u_rect_max;

    float rect_min[2];
    float rect_max[2];
} Mandelbrot;

void LoadShaderLocations(Mandelbrot *mandelbrot) {
    mandelbrot->u_resolution = glGetUniformLocation(mandelbrot->shader.id, "u_Resolution");
    mandelbrot->u_rect_min = glGetUniformLocation(mandelbrot->shader.id, "u_RectMin");
    mandelbrot->u_rect_max = glGetUniformLocation(mandelbrot->shader.id, "u_RectMax");
}

int main() {
    GLFWwindow *window;

    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(640, 480, "Mandelbrot-GPU", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    GLFWimage icon;
    icon.pixels = LoadBMP("Logo.bmp", &icon.width, &icon.height);
    if (icon.pixels) {
        glfwSetWindowIcon(window, 1, &icon);
    } else {
        fprintf(stderr, "Failed to load icon (Logo.bmp)");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load opengl functions\n");
        return -1;
    }

    glfwSwapInterval(1);

    Mandelbrot mandelbrot;

    if (!LoadShader(&mandelbrot.shader)) {
        return -1;
    }

    LoadShaderLocations(&mandelbrot);
    mandelbrot.rect_min[0] = -2.5f;
    mandelbrot.rect_min[1] = -2.0f;
    mandelbrot.rect_max[0] = 1.0f;
    mandelbrot.rect_max[1] = 2.0f;

    glfwSetWindowUserPointer(window, &mandelbrot);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_Vertices), g_Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)0);

    int width, height;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (ReloadShaderIfFileChanged(&mandelbrot.shader)) {
            mandelbrot.u_resolution = glGetUniformLocation(mandelbrot.shader.id, "u_Resolution");
            mandelbrot.u_rect_min = glGetUniformLocation(mandelbrot.shader.id, "u_RectMin");
            mandelbrot.u_rect_max = glGetUniformLocation(mandelbrot.shader.id, "u_RectMax");
        }

        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);

        glUseProgram(mandelbrot.shader.id);
        glUniform2f(mandelbrot.u_resolution, (float)width, (float)height);
        glUniform2f(mandelbrot.u_rect_min, mandelbrot.rect_min[0], mandelbrot.rect_min[1]);
        glUniform2f(mandelbrot.u_rect_max, mandelbrot.rect_max[0], mandelbrot.rect_max[1]);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
