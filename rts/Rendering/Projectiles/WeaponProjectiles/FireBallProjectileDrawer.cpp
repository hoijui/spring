/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "FireBallProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/FireBallProjectile.h"
#include "Sim/Weapons/WeaponDef.h"

#include "Game/Camera.h"
#include "Rendering/ProjectileDrawer.hpp"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/TextureAtlas.h"

void CFireBallProjectileDrawer::Render(const CWorldObject* object) const
{
	const CFireBallProjectile* proj = (const CFireBallProjectile*) object;

	CProjectile::inArray = true;

	unsigned char col[4] = { 255, 150, 100, 1 };

	float3 interPos = proj->GetCheckCol() ? proj->GetDrawPos() : proj->GetPos();
	float size = proj->GetRadius() * 1.3f;

	const SPARK_QUEUE& sparks = proj->GetSparks();

	const int numSparks = sparks.size();
	const int numFire = std::min(10, numSparks);

	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	va->EnlargeArrays((numSparks + numFire) * 4, 0, VA_SIZE_TC);

	for (int i = 0; i < numSparks; i++) {
		//! CAUTION: loop count must match EnlargeArrays above
		col[0] = (numSparks - i) * 12;
		col[1] = (numSparks - i) *  6;
		col[2] = (numSparks - i) *  4;

		#define ept projectileDrawer->explotex
		va->AddVertexQTC(sparks[i].pos - (camera->right * sparks[i].size) - (camera->up * sparks[i].size), ept->xstart, ept->ystart, col);
		va->AddVertexQTC(sparks[i].pos + (camera->right * sparks[i].size) - (camera->up * sparks[i].size), ept->xend,   ept->ystart, col);
		va->AddVertexQTC(sparks[i].pos + (camera->right * sparks[i].size) + (camera->up * sparks[i].size), ept->xend,   ept->yend,   col);
		va->AddVertexQTC(sparks[i].pos - (camera->right * sparks[i].size) + (camera->up * sparks[i].size), ept->xstart, ept->yend,   col);
		#undef ept
	}

	int maxCol = numFire;
	if (proj->GetCheckCol()) {
		maxCol = 10;
	}

	for (int i = 0; i < numFire; i++) {
		//! CAUTION: loop count must match EnlargeArrays above
		col[0] = (maxCol - i) * 25;
		col[1] = (maxCol - i) * 15;
		col[2] = (maxCol - i) * 10;
		#define dgt projectileDrawer->dguntex
		va->AddVertexQTC(interPos - (camera->right * size) - (camera->up * size), dgt->xstart, dgt->ystart, col);
		va->AddVertexQTC(interPos + (camera->right * size) - (camera->up * size), dgt->xend ,  dgt->ystart, col);
		va->AddVertexQTC(interPos + (camera->right * size) + (camera->up * size), dgt->xend ,  dgt->yend,   col);
		va->AddVertexQTC(interPos - (camera->right * size) + (camera->up * size), dgt->xstart, dgt->yend,   col);
		#undef dgt
		interPos = interPos - proj->GetSpeed() * 0.5f;
	}
}
