/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef FIRE_BALL_PROJECTILE_DRAWER_H
#define FIRE_BALL_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws fire-ball projectiles in a 3D OpenGL scene.
 */
class CFireBallProjectileDrawer : public WeaponProjectileDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // FIRE_BALL_PROJECTILE_DRAWER_H
