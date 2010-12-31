/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef EXPLOSIVE_PROJECTILE_DRAWER_H
#define EXPLOSIVE_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws Explosive projectiles in a 3D OpenGL scene.
 */
class CExplosiveProjectileDrawer : public WeaponProjectileDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // EXPLOSIVE_PROJECTILE_DRAWER_H
