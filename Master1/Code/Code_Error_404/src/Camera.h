#pragma once
#include <src/gKit/mat.h>

/**
* Englobe les données nécessaires à l'affichage
*/
class Camera {
public:
	Camera(int sizeX, int sizeY, float fov, float depthOrigin, float depthEnd);

	/*Permet la mise à jour "douce" du fov*/
	void update(const float delta);

	void setFOV(const float fov);
	void setDepth(const float depthOrigin, const float depthEnd);
	void Camera::updateProjection();

	Transform T();
	float FOV();
	Transform proj();
	void Camera::setT(Transform& t);

protected:
	float field_of_view;		 // FOV de la caméra
	float fov_target;			 // FOV à atteindre (permet d'avoir une transition quand la valeur est modifiée)
	float fov_smooth;			 // vitesse de transition entre fov_target et field_of_view
	float sizeX, sizeY;			 // taille de l'écran
	float depthOrigin, depthEnd; // "profondeur de champ"
	Transform transform;		 // transform dans le monde
	Transform projection;		 // matrice de projection
};