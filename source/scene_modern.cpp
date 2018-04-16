#include <vvr/scene_modern.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>
#include <QtGui> //gl.h

/*--------------------------------------------------------------------------------------*/
const char* vertex_shader =
        "#version 410\n"
        "in vec3 vp;"
        "void main() {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";

const char* fragment_shader =
        "#version 410\n"
        "out vec4 frag_colour;"
        "void main() {"
        "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
        "}";

/*--------------------------------------------------------------------------------------*/
float pts_data[] = {
   0.0f,  0.5f,  0.0f,
   0.5f, -0.5f,  0.0f,
  -0.5f, -0.5f,  0.0f
};

GLuint vbo = 0;
GLuint vao = 0;
GLuint vs, fs;
GLuint shader_program;

/*--------------------------------------------------------------------------------------*/
vvr::SceneModern::SceneModern()
{
    m_bg_col = vvr::grey;
}

void vvr::SceneModern::resize()
{
    if (m_first_resize) setupGL();
}

void vvr::SceneModern::setupGL()
{
    int  ok;
    char infoLog[512];

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), pts_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(vs, 512, NULL, infoLog); vvr_msg(infoLog); }

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(fs, 512, NULL, infoLog); vvr_msg(infoLog); }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);
}

void vvr::SceneModern::draw()
{
    enterPixelMode();
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    exitPixelMode();
}
/*--------------------------------------------------------------------------------------*/

vvr_invoke_main_with_scene(vvr::SceneModern)
