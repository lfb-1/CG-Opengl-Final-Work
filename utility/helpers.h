/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's evenly contributed by both members.

**************************************************/

#ifndef HELPERS_H
#define HELPERS_H
#include <string>

// Load the texture specified by given path and return the handle that the texture is bound to. A default white texture will be used in case of loading failure.
unsigned int set_texture(std::string path);

// A square perpendicular to X-Y axes used for rendering secondary camera.
unsigned int createSquareVAO();

// A square perpenducular to X-Z axes used for rendering ground.
unsigned int createGroundVAO();

// Calculate the largest value among three
float calculate_largest(float, float, float);

// Return the directory of a path. Effectively removing the filename at the end of the path.
std::string get_directory(const std::string &path);

#endif
