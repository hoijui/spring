/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "BeamLaserProjectile.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Weapons/WeaponDef.h"
// FIXME: the following should not be here
#include "Rendering/Textures/TextureAtlas.h"
#include "Rendering/Projectiles/WeaponProjectiles/BeamLaserProjectileDrawer.h"
#include "Rendering/Projectiles/WeaponProjectiles/BeamLaserProjectileMinimapDrawer.h"

CR_BIND_DERIVED(CBeamLaserProjectile, CWeaponProjectile,
	(float3(ZeroVector), float3(ZeroVector), 0.0f, 0.0f, float3(ZeroVector), NULL, NULL));

CR_REG_METADATA(CBeamLaserProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(startPos),
	CR_MEMBER(endPos),
	CR_MEMBER(coreColStart),
	CR_MEMBER(coreColEnd),
	CR_MEMBER(koColStart),
	CR_MEMBER(koColEnd),
	CR_MEMBER(thickness),
	CR_MEMBER(coreThickness),
	CR_MEMBER(flareSize),
	CR_MEMBER(decay),
	CR_MEMBER(midTexx),
	CR_RESERVED(16)
	));

CBeamLaserProjectile::CBeamLaserProjectile(
	const float3& startPos, const float3& endPos,
	float startAlpha, float endAlpha,
	const float3& color,
	CUnit* owner,
	const WeaponDef* weaponDef):

	CWeaponProjectile((startPos + endPos) * 0.5f, ZeroVector, owner, NULL, ZeroVector, weaponDef, NULL, (weaponDef? weaponDef->visuals.beamttl: 0)),
	startPos(startPos),
	endPos(endPos),
	thickness(weaponDef? weaponDef->thickness: 0.0f),
	coreThickness(weaponDef? weaponDef->corethickness: 0.0f),
	flareSize(weaponDef? weaponDef->laserflaresize: 0.0f),
	decay(weaponDef? weaponDef->visuals.beamdecay: 0.0f)
{
	projectileType = WEAPON_BEAMLASER_PROJECTILE;
	checkCol = false;
	useAirLos = true;

	SetRadius(pos.distance(endPos));

	if (weaponDef) {
		midTexx =
			(weaponDef->visuals.texture2->xstart +
			(weaponDef->visuals.texture2->xend - weaponDef->visuals.texture2->xstart) * 0.5f);

		coreColStart[0] = (weaponDef->visuals.color2.x * startAlpha);
		coreColStart[1] = (weaponDef->visuals.color2.y * startAlpha);
		coreColStart[2] = (weaponDef->visuals.color2.z * startAlpha);
		coreColStart[3] = 1;
		coreColEnd[0] = (weaponDef->visuals.color2.x * endAlpha);
		coreColEnd[1] = (weaponDef->visuals.color2.y * endAlpha);
		coreColEnd[2] = (weaponDef->visuals.color2.z * endAlpha);
		coreColEnd[3] = 1;
		koColStart[0] = (color.x * startAlpha);
		koColStart[1] = (color.y * startAlpha);
		koColStart[2] = (color.z * startAlpha);
		koColStart[3] = 1;
		koColEnd[0] = (color.x * endAlpha);
		koColEnd[1] = (color.y * endAlpha);
		koColEnd[2] = (color.z * endAlpha);
		koColEnd[3] = 1;
	} else {
		midTexx = 0.0f;
	}

	if (!cegTag.empty()) {
		ceg.Load(explGenHandler, cegTag);
	}
}



void CBeamLaserProjectile::Update()
{
	if (ttl <= 0) {
		deleteMe = true;
	} else {
		ttl--;
		for (int i = 0; i < 3; i++) {
			coreColStart[i] = (coreColStart[i] * decay);
			coreColEnd[i] = (coreColEnd[i] * decay);
			koColStart[i] = (koColStart[i] * decay);
			koColEnd[i] = (koColEnd[i] * decay);
		}

		if (!cegTag.empty()) {
			ceg.Explosion(startPos + ((endPos - startPos) / ttl), 0.0f, flareSize, NULL, 0.0f, NULL, endPos - startPos);
		}
	}
}

void CBeamLaserProjectile::Draw()
{
	GetDrawer()->Render(this);
}

void CBeamLaserProjectile::DrawOnMinimap(CVertexArray& lines, CVertexArray& points)
{
	ProjectileMinimapDrawer::SetLines(&lines);
	ProjectileMinimapDrawer::SetPoints(&points);
	GetMinimapDrawer()->Render(this);
}


ProjectileDrawer* CBeamLaserProjectile::myProjectileDrawer = NULL;

ProjectileDrawer* CBeamLaserProjectile::GetDrawer() {

	if (myProjectileDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileDrawer = new CBeamLaserProjectileDrawer();
	}

	return myProjectileDrawer;
}


ProjectileMinimapDrawer* CBeamLaserProjectile::myProjectileMinimapDrawer = NULL;

ProjectileMinimapDrawer* CBeamLaserProjectile::GetMinimapDrawer() {

	if (myProjectileMinimapDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileMinimapDrawer = new CBeamLaserProjectileMinimapDrawer();
	}

	return myProjectileMinimapDrawer;
}
