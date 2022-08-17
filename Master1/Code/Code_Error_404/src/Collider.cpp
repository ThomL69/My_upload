#include <Collider.h>
#include <GameManager.h>
DangerZone::DangerZone(Transform& T) {
	Point min, max;
	GameManager::singleton()->m_obstacle().bounds(min, max);
	box = Box(min, max);
	box.T = T;
	detroyed = false;
}

void DangerZone::draw_zone(Camera* camera) {
	if(!detroyed)
		draw(GameManager::singleton()->m_obstacle(), box.T, camera->T(), camera->proj());
}

bool DangerZone::collision(Box& other_box) {
	if (!detroyed && box.collides(other_box)) {
		if (!GameManager::singleton()->p()->boosted) {
			GameManager::singleton()->end();
			return true;
		}
		else {
			detroyed = true;
		}
	}

	return false;
}


PointZone::PointZone(Transform& T, int bonus) {
	this->bonus = bonus;
	Point min, max;
	GameManager::singleton()->m_bonus()->mesh.bounds(min, max);
	box = Box(min, max);
	box.T = T;
	picked = false;
}

void PointZone::draw_zone(Camera* camera) {
	if (!picked) {
		box.T = box.T(RotationY(3));
		GameManager::singleton()->m_bonus()->draw(camera, box.T);
	}
}

bool PointZone::collision(Box& other_box) {
	if (!picked && box.collides(other_box)) {
		picked = true;
		GameManager::singleton()->addScore(bonus);
	}
	return false;
}


PowerZone::PowerZone(Transform& T) {
	Point min, max;
	GameManager::singleton()->m_power()->mesh.bounds(min, max);
	box = Box(min, max);
	box.T = T;
	picked = false;
}

void PowerZone::draw_zone(Camera* camera) {
	if (!picked) {
		box.T = box.T(RotationY(3));
		GameManager::singleton()->m_power()->draw(camera, box.T);
	}
}

bool PowerZone::collision(Box& other_box) {
	if (!picked && box.collides(other_box)) {
		picked = true;
		GameManager::singleton()->p()->boost();
	}
	return false;
}