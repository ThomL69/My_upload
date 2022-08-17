#pragma once
#include <vector>
#include <color.h>
#include <mesh.h>
#include <Camera.h>

/**
* Gère l'importation et l'utilisation des matériaux dans un shader pour un mesh donné.
* (Très pratique pour pouvoir importer et modifier des modèles provenant de Blender)
*/
class MatMesh {
public:
	MatMesh(const char* mesh_path, const char* sharder_path);
	void draw(Camera* camera, const Transform& t, const float emission_factor = 1.0f);
	void release();
	~MatMesh();
	Mesh mesh;					 // mesh à afficher
	std::vector<Color> difuse;   // valeur de diffusion du matériaux
	std::vector<Color> emission; // valeur d'émission du matériaux
	GLuint shader;				 // shader
};