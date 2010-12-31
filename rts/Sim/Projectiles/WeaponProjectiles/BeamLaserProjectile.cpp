/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "StdAfx.h"
#include "mmgr.h"

#include "BeamLaserProjectile.h"
#include "Game/Camera.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Weapons/WeaponDef.h"

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

	inArray = true;

	float3 dif(pos - camera->pos);
	float camDist = dif.Length();
	dif /= camDist;

	const float3 ddir = (GetEndPos() - GetStartPos()).Normalize();
	const float3 dir1 = (dif.cross(ddir)).Normalize();
	const float3 dir2(dif.cross(dir1));

	const float size = GetThickness();
	const float coresize = size * GetCoreThickness();
	const float3& pos1 = GetStartPos();
	const float3& pos2 = GetEndPos();

	va->EnlargeArrays(32, 0, VA_SIZE_TC);

	#define WT1 weaponDef->visuals.texture1
	#define WT2 weaponDef->visuals.texture2
	#define WT3 weaponDef->visuals.texture3

	if (camDist < 1000.0f) {
		va->AddVertexQTC(pos1 - dir1 * size,                       GetMidTexx(),   WT2->ystart, GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size,                       GetMidTexx(),   WT2->yend,   GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size - dir2 * size,         WT2->xend,      WT2->yend,   GetKoColStart());
		va->AddVertexQTC(pos1 - dir1 * size - dir2 * size,         WT2->xend,      WT2->ystart, GetKoColStart());
		va->AddVertexQTC(pos1 - dir1 * coresize,                   GetMidTexx(),   WT2->ystart, GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize,                   GetMidTexx(),   WT2->yend,   GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize - dir2 * coresize, WT2->xend,      WT2->yend,   GetCoreColStart());
		va->AddVertexQTC(pos1 - dir1 * coresize - dir2 * coresize, WT2->xend,      WT2->ystart, GetCoreColStart());

		va->AddVertexQTC(pos1 - dir1 * size,                       WT1->xstart, WT1->ystart, GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size,                       WT1->xstart, WT1->yend,   GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size,                       WT1->xend,   WT1->yend,   GetKoColEnd());
		va->AddVertexQTC(pos2 - dir1 * size,                       WT1->xend,   WT1->ystart, GetKoColEnd());
		va->AddVertexQTC(pos1 - dir1 * coresize,                   WT1->xstart, WT1->ystart, GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize,                   WT1->xstart, WT1->yend,   GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize,                   WT1->xend,   WT1->yend,   GetCoreColEnd());
		va->AddVertexQTC(pos2 - dir1 * coresize,                   WT1->xend,   WT1->ystart, GetCoreColEnd());

		va->AddVertexQTC(pos2 - dir1 * size,                       GetMidTexx(),   WT2->ystart, GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size,                       GetMidTexx(),   WT2->yend,   GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size + dir2 * size,         WT2->xend,      WT2->yend,   GetKoColStart());
		va->AddVertexQTC(pos2 - dir1 * size + dir2 * size,         WT2->xend,      WT2->ystart, GetKoColStart());
		va->AddVertexQTC(pos2 - dir1 * coresize,                   GetMidTexx(),   WT2->ystart, GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize,                   GetMidTexx(),   WT2->yend,   GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize + dir2 * coresize, WT2->xend,      WT2->yend,   GetCoreColStart());
		va->AddVertexQTC(pos2 - dir1 * coresize + dir2 * coresize, WT2->xend,      WT2->ystart, GetCoreColStart());
	} else {
		va->AddVertexQTC(pos1 - dir1 * size,                       WT1->xstart, WT1->ystart, GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size,                       WT1->xstart, WT1->yend,   GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size,                       WT1->xend,   WT1->yend,   GetKoColEnd());
		va->AddVertexQTC(pos2 - dir1 * size,                       WT1->xend,   WT1->ystart, GetKoColEnd());
		va->AddVertexQTC(pos1 - dir1 * coresize,                   WT1->xstart, WT1->ystart, GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize,                   WT1->xstart, WT1->yend,   GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize,                   WT1->xend,   WT1->yend,   GetCoreColEnd());
		va->AddVertexQTC(pos2 - dir1 * coresize,                   WT1->xend,   WT1->ystart, GetCoreColEnd());
	}

	// draw flare
	float fsize = size * GetFlareSize();
	va->AddVertexQTC(pos1 - camera->right * fsize-camera->up * fsize, WT3->xstart, WT3->ystart, GetKoColStart());
	va->AddVertexQTC(pos1 + camera->right * fsize-camera->up * fsize, WT3->xend,   WT3->ystart, GetKoColStart());
	va->AddVertexQTC(pos1 + camera->right * fsize+camera->up * fsize, WT3->xend,   WT3->yend,   GetKoColStart());
	va->AddVertexQTC(pos1 - camera->right * fsize+camera->up * fsize, WT3->xstart, WT3->yend,   GetKoColStart());

	fsize = fsize * GetCoreThickness();
	va->AddVertexQTC(pos1 - camera->right * fsize-camera->up * fsize, WT3->xstart, WT3->ystart, GetCoreColStart());
	va->AddVertexQTC(pos1 + camera->right * fsize-camera->up * fsize, WT3->xend,   WT3->ystart, GetCoreColStart());
	va->AddVertexQTC(pos1 + camera->right * fsize+camera->up * fsize, WT3->xend,   WT3->yend,   GetCoreColStart());
	va->AddVertexQTC(pos1 - camera->right * fsize+camera->up * fsize, WT3->xstart, WT3->yend,   GetCoreColStart());

	#undef WT3
	#undef WT2
	#undef WT1
}

void CBeamLaserProjectile::DrawOnMinimap(CVertexArray& lines, CVertexArray& points)
{
	const unsigned char color[4] = {
			GetKoColStart()[0],
			GetKoColStart()[1],
			GetKoColStart()[2],
			255
			};

	lines.AddVertexQC(GetStartPos(), color);
	lines.AddVertexQC(GetEndPos(),   color);
}
