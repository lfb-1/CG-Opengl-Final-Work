/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1677895 Fengbei Liu

**************************************************/

#ifndef SKYBOX_H
#define SKYBOX_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include "../../utility/shader.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::string;
using std::vector;

class SkyBox
{
private:
  // vertices contains the cube map vertex
  float vertices[108] = {
      // positions
      -5.0f, 5.0f, -5.0f,
      -5.0f, -5.0f, -5.0f,
      5.0f, -5.0f, -5.0f,
      5.0f, -5.0f, -5.0f,
      5.0f, 5.0f, -5.0f,
      -5.0f, 5.0f, -5.0f,

      -5.0f, -5.0f, 5.0f,
      -5.0f, -5.0f, -5.0f,
      -5.0f, 5.0f, -5.0f,
      -5.0f, 5.0f, -5.0f,
      -5.0f, 5.0f, 5.0f,
      -5.0f, -5.0f, 5.0f,

      5.0f, -5.0f, -5.0f,
      5.0f, -5.0f, 5.0f,
      5.0f, 5.0f, 5.0f,
      5.0f, 5.0f, 5.0f,
      5.0f, 5.0f, -5.0f,
      5.0f, -5.0f, -5.0f,

      -5.0f, -5.0f, 5.0f,
      -5.0f, 5.0f, 5.0f,
      5.0f, 5.0f, 5.0f,
      5.0f, 5.0f, 5.0f,
      5.0f, -5.0f, 5.0f,
      -5.0f, -5.0f, 5.0f,

      -5.0f, 5.0f, -5.0f,
      5.0f, 5.0f, -5.0f,
      5.0f, 5.0f, 5.0f,
      5.0f, 5.0f, 5.0f,
      -5.0f, 5.0f, 5.0f,
      -5.0f, 5.0f, -5.0f,

      -5.0f, -5.0f, -5.0f,
      -5.0f, -5.0f, 5.0f,
      5.0f, -5.0f, -5.0f,
      5.0f, -5.0f, -5.0f,
      -5.0f, -5.0f, 5.0f,
      5.0f, -5.0f, 5.0f};

  // faces contains the order of skybox image
  vector<string> faces;

  GLuint programID;
  GLuint vao;
  GLuint cubeMapTexture;

public:
  /**
     * function                                     # configure vertices and bind vao
     * input                                        # void
     * output                                       # void
     * when is it called                            # constructor
     */
  void setVertices();

  /**
     * function                                     # bind vao
     * input                                        # void
     * output                                       # void
     * when is it called                            # after setVertices
     */
  void binding();

  /**
     * function                                     # render the skybox
     * input                                        # void
     * output                                       # void
     * when is it called                            # in the main render 
     */
  void render();

  void updateProgramID(GLuint);
  GLuint getProgramID();

  /**
     * function                                     # construct the basic of skybox
     * input                                        # string:path, path for skybox folder
     * output                                       # void
     * when is it called                            # when creating skybox unit
     */
  SkyBox(string);

  /**
     * function                                     # set skybox order 
     * input                                        # sting path: path for skybox folder
     * output                                       # void
     * when is it called                            # in constructor
     */
  void setFaces(std::string);

  vector<string> getFaces();
};
#endif
