/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "EmgProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/EmgProjectile.h"
#include "Sim/Weapons/WeaponDef.h"
#include "Sim/Projectiles/ProjectileHandler.h"

#include "Game/Camera.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/TextureAtlas.h"

void CEmgProjectileDrawer::Render(const CWorldObject* object) const
{
	const CEmgProjectile* proj = (const CEmgProjectile*) object;

	CProjectile::inArray = true;

	unsigned char col[4];
	col[0] = (unsigned char) (proj->GetColor().x * proj->GetIntensity() * 255);
	col[1] = (unsigned char) (proj->GetColor().y * proj->GetIntensity() * 255);
	col[2] = (unsigned char) (proj->GetColor().z * proj->GetIntensity() * 255);
	col[3] = 5; //proj->GetIntensity() * 255;

	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	const AtlasedTexture* tex = proj->GetWeaponDef()->visuals.texture1;
	va->AddVertexTC(proj->GetDrawPos() - (camera->right * proj->GetDrawRadius()) - (camera->up * proj->GetDrawRadius()), tex->xstart, tex->ystart, col);
	va->AddVertexTC(proj->GetDrawPos() + (camera->right * proj->GetDrawRadius()) - (camera->up * proj->GetDrawRadius()), tex->xend,   tex->ystart, col);
	va->AddVertexTC(proj->GetDrawPos() + (camera->right * proj->GetDrawRadius()) + (camera->up * proj->GetDrawRadius()), tex->xend,   tex->yend,   col);
	va->AddVertexTC(proj->GetDrawPos() - (camera->right * proj->GetDrawRadius()) + (camera->up * proj->GetDrawRadius()), tex->xstart, tex->yend,   col);
}
