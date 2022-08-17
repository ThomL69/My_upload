#pragma once
#include <mat.h>
#include <box.h>
#include <Camera.h>

/**
* Objet qui possède une zone de collision
*/
class Zone {
public:
	virtual void draw_zone(Camera* camera) = 0;
	virtual bool collision(Box& ohter_box) = 0;
};

/**
* Lorsque le joueur rentre en collision avec la zone de danger, cela fini la partie
* (sauf si le joueur est en mode boost)
*/
class DangerZone : public Zone{	
public:
	DangerZone(Transform& T);

	virtual void draw_zone(Camera* camera);

	virtual bool collision(Box& other_box);

	Box  box;
	bool detroyed;
};

/**
* Lorsque le joueur rentre en collision, 
* il gagne des points et l'objet disparait.
*/
class PointZone : public Zone {
public:
	PointZone(Transform& T, int bonus);

	virtual void draw_zone(Camera* camera);

	virtual bool collision(Box& other_box);

	int  bonus;
	Box  box;
	bool picked;
};

/**
* Lorsque le joueur rentre en collision,
* il rentre dans le mode boost et l'objet disparait.
*/
class PowerZone : public Zone {
public:
	PowerZone(Transform& T);

	virtual void draw_zone(Camera* camera);

	virtual bool collision(Box& other_box);

	Box  box;
	bool picked;
};

