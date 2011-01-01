/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "FlameProjectile.h"
#include "Map/Ground.h"
#include "Sim/Weapons/WeaponDef.h"
// FIXME: the following should not be here
#include "Rendering/Projectiles/WeaponProjectiles/FlameProjectileDrawer.h"

CR_BIND_DERIVED(CFlameProjectile, CWeaponProjectile, (ZeroVector, ZeroVector, ZeroVector, NULL, NULL, 0));

CR_REG_METADATA(CFlameProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(color),
	CR_MEMBER(color2),
	CR_MEMBER(intensity),
	CR_MEMBER(spread),
	CR_MEMBER(curTime),
	CR_MEMBER(physLife),
	CR_MEMBER(invTtl),
	CR_RESERVED(16)
	));


CFlameProjectile::CFlameProjectile(
	const float3& pos, const float3& speed, const float3& spread,
	CUnit* owner, const WeaponDef* weaponDef, int ttl):

	CWeaponProjectile(pos, speed, owner, NULL, ZeroVector, weaponDef, NULL, ttl),
	color(color),
	color2(color2),
	intensity(intensity),
	spread(spread),
	curTime(0)
{
	projectileType = WEAPON_FLAME_PROJECTILE;
	invTtl = 1.0f / ttl;

	if (weaponDef) {
		SetRadius(weaponDef->size * weaponDef->collisionSize);
		drawRadius = weaponDef->size;
		physLife = 1.0f / weaponDef->duration;
	}

	if (!cegTag.empty()) {
		ceg.Load(explGenHandler, cegTag);
	}
}

CFlameProjectile::~CFlameProjectile()
{
}

void CFlameProjectile::Collision()
{
	if (ground->GetHeightReal(pos.x, pos.z) < pos.y && weaponDef->waterweapon) {
		// prevent waterweapons from colliding with water
		return;
	}
	const float3 norm = ground->GetNormal(pos.x, pos.z);
	const float ns = speed.dot(norm);
	speed -= (norm * ns);
	pos.y += 0.05f;
	curTime += 0.05f;
}

void CFlameProjectile::Collision(CUnit* unit)
{
	CWeaponProjectile::Collision(unit);
}

void CFlameProjectile::Update()
{
	if (!luaMoveCtrl) {
		pos += speed;
		UpdateGroundBounce();
		speed += spread;
	}

	radius = radius + weaponDef->sizeGrowth;
	sqRadius = radius * radius;
	drawRadius = radius * weaponDef->collisionSize;

	curTime += invTtl;
	if (curTime > physLife) {
		checkCol = false;
	}
	if (curTime > 1) {
		curTime = 1;
		deleteMe = true;
	}

	if (!cegTag.empty()) {
		ceg.Explosion(pos, curTime, intensity, NULL, 0.0f, NULL, speed);
	}
}

void CFlameProjectile::Draw()
{
	GetDrawer()->Render(this);
}

ProjectileDrawer* CFlameProjectile::myProjectileDrawer = NULL;

ProjectileDrawer* CFlameProjectile::GetDrawer() {

	if (myProjectileDrawer == NULL) {
		// Note: This is never deleted, but it is to be (re-)moved anyway
		myProjectileDrawer = new CFlameProjectileDrawer();
	}

	return myProjectileDrawer;
}


int CFlameProjectile::ShieldRepulse(CPlasmaRepulser* shield, float3 shieldPos, float shieldForce, float shieldMaxSpeed)
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
