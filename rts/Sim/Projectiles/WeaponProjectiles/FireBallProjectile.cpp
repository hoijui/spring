/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "FireBallProjectile.h"
#include "Game/Camera.h"
#include "Map/Ground.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Weapons/WeaponDef.h"
#include "System/creg/STL_Deque.h"
// FIXME: the following should not be here
#include "Rendering/Projectiles/WeaponProjectiles/FireBallProjectileDrawer.h"

CR_BIND_DERIVED(CFireBallProjectile, CWeaponProjectile, (ZeroVector, ZeroVector, NULL, NULL, ZeroVector, NULL));
CR_BIND(CFireBallProjectile::Spark, );

CR_REG_METADATA(CFireBallProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(sparks),
	CR_RESERVED(8)
	));

CR_REG_METADATA_SUB(CFireBallProjectile,Spark,(
	CR_MEMBER(pos),
	CR_MEMBER(speed),
	CR_MEMBER(size),
	CR_MEMBER(ttl),
	CR_RESERVED(8)
	));

CFireBallProjectile::CFireBallProjectile(
	const float3& pos, const float3& speed,
	CUnit* owner, CUnit* target,
	const float3& targetPos,
	const WeaponDef* weaponDef):
	CWeaponProjectile(pos, speed, owner, target, targetPos, weaponDef, NULL, 1)
{
	projectileType = WEAPON_FIREBALL_PROJECTILE;

	if (weaponDef) {
		SetRadius(weaponDef->collisionSize);
		drawRadius = weaponDef->size;
	}

	if (!cegTag.empty()) {
		ceg.Load(explGenHandler, cegTag);
	}
}

CFireBallProjectile::~CFireBallProjectile()
{
}

void CFireBallProjectile::Draw()
{
	GetDrawer()->Render(this);
}

ProjectileDrawer* CFireBallProjectile::myProjectileDrawer = NULL;

ProjectileDrawer* CFireBallProjectile::GetDrawer() {

	if (myProjectileDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileDrawer = new CFireBallProjectileDrawer();
	}

	return myProjectileDrawer;
}


void CFireBallProjectile::Update()
{
	if (checkCol) {
		if (!luaMoveCtrl) {
			pos += speed;

			if (weaponDef->gravityAffected) {
				speed.y += mygravity;
			}
		}

		if (weaponDef->noExplode) {
			if (TraveledRange())
				checkCol = false;
		}

		EmitSpark();
	} else {
		if (sparks.empty()) {
			deleteMe = true;
		}
	}

	for (unsigned int i = 0; i < sparks.size(); i++) {
		sparks[i].ttl--;
		if (sparks[i].ttl == 0) {
			sparks.pop_back();
			break;
		}
		if (checkCol) {
			sparks[i].pos += sparks[i].speed;
		}
		sparks[i].speed *= 0.95f;
	}

	if (!cegTag.empty()) {
		ceg.Explosion(pos, ttl, (sparks.size() > 0) ? sparks[0].size : 0.0f, NULL, 0.0f, NULL, speed);
	}

	UpdateGroundBounce();
}

void CFireBallProjectile::EmitSpark()
{
	Spark spark;
	const float x = (rand() / (float) RAND_MAX) - 0.5f;
	const float y = (rand() / (float) RAND_MAX) - 0.5f;
	const float z = (rand() / (float) RAND_MAX) - 0.5f;
	spark.speed = (speed * 0.95f) + float3(x, y, z);
	spark.pos = pos - speed * (rand() / (float) RAND_MAX + 3) + spark.speed * 3;
	spark.size = 5.0f;
	spark.ttl = 15;

	sparks.push_front(spark);
}

void CFireBallProjectile::Collision()
{
	if (weaponDef->waterweapon && ground->GetHeightReal(pos.x, pos.z) < pos.y) {
		// make waterweapons not explode in water
		return;
	}

	CWeaponProjectile::Collision();
	deleteMe = false;
}
