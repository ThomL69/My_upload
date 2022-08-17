#pragma once
#include <vector>
#include <color.h>
#include <mesh.h>
#include <Camera.h>

/**
* G�re l'importation et l'utilisation des mat�riaux dans un shader pour un mesh donn�.
* (Tr�s pratique pour pouvoir importer et modifier des mod�les provenant de Blender)
*/
class MatMesh {
public:
	MatMesh(const char* mesh_path, const char* sharder_path);
	void draw(Camera* camera, const Transform& t, const float emission_factor = 1.0f);
	void release();
	~MatMesh();
	Mesh mesh;					 // mesh � afficher
	std::vector<Color> difuse;   // valeur de diffusion du mat�riaux
	std::vector<Color> emission; // valeur d'�mission du mat�riaux
	GLuint shader;				 // shader
};