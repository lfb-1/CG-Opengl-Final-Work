/*********************************************************
 Version 1.1
 
 Code provided by Michael Hemsley, Anthony Dick and Gustavo Carneiro
 for the course 
 COMP SCI 3014/7090 Computer Graphics
 School of Computer Science
 University of Adelaide
 
 Permission is granted for anyone to copy, use, modify, or distribute this
 program and accompanying programs and documents for any purpose, provided
 this copyright notice is retained and prominently displayed, along with
 a note saying that the original programs are available from the aforementioned 
 course web page.
 
 The programs and documents are distributed without any warranty, express or
 implied.  As the programs were written for research purposes only, they have
 not been tested to the degree that would be advisable in any important
 application.  All use of these programs is entirely at the user's own risk.
 *********************************************************/

/**
 * A method for rendering with lights using shaders.
 * Includes a way to send light parameters
 * to the shader program and a Phong style light rendering.
 *
 * Note: There is no single way to do lighting with GLSL, but Phong lighting
 * is a classic model and a good starting point.
 *
 * This program uses a normal matrix, which is the matrix that
 * transforms vertex normals into eye coordinate space.
 * The normal matrix is usually equal to the upper left 3x3 sub-matrix of the
 * modelview matrix. However, if we were to scale our modelview matrix
 * non-uniformly (say, only on the y-axis) then this shortcut no longer works.
 *
 * Includes  two sets of shaders, one for per vertex lighting
 * and the other for per fragment.
 *
 */

#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Sphere.hpp"
#include "Shader.hpp"

#define TICK_MILLS	0.01	// Time between 'ticks'

// The amount to animate each tick
#define BALL_BOUNCING_AMOUNT 0.3f
#define BALL_BOUNCING_LIMIT  3.0f



#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define LIGHT_SPEED 0.2f

// Window width and height
unsigned int winW = 640;
unsigned int winH = 480;

// This program makes use of the Sphere class, that constructs a sphere mesh.
Sphere *sphere;
unsigned int sphereVaoHandle;

// Our shader program
unsigned int ProgramID;

// Set of translations updated each tick
float ballTranslation = 0.0f;
float ballBouncingUp = true;


float lightX = 0.0f;
float lightY = 5.0f;
float lightZ = -2.0f;

/**
 * Creates a new vertex array object and loads sphere data onto GPU.
 */
int createSphereVAO()
{
    if (!sphere) {
        fprintf(stderr, "Error: initalise sphere before creating VAO!\n");
    }
    
	glGenVertexArrays(1, &sphereVaoHandle);
	glBindVertexArray(sphereVaoHandle);

	int vertLoc = glGetAttribLocation(ProgramID, "a_vertex");
	int normLoc = glGetAttribLocation(ProgramID, "a_normal");

	unsigned int buffer[3];
	glGenBuffers(3, buffer);

	// Set vertex position attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->vertCount,
                 sphere->vertices,
                 GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);
        
	// Normal attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * sphere->normCount,
                 sphere->normals,
                 GL_STATIC_DRAW);
	glEnableVertexAttribArray(normLoc);
	glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

	// Vertex indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * sphere->indCount,
                 sphere->indices,
                 GL_STATIC_DRAW);

	return 0; // success
}


/**
 * Sets the shader uniforms and generic attributes
 * @param program, Shader program object to use
 * @return success on 0, failure otherwise
 */
int setupShader(unsigned int id)
{
    glUseProgram(id);
    
	// Perspective projection matrix
    glm::mat4 projection;
    projection = glm::perspective(45.0, double(winW) / double(winH), 1.0, 30.0); 
    
	// Load it to the shader program
	int projHandle = glGetUniformLocation(ProgramID, "projection_matrix");
	if (projHandle == -1) {
        fprintf(stderr, "Error updating proj matrix\n");
        return 1;
    }
    glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
    
	// Using the sphere class to generate vertices and element indices
	sphere = new Sphere(1.0f, 16, 16);
	createSphereVAO();   

    // Uniform lighting variables
    int lightambientHandle = glGetUniformLocation(ProgramID, "light_ambient");
	int lightdiffuseHandle = glGetUniformLocation(ProgramID, "light_diffuse");
	int lightspecularHandle = glGetUniformLocation(ProgramID, "light_specular");
	if ( lightambientHandle == -1 ||
         lightdiffuseHandle == -1 ||
         lightspecularHandle == -1) {
        fprintf(stderr, "Error: can't find light uniform variables\n");
		return 1;
    }
    
	float lightambient[3] = { 0.12f, 0.1f, 0.1f };	// ambient light components
    float lightdiffuse[3] = { 0.0f, 1.0f, 0.0f };	// diffuse light components
    float lightspecular[3] = { 1.0f, 1.0f, 1.0f };	// specular light components

	glUniform3fv(lightambientHandle, 1, lightambient);
	glUniform3fv(lightdiffuseHandle, 1, lightdiffuse);
	glUniform3fv(lightspecularHandle, 1, lightspecular);    
    
	return 0;	// return success
}


/**
 * Renders a frame 
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ProgramID);

	int mvHandle = glGetUniformLocation(ProgramID, "modelview_matrix");
	int normHandle = glGetUniformLocation(ProgramID, "normal_matrix");
	int lightposHandle = glGetUniformLocation(ProgramID, "light_pos");
	if (mvHandle == -1 || normHandle==-1 || lightposHandle == -1) {
        fprintf(stderr, "Error: can't find matrix uniforms\n");
		exit(1);
    }
    
	// Update the light position
	float lightPos[4] = { lightX, lightY, lightZ, 1.0f };	
    glUniform4fv(lightposHandle, 1, lightPos); 

    // Uniform variables defining material colours
    // These can be changed for each sphere, to compare effects
    int mtlambientHandle = glGetUniformLocation(ProgramID, "mtl_ambient");
	int mtldiffuseHandle = glGetUniformLocation(ProgramID, "mtl_diffuse");
	int mtlspecularHandle = glGetUniformLocation(ProgramID, "mtl_specular");
	if ( mtlambientHandle == -1 ||
         mtldiffuseHandle == -1 ||
         mtlspecularHandle == -1) {
        fprintf(stderr, "Error: can't find material uniform variables\n");
		exit(1);
    }
    
	float mtlambient[3] = { 0.12f, 0.1f, 0.1f };	// ambient material
    float mtldiffuse[3] = { 0.0f, 1.0f, 0.0f };	// diffuse material
    float mtlspecular[3] = { 1.0f, 1.0f, 1.0f };	// specular material

	glUniform3fv(mtlambientHandle, 1, mtlambient);
	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
	glUniform3fv(mtlspecularHandle, 1, mtlspecular);    

    
    glm::mat4 mvMatrix;
    glm::mat3 normMatrix;

	// Set VAO to the sphere mesh
	glBindVertexArray(sphereVaoHandle);

	// Render three spheres in a row
	// We compute the normal matrix from the current modelview matrix
    // and give it to our program
    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, ballTranslation, -10.0f));
    normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));
	glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );	// Middle
	glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
	glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	
	glFlush();

}


void key_callback(GLFWwindow* window,
                  int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS &&  key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    switch(key) {
        case GLFW_KEY_LEFT:
            lightX -= LIGHT_SPEED;
            break;
        case GLFW_KEY_RIGHT:
            lightX += LIGHT_SPEED;
            break;
        case GLFW_KEY_UP:
            lightY += LIGHT_SPEED;
            break;
        case GLFW_KEY_DOWN:
            lightY -= LIGHT_SPEED;
            break;
        default:
            break;
    }
}




/**
 * This timer tick function is registered to be called back via glutTimerFunc.
 * It lets us to control how often the render function is called, allowing us to do animations
 */
void timer(void) {
    
	// We update the orbits of our planet models
    if ((ballTranslation < BALL_BOUNCING_LIMIT) && (ballBouncingUp))
        ballTranslation += BALL_BOUNCING_AMOUNT;
    else if ((ballTranslation > -BALL_BOUNCING_LIMIT) && (!ballBouncingUp))
        ballTranslation -= BALL_BOUNCING_AMOUNT;
    
    if (ballTranslation >= BALL_BOUNCING_LIMIT)
    {
        ballBouncingUp = false;
    }

    if (ballTranslation <= -BALL_BOUNCING_LIMIT)
    {
        ballBouncingUp = true;
    }
}

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}


/**
 * Program entry. Sets up OpenGL state, GLSL Shaders and GLUT window and function call backs
 */
int main(int argc, char **argv) {

	std::cout << "Controls: arrow keys - light movement.\n";

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        exit(1);
    }
    
    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create the window and OpenGL context
    window = glfwCreateWindow(winW, winH, "Bouncing Ball", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(1);
    }

	// GL state
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

    // pv-light is per-vertex lighting. pf-light is per fragment
    //ProgramID = LoadShaders("pv-light.vert", "pv-light.frag");
        ProgramID = LoadShaders("pf-light.vert", "pf-light.frag");
    

	if (ProgramID == 0) {
        fprintf(stderr, "Can't compile shaders!\n");
		return 1;
    }
    
    if (setupShader(ProgramID) !=0 ) {
        fprintf(stderr, "Can't initialise shaders!\n");
        exit(1);
    }

	glUseProgram(ProgramID);

    glfwSetKeyCallback(window, key_callback);

    // set timer to zero
    glfwSetTime(0.0);

    

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetTime() > TICK_MILLS){
            timer();
            glfwSetTime(0.0);
        }
        
        render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    
    
	return 0;
}
