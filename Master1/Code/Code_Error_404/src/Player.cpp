#include <Player.h>
#include <src/GameManager.h>

Player::Player(Vector p_start) {
	rotation_target = 0.f;
	current_rotation = 0.f;

	rotationSpeed = 1.2f;
	rotationCooldown = 1.f;

	power_boost = 3.f;
	boost_smooth = 2.5f;
	boost_duration = 25.f;
	boost_timer = 0.f;
	boosted = false;

	rotationTimer = rotationCooldown;
	position = 0.f;
	speed = 5.f;
	rotating = false;


	Point min, max;
	GameManager::singleton()->m_player()->mesh.bounds(min, max);
	box = Box(min, max);
}

void Player::update(float delta) {

	//Controle Rotation
	if (rotationTimer >= rotationCooldown) {
		if (key_state('d')) {
			move_right();
			rotationTimer = 0;
			rotating = true;
		}
		else if (key_state('q')) {
			move_left();
			rotationTimer = 0;
			rotating = true;
		}
	}

	// Controle ralentir, accélérer, boost (devrait pas être accessible normalement)
	if (key_state('l')) {
		speed = speed - delta;
		speed = (speed < 0) ? 0 : speed;
	}
	else if (key_state('m')) {
		speed = speed + delta;
	}
	else if (key_state('p')) {
		boost();
	}
	
	// animation rotation
	if (rotating) {
		float rot = delta * rotationSpeed;
		current_rotation += (current_rotation < rotation_target) ? rot : -rot;
		if (abs(current_rotation - rotation_target) <= rot + 0.01) {
			current_rotation = rotation_target;
			rotating = false;
		}
	}
	float movement = delta * speed;
	// animation boost 
	if (boosted) {
		boost_timer += delta;
		
		if (boost_timer >= boost_duration)
			boosted = false;
		// accélération
		if (boost_timer <= boost_smooth) {
			float factor = std::min(1.0f, boost_timer / boost_smooth);
			movement *= power_boost * factor;
		}//décélération
		else if (boost_timer >= boost_duration - boost_smooth) {
			float factor = ((boost_duration - boost_timer) / boost_smooth);
			movement *= std::max(1.0f, power_boost * factor);
		}
		else {
			movement *= power_boost;
		}
	}

	position += movement;
	
	rotationTimer += delta;
}

void Player::boost() {
	if (!boosted) {
		boost_timer = 0;
		boosted = true;
	}
	else {
		boost_timer = boost_smooth;
	}
}

void Player::move_right() {
	rotation_target++;

}

void Player::move_left() {
	rotation_target--;
}

const Transform& Player::T() const {
	return box.T;
}

void Player::setT(Transform& tranform) {
	box.T = tranform;
}