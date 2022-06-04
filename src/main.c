#include <math.h>
#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

typedef struct {
    float position[2];
} Vertex;

const char* vert_shader_source = "#version 330 core\n"
                                 "layout (location = 0) in vec2 in_pos;\n"
                                 "uniform mat4 proj;\n"
                                 "uniform mat4 scale;\n"
                                 "out vec4 vert_color;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = proj * vec4(in_pos.x, in_pos.y, 0.0f, 1.0f);\n"
                                 "}\0";
const char* frag_shader_source = "#version 330 core\n"
                                 "out vec4 frag_color;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   frag_color = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
                                 "}\n\0";

int main()
{
    int window_w = 1280, window_h = 720;
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* sdl_window = sdl_window =
        SDL_CreateWindow("SDL OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_w, window_h,
                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    int gl_major = 3, gl_minor = 3;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);
    SDL_GL_CreateContext(sdl_window);

    gl3wInit();

    // compile vert shader
    unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_shader_source, NULL);
    glCompileShader(vert_shader);

    // compile frag shader
    unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
    glCompileShader(frag_shader);

    // link shaders
    unsigned int quad_shader = glCreateProgram();
    glAttachShader(quad_shader, vert_shader);
    glAttachShader(quad_shader, frag_shader);
    glLinkProgram(quad_shader);

    // delete shaders
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    unsigned int quad_vao, quad_vbo;
    glUseProgram(quad_shader);
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    int should_close = 0;
    int current_width = 0, current_height = 0;
    while (!should_close) {
        SDL_Event ev = {0};
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    should_close = 1;
                    break;
            }
        }

        SDL_GetWindowSize(sdl_window, &window_w, &window_h);
        if (window_w != current_width || window_h != current_height) {
            current_width = window_w;
            current_height = window_h;
            glViewport(0, 0, window_w, window_h);
        }

        // orthographic projection matrix
        float ortho[16] = {};
        float left = 0.0f;
        float right = window_w;
        float bot = window_h;
        float top = 0.0f;
        float near_plane = -1.0f;
        float far_plane = 1.0f;

        ortho[0] = 2.0f / (right - left);
        ortho[5] = 2.0f / (top - bot);
        ortho[10] = 2.0f / (near_plane - far_plane);
        ortho[15] = 1.0f;
        ortho[3] = (left + right) / (left - right);
        ortho[7] = (bot + top) / (bot - top);
        ortho[11] = (far_plane + near_plane) / (near_plane - far_plane);

        float scale[16] = {};
        scale[0] = 1.0f;
        scale[5] = 1.0f;
        scale[10] = 1.0f;
        scale[15] = 1.0f;

        glUseProgram(quad_shader);
        uint32_t proj_loc = glGetUniformLocation(quad_shader, "proj");
        glUniformMatrix4fv(proj_loc, 1, GL_TRUE, ortho);
        uint32_t scale_loc = glGetUniformLocation(quad_shader, "scale");
        glUniformMatrix4fv(scale_loc, 1, GL_TRUE, scale);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Vertex vertices[6] = {
            {10.0f, 10.0f},   // left top
            {10.0f, 100.0f},  // left bot
            {100.0f, 10.0f},  // right top
            {100.0f, 10.0f},  // right top
            {10.0f, 100.0f},  // left bot
            {100.0f, 100.0f}, // right bot
        };

        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        SDL_GL_SwapWindow(sdl_window);
    }

    return 0;
}
