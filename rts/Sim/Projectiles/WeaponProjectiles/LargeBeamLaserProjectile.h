/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef LARGE_BEAM_LASER_PROJECTILE_H
#define LARGE_BEAM_LASER_PROJECTILE_H

#include "WeaponProjectile.h"

class CVertexArray;
class ProjectileDrawer;
class ProjectileMinimapDrawer;

class CLargeBeamLaserProjectile : public CWeaponProjectile
{
	CR_DECLARE(CLargeBeamLaserProjectile);
public:
	CLargeBeamLaserProjectile(const float3& startPos, const float3& endPos,
			const float3& color, const float3& color2, CUnit* owner,
			const WeaponDef* weaponDef);
	~CLargeBeamLaserProjectile();

	void Update();
	void Draw();
	virtual void DrawOnMinimap(CVertexArray& lines, CVertexArray& points);

	const float3& GetStartPos() const { return startPos; }
	const float3& GetEndPos() const { return endPos; }
	const unsigned char* GetCoreColStart() const { return coreColStart; }
	const unsigned char* GetKoColStart() const { return koColStart; }
	float GetThickness() const { return thickness; }
	float GetCoreThickness() const { return coreThickness; }
	float GetFlareSize() const { return flareSize; }
	float GetTileLength() const { return tileLength; }
	float GetScrollSpeed() const { return scrollSpeed; }
	float GetPulseSpeed() const { return pulseSpeed; }
	float GetDecay() const { return decay; }

private:
	float3 startPos;
	float3 endPos;
	unsigned char coreColStart[4];
	//unsigned char corecolend[4];
	unsigned char koColStart[4];
	//unsigned char kocolend[4];

	//float startAlpha;
	//float endAlpha;
	float thickness;
	float coreThickness;
	float flareSize;
	//float midtexx;
	float tileLength;
	float scrollSpeed;
	float pulseSpeed;
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

#endif // LARGE_BEAM_LASER_PROJECTILE_H
