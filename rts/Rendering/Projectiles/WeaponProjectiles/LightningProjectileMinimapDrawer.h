/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef LIGHTNING_PROJECTILE_MINIMAP_DRAWER_H
#define LIGHTNING_PROJECTILE_MINIMAP_DRAWER_H

#include "WeaponProjectileMinimapDrawer.h"

class CWorldObject;

/**
 * Draws lightning projectiles on a 2D mini-map.
 */
class CLightningProjectileMinimapDrawer : public WeaponProjectileMinimapDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // LIGHTNING_PROJECTILE_MINIMAP_DRAWER_H
