/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's evenly contributed by both members.

**************************************************/

#include "helpers.h"

#include "../lib/stb_image.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

unsigned int set_texture(std::string filename)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    int tex_width, tex_height, nr_channel;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data1 = stbi_load(filename.c_str(), &tex_width, &tex_height, &nr_channel, 0);
    if (nr_channel == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else if (nr_channel == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "failed to load texture\n";
    }
    stbi_image_free(data1);
    return id;
}

unsigned int createSquareVAO()
{
    // square has 4 vertices at its corners
    float squareVertices[] =
        {
            -2.0f, -2.0f, 0.0f,
            2.0f, -2.0f, 0.0f,
            2.0f, 2.0f, 0.0f,
            -2.0f, 2.0f, 0.0f};

    // square has 4 vertices at its corners
    float squareColours[] =
        {
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f};

    // Texture indices of each vertex
    float squareTexture[] =
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f};

    // Each square face is made up of two triangles
    unsigned int indices[] =
        {
            0, 1, 2, 2, 3, 0};

    unsigned int vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);
    // Buffers to store position, colour, normal and index data
    unsigned int buffer[4];
    glGenBuffers(4, buffer);

    // Set vertex position
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareColours), squareColours, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
    // We don't attach this to a shader label, instead it controls how rendering is performed
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices), indices, GL_STATIC_DRAW);

    // Texture attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(squareTexture), squareTexture, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    return vaoHandle;
}

unsigned int createGroundVAO()
{
    // square has 4 vertices at its corners
    float squareVertices[] =
        {
            -1.0f, -1.0f, 1.0f,
            1.0f,  -1.0f, 1.0f,
            1.0f,  -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f};

    // square has 4 vertices at its corners
    float squareColours[] =
        {
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f};

    // Texture indices of each vertex
    float squareTexture[] =
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f};

    // Each square face is made up of two triangles
    unsigned int indices[] =
        {
            0, 1, 2, 2, 3, 0};

    unsigned int vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);
    // Buffers to store position, colour, normal and index data
    unsigned int buffer[4];
    glGenBuffers(4, buffer);

    // Set vertex position
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(squareColours), squareColours, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
    // We don't attach this to a shader label, instead it controls how rendering is performed
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices), indices, GL_STATIC_DRAW);

    // Texture attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(squareTexture), squareTexture, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    return vaoHandle;
}

//TODO: Max to Thomas: We should change the input to vector for generalization.
float calculate_largest(float a, float b, float c)
{

    return a > b ? (b > c ? a : (a > c ? a : c)) : (a > c ? b : (b < c ? c : b));
}

std::string get_directory(const std::string &path)
{
    size_t found = path.find_last_of("/\\");
    return (path.substr(0, found));
}
