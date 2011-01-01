/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "LightningProjectileMinimapDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/LightningProjectile.h"

#include "Rendering/GL/VertexArray.h"

void CLightningProjectileMinimapDrawer::Render(const CWorldObject* object) const
{
	const CLightningProjectile* proj = (const CLightningProjectile*) object;

	const unsigned char color[4] = {
			(unsigned char) (proj->GetColor()[0] * 255),
			(unsigned char) (proj->GetColor()[1] * 255),
			(unsigned char) (proj->GetColor()[2] * 255),
			(unsigned char) (1                   * 255)
			};

	GetLines()->AddVertexQC(proj->GetPos(),    color);
	GetLines()->AddVertexQC(proj->GetEndPos(), color);
}
