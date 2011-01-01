/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef BEAM_LASER_PROJECTILE_H
#define BEAM_LASER_PROJECTILE_H

#include "WeaponProjectile.h"

class ProjectileDrawer;
class ProjectileMinimapDrawer;

class CBeamLaserProjectile: public CWeaponProjectile
{
	CR_DECLARE(CBeamLaserProjectile);
public:
	CBeamLaserProjectile(const float3& startPos, const float3& endPos,
		float startAlpha, float endAlpha, const float3& color,
		CUnit* owner, const WeaponDef* weaponDef);
	~CBeamLaserProjectile() {}

	void Update();

	/// @deprecated
	void Draw();
	/// @deprecated
	virtual void DrawOnMinimap(CVertexArray& lines, CVertexArray& points);

	const float3& GetStartPos() const { return startPos; }
	const float3& GetEndPos() const { return endPos; }
	const unsigned char* GetCoreColStart() const { return coreColStart; }
	const unsigned char* GetCoreColEnd() const { return coreColEnd; }
	const unsigned char* GetKoColStart() const { return koColStart; }
	const unsigned char* GetKoColEnd() const { return koColEnd; }
	float GetThickness() const { return thickness; }
	float GetCoreThickness() const { return coreThickness; }
	float GetFlareSize() const { return flareSize; }
	float GetDecay() const { return decay; }

private:
	float3 startPos;
	float3 endPos;
	unsigned char coreColStart[4];
	unsigned char coreColEnd[4];
	unsigned char koColStart[4];
	unsigned char koColEnd[4];

	float thickness;
	float coreThickness;
	float flareSize;

	float decay;

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

#endif // BEAM_LASER_PROJECTILE_H
