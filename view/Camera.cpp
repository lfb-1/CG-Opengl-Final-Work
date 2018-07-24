
/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/


#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"

using std::cout;
using std::endl;

using glm::asin;
using glm::atan;
using glm::cos;
using glm::degrees;
using glm::mat4;
using glm::max;
using glm::min;
using glm::normalize;
using glm::radians;
using glm::sin;
using glm::vec3;
using glm::vec4;

int Camera::cameraCount = 0;

Camera::Camera()
{
	ID = ++cameraCount;

	deltaTime = 0.0;
	cameraSpeed = 3.0f;

	yaw = 90.0;
	pitch = 0.0;
	roll = 0.0;
	cameraPos = vec3(0.0, 0.5, 0.0);
	cameraUp = vec3(0.0, 1.0, 0.0);
	cameraFront = getFront();
}

Camera::Camera(vec3 Pos) : Camera()
{
	cameraPos = Pos;
}

Camera::Camera(float yaw, float pitch, float roll) : Camera()
{
	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;
	cameraFront = getFront();
}

Camera::Camera(glm::vec3 Pos, float yaw, float pitch, float roll) : Camera()
{
	cameraPos = Pos;

	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;
	cameraFront = getFront();
}

// Getters
// getView() is the most important function, which yields the View matrix.
mat4 Camera::getView()
{
	return lookAt(cameraPos, cameraPos + getFront(), cameraUp);
}

vec3 Camera::getPos()
{
	return cameraPos;
}

vec3 Camera::getFront()
{
	vec3 temp;
	temp.x = cos(radians(yaw)) * cos(radians(pitch));
	temp.y = sin(radians(pitch));
	temp.z = sin(radians(yaw)) * cos(radians(pitch));

	cameraFront = normalize(temp);
	return cameraFront;
}

vec3 Camera::getUp()
{
	return cameraUp;
}

float Camera::getYaw()
{
	return yaw;
}

float Camera::getPitch()
{
	return pitch;
}

float Camera::getRoll()
{
	return roll;
}

int Camera::getID()
{
	return ID;
}

// Camera normal operations

void Camera::updateTime(float time)
{
	deltaTime = time;
}

void Camera::turnCamera(float x, float y)
{
	//CODE DEBUG:
	//cout<<"Turning by "<<x<<endl ;
	yaw += x;
	pitch += y;

	if (yaw > 360.0)
		yaw = 0.0;
	else if (yaw < 0.0)
		yaw = 360.0;

	if (pitch > 89.0)
		pitch = 89.0;
	else if (pitch < -89.0)
		pitch = -89.0;

	//CODE DEBUG
	//cout<<"yaw: "<<yaw<<endl ;

	getFront();
}

void Camera::moveCamera(glm::vec3 target)
{
	vec3 difference = target - cameraPos;
	vec3 direction = normalize(difference);

	// The position does not multiply target, but direction instead to proceed at constant speed.
	cameraPos += cameraSpeed * deltaTime * direction;
}

void Camera::liftCamera(float floating)
{
	cameraPos.y += floating;
}

// Setters

void Camera::setSpeed(float speed)
{
	cameraSpeed = speed;
}

void Camera::setPosition(glm::vec3 target)
{
	cameraPos = target;
}

void Camera::setDirection(float yaw, float pitch, float roll)
{
	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;
}

void Camera::setLookTarget(vec3 target)
{
	vec3 direction = normalize(target - cameraPos);
	pitch = degrees(asin(direction.y));
	yaw = degrees(atan(direction.z, direction.x));

	//if ( pitch < 0.0 ) pitch += 360.0 ;

	//CODE DEBUG
	//cout<<"Turned to yaw: "<<yaw<<endl ;

	if (yaw < 0.0)
		yaw += 360.0;

	cameraFront = getFront();
}
