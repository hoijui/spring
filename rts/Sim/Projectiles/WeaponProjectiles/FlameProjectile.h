/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _FLAME_PROJECTILE_H_
#define _FLAME_PROJECTILE_H_

#include "WeaponProjectile.h"

class ProjectileDrawer;

class CFlameProjectile : public CWeaponProjectile
{
	CR_DECLARE(CFlameProjectile);
public:
	CFlameProjectile(const float3& pos, const float3& speed,
			const float3& spread, CUnit* owner, const WeaponDef* weaponDef,
			int ttl = 50);
	~CFlameProjectile();

	void Update();
	void Draw();
	void Collision(CUnit* unit);
	void Collision();

	int ShieldRepulse(CPlasmaRepulser* shield, float3 shieldPos,
			float shieldForce, float shieldMaxSpeed);

	const float3& GetColor() const { return color; }
	const float3& GetColor2() const { return color2; }
	float GetIntensity() const { return intensity; }
	const float3& GetSpread() const { return spread; }
	float GetCurTime() const { return curTime; }
	/// precentage of lifetime when the projectile is active and can collide
	float GetPhysLife() const { return physLife; }
	float GetInvTtl() const { return invTtl; }

private:
	float3 color;
	float3 color2;
	float intensity;
	float3 spread;
	float curTime;
	/// precentage of lifetime when the projectile is active and can collide
	float physLife;
	float invTtl;

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

#endif // _FLAME_PROJECTILE_H_
