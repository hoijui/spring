/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef MISSILE_PROJECTILE_H
#define MISSILE_PROJECTILE_H

#include "WeaponProjectile.h"

class CUnit;
class ProjectileDrawer;

class CMissileProjectile : public CWeaponProjectile
{
	CR_DECLARE(CMissileProjectile);
protected:
	void UpdateGroundBounce();
public:
	CMissileProjectile(const float3& pos, const float3& speed, CUnit* owner,
			float areaOfEffect, float maxSpeed, int ttl, CUnit* target,
			const WeaponDef* weaponDef, float3 targetPos);
	~CMissileProjectile();

	void DependentDied(CObject* o);
	void Collision(CUnit* unit);
	void Collision();

	void Update();
	void Draw();

	int ShieldRepulse(CPlasmaRepulser* shield, float3 shieldPos,
			float shieldForce, float shieldMaxSpeed);

	float GetMaxSpeed() const { return maxSpeed; }
	float GetCurSpeed() const { return curSpeed; }
	float GetAreaOfEffect() const { return areaOfEffect; }
	int GetAge() const { return age; }
	const float3& GetOldSmoke() const { return oldSmoke; }
	const float3& GetOldDir() const { return oldDir; }
	const CUnit* GetTarget() const { return target; }
	const CProjectile* GetDecoyTarget() const { return decoyTarget; }
	bool GetDrawTrail() const { return drawTrail; }
	int GetNumParts() const { return numParts; }
	const float3& GetTargPos() const { return targPos; }

	bool GetIsWobbling() const { return isWobbling; }
	const float3& GetWobbleDir() const { return wobbleDir; }
	int GetWobbleTime() const { return wobbleTime; }
	const float3& GetWobbleDif() const { return wobbleDif; }

	bool GetIsDancing() const { return isDancing; }
	int GetDanceTime() const { return danceTime; }
	const float3& GetDanceMove() const { return danceMove; }
	const float3& GetDanceCenter() const { return danceCenter; }

	float GetExtraHeight() const { return extraHeight; }
	float GetExtraHeightDecay() const { return extraHeightDecay; }
	int GetExtraHeightTime() const { return extraHeightTime; }

private:
	float maxSpeed;
	float curSpeed;
	float areaOfEffect;
	int age;
	float3 oldSmoke;
	float3 oldDir;
	CUnit* target;
public:
	CProjectile* decoyTarget;
private:
	bool drawTrail;
	int numParts;
	float3 targPos;

	bool isWobbling;
	float3 wobbleDir;
	int wobbleTime;
	float3 wobbleDif;
	
	bool isDancing;
	int danceTime;
	float3 danceMove;
	/**
	 * Vector that points towards the center of the dance
	 * to keep the movement "coherent"
	 */
	float3 danceCenter;

	float extraHeight;
	float extraHeightDecay;
	int extraHeightTime;

public:
	/// the smokes life-time in frames
	static const float SMOKE_TIME;

private:
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

#endif /* MISSILE_PROJECTILE_H */
