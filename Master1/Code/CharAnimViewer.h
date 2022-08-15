
#ifndef _CHARANIMVIEWER_H
#define _CHARANIMVIEWER_H


#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
#include "Skeleton.h"
#include "TransformQ.h"
#include "ShallowWater.h"
#include "CharacterController.h"

#include <PhysicalWorld.h>

class CharAnimViewer : public Viewer
{
public:
    CharAnimViewer();

    int init();
    int render();
    int update( const float time, const float delta );

	static CharAnimViewer& singleton() { return *psingleton;  }

protected:
	void bvhDrawRec(const chara::BVHJoint& bvh, const Transform& f2w, int f);

    chara::BVH m_bvh;
    int m_frameNumber;

    Skeleton m_ske;

    ShallowWater m_sha;
    
    PhysicalWorld m_world;

    CharacterController m_cc;

	void draw_skeleton(const Skeleton& );

private:
	static CharAnimViewer* psingleton;
};



#endif
