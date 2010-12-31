/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef BEAM_LASER_PROJECTILE_DRAWER_H
#define BEAM_LASER_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws Beam-Laser projectiles in a 3D OpenGL scene.
 */
class CBeamLaserProjectileDrawer : public WeaponProjectileDrawer
{
public:
	CBeamLaserProjectileDrawer();

	virtual void Render(const CWorldObject* object) const;
};

#endif // BEAM_LASER_PROJECTILE_DRAWER_H
