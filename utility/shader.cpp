#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>

#include "../lib/stb_image.h"

#include <GL/glew.h>

#include "shader.hpp"
using std::vector;
int CompileShader(const char *ShaderPath, const GLuint ShaderID)
{
    // Read shader code from file
    std::string ShaderCode;
    std::ifstream ShaderStream(ShaderPath, std::ios::in);
    if (ShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(ShaderStream, Line))
        {
            ShaderCode += "\n" + Line;
        }
        ShaderStream.close();
    }
    else
    {
        std::cerr << "Cannot open " << ShaderPath << ". Are you in the right directory?" << std::endl;
        return 0;
    }

    // Compile Shader
    char const *SourcePointer = ShaderCode.c_str();
    glShaderSource(ShaderID, 1, &SourcePointer, NULL);
    glCompileShader(ShaderID);

    // Check Shader
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    printf("compiled shader %d %d\n", Result, InfoLogLength);
    if (InfoLogLength > 1)
    {
        char ShaderErrorMessage[InfoLogLength + 1];
        glGetShaderInfoLog(ShaderID,
                           InfoLogLength,
                           NULL,
                           &ShaderErrorMessage[0]);
        std::cerr << &ShaderErrorMessage[0] << std::endl;
        return 0;
    }
    return 1;
}

GLuint LoadShaders(const char *vertex_file_path,
                   const char *fragment_file_path)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile both shaders. Exit if compile errors.
    if (!CompileShader(vertex_file_path, VertexShaderID) || !CompileShader(fragment_file_path, FragmentShaderID))
    {
        return 0;
    }

    // Link the program
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cerr << &ProgramErrorMessage[0] << std::endl;
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

/**
 * https://learnopengl.com/Advanced-OpenGL/Cubemaps
 */

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
