#pragma once
#include <mat.h>
#include <vec.h>
#include <draw.h>
#include <Collider.h>
#include <Camera.h>
#include <Track.h>

/**
* G�n�re les pistes.
* Contient et permet d'acc�der aux informations des pistes.
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

	/** r�cup�re l'orientation d'un objet � une position et sur une piste donn�e
	* t = [0-1]  avec 0 le d�but et 1 la fin de la piste
	*/
	const Vector& getDirection(float t, int trackID = 1) const;

	// affiche les �l�ments de la piste (tuyaux, obstacles, points, etc.)
	const void draw(Camera* camera, bool debug = false);

	/** r�cup�re la position dans le monde d'un objet avec une rotation et une position sur la piste donn�e
	* t = [0-1]  avec 0 le d�but et 1 la fin de la piste*/
	Point getPositionInTrack(float t, float rotation, int trackID = 1);

	/** r�cup�re la position dans le monde d'un objet avec une rotation et une position sur la piste donn�e
	* t = [0-1]  avec 0 le d�but et 1 la fin de la piste*/
	Vector getRotatedNormal(float t, float rotation, int trackID = 1);

	/** r�cup�re la matrice de transformation dans le monde d'un objet avec une rotation et une position sur la piste donn�e
	* (oriente l'objet correctement pour qu'il soit align� avec la piste)
	* t = [0-1]  avec 0 le d�but et 1 la fin de la piste*/
	Transform getTransformInTrack(float t, float rotation, int trackID = 1);

	/** v�rifie les collisions selon une boite et une piste donn�es
	* (utilis�e pour v�rifier la collision du joueur avec les autres collider)*/
	bool checkCollisions(Box& box, int trackID = 1);

	/** g�n�re une nouvelle piste et la place en derni�re */
	void genereateNextTrack(int trackID);

	/** g�n�re les objets pr�sents sur la derni�re piste (danger, points, bonus) [Nom pas clair]  */
	void generate_danger();

	// type de g�n�ration/placement d'objet sur la piste lors de generate_danger
	void random_pattern();
	void spiral_pattern();
	void cross_pattern();

private:
	std::vector<Track*> tracks;			// les pistes qui ont �t� g�r�r�es
	std::vector<Point> tracksPoints;	// 

	// utilis� pour r�cup�rer l'orientation d'un objet (on regarde un point l�g�rement plus loin)
	const float look_distance = 0.02; 

	float c_distance;	// longueur de la piste
	float c_size;		// rayon de la piste
	float c_thickness;  // largeur d'un c�t�
	int   nb_sides;		// nombre de c�t�
	int   precision;	// qualit� de la piste (nombre de points qui compose le chemin)
};