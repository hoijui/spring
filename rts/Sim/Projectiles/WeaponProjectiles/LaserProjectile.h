/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef LASER_PROJECTILE_H
#define LASER_PROJECTILE_H

#include "WeaponProjectile.h"

class CLaserProjectile : public CWeaponProjectile
{
	CR_DECLARE(CLaserProjectile);
public:
	CLaserProjectile(const float3& pos, const float3& speed, CUnit* owner,
			float length, const float3& color, const float3& color2,
			float intensity, const WeaponDef* weaponDef, int ttl = 1000);
	virtual ~CLaserProjectile();

	void Draw();
	void Update();
	void Collision(CUnit* unit);
	void Collision(CFeature* feature);
	void Collision();

	int ShieldRepulse(CPlasmaRepulser* shield, float3 shieldPos,
			float shieldForce, float shieldMaxSpeed);

	float GetIntensity() const { return intensity; }
	const float3& GetColor() const { return color; }
	const float3& GetColor2() const { return color2; }
	float GetLength() const { return length; }
	float GetCurLength() const { return curLength; }
	float GetSpeedF() const { return speedF; }
	float GetIntensityFalloff() const { return intensityFalloff; }
	float GetMidTexx() const { return midTexx; }
	/**
	 * Number of frames the laser had left to expand
	 * if it impacted before reaching full length.
	 */
	float GetStayTime() const { return stayTime; }

private:
	float intensity;
	float3 color;
	float3 color2;
	float length;
	float curLength;
	float speedF;
	float intensityFalloff;
	float midTexx;
	/**
	 * Number of frames the laser had left to expand
	 * if it impacted before reaching full length.
	 */
	int stayTime;
};

#endif /* LASER_PROJECTILE_H */
