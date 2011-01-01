/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef FLAME_PROJECTILE_DRAWER_H
#define FLAME_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws flame(-thrower) projectiles in a 3D OpenGL scene.
 */
class CFlameProjectileDrawer : public WeaponProjectileDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // FLAME_PROJECTILE_DRAWER_H
