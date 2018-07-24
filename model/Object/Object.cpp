/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1677895 Fengbei Liu

**************************************************/

#include <vector>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Object.h"

#include "../../lib/tiny_obj_loader.h"
#include "../../utility/shader.hpp"
#include "../../utility/helpers.h"

Object::Object(std::string inputFile)
{
    modelMatrix = glm::mat4();

    this->inputFile = inputFile;
    inputDirectory = get_directory(inputFile) + "/";

    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputFile.c_str(), inputDirectory.c_str());
    if (!err.empty())
    {
        std::cerr << err << std::endl;
    }
    if (!ret)
    {
        std::cerr << "error in tiny obj loader" << std::endl;
        exit(1);
    }

    readMaterials();
    readShape();

    //first matrix scale obj into (0,0,0)-(1,0,0) or (-1,0,0)- (0,0,0)
    //secondd matrix translate obj to origin point
    glm::mat4 first, second;
    float largest = calculate_largest(fabs(maxX - minX), fabs(maxY - minY), fabs(maxZ - minZ));
    first = glm::scale(first, glm::vec3(1 / largest, 1 / largest, 1 / largest));
    second = glm::translate(second, glm::vec3(-calculateMiddlePoint(1), -calculateMiddlePoint(2), -calculateMiddlePoint(3)));
    modelMatrix = first * second;

    setTranslateDistance(glm::vec3(0.0, 0.0, 0.0));
    setScaler(glm::vec3(1.0, 1.0, 1.0));

    previousDegree = 0.0;
    previousRotateAxis = glm::vec3(1.0);
}

Object::Object(std::string filename, unsigned int id, std::string name) : Object(filename)
{
    this->id = id;
    this->name = name;
}

void Object::readMaterials()
{
    for (int i = 0; i < materials.size(); i++)
    {
        std::string tmp = materials.at(i).diffuse_texname;

        tmp = inputDirectory + tmp;
        dummyTexture.push_back(set_texture(tmp));
    }
}

void Object::readShape()
{
    //flag_cal for first time init max min for xyz
    int flag_cal = 1;
    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        vertex.clear();
        normal.clear();
        texcoord.clear();
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            int fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++)
            {

                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                tinyobj::real_t x = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t y = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t z = attrib.vertices[3 * idx.vertex_index + 2];

                if (flag_cal == 1)
                {
                    maxX = x;
                    minX = x;
                    maxY = y;
                    minY = y;
                    maxZ = z;
                    minZ = z;
                    flag_cal = 0;
                }
                else
                {
                    // if x > max then maxX = x
                    // else if x < minX then minX = x
                    x > maxX ? maxX = x : (x < minX ? minX = x : 1);

                    y > maxY ? maxY = y : (y < minY ? minY = y : 1);

                    z > maxZ ? maxZ = z : (z < minZ ? minZ = z : 1);
                }

                vertex.push_back(x);
                vertex.push_back(y);
                vertex.push_back(z);

                if (attrib.normals.size() != 0)
                {
                    normal.push_back(attrib.normals[3 * idx.normal_index + 0]);
                    normal.push_back(attrib.normals[3 * idx.normal_index + 1]);
                    normal.push_back(attrib.normals[3 * idx.normal_index + 2]);
                }
                else
                {
                    normal.push_back(0);
                    normal.push_back(0);
                    normal.push_back(0);
                }
                if (attrib.texcoords.size() != 0)
                {
                    texcoord.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    texcoord.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
            }
            index_offset += fv;
        }
        VAOs.push_back(0);
        glGenVertexArrays(1, &VAOs[s]);
        glBindVertexArray(VAOs[s]);
        GLuint vbo, vboNormal, vboTex;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &vboNormal);
        glGenBuffers(1, &vboTex);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(VAOs[s]);
        glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
        glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(float), &normal[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(VAOs[s]);
        glBindBuffer(GL_ARRAY_BUFFER, vboTex);
        glBufferData(GL_ARRAY_BUFFER, texcoord.size() * sizeof(float), &texcoord[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);

        //DEBUG: uncertain of handling texture error
        //perhaps come up with a method without using this dummyTexture
        // if (dummyTexture.at(shapes[s].mesh.material_ids[0]) != -1)
        // {

        textureIDs.push_back(dummyTexture.at(shapes[s].mesh.material_ids[0]));

        materialDiffuse.push_back(materials.at(shapes[s].mesh.material_ids[0]).diffuse[0]);
        materialDiffuse.push_back(materials.at(shapes[s].mesh.material_ids[0]).diffuse[1]);
        materialDiffuse.push_back(materials.at(shapes[s].mesh.material_ids[0]).diffuse[2]);

        materialAmbient.push_back(materials.at(shapes[s].mesh.material_ids[0]).ambient[0]);
        materialAmbient.push_back(materials.at(shapes[s].mesh.material_ids[0]).ambient[1]);
        materialAmbient.push_back(materials.at(shapes[s].mesh.material_ids[0]).ambient[2]);

        materialSpecular.push_back(materials.at(shapes[s].mesh.material_ids[0]).specular[0]);
        materialSpecular.push_back(materials.at(shapes[s].mesh.material_ids[0]).specular[1]);
        materialSpecular.push_back(materials.at(shapes[s].mesh.material_ids[0]).specular[2]);

        materialShininess.push_back(materials.at(shapes[s].mesh.material_ids[0]).shininess);
        // }
    }
}

void Object::objectRotate(float angle, glm::vec3 rotation_axis)
{

    //CODE DEBUG: Hardcoding for testing
    /*
    float degree = 0.0 ;
    glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0) ;

    glm::vec3 temp = translateDistance;
    //CODE DEBUG:std::cout << "current rotation point is " << rotation_axis.x << "," << rotation_axis.y << "," << rotation_axis.z << std::endl;

    glm::mat4 first, second, third, fourth;
    first = glm::translate(first, -temp);
    second = glm::rotate(second, glm::radians(-degree), axis);
    //third = glm::rotate(third, glm::radians(degree), axis);
    fourth = glm::translate(fourth, temp);


    glm::mat4 tmp = fourth * second * first;
    modelMatrix = tmp * modelMatrix;
*/

    glm::vec3 temp = translateDistance;
    //CODE DEBUG:std::cout << "current rotation point is " << rotation_axis.x << "," << rotation_axis.y << "," << rotation_axis.z << std::endl;

    glm::mat4 first, second, third, fourth;
    first = glm::translate(first, -temp);
    second = glm::rotate(second, glm::radians(-previousDegree), previousRotateAxis);
    third = glm::rotate(third, glm::radians(angle), rotation_axis);
    fourth = glm::translate(fourth, temp);

    previousDegree = angle;
    previousRotateAxis = rotation_axis;

    glm::mat4 tmp = fourth * third * second * first;
    modelMatrix = tmp * modelMatrix;
}

void Object::accumlateTranslate(glm::vec3 localPosition)
{
    glm::mat4 temp;
    temp = glm::translate(temp, localPosition);
    modelMatrix = temp * modelMatrix;
    //CODE DEBUG: std::cout << "current scaler is " << scaler.x << "," << scaler.y << "," << scaler.z << std::endl;
    translateDistance += localPosition;
}

void Object::teleportTranslate(glm::vec3 worldPosition)
{
    //return to origin
    glm::mat4 first, second;
    first = glm::translate(first, glm::vec3(-translateDistance));
    second = glm::translate(second, glm::vec3(worldPosition));
    //CODE DEBUG: std::cout << "current translate distance is " << translateDistance.x << "," << translateDistance.y << "," << translateDistance.z << std::endl;
    modelMatrix = second * first * modelMatrix;
    //after translation, new translaeDistance will be the new worldPosition
    translateDistance = worldPosition;
}

void Object::normalScale(glm::vec3 newScaler)
{
    glm::vec3 temp = translateDistance;
    //CODE DEBUG:std::cout << "current rotation point is " << rotation_axis.x << "," << rotation_axis.y << "," << rotation_axis.z << std::endl;

    glm::mat4 first, second, third;
    first = glm::translate(first, -temp);
    second = glm::scale(second, newScaler);
    third = glm::translate(third, temp);

    glm::mat4 tmp = third * second * first;
    modelMatrix = tmp * modelMatrix;
    //normal scale will change current scaler to the new scaler
    scaler = newScaler;
}

void Object::accumlateScale(glm::vec3 newScaler)
{
    scaler += newScaler;
    glm::vec3 temp = translateDistance;
    //CODE DEBUG:
    //std::cout << "current rotation point is " << rotation_axis.x << "," << rotation_axis.y << "," << rotation_axis.z << std::endl;

    glm::mat4 first, second, third;
    first = glm::translate(first, -temp);
    second = glm::scale(second, scaler);
    third = glm::translate(third, temp);

    glm::mat4 tmp = third * second * first;
    modelMatrix = tmp * modelMatrix;
}

int Object::drawingSize()
{
    return vertex.size();
}

float Object::calculateMiddlePoint(int s)
{
    switch (s)
    {
    case 1:
        return maxX - (maxX - minX) / 2.0f;
    case 2:
        return maxY - (maxY - minY) / 2.0f;
    case 3:
        return maxZ - (maxZ - minZ) / 2.0f;
    default:
        std::cerr << "unknown option\n";
    }
}

glm::mat4 Object::getModelMatrix()
{
    return modelMatrix;
}

std::vector<GLuint> Object::getVAO()
{
    return VAOs;
}

void Object::setTranslateDistance(glm::vec3 translateDistance)
{
    this->translateDistance = translateDistance;
}

glm::vec3 Object::getPosition()
{
    return translateDistance;
}

void Object::setScaler(glm::vec3 scaler)
{
    this->scaler = scaler;
}

glm::vec3 Object::getScaler()
{
    return scaler;
}

unsigned int Object::getId()
{
    return id;
}

std::string Object::getName()
{
    return name;
}

std::vector<GLuint> Object::getTexture()
{
    return textureIDs;
}

void Object::render()
{
    unsigned int modelLoca = glGetUniformLocation(currentProgramID, "model_matrix");
    glUniformMatrix4fv(modelLoca, 1, false, glm::value_ptr(modelMatrix));

    for (int i = 0; i < VAOs.size(); i++)
    {
        glBindVertexArray(VAOs[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        generalSetup();

        glDrawArrays(GL_TRIANGLES, 0, drawingSize());
    }
}

void Object::updateProgramID(GLuint newID)
{
    currentProgramID = newID;
}

std::vector<tinyobj::real_t> Object::getDiffuse()
{
    return materialDiffuse;
}

std::vector<tinyobj::real_t> Object::getAmbient()
{
    return materialAmbient;
}

std::vector<tinyobj::real_t> Object::getSpecular()
{
    return materialSpecular;
}

std::vector<float> Object::getShininess()
{
    return materialShininess;
}

void Object::generalSetup()
{
    glUniform1f(glGetUniformLocation(currentProgramID, "material.shininess"), getShininess()[0]);

    //CODE ALTER: Maximilian: It seems the program can work without these two lines.
    glUniform1i(glGetUniformLocation(currentProgramID, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(currentProgramID, "material.specular"), 1);

    glUniform3f(glGetUniformLocation(currentProgramID, "material.diffuse_property"), getDiffuse()[0], getDiffuse()[1], getDiffuse()[2]);
    glUniform3f(glGetUniformLocation(currentProgramID, "material.ambient_property"), getAmbient()[0], getAmbient()[1], getAmbient()[2]);
    glUniform3f(glGetUniformLocation(currentProgramID, "material.specular_property"), getSpecular()[0], getSpecular()[1], getSpecular()[2]);
}
