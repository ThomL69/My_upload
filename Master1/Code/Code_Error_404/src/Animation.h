#pragma once
#include <src/gKit/mat.h>
#include <vector>
class Animation {
public:
	enum AnimationType {
		TRANSLATION, ROTATION
	};

	Animation(AnimationType type, float duration, std::vector<float>& animations, Vector axis);

	Transform update(float delta, Transform& t);

	std::vector<float> animations;
	AnimationType type;
	Vector axis;
	float speed;
	float current;
	float target;
	int animID;
};