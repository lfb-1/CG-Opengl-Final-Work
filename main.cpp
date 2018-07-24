/*

===========================================================================
Computer Graphics 2018 Assignment 3 part 2

Animation project: Trubute to Lost Odyssey

Group members:
a1677895 Fengbei.Liu@student.adelaide.edu.au (Liu, or Thomas)
a1695329 Yong.Yang@student.adelaide.edu.au (Yang, or Maximilian)


This is coded by Maximilian.


This main program takes _____ as input parameters. It keeps running
with a 7 minute background music Eclipse of Time, by Nobuo Uematsu,
from the game Lost Odyssey, to which this project is paying tribute
to. The main characters in both the game and this project is an immortal
who lived thousands of years, and saw the loss of friends innumerable.

Comment guide:     asterisks (*) following // indicate importance ;
            the more asterisks there are, the more critical it is ;
            equal signs (=) are used to separate major sections and 
            inevitably repetitive code ; dashses (-) along with (=)
            are used to indicate section info both above and below.


            Commented out code begins with "//CODE"
            
            No multipline comments, except for this section ;
===========================================================================

References: Refer to readme.pdf
*/

#define GLFW_INCLUDE_NONE

#include <stdlib.h>
#include <iostream>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "lib/stb_image.h"
#include "lib/tiny_obj_loader.h"

#include "view/Camera.hpp"
#include "model/Object/Object.h"
#include "model/SkyBox/SkyBox.h"
#include "model/Particles/ParticleGenerator.hpp"
#include "utility/shader.hpp"
#include "utility/helpers.h"

using std::abs;
using std::cout;
using std::endl;
using std::vector;

using glm::cos;
using glm::cross;
using glm::degrees;
using glm::mat4;
using glm::max;
using glm::normalize;
using glm::perspective;
using glm::radians;
using glm::rotate;
using glm::scale;
using glm::sin;
using glm::translate;
using glm::vec2;
using glm::vec3;
using glm::vec4;

// *** ========== Settings ==========

// The default map is 10 by 10.
int map_size = 10 ;


int screen_width = 1920 ;
int screen_height = 1080 ;

float sun_speed = 0.03 ;

// Attributes of camera in keyboard control mode. They determine how fast the camera moves and turns.
float camera_agility = 0.05;
float camera_sensitivity = 0.1;

// Attributes of camera in mouse control mode.
bool firstMouse = true;
float last_mouse_x, last_mouse_y;

vec3 initial_camera_position = vec3(-9.0, 1.0, 9.0);
// Yaw is always between 0 and 360
float initial_camera_yaw = 270.0;
float initial_camera_pitch = 0.0;
float initial_camera_roll = 0.0;

// ========== Function Declarations ==========
// Called when the window is resized.
void reshape_callback(GLFWwindow *window, int x, int y);
void keypress_callback(GLFWwindow *window, int, int, int, int);
void mouse_pos_callback(GLFWwindow *window, double x_pos, double y_pos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

//calculate projection matrix based on cursor moving position;
void set_projection();
void set_model();
void set_view();

// separate render into different parts
// both will be called in the main render with their programId
void render();
void render_obj();
void render_depth();
void render_ceiling();
void render_ground();
void render_skybox();
void render_particles();
void render_camera_framebuffer();
void render_space_cowboy();

// lighting mode
void lightingSetup();

// ========== Global Variables ==========

// Whether the camera turning will be controlled by the mouse move or up/down/a/z/ keys.
float mouseTurning = true;
// Time between current frame and last frame
float delta_time = 0.0f;
// Time of last frame
float last_frame = 0.0f;
// Whether the main_camera shoud move or turn
bool should_move = false;
bool textured = false;

// Determine whether the set_view() function should return secondary camera's view.
bool secondary_mode = false;

// Camera/player actions variable
vec3 move_target = vec3(0.0, 0.0, 0.0);
vec2 turn_target = vec2(0.0, 0.0);

Camera *main_camera;
Camera *secondary_camera;

// Object instances

Object *pirate, *rock1, *rock2, *rock3, *rock4, *rock5, *tree1,*tree2,*tree3, *space_cowboy, *house;

// Skybox instance
SkyBox *environment;

ParticleGenerator *snowflake;

GLuint program_debug;
GLuint program_ground;
GLuint program_skybox;
GLuint program_shadow;
GLuint program_ceiling;
GLuint program_particle;
GLuint program_secondary_camera;
GLuint program_space_cowboy;
// Due to multiple program ids, better come up with this current program id to trace status
GLuint current_program;

// Framebuffer for secondary camera framebuffer
GLuint secondary_camera_framebuffer;
GLuint rendered_camera_texture;
GLuint depth_RBO;

// Framebuffer for shadow mapping
GLuint shadow_framebuffer;
GLuint rendered_depth_texture;
mat4 depth_MVP, depth_bias_MVP;

// VAOs and textures for sceneries.
GLuint snow_texture, ground_texture, ceiling_texture, ground_VAO, square_VAO;

// Model matrices for generating the ground by blocks
vector< vector<mat4> > block_positions ;

// ========== Callback functions ==========

int main(int argc, char **argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "Random Scene", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "failed to create glfw" << std::endl;
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keypress_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, keypress_callback);
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "glew did not init\n";
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //CODE ALTER: Mike's tentative addition to alter shadow effect.
    /*
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    */

    // Set program IDs
    program_debug = LoadShaders("shader/debug.vert", "shader/debug.frag");
    program_shadow = LoadShaders("shader/depth.vert", "shader/depth.frag");
    program_ground = LoadShaders("shader/ground.vert", "shader/ground.frag");
    program_skybox = LoadShaders("shader/skybox.vert", "shader/skybox.frag");
    program_particle = LoadShaders("shader/particle.vert", "shader/particle.frag");
    program_secondary_camera = LoadShaders("shader/secondary_camera.vert", "shader/secondary_camera.frag");
    program_space_cowboy = LoadShaders("shader/cube.vert", "shader/cube.frag");
    current_program = program_debug;

    // ========== Objects initiation ==========
    // Camera configuration
    main_camera = new Camera(initial_camera_position, initial_camera_yaw, initial_camera_pitch, initial_camera_roll);
    secondary_camera = new Camera(initial_camera_position, initial_camera_yaw, initial_camera_pitch, initial_camera_roll);

    string protagonist = "res/pirate/Pirate.obj";
    string object_file2 = "res/rock/rock.obj";
    string object_file3 = "res/deadTrees/DeadTree1.obj";
    string object_file4 = "res/boy/ok.obj";
    string object_file5 = "res/tavern/Tavern.obj";
    string object_file6 = "res/deadTrees/DeadTree21.obj";
    pirate = new Object(protagonist);

    rock1 = new Object(object_file2);
    rock2 = new Object(object_file2);
    rock3 = new Object(object_file2);
    rock4 = new Object(object_file2);
    rock5 = new Object(object_file2);

    tree1 = new Object(object_file3);
    tree2 = new Object(object_file6);
    tree3 = new Object(object_file3);

    space_cowboy = new Object(object_file4);
    house = new Object(object_file5);
    // REMINDER: the string format is IMPORTANT! Must contain a "/" at the end
    environment = new SkyBox("res/skybox/");

    snow_texture = set_texture("res/sprites/snowflake.png");
    ground_texture = set_texture("res/ground.jpg");

    snowflake = new ParticleGenerator(program_particle, snow_texture, 50000);

    ground_VAO = createGroundVAO();

    vector<mat4> temp ;
    mat4 identity = mat4() ;

    // A vector of identity matrices
    for (int i = 0; i < map_size; i++) temp.push_back( mat4() ) ;
    // A vector of vectors of identity matrices
    for (int i = 0; i < map_size; i++) block_positions.push_back(temp) ;
    // Define the nested vector with the block positions.
    for (int l = 0; l < map_size; l++) for (int i = 0; i < map_size; i++) block_positions[l][i] = translate(identity, vec3(2.0 * i - map_size/2.0 - 7.0, 0.0, 2.0 * l - map_size/2.0 - 3.0)) ;


    // ========== Prepare framebuffer for secondary camera ==========

    square_VAO = createSquareVAO();
    glGenFramebuffers(1, &secondary_camera_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, secondary_camera_framebuffer);

    glGenTextures(1, &rendered_camera_texture);
    glBindTexture(GL_TEXTURE_2D, rendered_camera_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendered_camera_texture, 0);

    //CODE ALTER: Code following opengl-tutorial, its effect is uncertain yet.
    //GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0} ;
    //glDrawBuffers(1, draw_buffers) ;

    glGenRenderbuffers(1, &depth_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "Framebuffer incomplete. Exiting." << endl;
        exit(2);
    }

    // ========== Prepare framebuffer for shadow mapping ==========

    glGenFramebuffers(1, &shadow_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_framebuffer);

    glGenTextures(1, &rendered_depth_texture);
    glBindTexture(GL_TEXTURE_2D, rendered_depth_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screen_width, screen_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rendered_depth_texture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "Framebuffer incomplete. Exiting." << endl;
        exit(2);
    }

    // Bind the default framebuffer for rendering to the screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    // ========== Read Obj1 ==========

    // pass program id to Object
    pirate->updateProgramID(current_program);

    // ========== Read Obj2 ==========

    rock1->updateProgramID(current_program);
    rock2->updateProgramID(current_program);
    rock3->updateProgramID(current_program);
    rock4->updateProgramID(current_program);
    rock5->updateProgramID(current_program);

    // ========== Read Obj3 ==========

    tree1->updateProgramID(current_program);
    tree2->updateProgramID(current_program);
    tree3->updateProgramID(current_program);

    space_cowboy->updateProgramID(program_space_cowboy);

    house->updateProgramID(current_program);

    // ========= Skybox ==============
    environment->updateProgramID(program_skybox);

    // ========== Move two obj ==========

    //bind pirate to the camera
    pirate->teleportTranslate(main_camera->getPos() - vec3(0.0, 1.0, 0.0));

    rock1->teleportTranslate(vec3(-3.0,  0.5, 7.0));
    rock2->teleportTranslate(vec3(0.0, -0.2, 9.0f));
    rock3->teleportTranslate(vec3(-3.0, 0.0, 3.0f));
    rock4->teleportTranslate(vec3(-5.0, 0.3, 5.0f));
    rock5->teleportTranslate(vec3(-3.0, 0.5, 2.0f));

    tree1->teleportTranslate(vec3(-11.0f,0.0,-3.0f));
    tree1->accumlateScale(vec3(3.0));
    tree2->teleportTranslate(vec3(-9.0,0.0,1.0));
    tree2->accumlateScale(vec3(3.0));
    tree3->teleportTranslate(vec3(-11.0,0.0,1.0));
    tree3->accumlateScale(vec3(3.0));
    tree3->objectRotate(30.0f,vec3(0.0f,0.0f,1.0f));

    house->accumlateTranslate(vec3(-3.0, 2.0, -2.0));
    //house->objectRotate(-90.0f,vec3(0.0f,1.0f,0.0f));
    house->accumlateScale(vec3(5.0));

    space_cowboy->accumlateTranslate(vec3(6.0, 1.0, 3.0));
    space_cowboy->objectRotate(-90.0f,vec3(0.0f,1.0f,0.0f));
    space_cowboy->accumlateScale(vec3(2.0));

    secondary_camera->setLookTarget(space_cowboy->getPosition());

    //CODE ALTER: BGM
    system("aplay res/Chrono_Cross_OST-On_the_Beach_of_Dreams.wav &");


    while (!glfwWindowShouldClose(window))
    {
        // Update time frame (delta_time)
        float current_time = glfwGetTime();
        delta_time = current_time - last_frame;
        last_frame = current_time;

        //CODE ALTER: ---B--- enable along with ---A--- for particle effect.
        snowflake->Update(delta_time, rock1, 2, vec3(0.7f, 0.0f, 0.7f));


        // MVP matrix for shadow mapping, from directional sun light.
        vec3 lightInvDir = vec3(-5.0f * sin(current_time * sun_speed + 1.5), 4.0f, 2.0f);
        mat4 depth_projection_matrix = glm::ortho<float>(-20, 20, -20, 20, -20, 30);
        mat4 depth_view_matrix = lookAt(lightInvDir, vec3(0, 0, 0), vec3(0, 1, 0));
        mat4 depth_model_matrix = mat4(1.0);

        depth_MVP = depth_projection_matrix * depth_view_matrix * depth_model_matrix;
        mat4 bias_matrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0);

        depth_bias_MVP = bias_matrix * depth_MVP;

        float float_around1 = sin(current_time * 0.2) ;
        float float_around2 = cos(current_time * 0.2) ;
        float float_around3 = sin(current_time * 0.5) ;
        float float_around4 = cos(current_time * 0.5) ;
        float float_around5 = sin(current_time * 0.5 + 20) ;
        float float_around6 = cos(current_time * 0.5 + 20) ;

        rock1->objectRotate(current_time * 30, vec3(0.0f, 1.0f, 0.0f));
        rock2->teleportTranslate(vec3(-3.0 - float_around1,  0.7 - float_around1, 7.0 - float_around2));
        rock3->teleportTranslate(vec3(-3.0 + float_around1 * 1.5,  0.5, 7.0 + float_around2 * 1.5));
        rock4->teleportTranslate(vec3(-3.0 + float_around3 * 2.5,  0.5 - float_around1, 7.0 + float_around4 * 2.5));
        rock5->teleportTranslate(vec3(-3.0 - float_around5 * 3,  0.5 + float_around1, 7.0 - float_around6 * 3));

        // When the main camera hasn't finished a moving command.
        if (should_move)
        { // When the camera is very close to destination, set (teleport) it to the destination exactly to avoid inaccurate positioning caused by lag or other misfortuante events.
            if (abs(main_camera->getPos().x - move_target.x) < 0.05 && abs(main_camera->getPos().z - move_target.z) < 0.05)
            {
                main_camera->setPosition(move_target);
                secondary_camera->setPosition(move_target);
                should_move = false;
            }
            else
            {
                main_camera->updateTime(delta_time);
                main_camera->moveCamera(move_target);

                secondary_camera->updateTime(delta_time);
                secondary_camera->moveCamera(move_target);
            }

            // The protagonist moves with the camera
            pirate->teleportTranslate(main_camera->getPos() - vec3(0.0, 1.0, 0.0));
            // The secondary camera always looks at the target.
            secondary_camera->setLookTarget(space_cowboy->getPosition());
        }

        // ***** main render
        render();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    system("kill `pidof aplay`");
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void keypress_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 1st Key for debugging functions.
    // Maximilian: currently used to test camera setLookTarget function.
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        vec3 target = house->getPosition() ;
        main_camera->setLookTarget(target);
    }

    // 2nd Key for debugging functions.
    // Maximilian: currently used to test camera setLookTarget function.
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        vec3 target = space_cowboy->getPosition() ;
        main_camera->setLookTarget(target);
    }

    // The camera turning can be controlled by either the keyboard or the mouse
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (mouseTurning == false)
        {
            firstMouse = true;
            mouseTurning = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            mouseTurning = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    // When the main camera is still moving, don't accept move or turn commands
    if (should_move)
        return;

    // Camera behaviours
    vec3 current_pos = main_camera->getPos();
    vec3 target = current_pos;

    // Moving forward or backward, as required by assignment 2, no left or right move.
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        
        vec3 camera_front = main_camera->getFront();
        vec3 camera_up = main_camera->getUp();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            target += camera_agility * camera_front;
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            target -= camera_agility * camera_front;
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            target -= camera_agility * normalize(cross(camera_front, camera_up));
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            target += camera_agility * normalize(cross(camera_front, camera_up));

        //CODE DEBUG:
        //cout << key << " key pressed. Moving to " << target.x << ", " << target.y << ", " << target.z << endl;

        //CODE ALTER: --- C --- The next line is restricting movement along Y axiss. We are currently restricting the user to move in the xz plane.
        target.y = 1.0;
        move_target = target;
        should_move = true;
    }

    // Enabling/Disabling texture as required in assignment 2.
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        if (textured == 0)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            textured = 1;
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            textured = 0;
        }

        /**
         * CODETODO: everytime transform uniform value need to ensure current_program status
         * use current program
         * do your transforming
         * I might need to pack these steps into separate functions 
         */
        current_program = program_debug;
        glUseProgram(current_program);
        glUniform1i(glGetUniformLocation(current_program, "flag"), textured);
    }
}

void set_projection()
{
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screen_width / screen_height, 0.1f, 100.0f);
    unsigned int projectionLoca = glGetUniformLocation(current_program, "projection");
    glUniformMatrix4fv(projectionLoca, 1, GL_FALSE, glm::value_ptr(projection));
}

void set_view()
{

    glm::mat4 view;

    if (secondary_mode == false)
        view = main_camera->getView();
    else
        view = secondary_camera->getView();

    // make sure the user will not move out of the skybox
    // but also create a fade in fade away effect..
    if (current_program == program_skybox)
    {
        view = glm::mat4(glm::mat3(view));
    }

    unsigned int viewLoca = glGetUniformLocation(current_program, "view");
    glUniformMatrix4fv(viewLoca, 1, false, glm::value_ptr(view));
}

void render()
{
    // ===== ** Render depth map =====

    glBindFramebuffer(GL_FRAMEBUFFER, shadow_framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, screen_width, screen_height);
    current_program = program_shadow;

    // To avoid peter panning
    //glCullFace(GL_FRONT);
    render_depth();
    //glCullFace(GL_BACK);

    // ===== ** Render secondary camera content =====

    secondary_mode = true;

    glBindFramebuffer(GL_FRAMEBUFFER, secondary_camera_framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, screen_width, screen_height);

    current_program = program_skybox;
    render_skybox();

    current_program = program_ground;
    render_ground();

    current_program = program_debug;
    render_obj();

    current_program = program_space_cowboy;
    render_space_cowboy();

    //CODE ALTER: ---A--- enable along with ---B--- for particle effect.
    current_program = program_particle;
    render_particles();

    // ===== ** Render main camera content =====

    secondary_mode = false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, screen_width, screen_height);

    current_program = program_ground;
    render_ground();

    current_program = program_debug;
    render_obj();

    //CODE ALTER: ---A--- enable along with ---B--- for particle effect.
    current_program = program_particle;
    render_particles();

    current_program = program_skybox;
    render_skybox();

    current_program = program_space_cowboy;
    render_space_cowboy();

    // ** Render the second camera's content over main camera content
    current_program = program_secondary_camera;
    glViewport(0, 0, screen_width / 3, screen_height / 3);
    render_camera_framebuffer();
}

void render_space_cowboy()
{
    glUseProgram(current_program);
    set_projection();
    set_view();
    glUniform3f(glGetUniformLocation(current_program, "cameraPos"), main_camera->getPos().x, main_camera->getPos().y, main_camera->getPos().z);
    glUniform1f(glGetUniformLocation(current_program,"Fogstart"),0.0f);
    glUniform1f(glGetUniformLocation(current_program,"FogEnd"),10.0f);
    glUniform4f(glGetUniformLocation(current_program,"fogColor"),0.2f,0.2f,0.2f,0.2f);

    space_cowboy->render() ;
    
}

//obj render
void render_obj()
{
    glUseProgram(current_program);

    //use Object render to set model;

    //well, for now manully set view and projection
    set_projection();
    set_view();
    lightingSetup();

    // For shadow mapping
    glUniform1i(glGetUniformLocation(current_program, "shadowMap"), 3);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, rendered_depth_texture);

    glUniformMatrix4fv(glGetUniformLocation(current_program, "depth_bias_MVP"), 1, GL_FALSE, &depth_bias_MVP[0][0]);


    pirate->render();
    rock1->render();
    rock2->render();
    rock3->render();
    rock4->render();
    rock5->render();
    tree1->render();
    tree2->render();
    tree3->render();
    house->render();
}

void render_depth()
{
    // ===== Render ground =====
    glUseProgram(program_shadow);
    mat4 Identity = mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(program_shadow, "depth_MVP"), 1, GL_FALSE, glm::value_ptr(depth_MVP));
    glBindVertexArray(ground_VAO);

    for ( int l = 0; l < map_size; l++ ) for ( int i = 0; i < map_size; i++ )
    {
        glUniformMatrix4fv(glGetUniformLocation(program_shadow, "model"), 1, GL_FALSE, &block_positions[l][i][0][0]) ;
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // ===== Render objects =====
    pirate->updateProgramID(current_program);
    
    rock1->updateProgramID(current_program);
    rock2->updateProgramID(current_program);
    rock3->updateProgramID(current_program);
    rock4->updateProgramID(current_program);
    rock5->updateProgramID(current_program);

    tree1->updateProgramID(current_program);
    tree2->updateProgramID(current_program);
    tree3->updateProgramID(current_program);

    house->updateProgramID(current_program);

    pirate->render();
    rock1->render();
    rock2->render();
    rock3->render();
    rock4->render();
    rock5->render();
    tree1->render();
    tree2->render();
    tree3->render();
    house->render();

    pirate->updateProgramID(program_debug);
    rock1->updateProgramID(program_debug);
    rock2->updateProgramID(program_debug);
    rock3->updateProgramID(program_debug);
    rock4->updateProgramID(program_debug);
    rock5->updateProgramID(program_debug);

    tree1->updateProgramID(program_debug);
    tree2->updateProgramID(program_debug);
    tree3->updateProgramID(program_debug);
    house->updateProgramID(program_debug);
}

void render_ground()
{
    glUseProgram(current_program);

    set_projection();
    set_view();

    glBindVertexArray(ground_VAO);
    
    lightingSetup() ;
    
    glUniform1i(glGetUniformLocation(current_program, "texMap"), 0);
    glUniform1i(glGetUniformLocation(current_program, "shadowMap"), 3);
    glUniformMatrix4fv(glGetUniformLocation(current_program, "depth_bias_MVP"), 1, GL_FALSE, &depth_bias_MVP[0][0]);

    glUniform3f(glGetUniformLocation(current_program, "camPos"), main_camera->getPos().x, main_camera->getPos().y, main_camera->getPos().z);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ground_texture);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, rendered_depth_texture);

    for ( int l = 0; l < map_size; l++ ) for ( int i = 0; i < map_size; i++ )
    {
        glUniformMatrix4fv(glGetUniformLocation(current_program, "model"), 1, GL_FALSE, &block_positions[l][i][0][0]) ;
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }


    glActiveTexture(GL_TEXTURE0);
}

// sky box render
void render_skybox()
{
    glUseProgram(current_program);

    set_projection();
    set_view();

    environment->render();
}

void render_particles()
{
    glUseProgram(current_program);

    set_projection();
    set_view();

    snowflake->Draw();
}

void render_camera_framebuffer()
{
    glUseProgram(current_program);

    glBindVertexArray(square_VAO);
    glBindTexture(GL_TEXTURE_2D, rendered_camera_texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void reshape_callback(GLFWwindow *window, int x, int y)
{
    screen_width = x;
    screen_height = y;
    set_projection();
    glViewport(0, 0, screen_width, screen_height);
}

void mouse_pos_callback(GLFWwindow *window, double x_pos, double y_pos)
{
    // Do nothing if the camera is expected to be controlled by keyboard instead of mouse.
    if (mouseTurning == false)
        return;

    if (firstMouse)
    {
        last_mouse_x = x_pos;
        last_mouse_y = y_pos;
        firstMouse = false;
    }

    float xoffset = x_pos - last_mouse_x;
    float yoffset = last_mouse_y - y_pos;
    last_mouse_x = x_pos;
    last_mouse_y = y_pos;

    xoffset *= camera_sensitivity;
    yoffset *= camera_sensitivity;

    main_camera->turnCamera(xoffset, yoffset);

    // The protagonist always faces the camera's direction.
    //cout<<"Current Yaw: "<< main_camera->getYaw()<< endl ;

    pirate->objectRotate(-main_camera->getYaw() + 90, vec3(0.0, 1.0, 0.0));
}

void lightingSetup()
{
    //0.0f, 0.0f, 0.0f
    //direction
    glUniform3f(glGetUniformLocation(current_program, "dirLight.direction"), 5.0 * sin(glfwGetTime() * sun_speed + 1.5), -4.0, -2.0);
    glUniform3f(glGetUniformLocation(current_program, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(current_program, "dirLight.diffuse"), 1.1f, 1.1f, 1.1f);
    glUniform3f(glGetUniformLocation(current_program, "dirLight.specular"), 0.9f, 0.9f, 0.9f);

    //point
    glUniform3fv(glGetUniformLocation(current_program, "pointLight.position"), 1, &main_camera->getPos()[0]);
    glUniform3f(glGetUniformLocation(current_program, "pointLight.ambient"), 0.0f, 0.0f, 0.0f);
    glUniform3f(glGetUniformLocation(current_program, "pointLight.diffuse"), 0.1f, 0.1f, 0.1f);
    glUniform3f(glGetUniformLocation(current_program, "pointLight.specular"), 0.0f, 0.0f, 0.0f);

    glUniform1f(glGetUniformLocation(current_program, "pointLight.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(current_program, "pointLight.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(current_program, "pointLight.quadratic"), 0.032f);

    //spot
    glUniform3fv(glGetUniformLocation(current_program, "spotLight.position"), 1, &main_camera->getPos()[0]);

    //DEBUG: something wrong with camera front. disable other light and only use spot light will see the flashlight front is unstable..
    glUniform3fv(glGetUniformLocation(current_program, "spotLight.direction"), 1, &main_camera->getFront()[0]);
    glUniform3f(glGetUniformLocation(current_program, "spotLight.ambient"), 0.01f, 0.01f, 0.01f);
    glUniform3f(glGetUniformLocation(current_program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(current_program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);

    glUniform1f(glGetUniformLocation(current_program, "spotLight.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(current_program, "spotLight.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(current_program, "spotLight.quadratic"), 0.075f);

    glUniform1f(glGetUniformLocation(current_program, "spotLight.cutOff"), glm::cos(glm::radians(7.0f)));
    glUniform1f(glGetUniformLocation(current_program, "spotLight.outerCutOff"), glm::cos(glm::radians(10.5f)));
}
