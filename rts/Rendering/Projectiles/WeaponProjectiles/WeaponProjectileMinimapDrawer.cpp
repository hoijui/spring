/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "WeaponProjectileMinimapDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/WeaponProjectile.h"

#include "Rendering/GL/VertexArray.h"
#include "Rendering/Colors.h"

void WeaponProjectileMinimapDrawer::Render(const CWorldObject* object) const
{
	const CWeaponProjectile* proj = (const CWeaponProjectile*) object;

	GetPoints()->AddVertexQC(proj->GetPos(), color4::yellow);
}
