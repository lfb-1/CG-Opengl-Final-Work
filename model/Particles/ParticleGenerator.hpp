

/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/

// As shown above, the code is adapted from the Breakout game, found on learnOpenGL.

// Modified by Yang for this CG project.

#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../Object/Object.h"

// Represents a single particle and its state
struct Particle
{
    glm::vec3 position, velocity;
    glm::vec4 color;
    GLfloat life;

    Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
};

// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator
{
  public:
    // Constructor
    ParticleGenerator(GLuint program, GLuint texture, GLuint totalAmount);
    // Update all particles
    void Update(GLfloat dt, Object *object, GLuint newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    // Render all particles
    void Draw();

  private:
    // The central position around which particles appear. Calculated by object position plus given offset.
    glm::vec3 spawnPosition;
    // State
    std::vector<Particle> particles;
    // Total amount of particles emitted by this generator.
    GLuint totalAmount;
    // Render state
    GLuint textureID;
    GLuint programID;
    GLuint VAO;
    // Initializes buffer and vertex attributes
    void init();
    // Returns the first Particle index that's currently unused e.g. life <= 0.0f or 0 if no particle is currently inactive
    GLuint firstUnusedParticle();
    // Respawns particle
    void respawnParticle(Particle &particle, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};

#endif
