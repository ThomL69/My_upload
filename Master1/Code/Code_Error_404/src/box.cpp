#include "box.h"

Box::Box() {
	pmin = Origin();
	pmax = Origin();
	T = Identity();
}

Box::Box(const Point& i_pmin, const Point& i_pmax) {
	pmin = i_pmin;
	pmax = i_pmax;
	T = Identity();
}

bool checkCollision(const Box& a, const Box& b, const int i) {
	int factor = (i % 2 == 0) ? 1 : -1;

	Vector direction;
	direction.x = (i < 2) ? factor : 0;
	direction.y = (i >= 2 && i < 4) ? factor : 0;
	direction.z = (i >= 4) ? factor : 0;
	direction = a.T(direction); // to world
	direction = b.T.inverse()(direction); // to B

	Point corner;
	corner.x = (direction.x > 0) ? b.pmin.x : b.pmax.x;
	corner.y = (direction.y > 0) ? b.pmin.y : b.pmax.y;
	corner.z = (direction.z > 0) ? b.pmin.z : b.pmax.z;
	corner = b.T(corner); // to world
	corner = a.T.inverse()(corner); // to A

	switch (i) {
		case 0: return a.pmax.x <= corner.x;
		case 1: return a.pmin.x >= corner.x;
		case 2: return a.pmax.y <= corner.y;
		case 3: return a.pmin.y >= corner.y;
		case 4: return a.pmax.z <= corner.z;
		default: return a.pmin.z >= corner.z;
	}
}

bool Box::collides(const Box& rhs) {
	
	for (int i = 0; i < 6; i++) {
		if (checkCollision(*this, rhs, i) || checkCollision(rhs, *this, i)) {
			return false;
		}
	}
	return true;
}


/* Translation only
bool Box::collides(const Box& rhs) {
	
	Point min = T(pmin);
	Point max = T(pmax);
	Point rmin = rhs.T(rhs.pmin);
	Point rmax = rhs.T(rhs.pmax);
	return (min.x <= rmax.x && max.x >= rmin.x) &&
		(min.y <= rmax.y && max.y >= rmin.y) &&
		(min.z <= rmax.z && max.z >= rmin.z);
}*/

