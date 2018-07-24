
/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/


#ifndef CAMERA_H
#define CAMERA_H

/**
 * The camera class is responsible for producing and managing the View matrix in the OpenGL transformation model.
 * Specifically, the location of the camera/viewer and its direction, as well as the speed it moves.
 * 
 * The camera has two sets of movement instructions.
 * One set contains the normal ones including turn/move/lift camera that gradually places the camera at the desired position or makes the camera face the desired direction.
 * The other set includes functions for immediate changes, or setters (teleport, etc.).
 */

class Camera
{
  private:
	static int cameraCount;

	int ID;

	// Time before last action, used to calculated expected displacement.
	float deltaTime;

	// Camera attributes.
	float cameraSpeed;

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	// These three attributes decide the direction the camera is facing, or the cameraFront vector.
	float yaw;
	float pitch;
	float roll;

  public:
	Camera();
	Camera(glm::vec3 Pos);
	Camera(float yaw, float pitch, float roll);
	Camera(glm::vec3 Pos, float yaw, float pitch, float roll);

	// ========== Getters ==========
	// getView() is the most important function, which yields the View matrix.
	glm::mat4 getView();
	glm::vec3 getPos();
	glm::vec3 getFront();
	glm::vec3 getUp();
	float getYaw();
	float getPitch();
	float getRoll();
	int getID();

	/**
	 * Camera operations
	 * 
	 * updateTime: updates deltaTime, called before every movement.
	 * turnCamera: updates the camera's yaw and pitch by the x and y displacement on the screen.
	 * moveCamera: updates the camera's xyz position by moving toward target based on deltaTime.
	 * liftCamera: updates the camera's y position by adding the input to camera's y position.
	 * 
	 */

	//CODE TODO: add zooming function by changing the fieldOfView
	// void zoomCamera(float) ;

	void updateTime(float time);
	void turnCamera(float x, float y);
	void moveCamera(glm::vec3 target);
	void liftCamera(float floating);

	// ========== Setters ==========
	// We can set the camera's speed, position, facing direction by either giving the yaw, pitch, roll or a target point.
	void setSpeed(float speed);
	void setPosition(glm::vec3 target);
	void setDirection(float yaw, float pitch, float roll);
	void setLookTarget(glm::vec3 target);
};

#endif
