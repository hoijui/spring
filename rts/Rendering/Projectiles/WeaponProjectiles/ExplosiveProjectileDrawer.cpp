/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "ExplosiveProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/ExplosiveProjectile.h"
#include "Sim/Weapons/WeaponDef.h"

#include "Game/Camera.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/ColorMap.h"
#include "Rendering/Textures/TextureAtlas.h"

void CExplosiveProjectileDrawer::Render(const CWorldObject* object) const
{
	const CExplosiveProjectile* proj = (const CExplosiveProjectile*) object;

	if (proj->GetModel()) {
		// do not draw if a 3D model has been defined for us
		return;
	}

	CProjectile::inArray = true;

	unsigned char col[4] = {0};

	if (proj->GetWeaponDef()->visuals.colorMap) {
		proj->GetWeaponDef()->visuals.colorMap->GetColor(col, proj->GetCurTime());
	} else {
		col[0] = proj->GetWeaponDef()->visuals.color.x * 255;
		col[1] = proj->GetWeaponDef()->visuals.color.y * 255;
		col[2] = proj->GetWeaponDef()->visuals.color.z * 255;
		col[3] = proj->GetWeaponDef()->intensity       * 255;
	}

	const AtlasedTexture* tex = proj->GetWeaponDef()->visuals.texture1;
	const float  alphaDecay = proj->GetWeaponDef()->visuals.alphaDecay;
	const float  sizeDecay  = proj->GetWeaponDef()->visuals.sizeDecay;
	const float  separation = proj->GetWeaponDef()->visuals.separation;
	const bool   noGap      = proj->GetWeaponDef()->visuals.noGap;
	const int    stages     = proj->GetWeaponDef()->visuals.stages;
	const float  invStages  = 1.0f / stages;

	const float3 ndir = proj->GetDir() * separation * 0.6f;

	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	va->EnlargeArrays(stages * 4, 0, VA_SIZE_TC);

	for (int stage = 0; stage < stages; ++stage) { //! CAUTION: loop count must match EnlargeArrays above
		const float stageDecay = (stages - (stage * alphaDecay)) * invStages;
		const float stageSize  = proj->GetDrawRadius() * (1.0f - (stage * sizeDecay));

		const float3 ydirCam  = camera->up    * stageSize;
		const float3 xdirCam  = camera->right * stageSize;
		const float3 stageGap = (noGap)? (ndir * stageSize * stage): (ndir * proj->GetDrawRadius() * stage);
		const float3 stagePos = proj->GetDrawPos() - stageGap;

		col[0] = stageDecay * col[0];
		col[1] = stageDecay * col[1];
		col[2] = stageDecay * col[2];
		col[3] = stageDecay * col[3];

		va->AddVertexQTC(stagePos - xdirCam - ydirCam, tex->xstart, tex->ystart, col);
		va->AddVertexQTC(stagePos + xdirCam - ydirCam, tex->xend,   tex->ystart, col);
		va->AddVertexQTC(stagePos + xdirCam + ydirCam, tex->xend,   tex->yend,   col);
		va->AddVertexQTC(stagePos - xdirCam + ydirCam, tex->xstart, tex->yend,   col);
	}
}
