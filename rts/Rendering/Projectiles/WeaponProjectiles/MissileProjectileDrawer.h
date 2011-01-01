/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef MISSILE_PROJECTILE_DRAWER_H
#define MISSILE_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws missile projectiles in a 3D OpenGL scene.
 */
class CMissileProjectileDrawer : public WeaponProjectileDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // MISSILE_PROJECTILE_DRAWER_H
