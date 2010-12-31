/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef BEAM_LASER_PROJECTILE_MINIMAP_DRAWER_H
#define BEAM_LASER_PROJECTILE_MINIMAP_DRAWER_H

#include "WeaponProjectileMinimapDrawer.h"

class CWorldObject;

/**
 * Draws Beam-Laser projectiles on a 2D mini-map.
 */
class CBeamLaserProjectileMinimapDrawer : public WeaponProjectileMinimapDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // BEAM_LASER_PROJECTILE_MINIMAP_DRAWER_H
