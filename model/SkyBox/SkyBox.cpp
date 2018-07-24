/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1677895 Fengbei Liu

**************************************************/

#include "SkyBox.h"
#include <iostream>
#include <vector>

SkyBox::SkyBox(std::string path)
{
    setFaces(path);
    binding();
    // setVertices();
}

void SkyBox::setVertices()
{
    float vertices[108] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -100.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    binding();
}

void SkyBox::updateProgramID(GLuint programID)
{
    this->programID = programID;
    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID, "skybox"), 0);
}

void SkyBox::binding()
{
    GLuint skyboxVao, skyboxVbo;
    glGenVertexArrays(1, &skyboxVao);
    glGenBuffers(1, &skyboxVbo);
    glBindVertexArray(skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    vao = skyboxVao;
}

// REMINDER: recommend rename skybox image, but might change later to be more flexible
void SkyBox::setFaces(std::string path)
{
    faces.push_back(path + "right.jpg");
    faces.push_back(path + "left.jpg");
    faces.push_back(path + "bottom.jpg");
    faces.push_back(path + "top.jpg");
    faces.push_back(path + "back.jpg");
    faces.push_back(path + "front.jpg");

    cubeMapTexture = loadCubemap(faces);
}

vector<string> SkyBox::getFaces()
{
    return faces;
}

void SkyBox::render()
{
    glDepthFunc(GL_LEQUAL);
    glUseProgram(programID);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
