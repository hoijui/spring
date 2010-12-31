/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef WEAPON_PROJECTILE_MINIMAP_DRAWER_H
#define WEAPON_PROJECTILE_MINIMAP_DRAWER_H

#include "Rendering/Projectiles/ProjectileMinimapDrawer.h"

class CWorldObject;

class WeaponProjectileMinimapDrawer : public ProjectileMinimapDrawer
{
	virtual void Render(const CWorldObject* object) const;
};

#endif // WEAPON_PROJECTILE_MINIMAP_DRAWER_H
