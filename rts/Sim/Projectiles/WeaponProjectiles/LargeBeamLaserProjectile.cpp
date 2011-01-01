/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "LargeBeamLaserProjectile.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Weapons/WeaponDef.h"
// FIXME: the following should not be here
#include "Rendering/Projectiles/WeaponProjectiles/LargeBeamLaserProjectileDrawer.h"
#include "Rendering/Projectiles/WeaponProjectiles/LargeBeamLaserProjectileMinimapDrawer.h"

CR_BIND_DERIVED(CLargeBeamLaserProjectile, CWeaponProjectile, (ZeroVector, ZeroVector, ZeroVector, ZeroVector, NULL, NULL));

CR_REG_METADATA(CLargeBeamLaserProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(startPos),
	CR_MEMBER(endPos),
	CR_MEMBER(coreColStart),
	CR_MEMBER(koColStart),
	CR_MEMBER(thickness),
	CR_MEMBER(coreThickness),
	CR_MEMBER(flareSize),
	CR_MEMBER(tileLength),
	CR_MEMBER(scrollSpeed),
	CR_MEMBER(pulseSpeed),
	CR_RESERVED(16)
	));

CLargeBeamLaserProjectile::CLargeBeamLaserProjectile(
		const float3& startPos, const float3& endPos,
		const float3& color, const float3& color2,
		CUnit* owner, const WeaponDef* weaponDef)
	: CWeaponProjectile(startPos + (endPos - startPos) * 0.5f, ZeroVector, owner, NULL, ZeroVector, weaponDef, NULL, 1)
	, startPos(startPos)
	, endPos(endPos)
	, decay(1.0f)
{
	projectileType = WEAPON_LARGEBEAMLASER_PROJECTILE;
	checkCol = false;
	useAirLos = true;

	SetRadius(pos.distance(endPos));

	coreColStart[0] = (unsigned char)(color2.x * 255);
	coreColStart[1] = (unsigned char)(color2.y * 255);
	coreColStart[2] = (unsigned char)(color2.z * 255);
	coreColStart[3] = 1;
	koColStart[0]   = (unsigned char)(color.x * 255);
	koColStart[1]   = (unsigned char)(color.y * 255);
	koColStart[2]   = (unsigned char)(color.z * 255);
	koColStart[3]   = 1;

	if (weaponDef) {
		thickness     = weaponDef->thickness;
		coreThickness = weaponDef->corethickness;
		flareSize     = weaponDef->laserflaresize;
		tileLength    = weaponDef->visuals.tilelength;
		scrollSpeed   = weaponDef->visuals.scrollspeed;
		pulseSpeed    = weaponDef->visuals.pulseSpeed;
		ttl           = weaponDef->visuals.beamttl;
		decay         = weaponDef->visuals.beamdecay;
	}

	if (!cegTag.empty()) {
		ceg.Load(explGenHandler, cegTag);
	}
}

CLargeBeamLaserProjectile::~CLargeBeamLaserProjectile()
{
}


void CLargeBeamLaserProjectile::Update()
{
	if (ttl > 0) {
		ttl--;
		for (int i = 0; i < 3; i++) {
			coreColStart[i] = (unsigned char) (coreColStart[i] * decay);
			koColStart[i] = (unsigned char) (koColStart[i] * decay);
		}

		if (!cegTag.empty()) {
			ceg.Explosion(startPos + ((endPos - startPos) / ttl), 0.0f, flareSize, NULL, 0.0f, NULL, endPos - startPos);
		}
	}
	else {
		deleteMe = true;
	}
}

void CLargeBeamLaserProjectile::Draw()
{
	GetDrawer()->Render(this);
}

void CLargeBeamLaserProjectile::DrawOnMinimap(CVertexArray& lines, CVertexArray& points)
{
	ProjectileMinimapDrawer::SetLines(&lines);
	ProjectileMinimapDrawer::SetPoints(&points);
	GetMinimapDrawer()->Render(this);
}


ProjectileDrawer* CLargeBeamLaserProjectile::myProjectileDrawer = NULL;

ProjectileDrawer* CLargeBeamLaserProjectile::GetDrawer() {

	if (myProjectileDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileDrawer = new CLargeBeamLaserProjectileDrawer();
	}

	return myProjectileDrawer;
}


ProjectileMinimapDrawer* CLargeBeamLaserProjectile::myProjectileMinimapDrawer = NULL;

ProjectileMinimapDrawer* CLargeBeamLaserProjectile::GetMinimapDrawer() {

	if (myProjectileMinimapDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileMinimapDrawer = new CLargeBeamLaserProjectileMinimapDrawer();
	}

	return myProjectileMinimapDrawer;
}
