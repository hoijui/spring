/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef LIGHTNING_PROJECTILE_H
#define LIGHTNING_PROJECTILE_H

#include "WeaponProjectile.h"

class CWeapon;
class ProjectileDrawer;
class ProjectileMinimapDrawer;

class CLightningProjectile : public CWeaponProjectile
{
	CR_DECLARE(CLightningProjectile);
public:
	CLightningProjectile(const float3& pos, const float3& end, CUnit* owner,
			const float3& color, const WeaponDef* weaponDef, int ttl = 10,
			CWeapon* weap = NULL);
	~CLightningProjectile();

	void Update();
	void Draw();
	virtual void DrawOnMinimap(CVertexArray& lines, CVertexArray& points);
	void DependentDied(CObject* o);

	const float3& GetColor() const { return color; }
	const float3& GetEndPos() const { return endPos; }
	const CWeapon* GetWeapon() const { return weapon; }
	const float* GetDisplacements() const { return displacements; }
	const float* GetDisplacements2() const { return displacements2; }

private:
	float3 color;
	float3 endPos;
	CWeapon* weapon;

	#define displacements_size 10
	float displacements[displacements_size];
	float displacements2[displacements_size];

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

	/**
	 * Will be externalized into somewhere under Rendering later on.
	 * @deprecated
	 */
	static ProjectileMinimapDrawer* myProjectileMinimapDrawer;
	/**
	 * @see myProjectileMinimapDrawer
	 * @deprecated
	 */
	static ProjectileMinimapDrawer* GetMinimapDrawer();
};

#endif /* LIGHTNING_PROJECTILE_H */
