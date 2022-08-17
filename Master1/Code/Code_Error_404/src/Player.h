#pragma once
#include <draw.h>
#include <mat.h>
#include <box.h>

#ifndef PLAYER_H
#define PLAYER_H

/**
* Controller du joueur. 
* G�re les input, la vitesse de d�placement, la rotation et la capacit� de boost.
*/
class Player {
public:
	Player(Vector p_start);

	void update(float delta);
	
	void move_right();
	void move_left();

	void boost(); // active le boost ou le prolonge

	const Transform& T() const;

	void setT(Transform& tranform);

	Box box;				// boite de collision
	float current_rotation; // rotation courante
	int   rotation_target;  // rotation � atteindre
	float rotationSpeed;    // rapidit� pour atteindre la rotation_target
	float rotationCooldown; // temps entre deux rotations
	float rotationTimer;
	bool  rotating;	

	float power_boost;		// multiplicateur de vitesse du boost
	float boost_duration;   // dur�e du boost
	float boost_smooth;     // dur�e d'acc�l�ration et d�c�l�ration au d�but et � la fin
	float boost_timer;
	bool  boosted;			

	float position;			// position sur la piste
	float speed;			// vitesse de "course"
};
#endif