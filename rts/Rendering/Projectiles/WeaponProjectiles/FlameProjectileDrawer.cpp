/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "FlameProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/FlameProjectile.h"
#include "Sim/Weapons/WeaponDef.h"

#include "Game/Camera.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/ColorMap.h"
#include "Rendering/Textures/TextureAtlas.h"

void CFlameProjectileDrawer::Render(const CWorldObject* object) const
{
	const CFlameProjectile* proj = (const CFlameProjectile*) object;

	CProjectile::inArray = true;

	unsigned char col[4];
	proj->GetWeaponDef()->visuals.colorMap->GetColor(col, proj->GetCurTime());

	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	const AtlasedTexture* tex = proj->GetWeaponDef()->visuals.texture1;
	va->AddVertexTC(proj->GetDrawPos() - (camera->right * proj->GetRadius()) - (camera->up * proj->GetRadius()), tex->xstart, tex->ystart, col);
	va->AddVertexTC(proj->GetDrawPos() + (camera->right * proj->GetRadius()) - (camera->up * proj->GetRadius()), tex->xend,   tex->ystart, col);
	va->AddVertexTC(proj->GetDrawPos() + (camera->right * proj->GetRadius()) + (camera->up * proj->GetRadius()), tex->xend,   tex->yend,   col);
	va->AddVertexTC(proj->GetDrawPos() - (camera->right * proj->GetRadius()) + (camera->up * proj->GetRadius()), tex->xstart, tex->yend,   col);
}
