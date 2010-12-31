/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _EXPLOSIVE_PROJECTILE_H
#define _EXPLOSIVE_PROJECTILE_H

#include "WeaponProjectile.h"

class ProjectileDrawer;

class CExplosiveProjectile : public CWeaponProjectile
{
	CR_DECLARE(CExplosiveProjectile);
public:
	CExplosiveProjectile(const float3& pos, const float3& speed,
		CUnit* owner, const WeaponDef* weaponDef,
		int ttl = 100000, float areaOfEffect = 8.0f,
		float gravity = 0.0f);

	void Update();
	void Draw();
	void Collision(CUnit* unit);
	void Collision();

	int ShieldRepulse(CPlasmaRepulser* shield, float3 shieldPos,
			float shieldForce, float shieldMaxSpeed);

	float GetareaOfEffect() const { return areaOfEffect; }
	float GetInvTtl() const { return invTtl; }
	float GetCurTime() const { return curTime; }

private:
	float areaOfEffect;
	float invTtl;
	float curTime;

	/**
	 * Will be externalized into somewhere under Rendering later on.
	 * @deprecated
	 */
	static ProjectileDrawer* myProjectileDrawer;
	/**
	 * @see myProjectileDrawer
	 * @deprecated
	 */
	static ProjectileDrawer* GetDrawer();
};

#endif // _EXPLOSIVE_PROJECTILE_H
