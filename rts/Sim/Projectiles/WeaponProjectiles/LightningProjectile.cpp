/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "LightningProjectile.h"
#include "Sim/Misc/GlobalSynced.h"
#include "Sim/Weapons/Weapon.h"
#include "Sim/Weapons/WeaponDef.h"
// FIXME: the following should not be here
#include "Rendering/Projectiles/WeaponProjectiles/LightningProjectileDrawer.h"
#include "Rendering/Projectiles/WeaponProjectiles/LightningProjectileMinimapDrawer.h"

#ifdef TRACE_SYNC
	#include "Sync/SyncTracer.h"
#endif

CR_BIND_DERIVED(CLightningProjectile, CWeaponProjectile, (ZeroVector, ZeroVector, NULL, ZeroVector, NULL, 0, NULL));

CR_REG_METADATA(CLightningProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(color),
	CR_MEMBER(endPos),
	CR_MEMBER(weapon),
	CR_MEMBER(displacements),
	CR_MEMBER(displacements2),
	CR_RESERVED(16)
	));

CLightningProjectile::CLightningProjectile(
		const float3& pos, const float3& end,
		CUnit* owner,
		const float3& color,
		const WeaponDef* weaponDef,
		int ttl, CWeapon* weap)
	: CWeaponProjectile(pos, ZeroVector, owner, NULL, ZeroVector, weaponDef, NULL, ttl)
	, color(color)
	, endPos(end)
	, weapon(weap)
{
	projectileType = WEAPON_LIGHTNING_PROJECTILE;
	checkCol = false;
	drawRadius = pos.distance(endPos);

	displacements[0] = 0.0f;
	for (size_t d = 1; d < displacements_size; ++d) {
		displacements[d]  = (gs->randFloat() - 0.5f) * drawRadius * 0.05f;
	}

	displacements2[0] = 0.0f;
	for (size_t d = 1; d < displacements_size; ++d) {
		displacements2[d] = (gs->randFloat() - 0.5f) * drawRadius * 0.05f;
	}

	if (weapon) {
		AddDeathDependence(weapon);
	}

#ifdef TRACE_SYNC
	tracefile << "New lightning: ";
	tracefile << pos.x << " " << pos.y << " " << pos.z << " " << end.x << " " << end.y << " " << end.z << "\n";
#endif

	if (!cegTag.empty()) {
		ceg.Load(explGenHandler, cegTag);
	}
}

CLightningProjectile::~CLightningProjectile()
{
}

void CLightningProjectile::Update()
{
	if (--ttl <= 0) {
		deleteMe = true;
	} else {
		if (!cegTag.empty()) {
			ceg.Explosion(pos + ((endPos - pos) / ttl), 0.0f, displacements[0], NULL, 0.0f, NULL, endPos - pos);
		}
	}

	if (weapon && !luaMoveCtrl) {
		pos = weapon->weaponMuzzlePos;
	}

	for (size_t d = 1; d < displacements_size; ++d) {
		displacements[d]  += (gs->randFloat() - 0.5f) * 0.3f;
		displacements2[d] += (gs->randFloat() - 0.5f) * 0.3f;
	}
}


void CLightningProjectile::Draw()
{
	GetDrawer()->Render(this);
}

ProjectileDrawer* CLightningProjectile::myProjectileDrawer = NULL;

ProjectileDrawer* CLightningProjectile::GetDrawer() {

	if (myProjectileDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileDrawer = new CLightningProjectileDrawer();
	}

	return myProjectileDrawer;
}


void CLightningProjectile::DrawOnMinimap(CVertexArray& lines, CVertexArray& points)
{
	ProjectileMinimapDrawer::SetLines(&lines);
	ProjectileMinimapDrawer::SetPoints(&points);
	GetMinimapDrawer()->Render(this);
}

ProjectileMinimapDrawer* CLightningProjectile::myProjectileMinimapDrawer = NULL;

ProjectileMinimapDrawer* CLightningProjectile::GetMinimapDrawer() {

	if (myProjectileMinimapDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileMinimapDrawer = new CLightningProjectileMinimapDrawer();
	}

	return myProjectileMinimapDrawer;
}


void CLightningProjectile::DependentDied(CObject* o)
{
	if (o == weapon) {
		weapon = NULL;
	}
}
