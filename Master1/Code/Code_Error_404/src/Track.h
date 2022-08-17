#pragma once
#include <vec.h>
#include <draw.h>
#include <vector>
#include <Camera.h>
#include <Collider.h>

using namespace std;

class Track {
public:
	
	Track(const vector<Point>& path, Vector firstNormal, const int precision, const int nb_sides, const float c_size, bool debug = false);

	void draw_cylinder(Camera* camera);
	void draw_edges(Camera* camera);
	void draw_curve(Camera* camera);
	void draw_zones(Camera* camera);

	Point getLastPostion();

	Vector getLastNormal();

	const float track_length() const;

	const int size() const;

	void addZone(Zone* danger);
	
	const Point getPosition(const float t) const;
	
	const Vector getNormal(const float t) const;
	
	/** OBSOLETE
	* Génère un chemin avec la technique de la subdivision (bugé)
	*/
	void buildPath(const vector<Point>& path, const int precision, bool debug);

	/** 
	* Génère un chemin avec "Centripetal Catmull–Rom spline"
	* https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline
	* Permet de maitrise le nombre de points sur notre courbe (augmentation linéaire au lieu d'exponentiel)
	* Garantie une continuité dans notre chemin
	* Plus rapide
	*/
	void buildPath2(const vector<Point>& path, const int precision, bool debug);

	/**Construit les normales le long du chemin nécessaire à la génération du tube
	* Récupère la dernière normale du tube précédent pour l'utilisé en tant que première normale
	* (Evite de se retrouver avec des normales très différentes entre deux tubes consécutif)
	*/
	void buildNormals(Vector firstNormal, const float c_size, bool debug);

	/**Construit le modèle 3D du tube
	*/
	void buildCylinder(const int nb_sides, const float c_size, bool debug);

	~Track();

	float t_length;   // longueur de la courbe
	Mesh  t_cylinder; // mesh 3D du tube
	Mesh  t_edges;	  // mesh (lignes) des cercles du tube
	Mesh  t_curve;	  // mesh (lignes) du chemin et des normales

	vector<Point>  t_path;		//les points utilisées pour générer le tube (4 points)
	vector<Point>  t_points;	//les points composants notre chemin
	vector<Vector> t_normals;	//les normales sur le chemin
	vector<Zone*>  t_zones;		//les zones de collisions


};
