/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "LaserProjectile.h"
#include "Map/Ground.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Projectiles/Unsynced/SimpleParticleSystem.h"
#include "Sim/Weapons/WeaponDef.h"
#include "System/LogOutput.h"
// FIXME: the following should not be here
#include "Rendering/Projectiles/WeaponProjectiles/LaserProjectileDrawer.h"

#ifdef TRACE_SYNC
	#include "Sync/SyncTracer.h"
#endif

CR_BIND_DERIVED(CLaserProjectile, CWeaponProjectile, (ZeroVector, ZeroVector, NULL, 0.0f, ZeroVector, ZeroVector, 0.0f, NULL, 0));

CR_REG_METADATA(CLaserProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(dir),
	CR_MEMBER(intensity),
	CR_MEMBER(color),
	CR_MEMBER(color2),
	CR_MEMBER(length),
	CR_MEMBER(curLength),
	CR_MEMBER(speedF),
	CR_MEMBER(stayTime),
	CR_MEMBER(intensityFalloff),
	CR_RESERVED(16)
	));

CLaserProjectile::CLaserProjectile(
	const float3& pos, const float3& speed,
	CUnit* owner, float length,
	const float3& color, const float3& color2,
	float intensity,
	const WeaponDef* weaponDef, int ttl):

	CWeaponProjectile(pos, speed, owner, NULL, ZeroVector, weaponDef, NULL, ttl),
	intensity(intensity),
	color(color),
	color2(color2),
	length(length),
	curLength(0),
	intensityFalloff(weaponDef ? (intensity * weaponDef->falloffRate) : 0),
	stayTime(0)
{
	projectileType = WEAPON_LASER_PROJECTILE;

	speedF = speed.Length();
	dir = speed / speedF;

	if (weaponDef) {
		SetRadius(weaponDef->collisionSize);
	}

	drawRadius = length;

#ifdef TRACE_SYNC
	tracefile << "New laser: ";
	tracefile << pos.x << " " << pos.y << " " << pos.z << " " << speed.x << " " << speed.y << " " << speed.z << "\n";
#endif

	if (!cegTag.empty()) {
		ceg.Load(explGenHandler, cegTag);
	}
}

CLaserProjectile::~CLaserProjectile()
{
}

void CLaserProjectile::Update()
{
	if (!luaMoveCtrl) {
		pos += speed;
	}

	if (checkCol) {
		// normal
		curLength += speedF;
		if (curLength > length)
			curLength = length;
	} else {
		// fading out after hit
		if (stayTime <= 0)
			curLength -= speedF;
		else
			stayTime--;
		if (curLength <= 0) {
			deleteMe = true;
			curLength = 0;
		}
	}


	if (--ttl > 0 && checkCol) {
		if (!cegTag.empty()) {
			ceg.Explosion(pos, ttl, intensity, NULL, 0.0f, NULL, speed);
		}
	}

	if (weaponDef->visuals.hardStop) {
		if (ttl == 0 && checkCol) {
			checkCol = false;
			speed = ZeroVector;
			if (curLength < length) {
				// if the laser wasn't fully extended yet,
				// remember how long until it would have been
				// fully extended
				stayTime = int(1 + (length - curLength) / speedF);
			}
		}
	} else {
		if (ttl < 5 && checkCol) {
			intensity -= intensityFalloff * 0.2f;
			if (intensity <= 0) {
				deleteMe = true;
				intensity = 0;
			}
		}
	}

	if (!luaMoveCtrl) {
		float3 tempSpeed = speed;
		UpdateGroundBounce();

		if (tempSpeed != speed) {
			dir = speed;
			dir.Normalize();
		}
	}
}



void CLaserProjectile::Collision(CUnit* unit)
{
	float3 oldPos = pos;
	CWeaponProjectile::Collision(unit);

	deleteMe = false; // we will fade out over some time
	if (!weaponDef->noExplode) {
		checkCol = false;
		speed = ZeroVector;
		pos = oldPos;
		if (curLength < length) {
			// if the laser wasn't fully extended yet
			// and was too short for some reason,
			// remember how long until it would have
			// been fully extended
			curLength += speedF;
			stayTime = int(1 + (length - curLength) / speedF);
		}
	}
}

void CLaserProjectile::Collision(CFeature* feature)
{
	float3 oldPos = pos;
	CWeaponProjectile::Collision(feature);

	// we will fade out over some time
	deleteMe = false;
	if (!weaponDef->noExplode) {
		checkCol = false;
		speed = ZeroVector;
		pos = oldPos;
		if (curLength < length) {
			// if the laser wasn't fully extended yet,
			// remember how long until it would have been
			// fully extended
			stayTime = int(1 + (length - curLength) / speedF);
		}
	}
}

void CLaserProjectile::Collision()
{
	if (weaponDef->waterweapon && ground->GetHeightReal(pos.x, pos.z) < pos.y) {
		// prevent impact on water if waterweapon is set
		return;
	}
	float3 oldPos = pos;
	CWeaponProjectile::Collision();

	// we will fade out over some time
	deleteMe = false;
	if (!weaponDef->noExplode) {
		checkCol = false;
		speed = ZeroVector;
		pos = oldPos;
		if (curLength < length) {
			// if the laser wasn't fully extended yet,
			// remember how long until it would have been
			// fully extended
			stayTime = (int)(1 + (length - curLength) / speedF);
		}
	}
}


void CLaserProjectile::Draw()
{
	GetDrawer()->Render(this);
}

ProjectileDrawer* CLaserProjectile::myProjectileDrawer = NULL;

ProjectileDrawer* CLaserProjectile::GetDrawer() {

	if (myProjectileDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileDrawer = new CLaserProjectileDrawer();
	}

	return myProjectileDrawer;
}


int CLaserProjectile::ShieldRepulse(CPlasmaRepulser* shield, float3 shieldPos, float shieldForce, float shieldMaxSpeed)
{
	if (!luaMoveCtrl) {
		const float3 rdir = (pos - shieldPos).Normalize();

		if (rdir.dot(speed) < 0.0f) {
			speed -= (rdir * rdir.dot(speed) * 2.0f);
			dir = speed;
			dir.Normalize();
			return 1;
		}
	}

	return 0;
}
