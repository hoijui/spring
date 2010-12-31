/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "ExplosiveProjectile.h"
#include "Game/Camera.h"
#include "Map/Ground.h"
#include "Sim/Misc/InterceptHandler.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Weapons/WeaponDef.h"
// FIXME: the following should not be here
#include "Rendering/Projectiles/WeaponProjectiles/ExplosiveProjectileDrawer.h"

#ifdef TRACE_SYNC
	#include "System/Sync/SyncTracer.h"
#endif

CR_BIND_DERIVED(CExplosiveProjectile, CWeaponProjectile, (ZeroVector, ZeroVector, NULL, NULL, 1, 0));

CR_REG_METADATA(CExplosiveProjectile, (
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(areaOfEffect),
	CR_RESERVED(16)
));

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExplosiveProjectile::CExplosiveProjectile(
		const float3& pos, const float3& speed,
		CUnit* owner, const WeaponDef* weaponDef,
		int ttl, float areaOfEffect, float g)
	: CWeaponProjectile(pos, speed, owner, NULL, ZeroVector, weaponDef, NULL, ttl)
	, areaOfEffect(areaOfEffect)
	, curTime(0)
{
	projectileType = WEAPON_EXPLOSIVE_PROJECTILE;

	//! either map or weaponDef gravity
	mygravity = g;
	useAirLos = true;

	if (weaponDef) {
		SetRadius(weaponDef->collisionSize);
		drawRadius = weaponDef->size;
	}

	if (ttl <= 0) {
		invTtl = 1;
	} else {
		invTtl = 1.0f / ttl;
	}

#ifdef TRACE_SYNC
	tracefile << "New explosive: ";
	tracefile << pos.x << " " << pos.y << " " << pos.z << " " << speed.x << " " << speed.y << " " << speed.z << "\n";
#endif

	if (!cegTag.empty()) {
		ceg.Load(explGenHandler, cegTag);
	}
}

void CExplosiveProjectile::Update()
{
//	if (!luaMoveCtrl) {
//		pos += speed;
//		speed.y += mygravity;
//	}
	CProjectile::Update();

	if (--ttl == 0) {
		Collision();
	} else {
		if (!cegTag.empty() && ttl > 0) {
			ceg.Explosion(pos, ttl, areaOfEffect, NULL, 0.0f, NULL, speed);
		}
	}

	if (weaponDef->noExplode) {
		if (TraveledRange()) {
			CProjectile::Collision();
		}
	}

	curTime += invTtl;
	if (curTime > 1) {
		curTime = 1;
	}
	UpdateGroundBounce();
}

void CExplosiveProjectile::Collision()
{
	if (!weaponDef->noExplode) {
		const float h = ground->GetHeightReal(pos.x, pos.z);
		const float3& n = ground->GetNormal(pos.x, pos.z);

		if (h > pos.y) {
			pos -= speed * std::max(0.0f, std::min(1.0f, float((h - pos.y) * n.y / n.dot(speed) + 0.1f)));
		} else if (weaponDef->waterweapon) {
			return; //let waterweapons go underwater
		}
	}

	CWeaponProjectile::Collision();
}

void CExplosiveProjectile::Collision(CUnit* unit)
{
	CWeaponProjectile::Collision(unit);
}

void CExplosiveProjectile::Draw()
{
	GetDrawer()->Render(this);
}

ProjectileDrawer* CExplosiveProjectile::myProjectileDrawer = NULL;

ProjectileDrawer* CExplosiveProjectile::GetDrawer() {

	if (myProjectileDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileDrawer = new CExplosiveProjectileDrawer();
	}

	return myProjectileDrawer;
}

int CExplosiveProjectile::ShieldRepulse(CPlasmaRepulser* shield, float3 shieldPos, float shieldForce, float shieldMaxSpeed)
{
	if (!luaMoveCtrl) {
		const float3 rdir = (pos - shieldPos).Normalize();

		if (rdir.dot(speed) < shieldMaxSpeed) {
			speed += (rdir * shieldForce);
			return 2;
		}
	}

	return 0;
}
