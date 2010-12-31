/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "BeamLaserProjectileMinimapDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/BeamLaserProjectile.h"

#include "Rendering/GL/VertexArray.h"

void CBeamLaserProjectileMinimapDrawer::Render(const CWorldObject* object) const
{
	const CBeamLaserProjectile* proj = (const CBeamLaserProjectile*) object;

	const unsigned char color[4] = {
			proj->GetKoColStart()[0],
			proj->GetKoColStart()[1],
			proj->GetKoColStart()[2],
			255
			};

	GetLines()->AddVertexQC(proj->GetStartPos(), color);
	GetLines()->AddVertexQC(proj->GetEndPos(),   color);
}
