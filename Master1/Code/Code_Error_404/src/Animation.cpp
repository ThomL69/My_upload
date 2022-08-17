#include <Animation.h>

Animation::Animation(AnimationType type, float duration, std::vector<float>& animations, Vector axis) {
	this->speed = 1 / duration;
	this->type = type;
	this->animations = animations;
	this->current = 0;
	this->target = animations[0];
	this->animID = 0;
	this->axis = axis;
}

Transform Animation::update(float delta, Transform& t) {

	current += target * speed * delta;
	if (current >= target) {
		animID++;
		if (animations.size() - 1 <= animID) {
			animID = 0;
		}
		current = 0;
		target = animations[animID];
	}

	if (type == ROTATION) {
		return Rotation(Vector(1,0,0), target * speed * delta)(t);
	}
	else if (type == TRANSLATION) {
		return Translation(axis * (target * (speed * delta)))(t);
	}
}