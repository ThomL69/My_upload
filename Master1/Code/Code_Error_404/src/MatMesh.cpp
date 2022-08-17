#include "MatMesh.h"
#include <src/gKit/wavefront.h>
#include <src/gKit/program.h>
#include <src/gKit/uniforms.h>
#include <src/GameManager.h>

MatMesh::MatMesh(const char* mesh_path, const char* sharder_path) {
	mesh = read_mesh(mesh_path);
	shader = read_program(sharder_path);

	// part du principe qu'il n'y a pas plus de 16 matériaux pour notre mesh
	difuse.resize(16);
	emission.resize(16);

	// copie les matieres utilisees
	const Materials& materials = mesh.materials();
	for (int i = 0; i < materials.count(); i++)
	{
		difuse[i] = materials.material(i).diffuse;
		emission[i] = materials.material(i).emission;
	}
}

void MatMesh::draw(Camera* camera, const Transform& t, const float emission_factor) {
	glUseProgram(shader);
	
	// les matrices nécessaires pour le vertexe shader (récupéré d'un tuto gkit)
	Transform mv = camera->T() * t;
	Transform mvp = camera->proj() * mv;
	// les ajoutes à notre programme
	program_uniform(shader, "mvpMatrix", mvp);
	program_uniform(shader, "mvMatrix", mv);

	//charge nos matériaux dans le programme
	int location = glGetUniformLocation(shader, "diffuses");
	glUniform4fv(location, difuse.size(), &difuse[0].r);

	location = glGetUniformLocation(shader, "emissions");
	glUniform4fv(location, emission.size(), &emission[0].r);
	
	//permet de maitriser l'impact de la couleur d'émission dans le shader
	// (utilisé pour faire un effet sur le joueur)
	location = glGetUniformLocation(shader, "emission_factor");
	glUniform1f(location, emission_factor);

	// direction de la lumière (pas très pertinent)
	program_uniform(shader, "light_dir", Vector(-1,-1,0));

	mesh.draw(shader, true, false, true, false, true);
}

void MatMesh::release() {
	mesh.release();
	release_program(shader);
	difuse.clear();
	emission.clear();
}

MatMesh::~MatMesh() {
	release();
}