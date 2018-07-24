/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1677895 Fengbei Liu

**************************************************/

#ifndef OBJECT_H
#define OBJECT_H

#include "../../lib/tiny_obj_loader.h"

class Object
{
private:
  /**
     * basic parameter needed by tiny obj loader
     */
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  /**
     * vertex                                       # vertex points
     * normal                                       # normal vector
     * texcoord                                     # uv coord
     */
  std::vector<tinyobj::real_t> vertex;
  std::vector<tinyobj::real_t> normal;
  std::vector<tinyobj::real_t> texcoord;

  /**
     * texture                                      # texture ids
     * materialShininess                            # material shininess
     * material_diffuse                             # material diffuse property
     * material_ambient                             # material ambient property
     * material_specular                            # material specular property
     */
  std::vector<GLuint> dummyTexture;
  std::vector<GLuint> textureIDs;
  std::vector<tinyobj::real_t> materialShininess;
  std::vector<tinyobj::real_t> materialDiffuse;
  std::vector<tinyobj::real_t> materialAmbient;
  std::vector<tinyobj::real_t> materialSpecular;

  /**
     * input_file                                   # input obj file
     * mtl_folder                                   # material folder path
     */
  std::string inputFile;
  std::string inputDirectory;

  tinyobj::real_t maxX, maxY, maxZ, minX, minY, minZ;

  /**
     * model matrix per object
     */
  glm::mat4 modelMatrix;

  std::vector<GLuint> VAOs;

  /**
     * translate_distance                           # vec3 used by translate function
     * scaler                                       # vec3 used by glm::scalee function
     * rotate_point                                 # vec3 used by glm::rotate as the rotation based point
     */
  glm::vec3 translateDistance;
  glm::vec3 scaler;
  float previousDegree;
  glm::vec3 previousRotateAxis;
  //CODE TODO: set a proper variable to record rotation status. Maybe an inverse rotation matrix, or none at all.
  //glm::vec3 rotate_point;

  GLuint currentProgramID;

  /**
     * function                                     # calculate the middle point of this obj
     * input                                        # int s accept 1(x) or 2(y) or 3(z)
     * output                                       # return float value for middle point xyz
     * when is it called                            # called by reSetUp() function
     */
  float calculateMiddlePoint(int s);

  unsigned int id;
  std::string name;

public:
  /**
     * function                                     # constructor, read the input path with obj file and check for error
     * input                                        # fileName indicated input obj file name and path
     * when is it called                            # when creating Object class pointer object
     */
  Object(std::string fileName);

  Object(std::string fileName,unsigned int id, std::string name);

  

  /**
     * function                                     # read materials property
     * input                                        # void
     * output                                       # void
     * when is it called                            # after loading obj and before readShape()
     */
  void readMaterials();

  /**
     * function                                     # read shape property, bind vbo with vao and sort materials property
     * input                                        # void
     * output                                       # void
     * when is it called                            # after loading obj and readMaterials()
     */
  void readShape();

  /**
     * function                                     # multiple model_matrix with rotate effect
     * input                                        # rotate angle, rotation_axis 
     * output                                       # void
     * when is it called                            # user wants to rotate obj
     */
  void objectRotate(float angle, glm::vec3 rotation_axis);

  /**
     * function                                     # translate obj with vec3 local posiiton and accumlate translateDistance with new vec3
     * input                                        # vec3 direction, represents position compare to object itself
     * output                                       # void
     * when is it called                            # user does not know the current location of obj and resetOrigin()
     */
  void accumlateTranslate(glm::vec3 localPosition);

  /**
     * function                                     # translate obj with vec3 position and replace translateDistance with this new vec3
     * input                                        # vec3 worldPosition, represents position in world coord
     * output                                       # void
     * when is it called                            # user gives a specific location of new position in world corrd
     */
  void teleportTranslate(glm::vec3 worldPosition);
  /**
     * function                                     # scale the obj and set scaler to this new sscaler
     * input                                        # vec3 scaler
     * output                                       # void 
     * when is it called                            # user gives a specific size  and resetOrigin()
     */
  void normalScale(glm::vec3 newScaler);

  /**
     * function                                     # scale the obj and accumlate scaler with newScaler
     * input                                        # vec3 scaler
     * output                                       # void
     * when is it called                            # user does not know the specific scaler and add this scaler to the total scaler.
     */
  void accumlateScale(glm::vec3 newScaler);

  /**
     * function                                     # give the integer of draw arrays size based on vertex normal and texcoord
     * input                                        # void
     * output                                       # int this object sum of vertex, normal and texcoord
     * when is it called                            # when drawArrays been called
     */
  int drawingSize();
  /**
     * Getter and setter
  */
  glm::mat4 getModelMatrix();
  std::vector<GLuint> getVAO();
  GLuint getProgramID();

  /**
   * return the material property
   */
  std::vector<GLuint> getTexture();
  std::vector<tinyobj::real_t> getDiffuse();
  std::vector<float> getShininess();
  std::vector<tinyobj::real_t> getAmbient();
  std::vector<tinyobj::real_t> getSpecular();

  //set translate distance and scaler to desire value
  void setTranslateDistance(glm::vec3);
  void setScaler(glm::vec3);
  void setNameId(unsigned id, std::string name);

  glm::vec3 getPosition();
  glm::vec3 getScaler();
  unsigned int getId();
  std::string getName();
  //obj render, pass model matrix to shader
  void render();

  //material properties setup, read from the material file
  void generalSetup();

  //pass program id to this obj
  void updateProgramID(GLuint new_id);
};
#endif
