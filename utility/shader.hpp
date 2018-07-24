#ifndef SHADER_HPP
#define SHADER_HPP
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
/**************************************************
 * Simple function to read GLSL shader source from a file,
 * Then compile it and link to create a shader program ready for use.
 * Returns the ID of the shader program (assigned by OpenGL)
 * or 0 if error.
**************************************************/

GLuint LoadShaders(const char *vertex_file_path,
                   const char *fragment_file_path);
unsigned int loadCubemap(std::vector<std::string>);

#endif
