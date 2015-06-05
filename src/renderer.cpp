#include "renderer.h"



int main(){
    Renderer r;
    r.init();
    r.render();
    return 0;
}


void Renderer::init(){
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);

    GLuint vertex_shader = make_shader(GL_VERTEX_SHADER, "data/vertex_shader.glsl");
    GLuint fragment_shader = make_shader(GL_FRAGMENT_SHADER, "data/fragment_shader.glsl");
    int program = make_program(vertex_shader, fragment_shader);

    if(!make_resources()){
        exit(EXIT_FAILURE);
    }
}

void Renderer::render() {
    while (!glfwWindowShouldClose(window)){
        draw();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void Renderer::draw(){
    glClearColor(0.3f, 0.3f, 0.3f, 0.3f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(g_resources.program);
    glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
    glVertexAttribPointer(
                          g_resources.attributes.position,  /* attribute */
                          4,                                /* size */
                          GL_FLOAT,                         /* type */
                          GL_FALSE,                         /* normalized? */
                          sizeof(GLfloat)*4,                /* stride */
                          (void*)0                          /* array buffer offset */
                          );
    glEnableVertexAttribArray(g_resources.attributes.position);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
//    glLoadIdentity();
//    glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

    glDrawElements(
                   GL_TRIANGLE_STRIP,  /* mode */
                   4,                  /* count */
                   GL_UNSIGNED_SHORT,  /* type */
                   (void*)0            /* element array buffer offset */
                   );
    glDisableVertexAttribArray(g_resources.attributes.position);
    glfwSwapBuffers(window);
    glfwPollEvents();
    
}



/* OpenGL Infrastructure */

void Renderer::error_callback(int error, const char* description){
    fputs(description, stderr);
}

void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint Renderer::make_shader(GLenum type, const char *filename){
    /* Read shader source code */
    std::ifstream in(filename);
    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    const GLchar *source = contents.c_str();
    GLint length = contents.length();

    GLuint shader;
    GLint shader_ok;

    if (!source){ return 0; }
    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if (!shader_ok) {
        fprintf(stderr, "Failed to compile %s:\n", filename);
        char glErrorLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, glErrorLog);
        std::cout << glErrorLog << std::endl;
        printf("Source: \n %s", source);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint Renderer::make_program(GLuint vertex_shader, GLuint fragment_shader){
    GLint program_ok;

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
        char glErrorLog[1024];
        glGetShaderInfoLog(program, 1024, NULL, glErrorLog);
        std::cout << glErrorLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

GLuint Renderer::make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
    return buffer;
}


int Renderer::make_resources(void){
    /* make buffers and textures ... */
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
        1.0f,  1.0f, 0.0f, 1.0f
    };

    GLushort indices[] = { 0,1,2,3 };


    g_resources.vertex_buffer = make_buffer(GL_ARRAY_BUFFER, vertices, sizeof(vertices));

    g_resources.element_buffer = make_buffer(GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices));

    /* Shaders */
    g_resources.vertex_shader = make_shader(GL_VERTEX_SHADER, "data/frustum.v.glsl");
    if (g_resources.vertex_shader == 0) {
        return 0;
    }

    g_resources.fragment_shader = make_shader(GL_FRAGMENT_SHADER, "data/fragment_shader.glsl");
    if (g_resources.fragment_shader == 0) {
        return 0;
    }

    g_resources.program = make_program(g_resources.vertex_shader, g_resources.fragment_shader);
    if (g_resources.program == 0) {
        return 0;
    }


//    g_resources.uniforms.fade_factor
//    = glGetUniformLocation(g_resources.program, "fade_factor");
//    g_resources.uniforms.textures[0]
//    = glGetUniformLocation(g_resources.program, "textures[0]");
//    g_resources.uniforms.textures[1]
//    = glGetUniformLocation(g_resources.program, "textures[1]");
//
    g_resources.attributes.position = glGetAttribLocation(g_resources.program, "position");

    return 1;
}