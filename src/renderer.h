//
//  renderer.h
//  project
//
//  Created by Arve Nygård on 19/05/15.
//  Copyright (c) 2015 Arve Nygård. All rights reserved.
//

#ifndef __project__renderer__
#define __project__renderer__

#include <stdio.h>
#include <GLFW/glfw3.h>
#include <GLUT/GLUT.h>
#include <stdlib.h>
#include <glm/vec3.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <OpenGL/OpenGL.h>

#include "util.h"

typedef struct {
    GLuint vertex_buffer, element_buffer;
    GLuint textures[2];
    GLuint fragment_shader, vertex_shader, program;
    float mouse_pos_x, mouse_pos_y;
    struct {
        GLfloat timer;
        GLuint textures[2];
        GLfloat mouse_x;
        GLfloat mouse_y;
    } uniforms;

    struct {
        GLint position;
        GLfloat uv;
    } attributes;

    GLfloat timer;
} gl_resources;

class Renderer{
public:
    glm::vec3 camPosition;
    GLFWwindow* window;
    gl_resources g_resources;

    void render();
    void putMesh();
    void init();
    void draw();
    static GLuint make_shader(GLenum type, const char *filename);
    static GLuint make_program(GLuint vertex_shader, GLuint fragment_shader);

    int make_resources(void);
    static GLuint make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size);
    static GLuint make_texture(const char *filename);

    static void mouse_position_callback(GLFWwindow * window, double x, double y);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void error_callback(int error, const char* description);

};


#endif /* defined(__project__renderer__) */
