/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "BeamLaserProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/BeamLaserProjectile.h"
#include "Sim/Weapons/WeaponDef.h"
#include "Sim/Projectiles/ProjectileHandler.h"

#include "Game/Camera.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/TextureAtlas.h"

CBeamLaserProjectileDrawer::CBeamLaserProjectileDrawer()
{
	/*if (!cegTag.empty()) {
		ceg.Load(explGenHandler, cegTag);
	}*/
}

void CBeamLaserProjectileDrawer::Render(const CWorldObject* object) const
{
	const CBeamLaserProjectile* proj = (const CBeamLaserProjectile*) object;

	CProjectile::inArray = true;

	float3 dif(proj->pos - camera->pos);
	float camDist = dif.Length();
	dif /= camDist;

	const float3 ddir = (proj->GetEndPos() - proj->GetStartPos()).Normalize();
	const float3 dir1 = (dif.cross(ddir)).Normalize();
	const float3 dir2(dif.cross(dir1));

	const float size = proj->GetThickness();
	const float coresize = size * proj->GetCoreThickness();
	const float3& pos1 = proj->GetStartPos();
	const float3& pos2 = proj->GetEndPos();
	
	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	va->EnlargeArrays(32, 0, VA_SIZE_TC);

	#define WT1 proj->GetWeaponDef()->visuals.texture1
	#define WT2 proj->GetWeaponDef()->visuals.texture2
	#define WT3 proj->GetWeaponDef()->visuals.texture3

	if (camDist < 1000.0f) {
		va->AddVertexQTC(pos1 - dir1 * size,                       proj->GetMidTexx(),   WT2->ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size,                       proj->GetMidTexx(),   WT2->yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size - dir2 * size,         WT2->xend,            WT2->yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos1 - dir1 * size - dir2 * size,         WT2->xend,            WT2->ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 - dir1 * coresize,                   proj->GetMidTexx(),   WT2->ystart, proj->GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize,                   proj->GetMidTexx(),   WT2->yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize - dir2 * coresize, WT2->xend,            WT2->yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos1 - dir1 * coresize - dir2 * coresize, WT2->xend,            WT2->ystart, proj->GetCoreColStart());

		va->AddVertexQTC(pos1 - dir1 * size,                       WT1->xstart, WT1->ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size,                       WT1->xstart, WT1->yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size,                       WT1->xend,   WT1->yend,   proj->GetKoColEnd());
		va->AddVertexQTC(pos2 - dir1 * size,                       WT1->xend,   WT1->ystart, proj->GetKoColEnd());
		va->AddVertexQTC(pos1 - dir1 * coresize,                   WT1->xstart, WT1->ystart, proj->GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize,                   WT1->xstart, WT1->yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize,                   WT1->xend,   WT1->yend,   proj->GetCoreColEnd());
		va->AddVertexQTC(pos2 - dir1 * coresize,                   WT1->xend,   WT1->ystart, proj->GetCoreColEnd());

		va->AddVertexQTC(pos2 - dir1 * size,                       proj->GetMidTexx(),   WT2->ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size,                       proj->GetMidTexx(),   WT2->yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size + dir2 * size,         WT2->xend,            WT2->yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 - dir1 * size + dir2 * size,         WT2->xend,            WT2->ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos2 - dir1 * coresize,                   proj->GetMidTexx(),   WT2->ystart, proj->GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize,                   proj->GetMidTexx(),   WT2->yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize + dir2 * coresize, WT2->xend,            WT2->yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 - dir1 * coresize + dir2 * coresize, WT2->xend,            WT2->ystart, proj->GetCoreColStart());
	} else {
		va->AddVertexQTC(pos1 - dir1 * size,                       WT1->xstart, WT1->ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size,                       WT1->xstart, WT1->yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size,                       WT1->xend,   WT1->yend,   proj->GetKoColEnd());
		va->AddVertexQTC(pos2 - dir1 * size,                       WT1->xend,   WT1->ystart, proj->GetKoColEnd());
		va->AddVertexQTC(pos1 - dir1 * coresize,                   WT1->xstart, WT1->ystart, proj->GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize,                   WT1->xstart, WT1->yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize,                   WT1->xend,   WT1->yend,   proj->GetCoreColEnd());
		va->AddVertexQTC(pos2 - dir1 * coresize,                   WT1->xend,   WT1->ystart, proj->GetCoreColEnd());
	}

	// draw flare
	float fsize = size * proj->GetFlareSize();
	va->AddVertexQTC(pos1 - camera->right * fsize-camera->up * fsize, WT3->xstart, WT3->ystart, proj->GetKoColStart());
	va->AddVertexQTC(pos1 + camera->right * fsize-camera->up * fsize, WT3->xend,   WT3->ystart, proj->GetKoColStart());
	va->AddVertexQTC(pos1 + camera->right * fsize+camera->up * fsize, WT3->xend,   WT3->yend,   proj->GetKoColStart());
	va->AddVertexQTC(pos1 - camera->right * fsize+camera->up * fsize, WT3->xstart, WT3->yend,   proj->GetKoColStart());

	fsize = fsize * proj->GetCoreThickness();
	va->AddVertexQTC(pos1 - camera->right * fsize-camera->up * fsize, WT3->xstart, WT3->ystart, proj->GetCoreColStart());
	va->AddVertexQTC(pos1 + camera->right * fsize-camera->up * fsize, WT3->xend,   WT3->ystart, proj->GetCoreColStart());
	va->AddVertexQTC(pos1 + camera->right * fsize+camera->up * fsize, WT3->xend,   WT3->yend,   proj->GetCoreColStart());
	va->AddVertexQTC(pos1 - camera->right * fsize+camera->up * fsize, WT3->xstart, WT3->yend,   proj->GetCoreColStart());

	#undef WT3
	#undef WT2
	#undef WT1
}
