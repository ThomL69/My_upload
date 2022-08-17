#include "Camera.h"

Camera::Camera(int sizeX, int sizeY, float fov, float depthOrigin, float depthEnd) {
	this->field_of_view = fov;
	this->fov_target = fov;
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->fov_smooth = 4.0f;
	this->depthOrigin = std::max(0.1f,depthOrigin);
	this->depthEnd = std::max(1.0f, depthEnd);
	updateProjection();
}
void Camera::setFOV(const float fov) {
	this->fov_target = fov;
}

void Camera::update(float delta) {
	// mise à jour douce du FOV
	if (fov_target != field_of_view) {
		float rot = delta * fov_smooth;
		field_of_view += (field_of_view < fov_target) ? rot : -rot;
		if (abs(field_of_view - fov_target) <= rot + 0.01) {
			field_of_view = fov_target;
		}
		updateProjection();
	}
}

void Camera::setDepth(const float depthOrigin, const float depthEnd) {
	this->depthOrigin = depthOrigin;
	this->depthEnd = depthEnd;
	updateProjection();
}

void Camera::updateProjection() {
	projection = Perspective(field_of_view, sizeX / sizeY, depthOrigin, depthEnd);
}

float Camera::FOV() {
	return fov_target;
}

Transform Camera::T() {
	return transform;
}

void Camera::setT(Transform& t) {
	transform = t;
}

Transform Camera::proj() {
	return projection;
}