/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef LASER_PROJECTILE_DRAWER_H
#define LASER_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws laser projectiles in a 3D OpenGL scene.
 */
class CLaserProjectileDrawer : public WeaponProjectileDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // LASER_PROJECTILE_DRAWER_H
