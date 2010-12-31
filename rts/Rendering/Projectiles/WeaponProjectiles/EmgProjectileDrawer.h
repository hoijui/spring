/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef EMG_PROJECTILE_DRAWER_H
#define EMG_PROJECTILE_DRAWER_H

#include "WeaponProjectileDrawer.h"

class CWorldObject;

/**
 * Draws EMG projectiles in a 3D OpenGL scene.
 */
class CEmgProjectileDrawer : public WeaponProjectileDrawer
{
public:
	virtual void Render(const CWorldObject* object) const;
};

#endif // EMG_PROJECTILE_DRAWER_H
