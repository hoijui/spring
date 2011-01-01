/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef LARGE_BEAM_LASER_PROJECTILE_DRAWER_H
#define LARGE_BEAM_LASER_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws large beam-laser projectiles in a 3D OpenGL scene.
 */
class CLargeBeamLaserProjectileDrawer : public WeaponProjectileDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // LARGE_BEAM_LASER_PROJECTILE_DRAWER_H
