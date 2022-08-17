#pragma once
#include <mesh.h>
#include <Camera.h>
#include <Player.h>
#include <TrackManager.h>
#include <src/gKit/text.h>
#include <src/MatMesh.h>
/** 
* Gère le fonctionnement globale du runner.
* Contient la boucle de jeu avec mise à jour de l'affichage/positions, collision, points, niveau, etc.
*/
class GameManager {
public:
	GameManager();

	static GameManager* GameManager::singleton() {
		return instance;
	}

	Mesh& m_obstacle();
	Player* p() const;
	MatMesh* m_bonus() const;
	MatMesh* m_player() const;
	MatMesh* m_power() const;

	void addScore(float s);

	const float score() const;
	
	void end();

	const bool finished() const;

	/** Initialise ou réinitialise le jeu à dans un état de départ.
	* Charge les mesh, met à jour les variables etc.
	*/
	int init(bool first = true);

	/** La boucle de jeu avec mise à jour des positions/pistes, collisions, affichages*/
	int update(float delta);

	/**Affiche les objets de notre scène (dans update)*/
	void drawObjects(float delta);

	/**Vérifie les collisions entre le joueur et les obstacles/bonus/capacités */
	void checkCollisions(float delta);

	/**Met à jour la position du joueur et de la caméra sur la piste*/
	void updatePositions(float delta);

	/**Quitte l'application*/
	int quit();

	~GameManager();

private:
	float pipe_length;	  // longueur d'un tube
	float pipe_radius;	  // rayon d'un tube
	int	  pipe_nbside;	  // nombre de côtés d'un tube
	int   pipe_precision; // 2^n points pour créer
	float distance;		  // distance parcourue par le joueur
	int   level;		  // niveau de difficulté en cours
	float objective;	  // distance à atteindre pour aller au prochain niveau
	bool  game_finished;
	bool  debug;
	float game_score;


	//Controller
	Camera* camera;
	TrackManager* trackManager; // générateur de la piste
	Player* player;				// le controleur du joueur

	// Mesh
	MatMesh* mesh_player; 
	MatMesh* mesh_bonus;
	MatMesh* mesh_power;

	Text text_display;
	Text text_display_color;
	Mesh mesh_obstacle;

	static GameManager* instance;
};


