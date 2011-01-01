/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef LIGHTNING_PROJECTILE_DRAWER_H
#define LIGHTNING_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws lightning projectiles in a 3D OpenGL scene.
 */
class CLightningProjectileDrawer : public WeaponProjectileDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // LIGHTNING_PROJECTILE_DRAWER_H
