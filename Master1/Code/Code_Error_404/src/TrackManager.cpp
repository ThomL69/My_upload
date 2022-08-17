#include <TrackManager.h>



TrackManager::TrackManager(float c_distance, float c_size, int nb_sides, int precision) {
	this->c_distance = c_distance;
	this->c_size = c_size;
	this->nb_sides = nb_sides;
	this->precision = precision;
	tracksPoints.push_back(Point(0, 0, -c_distance));
	tracksPoints.push_back(Point(0, 0, 0));
	tracksPoints.push_back(Point(0, 0, c_distance));


	for (int i = 0; i < 5; i++) {
		genereateNextTrack(i);
		if (i == 0)
			c_thickness = length(getRotatedNormal(0, 1, 0) * c_size - getRotatedNormal(0, 0, 0) * c_size);
		else if (i >= 2)
			generate_danger();
	}
}

//return last position of current track
const int TrackManager::end_of_track(int trackID) const {
	return tracks[trackID]->size() - 1;
}

//return length of track
const float TrackManager::length_of_track(int trackID) const {
	return tracks[trackID]->t_length;
}

//return current track
const Track* TrackManager::curr() const {
	return tracks[1];
}

//create new track and delete the first one
void TrackManager::nextTrack() {
	genereateNextTrack(tracks.size() - 1);
	generate_danger();
	Track* dtrack = tracks[0];
	tracks.erase(tracks.begin());
	tracksPoints.erase(tracksPoints.begin());
	delete dtrack;
}


const Vector& TrackManager::getDirection(float t, int trackID) const {
	Point point = tracks[trackID]->getPosition(t);
	if (t + look_distance < 1.0f)
		return normalize(tracks[trackID]->getPosition(t + look_distance) - point);
	else // si le point en avant sort du tube, on utilise un point derrière notre point
		return normalize(point - tracks[trackID]->getPosition(t - look_distance));
}

const void TrackManager::draw(Camera* camera, bool debug) {
	for each (Track * t in tracks)
	{
		t->draw_cylinder(camera);
		t->draw_zones(camera);
		t->draw_edges(camera);
		if (debug) {
			t->draw_curve(camera);
		}
	}
}

Point TrackManager::getPositionInTrack(float t, float rotation, int trackID) {
	return tracks[trackID]->getPosition(t);
}

Vector TrackManager::getRotatedNormal(float t, float rotation, int trackID) {
	// décale par un angle fixe
	// 180.0 / nb_sides pour placer sur une face et pas un coté du tube
	float angle = 180.0f / nb_sides + (360.0f / nb_sides) * rotation;
	Vector axis = getDirection(t, trackID);
	Transform r = Rotation(axis, angle);
	return r(tracks[trackID]->getNormal(t));
}

Transform TrackManager::getTransformInTrack(float t, float rotation, int trackID) {
	Vector normal = getRotatedNormal(t, rotation, trackID);
	Vector dir = getDirection(t, trackID);
	Point position = getPositionInTrack(t, rotation, trackID) + normal * c_size;
	Vector axis_x = cross(dir, normal);
	// lookat inversé
	Transform m(
		axis_x.x, normal.x, -dir.x, position.x,
		axis_x.y, normal.y, -dir.y, position.y,
		axis_x.z, normal.z, -dir.z, position.z,
		0, 0, 0, 1);


	return m;
}

bool TrackManager::checkCollisions(Box& box, int trackID) {
	for (Zone* zone : tracks[trackID]->t_zones) {
		if (zone->collision(box)) {
			return true;
		}
	}
	return false;
}

void TrackManager::genereateNextTrack(int trackID) {
	int lastId = tracksPoints.size() - 1;
	Vector firstNormal;
	vector<Point> points;
	// on a besoin de 4 points pour générer la piste
	// le chemin commence aux points 2 et fini au 3
	// les autres nous garantisse une cohérence entre la nouvelle est lancienne piste
	points.push_back(tracksPoints[lastId - 2]);
	points.push_back(tracksPoints[lastId - 1]);
	points.push_back(tracksPoints[lastId]);

	// si on génère la toute première piste, on génère la normale de départ (pour la génération du mesh)
	if (trackID == 0) {
		Vector a = points[1] - points[0];
		Vector b(0., 1., 0.);
		firstNormal = normalize(cross(a, b));
	}
	else { // sinon c'est la dernière de la piste précédente
		firstNormal = tracks[trackID - 1]->getLastNormal();
	}
	std::cout << "first normal " << firstNormal << std::endl;

	// fait varié aléatoirement la position du prochain point
	int factor = 20;

	double theta = 2 * 3.14159265358979323846264 * (double)rand() / RAND_MAX;
	double r = sqrt((double)rand() / RAND_MAX);
	float dirX = factor * 0.5f - r * factor * cos(theta);
	float dirY = factor * 0.5f - r * factor * sin(theta);

	//sauvegarde du nouveau point
	Vector offset(dirX, dirY, c_distance);
	tracksPoints.push_back(tracksPoints[lastId] + offset);
	points.push_back(tracksPoints[lastId + 1]);
	// génération et ajout de la piste
	Track* newTrack = new Track(points, firstNormal, precision, nb_sides, c_size, true);
	tracks.push_back(newTrack);
}

void TrackManager::generate_danger() {
	int r = rand()%3;
	switch (r)
	{
		case 0: random_pattern(); break;
		case 1: cross_pattern(); break;
		case 2: spiral_pattern(); break;
		default:
			random_pattern();
			break;
	}

}

void TrackManager::random_pattern() {

	int trackID = tracks.size() - 1;
	Track* last = tracks[trackID];
	float pos; // position de l'obstacle dans le tube [0,1]
	Transform s = Scale(c_thickness * 0.8f);
	Transform s_small = Scale(c_thickness * 0.2f);
	//Transform bigS = Scale(c_thickness * 0.5f, c_size * 2.0f + 8, c_thickness * 0.5f);

	//génération des obstacles
	int nb_obstacle = rand() % 20;
	for (int i = 0; i < nb_obstacle; i++) {
		int rotation = rand() % nb_sides;
		pos = (float)i / nb_obstacle; // [0-1]

		DangerZone* danger = new DangerZone(getTransformInTrack(pos, rotation, trackID)(s));
		PointZone* bonus1 = new PointZone(getTransformInTrack(pos, rotation + 1, trackID)(s_small), 50);
		PointZone* bonus2 = new PointZone(getTransformInTrack(pos, rotation - 1, trackID)(s_small), 50);
		if (i == nb_obstacle / 2) {
			PowerZone* power = new PowerZone(getTransformInTrack(pos, rotation + nb_sides / 2, trackID)(s_small));
			last->addZone(power);
		}
		last->addZone(danger);
		last->addZone(bonus1);
		last->addZone(bonus2);
	}
}


void TrackManager::cross_pattern() {

	int trackID = tracks.size() - 1;
	Track* last = tracks[trackID];
	float pos; // position de l'obstacle dans le tube [0,1]
	Transform s = Scale(c_thickness * 0.8f);
	Transform s_small = Scale(c_thickness * 0.2f);
	//Transform bigS = Scale(c_thickness * 0.5f, c_size * 2.0f + 8, c_thickness * 0.5f);

	//génération des obstacles
	int nb_cross = 4;
	int rotation;

	for (int i = 0; i < nb_cross; i++) {
		for (int k = 0; k < 4; k++) {
			int face = (i * 2 + 1);
			rotation = face % nb_sides;
			if (i % 2 == 1) {
				int face = (i * 2);
				rotation = face % nb_sides;
			}
		}

		pos = ((float)i) / nb_cross; // [0-1]
		DangerZone* danger;
		for (int j = 0; j < 7; j += 2) {
			danger = new DangerZone(getTransformInTrack(pos, rotation + 2 * j, trackID)(s));
			last->addZone(danger);
		}

		PointZone* bonus1 = new PointZone(getTransformInTrack(pos, rotation + 1, trackID)(s_small), 50);
		PointZone* bonus2 = new PointZone(getTransformInTrack(pos, rotation - 1, trackID)(s_small), 50);

		if (i == nb_cross / 2) {
			PowerZone* power = new PowerZone(getTransformInTrack(pos, rotation + nb_sides / 2, trackID)(s_small));
			last->addZone(power);
		}


		last->addZone(bonus1);
		last->addZone(bonus2);
	}
}


void TrackManager::spiral_pattern() {

	int trackID = tracks.size() - 1;
	Track* last = tracks[trackID];
	float pos; // position de l'obstacle dans le tube [0,1]
	Transform s = Scale(c_thickness * 0.8f);
	Transform s_small = Scale(c_thickness * 0.2f);
	//Transform bigS = Scale(c_thickness * 0.5f, c_size * 2.0f + 8, c_thickness * 0.5f);

	//génération des obstacles
	int nb_obstacle = 10;
	int rotation_spiral = 0;
	for (int i = 0; i < nb_obstacle; i++) {
		for (int k = 0; k < nb_sides; k++) {
			int face = i;
			rotation_spiral = face % nb_sides;
		}
		int rotation = rand() % nb_sides;
		pos = (float)i / nb_obstacle; // [0-1]

		DangerZone* danger = new DangerZone(getTransformInTrack(pos, rotation_spiral, trackID)(s));
		PointZone* bonus1 = new PointZone(getTransformInTrack(pos, rotation + 1, trackID)(s_small), 50);
		PointZone* bonus2 = new PointZone(getTransformInTrack(pos, rotation - 1, trackID)(s_small), 50);
		if (i == nb_obstacle / 2) {
			PowerZone* power = new PowerZone(getTransformInTrack(pos, rotation + rand(), trackID)(s_small));
			last->addZone(power);
		}
		last->addZone(danger);
		last->addZone(bonus1);
		last->addZone(bonus2);
	}
}