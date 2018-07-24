

/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.


** As shown above, the code is based on learnOpenGL's Breakout
game.


******************************************************************/


/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/



#include "ParticleGenerator.hpp"

#include <iostream>
#include <stdlib.h> // For rand()
#include <math.h>   // For sqrt() and pow()

ParticleGenerator::ParticleGenerator(GLuint program, GLuint texture, GLuint totalAmount)
    : programID(program), textureID(texture), totalAmount(totalAmount)
{
    spawnPosition = glm::vec3(0.0f);
    this->init();
}

void ParticleGenerator::Update(GLfloat dt, Object *object, GLuint newParticles, glm::vec3 offset)
{
    spawnPosition = object->getPosition() + offset;

    // Add new particles
    for (GLuint i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], offset);
    }
    // Update all particles
    for (GLuint i = 0; i < this->totalAmount; ++i)
    {
        Particle &p = this->particles[i];
        p.life -= dt; // reduce life
        if (p.life > 0.0f)
        { // particle is alive, thus update
            p.position += p.velocity * dt;

            if (p.position.y <= -1.0)
                p.life = -1.0;

            p.color.g -= dt * 0.2;
            p.color.b -= dt * 0.2;
            p.color.a -= dt * 2.5;
        }
    }
}

// Render all particles
void ParticleGenerator::Draw()
{
    // Use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i].life > 0.0f)
        {
            glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &particles[i].position[0]);
            glUniform4fv(glGetUniformLocation(programID, "color"), 1, &particles[i].color[0]);
            if (textureID != -1)
                glBindTexture(GL_TEXTURE_2D, textureID);

            glEnable(GL_PROGRAM_POINT_SIZE);
            glEnable(GL_POINT_SPRITE);

            glBindVertexArray(this->VAO);
            glDrawArrays(GL_POINTS, 0, 1);
            glBindVertexArray(0);
        }
    }
    // Don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    // Set up mesh and attribute properties
    GLuint VBO;
    GLfloat particle_point[] = {

        0.0f, 0.0f, 0.0f};
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    // Fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_point), particle_point, GL_STATIC_DRAW);
    // Set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glBindVertexArray(0);

    // Create this->totalAmount default particle instances
    for (GLuint i = 0; i < this->totalAmount; ++i)
        this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
    // First search from last used particle, this will usually return almost instantly
    for (GLuint i = lastUsedParticle; i < this->totalAmount; ++i)
    {
        if (this->particles[i].life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (GLuint i = 0; i < lastUsedParticle; ++i)
    {
        if (this->particles[i].life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec3 offset)
{
    // Possible
    GLuint discrete_range = 200;

    GLfloat random_X = ((rand() % discrete_range) - discrete_range / 2) / 500.0;
    GLfloat random_Y = ((rand() % discrete_range) - discrete_range / 2) / 500.0;
    GLfloat random_Z = ((rand() % discrete_range) - discrete_range / 2) / 500.0;

    particle.position = spawnPosition;
    particle.position.x = particle.position.x + random_X * 200.0 - 30.0;
    particle.position.y = 5.0;
    //CODE ALTER: without this line snow flakes only appear from the 'roof', instead of out of thin air directly.
    //particle.position.y += random_Y * 50.0 ;
    particle.position.z = particle.position.z + random_Z * 100.0 - 20.0;

    GLfloat rcolor = 0.3 + ((rand() % 100) / 100.0f);

    particle.color = glm::vec4(rcolor, rcolor, 0.5 * rcolor, 1.0f);
    particle.life = 50.0f + (rand() % 100) / 10.0;
    particle.velocity = glm::vec3(random_X * 2, -0.1f, random_Z * 2);
}
