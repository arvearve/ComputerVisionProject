#include "renderer.h"


Renderer r;
HeadTracker ft; // Starts tracking automatically.
int main(){

    namedWindow("debug");

    r.init();

    /* Read tracked values, update display */

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


    glfwSetCursorPosCallback(window, mouse_position_callback);
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);

    if(!make_resources()){
        exit(EXIT_FAILURE);
    }
}

void Renderer::render() {
    while (!glfwWindowShouldClose(window)){
//        printf("%f, %f\n", ft.normalizedPosition.x, ft.normalizedPosition.y);
        g_resources.mouse_pos_x = -ft.normalizedPosition.x;
        g_resources.mouse_pos_y = ft.normalizedPosition.y;
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
    glUniform1f(g_resources.uniforms.timer, g_resources.timer);
    glUniform1f(g_resources.uniforms.mouse_x, g_resources.mouse_pos_x);
    glUniform1f(g_resources.uniforms.mouse_y, g_resources.mouse_pos_y);
    glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
    glVertexAttribPointer(
                          g_resources.attributes.position,  /* attribute */
                          4,                                /* size */
                          GL_FLOAT,                         /* type */
                          GL_FALSE,                         /* normalized? */
                          sizeof(GLfloat)*6,                /* stride */
                          (void*)0                          /* array buffer offset */
                          );
    g_resources.timer = glfwGetTime();
    glEnableVertexAttribArray(g_resources.attributes.position);
    /* Draw the cube */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
    glDrawElements(
                   GL_TRIANGLES,       /* mode */
                   36,                  /* count */
                   GL_UNSIGNED_SHORT,  /* type */
                   (void*)0            /* element array buffer offset */
                   );

    /* back wall */

    glDrawElements(GL_TRIANGLE_STRIP, 2, GL_UNSIGNED_SHORT, (void*)10);

    glDisableVertexAttribArray(g_resources.attributes.position);
    glfwSwapBuffers(window);
    glfwPollEvents();
    
}



/* OpenGL Infrastructure */

void Renderer::mouse_position_callback(GLFWwindow * window, double x, double y){
    // Hack: We know the window size is 640*480
    int xhalf = 640/2;
    int yhalf = 480/2;
    r.g_resources.mouse_pos_x = (x-xhalf)/xhalf; // position relative to window center
    r.g_resources.mouse_pos_y = (y-yhalf)/yhalf; // position relative to window center
}

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
    GLint attached_shaders;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &attached_shaders);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program.\n");
        char glErrorLog[1024];
        int length;
        glGetShaderInfoLog(program, 1024, &length, glErrorLog);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return buffer;
}

GLuint Renderer::make_texture(const char *filename){
    GLuint texture;
    int width, height;
    void *pixels = read_tga(filename, &width, &height);

    if (!pixels) {
        return 0;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexImage2D(
                 GL_TEXTURE_2D, 0,           /* target, level of detail */
                 GL_RGB8,                    /* internal format */
                 width, height, 0,           /* width, height, border */
                 GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
                 pixels                      /* pixels */
                 );
    return texture;
}

int Renderer::make_resources(void){
    /* make buffers and textures ... */
    GLfloat vertices[] = {
        // front                   UV
       -1.0, -1.0, 1.0, 1.0,      0.0, 0.0,
        1.0, -1.0, 1.0, 1.0,      1.0, 0.0,
        1.0,  1.0, 1.0, 1.0,      1.0, 1.0,
       -1.0,  1.0, 1.0, 1.0,      0.0, 1.0,
        // back
       -1.0, -1.0, -1.0, 1.0,     0.0, 0.0,
        1.0, -1.0, -1.0, 1.0,     1.0, 0.0,
        1.0,  1.0, -1.0, 1.0,     1.0, 1.0,
       -1.0,  1.0, -1.0, 1.0,     0.0, 1.0,
    };

    GLushort indices[] = {
        // top
        3, 2, 6,
        6, 7, 3,
        // back
        7, 6, 5,
        5, 4, 7,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // left
        4, 0, 3,
        3, 7, 4,
        // right
        1, 5, 6,
        6, 2, 1,
        // front
        0, 1, 2,
        2, 3, 0,
    };

    g_resources.textures[0] = make_texture("data/hello1.tga");
    g_resources.textures[1] = make_texture("data/hello2.tga");

    if (g_resources.textures[0] == 0 || g_resources.textures[1] == 0){
        printf("Error: could not init textures!");
        return 0;
    }

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

    g_resources.uniforms.timer = glGetUniformLocation(g_resources.program, "timer");
    g_resources.uniforms.mouse_x = glGetUniformLocation(g_resources.program, "mouse_x");
    g_resources.uniforms.mouse_y = glGetUniformLocation(g_resources.program, "mouse_y");
    g_resources.uniforms.textures[0] = glGetUniformLocation(g_resources.program, "textures[0]");
    g_resources.uniforms.textures[1] = glGetUniformLocation(g_resources.program, "textures[1]");
//
    g_resources.attributes.position = glGetAttribLocation(g_resources.program, "position");

    return 1;
}