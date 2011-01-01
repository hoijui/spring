/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "Game/Camera.h"
#include "LaserProjectile.h"
#include "LogOutput.h"
#include "Map/Ground.h"
#include "Rendering/GL/VertexArray.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Projectiles/Unsynced/SimpleParticleSystem.h"
#include "Sim/Weapons/WeaponDef.h"
#include "GlobalUnsynced.h"

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
	CR_MEMBER(midTexx),
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

		const AtlasedTexture* tex2 = weaponDef->visuals.texture2;
		midTexx = (tex2->xstart + (tex2->xend - tex2->xstart) * 0.5f);
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
	if (model) {
		// dont draw if a 3d model has been defined for us
		return;
	}

	inArray = true;
	float3 dif(pos - camera->pos);
	const float camDist = dif.Length();
	dif /= camDist;
	float3 dir1(dif.cross(dir));
	dir1.Normalize();
	float3 dir2(dif.cross(dir1));

	unsigned char col[4];
	col[0]  = (unsigned char) (color.x  * intensity * 255);
	col[1]  = (unsigned char) (color.y  * intensity * 255);
	col[2]  = (unsigned char) (color.z  * intensity * 255);
	col[3]  = 1; //intensity*255;
	unsigned char col2[4];
	col2[0] = (unsigned char) (color2.x * intensity * 255);
	col2[1] = (unsigned char) (color2.y * intensity * 255);
	col2[2] = (unsigned char) (color2.z * intensity * 255);
	col2[3] = 1; //intensity*255;

	const float size = weaponDef->thickness;
	const float coresize = size * weaponDef->corethickness;

	va->EnlargeArrays(32, 0, VA_SIZE_TC);

	const AtlasedTexture* tex1 = weaponDef->visuals.texture1;
	if (camDist < weaponDef->lodDistance) {
		const AtlasedTexture* tex2 = weaponDef->visuals.texture2;
		const float3 pos2 = drawPos - (dir * curLength);
		float texStartOffset;
		float texEndOffset;
		if (checkCol) { // expanding or contracting?
			texStartOffset = 0;
			texEndOffset   = (1.0f - (curLength / length)) * (tex1->xstart - tex1->xend);
		} else {
			texStartOffset = (-1.0f + (curLength / length) + ((float)stayTime * (speedF / length))) * (tex1->xstart - tex1->xend);
			texEndOffset   = ((float)stayTime * (speedF / length)) * (tex1->xstart - tex1->xend);
		}

		va->AddVertexQTC(drawPos - (dir1 * size),                         midTexx,      tex2->ystart, col);
		va->AddVertexQTC(drawPos + (dir1 * size),                         midTexx,      tex2->yend,   col);
		va->AddVertexQTC(drawPos + (dir1 * size) - (dir2 * size),         tex2->xstart, tex2->yend,   col);
		va->AddVertexQTC(drawPos - (dir1 * size) - (dir2 * size),         tex2->xstart, tex2->ystart, col);
		va->AddVertexQTC(drawPos - (dir1 * coresize),                     midTexx,      tex2->ystart, col2);
		va->AddVertexQTC(drawPos + (dir1 * coresize),                     midTexx,      tex2->yend,   col2);
		va->AddVertexQTC(drawPos + (dir1 * coresize) - (dir2 * coresize), tex2->xstart, tex2->yend,   col2);
		va->AddVertexQTC(drawPos - (dir1 * coresize) - (dir2 * coresize), tex2->xstart, tex2->ystart, col2);

		va->AddVertexQTC(drawPos - (dir1 * size),     tex1->xstart + texStartOffset, tex1->ystart, col);
		va->AddVertexQTC(drawPos + (dir1 * size),     tex1->xstart + texStartOffset, tex1->yend,   col);
		va->AddVertexQTC(pos2    + (dir1 * size),     tex1->xend   + texEndOffset,   tex1->yend,   col);
		va->AddVertexQTC(pos2    - (dir1 * size),     tex1->xend   + texEndOffset,   tex1->ystart, col);
		va->AddVertexQTC(drawPos - (dir1 * coresize), tex1->xstart + texStartOffset, tex1->ystart, col2);
		va->AddVertexQTC(drawPos + (dir1 * coresize), tex1->xstart + texStartOffset, tex1->yend,   col2);
		va->AddVertexQTC(pos2    + (dir1 * coresize), tex1->xend   + texEndOffset,   tex1->yend,   col2);
		va->AddVertexQTC(pos2    - (dir1 * coresize), tex1->xend   + texEndOffset,   tex1->ystart, col2);

		va->AddVertexQTC(pos2    - (dir1 * size),                         midTexx,    tex2->ystart, col);
		va->AddVertexQTC(pos2    + (dir1 * size),                         midTexx,    tex2->yend,   col);
		va->AddVertexQTC(pos2    + (dir1 * size) + (dir2 * size),         tex2->xend, tex2->yend,   col);
		va->AddVertexQTC(pos2    - (dir1 * size) + (dir2 * size),         tex2->xend, tex2->ystart, col);
		va->AddVertexQTC(pos2    - (dir1 * coresize),                     midTexx,    tex2->ystart, col2);
		va->AddVertexQTC(pos2    + (dir1 * coresize),                     midTexx,    tex2->yend,   col2);
		va->AddVertexQTC(pos2    + (dir1 * coresize) + (dir2 * coresize), tex2->xend, tex2->yend,   col2);
		va->AddVertexQTC(pos2    - (dir1 * coresize) + (dir2 * coresize), tex2->xend, tex2->ystart, col2);
	} else {
		const float3 pos1 = drawPos + (dir * (size * 0.5f));
		const float3 pos2 = pos1 - (dir * (curLength + size));
		float texStartOffset;
		float texEndOffset;
		if (checkCol) { // expanding or contracting?
			texStartOffset = 0;
			texEndOffset   = (1.0f - (curLength / length)) * (tex1->xstart - tex1->xend);
		} else {
			texStartOffset = (-1.0f + (curLength / length) + ((float)stayTime * (speedF / length))) * (tex1->xstart - tex1->xend);
			texEndOffset   = ((float)stayTime * (speedF / length)) * (tex1->xstart - tex1->xend);
		}

		va->AddVertexQTC(pos1 - (dir1 * size),     tex1->xstart + texStartOffset, tex1->ystart, col);
		va->AddVertexQTC(pos1 + (dir1 * size),     tex1->xstart + texStartOffset, tex1->yend,   col);
		va->AddVertexQTC(pos2 + (dir1 * size),     tex1->xend   + texEndOffset,   tex1->yend,   col);
		va->AddVertexQTC(pos2 - (dir1 * size),     tex1->xend   + texEndOffset,   tex1->ystart, col);
		va->AddVertexQTC(pos1 - (dir1 * coresize), tex1->xstart + texStartOffset, tex1->ystart, col2);
		va->AddVertexQTC(pos1 + (dir1 * coresize), tex1->xstart + texStartOffset, tex1->yend,   col2);
		va->AddVertexQTC(pos2 + (dir1 * coresize), tex1->xend   + texEndOffset,   tex1->yend,   col2);
		va->AddVertexQTC(pos2 - (dir1 * coresize), tex1->xend   + texEndOffset,   tex1->ystart, col2);
	}
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
