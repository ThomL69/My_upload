#include "GameManager.h"
#include <wavefront.h>
#include <src/gKit/uniforms.h>

GameManager* GameManager::instance = nullptr;


GameManager::GameManager() {
	if (instance == nullptr) {
		instance = this;
	}
	mesh_obstacle = read_mesh("data/cube.obj");
	mesh_obstacle.default_color(Red());

	pipe_length = 60;
	pipe_radius = 5;
	pipe_nbside = 8;
	pipe_precision = 6;
}


int GameManager::init(bool first) {
	game_score = 0;
	distance = 0;
	level = 1;
	objective = pipe_length * 3;
	game_finished = false;
	debug = false;
	
	//on ne recharge pas tout après un game over
	if (first) {
		text_display = create_text();
		text_display_color = create_text();
		default_color(text_display_color, Color(0.2, 1, 1));
		camera = new Camera(window_width(), window_height(), 90, 0.1, 200);
		mesh_player = new MatMesh("data/SpaceShip.obj", "src/shader/player.glsl");
		mesh_bonus = new MatMesh("data/bonus.obj", "src/shader/global.glsl");
		mesh_power = new MatMesh("data/power.obj", "src/shader/global.glsl");	
	}
	player = new Player(Vector(0, 0, 0));
	trackManager = new TrackManager(pipe_length, pipe_radius, pipe_nbside, pipe_precision);

	// etat openGL par defaut
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre

	// etape 3 : configuration du pipeline.
	glClearDepth(100.f);                        // profondeur par defaut
	glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
	glEnable(GL_DEPTH_TEST);                    // activer le ztest
	
	return 0;
}

int GameManager::update(float delta) {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	clear(text_display);
	clear(text_display_color);
	
	if (!game_finished) {
		// change le FOV lors du boost pour donner une meilleure impression d'accélération
		bool isFast = player->boost_timer <= (player->boost_duration - player->boost_smooth);
		if (player->boosted && isFast && camera->FOV() != 120.0f)
			camera->setFOV(120.0f);
		else if (player->boosted && !isFast && camera->FOV() != 90.0f)
			camera->setFOV(90.0f);

		camera->update(delta);
		player->update(delta);

		//augmentation de la difficulté en fonction de la distance totale parcourue
		distance += player->speed * delta;
		if (distance >= objective) {
			objective = objective * 2;
			level++;
			player->speed *= 1.1;
			player->rotationSpeed *= 1.05;
			player->rotationCooldown *= 0.95;
		}
		//le score augmente avec la distance
		addScore(player->speed * delta);

		updatePositions(delta);
		checkCollisions(delta);
	} 
	else {
		// le joueur à perdu affichage du texte de défaite
		printf(text_display, 50, 15, "APPUYEZ SUR R POUR RECOMMENCER");
		printf(text_display_color, 60, 8, "ERROR 404");

		// input r pour recommencer la partie
		if (key_state('r')) {
			delete trackManager;
			delete player;
			init(false);
		}
	}
	// texte sur interfaces
	printf(text_display, 60, 1, "SCORE : %d", (int)score());
	printf(text_display, 60, 3, "LEVEL : %d", level);
	printf(text_display, 0, 0, "TOURNER : Q/D", level);

	// affichages des éléments de la scène
	drawObjects(delta);

	if (key_state('o')) {
		debug = true;
	}
	else if (key_state('i')) {
		debug = false;
	}

	return 1;
}


void GameManager::updatePositions(float delta) {

	//player position
	while (player->position >= trackManager->length_of_track()) {
		player->position -= trackManager->length_of_track();
		trackManager->nextTrack();
	}
	float t = player->position / trackManager->length_of_track();

	player->setT(trackManager->getTransformInTrack(t, player->current_rotation));
	
	//camera positions
	Vector way = trackManager->getDirection(t);
	Vector normal = trackManager->getRotatedNormal(t, player->current_rotation);
	Point player_world_position = trackManager->getPositionInTrack(t, player->current_rotation) + normal * pipe_radius;
	Vector offset_camera = way * 4 - normal * pipe_radius * 0.7;
	Vector offset_target = normal * pipe_radius * 0.2 + way;
	
	camera->setT(Lookat(player_world_position - offset_camera, player_world_position, normal));

}


void GameManager::checkCollisions(float delta) {
	trackManager->checkCollisions(player->box);
}

void GameManager::drawObjects(float delta) {
	trackManager->draw(camera, debug);
	float emmission_factor = 0.2f;
	// permet de faire briller le "centre" du modèle du joueur lorsqu'il est en mode boost
	if (player->boosted) {
		if (player->boost_timer > (player->boost_duration - player->boost_smooth))
			emmission_factor = std::max((player->boost_duration - player->boost_timer) / player->boost_smooth, emmission_factor);
		else if(player->boost_timer < player->boost_smooth)
			emmission_factor = std::min(emmission_factor + (player->boost_timer / player->boost_smooth) * (1.0f - emmission_factor), 1.0f);
		else
			emmission_factor = 1.0f;
	}
	// affiche joueur
	mesh_player->draw(camera, player->T(), emmission_factor);
	// affiche texte
	draw(text_display, window_width(), window_height());
	draw(text_display_color, window_width(), window_height());
}


int GameManager::quit()
{
	// etape 3 : detruire la description du triangle
	return 0;   // ras, pas d'erreur
}

GameManager::~GameManager() {
	delete camera;
	delete trackManager;
	delete player;
	mesh_obstacle.release();
	mesh_player->release();
	mesh_power->release();
	delete mesh_player;
	delete mesh_bonus;
	delete mesh_power;
	release_text(text_display);
	release_text(text_display_color);
}


Player* GameManager::p() const {
	return player;
}

Mesh& GameManager::m_obstacle() {
	return mesh_obstacle;
}

MatMesh* GameManager::m_bonus() const {
	return mesh_bonus;
}

MatMesh* GameManager::m_player() const {
	return mesh_player;
}
MatMesh* GameManager::m_power() const {
	return mesh_power;
}

void GameManager::addScore(float s) {
	game_score += s;
}

const float GameManager::score() const {
	return game_score;
}

void GameManager::end() {
	game_finished = true;
}

const bool GameManager::finished() const {
	return game_finished;
}