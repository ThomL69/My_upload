
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"

using namespace std;
using namespace chara;


CharAnimViewer* CharAnimViewer::psingleton = NULL;


CharAnimViewer::CharAnimViewer() : Viewer(), m_frameNumber(0)
{
	psingleton = this;
}


int CharAnimViewer::init()
{
    Viewer::init();
    cout<<"==>master_CharAnim/CharAnimViewer"<<endl;
    m_camera.lookat( Point(0,75,0), 300 );
	m_camera.rotation(180, 0);
    gl.light( Point(50, 25, 50) );

    //b_draw_grid = false;

    m_world.setParticlesCount( 10 );
	m_sha.init(50,50);

    init_cylinder();
    init_sphere();

    //m_bvh.init( smart_path("data/bvh/Robot.bvh") );
	m_bvh.init( smart_path("data/bvh/danse.bvh") );

    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;

    m_ske.init( m_bvh );
    m_ske.setPose( m_bvh, -1);// met le skeleton a la pose au repos

	m_cc.setVelocityMax(2);
	m_cc.setVelocityNorm(0);

    return 0;
}


void CharAnimViewer::draw_skeleton(const Skeleton& ske)
{
	// TODO
	Point position;
	Point positionP;
	int parent;
	int jointSize = 8;
	int bonesSize = 5;

	Transform transform = Translation(Vector(0, 100, 0));

	for (int i = 0; i < ske.numberOfJoint(); i++) {
		position = ske.getJointPosition(i);
		parent = ske.getParentId(i);
		if (parent != -1) {
			positionP = ske.getJointPosition(parent);
			draw_cylinder(transform(position), transform(positionP), bonesSize);
		}
		draw_sphere(transform(position), jointSize);
	}
}



int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw_quad( RotationX(-90)*Scale(500,500,1) );

    Viewer::manageCameraLight();
    gl.camera(m_camera);

	// Affiche les particules physiques (Question 3 : interaction personnage sphere/ballon)
    //m_world.draw();

	// Affiche le skeleton � partir de la structure lin�aire (tableau) Skeleton
    //draw_skeleton( m_ske );

	// Affiche le Swallow Water
	m_sha.render(gl, m_camera);

	// Cr�ation d'une sph�re qu'on peut contr�ler;
	//draw_sphere((Point)m_cc.position(), 10.f);


#if 0			// exercice du cours
	float A[] = { 0, -10, -20, -30, -40 };
	float B[] = { 0, -10, -20, -30, -40 };
	float C[] = { 0, -10, -20, -30, -40 };
	float D[] = { 0, 10, 20, 30, 40 };
	float a, b, c, d;

	static int t = 3;
	//if (t == 4) t = 0; else t++;

	a = A[t];
	b = B[t];
	c = C[t];
	d = D[t];

	Transform scaleS = Scale(12, 12, 12);
	Transform scaleA = Scale(20, 20, 20);
	Transform scale = Scale(10, 100, 10);
	Transform A2W = RotationZ(a);
	draw_cylinder(A2W*scale);
	draw_sphere(A2W*scaleS);
	draw_axe(A2W*scaleA);

	Transform B2A = Translation(0, 100, 0) * RotationZ(b);
	Transform B2W = A2W * B2A;
	draw_cylinder(B2W*scale);
	draw_sphere(B2W*scaleS);
	draw_axe(B2W*scaleA);

	Transform scaleP = Scale(5, 50, 5);
	Transform C2B = Translation(0, 100, 0) * RotationZ(c);
	Transform C2W = B2W * C2B;
	draw_cylinder(C2W*scaleP);
	draw_sphere(C2W*scaleS);
	draw_axe(C2W*scaleA);

	Transform D2B = Translation(0, 100, 0) * RotationZ(d);
	Transform D2W = B2W * D2B;
	draw_cylinder(D2W*scaleP);
	draw_axe(D2W*scaleA);
	draw_sphere(D2W*scaleS);
#endif


    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.

	if (key_state('n')) { m_frameNumber++; cout << m_frameNumber << endl; }
	if (key_state('b')) { m_frameNumber--; cout << m_frameNumber << endl; }

	//m_ske.setPose( m_bvh, m_frameNumber );

    //m_world.update(0.1f);

	m_sha.update(0.2f);

	m_cc.update(0.5f);

    return 0;
}