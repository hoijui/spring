/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "LightningProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/LightningProjectile.h"
#include "Sim/Weapons/WeaponDef.h"
#include "Game/Camera.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/TextureAtlas.h"

void CLightningProjectileDrawer::Render(const CWorldObject* object) const
{
	const CLightningProjectile* proj = (const CLightningProjectile*) object;

	CProjectile::inArray = true;

	unsigned char col[4];
	col[0] = (unsigned char) (proj->GetColor().x * 255);
	col[1] = (unsigned char) (proj->GetColor().y * 255);
	col[2] = (unsigned char) (proj->GetColor().z * 255);
	col[3] = 1; //intensity*255;

	const float3 ddir = (proj->GetEndPos() - proj->GetPos()).Normalize();
	float3 dif(proj->GetPos() - camera->pos);
	float camDist = dif.Length();
	dif /= camDist;
	const float3 dir1 = (dif.cross(ddir)).Normalize();
	float3 tempPos = proj->GetPos();

	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	va->EnlargeArrays(18 * 4, 0, VA_SIZE_TC);

	const float thickness = proj->GetWeaponDef()->thickness;
	const AtlasedTexture* tex1 = proj->GetWeaponDef()->visuals.texture1;
	for (size_t d = 1; d < displacements_size-1; ++d) {
		float f = (d + 1) * 0.111f;

		va->AddVertexQTC(tempPos + (dir1 * (proj->GetDisplacements()[d    ] + thickness)), tex1->xstart, tex1->ystart, col);
		va->AddVertexQTC(tempPos + (dir1 * (proj->GetDisplacements()[d    ] - thickness)), tex1->xstart, tex1->yend,   col);
		tempPos = (proj->GetPos() * (1.0f - f)) + (proj->GetEndPos() * f);
		va->AddVertexQTC(tempPos + (dir1 * (proj->GetDisplacements()[d + 1] - thickness)), tex1->xend,   tex1->yend,   col);
		va->AddVertexQTC(tempPos + (dir1 * (proj->GetDisplacements()[d + 1] + thickness)), tex1->xend,   tex1->ystart, col);
	}

	tempPos = proj->GetPos();
	for (size_t d = 1; d < displacements_size-1; ++d) {
		float f = (d + 1) * 0.111f;

		va->AddVertexQTC(tempPos + dir1 * (proj->GetDisplacements2()[d    ] + thickness), tex1->xstart, tex1->ystart, col);
		va->AddVertexQTC(tempPos + dir1 * (proj->GetDisplacements2()[d    ] - thickness), tex1->xstart, tex1->yend,   col);
		tempPos = proj->GetPos() * (1.0f - f) + proj->GetEndPos() * f;
		va->AddVertexQTC(tempPos + dir1 * (proj->GetDisplacements2()[d + 1] - thickness), tex1->xend,   tex1->yend,   col);
		va->AddVertexQTC(tempPos + dir1 * (proj->GetDisplacements2()[d + 1] + thickness), tex1->xend,   tex1->ystart, col);
	}
}
