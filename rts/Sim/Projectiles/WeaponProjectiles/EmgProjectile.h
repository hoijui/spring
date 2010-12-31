/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _EMG_PROJECTILE_H
#define _EMG_PROJECTILE_H

#include "WeaponProjectile.h"

class ProjectileDrawer;

class CEmgProjectile : public CWeaponProjectile
{
	CR_DECLARE(CEmgProjectile);
public:
	CEmgProjectile(const float3& pos, const float3& speed, CUnit* owner,
			const float3& color, float intensity, int ttl,
			const WeaponDef* weaponDef);
	virtual ~CEmgProjectile();

	void Update();
	void Draw();
	void Collision(CUnit* unit);
	void Collision();

	int ShieldRepulse(CPlasmaRepulser* shield, float3 shieldPos,
			float shieldForce, float shieldMaxSpeed);

	float GetIntensity() const { return intensity; }
	const float3& GetColor() const { return color; }

private:
	float intensity;
	float3 color;

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

#endif // _EMG_PROJECTILE_H
