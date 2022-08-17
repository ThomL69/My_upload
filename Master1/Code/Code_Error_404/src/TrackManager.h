#pragma once
#include <mat.h>
#include <vec.h>
#include <draw.h>
#include <Collider.h>
#include <Camera.h>
#include <Track.h>

/**
* Génère les pistes.
* Contient et permet d'accéder aux informations des pistes.
* Notamment permet de calculer le placement d'un objet sur une piste.
*/
class TrackManager {
public:

	TrackManager(float c_distance, float c_size, int nb_sides, int precision);

	//return last position of current track
	const int end_of_track(int trackID = 1) const;

	//return length of track
	const float length_of_track(int trackID = 1) const;

	//return current track
	const Track* curr() const;
	
	//create new track and delete the first one
	void nextTrack();

	/** récupère l'orientation d'un objet à une position et sur une piste donnée
	* t = [0-1]  avec 0 le début et 1 la fin de la piste
	*/
	const Vector& getDirection(float t, int trackID = 1) const;

	// affiche les éléments de la piste (tuyaux, obstacles, points, etc.)
	const void draw(Camera* camera, bool debug = false);

	/** récupère la position dans le monde d'un objet avec une rotation et une position sur la piste donnée
	* t = [0-1]  avec 0 le début et 1 la fin de la piste*/
	Point getPositionInTrack(float t, float rotation, int trackID = 1);

	/** récupère la position dans le monde d'un objet avec une rotation et une position sur la piste donnée
	* t = [0-1]  avec 0 le début et 1 la fin de la piste*/
	Vector getRotatedNormal(float t, float rotation, int trackID = 1);

	/** récupère la matrice de transformation dans le monde d'un objet avec une rotation et une position sur la piste donnée
	* (oriente l'objet correctement pour qu'il soit aligné avec la piste)
	* t = [0-1]  avec 0 le début et 1 la fin de la piste*/
	Transform getTransformInTrack(float t, float rotation, int trackID = 1);

	/** vérifie les collisions selon une boite et une piste données
	* (utilisée pour vérifier la collision du joueur avec les autres collider)*/
	bool checkCollisions(Box& box, int trackID = 1);

	/** génère une nouvelle piste et la place en dernière */
	void genereateNextTrack(int trackID);

	/** génère les objets présents sur la dernière piste (danger, points, bonus) [Nom pas clair]  */
	void generate_danger();

	// type de génération/placement d'objet sur la piste lors de generate_danger
	void random_pattern();
	void spiral_pattern();
	void cross_pattern();

private:
	std::vector<Track*> tracks;			// les pistes qui ont été gérérées
	std::vector<Point> tracksPoints;	// 

	// utilisé pour récupérer l'orientation d'un objet (on regarde un point légèrement plus loin)
	const float look_distance = 0.02; 

	float c_distance;	// longueur de la piste
	float c_size;		// rayon de la piste
	float c_thickness;  // largeur d'un côté
	int   nb_sides;		// nombre de côté
	int   precision;	// qualité de la piste (nombre de points qui compose le chemin)
};