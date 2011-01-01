/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "LargeBeamLaserProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/LargeBeamLaserProjectile.h"
#include "Sim/Weapons/WeaponDef.h"
#include "System/GlobalUnsynced.h"
#include "Game/Camera.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/ColorMap.h"
#include "Rendering/Textures/TextureAtlas.h"

void CLargeBeamLaserProjectileDrawer::Render(const CWorldObject* object) const
{
	const CLargeBeamLaserProjectile* proj = (const CLargeBeamLaserProjectile*) object;

	CProjectile::inArray = true;

	AtlasedTexture beamTex;
	AtlasedTexture side;
	if (proj->GetWeaponDef()) {
		beamTex = *(proj->GetWeaponDef()->visuals.texture1);
		side    = *(proj->GetWeaponDef()->visuals.texture3);
	}

	float3 dif(proj->GetPos() - camera->pos);
	float camDist = dif.Length();
	dif /= camDist;

	float3 ddir(proj->GetEndPos() - proj->GetStartPos());
	float beamlength = ddir.Length();
	ddir = ddir / beamlength;

	const float3 dir1(dif.cross(ddir).Normalize());
	const float3 dir2(dif.cross(dir1));

	float3 pos1 = proj->GetStartPos();
	float3 pos2 = proj->GetEndPos();

	float starttex = gu->modGameTime * proj->GetScrollSpeed();
	starttex = 1.0f - (starttex - (int)starttex);
	const float texxsize = beamTex.xend - beamTex.xstart;
	AtlasedTexture tex = beamTex;
	const float& size = proj->GetThickness();
	const float& coresize = size * proj->GetCoreThickness();

	float polylength = (tex.xend - beamTex.xstart) * (1 / texxsize) * proj->GetTileLength();

	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	float istart = polylength * (1 - starttex);
	float iend = beamlength - proj->GetTileLength();
	va->EnlargeArrays(64 + (8 * ((int)((iend - istart) / proj->GetTileLength()) + 2)), 0, VA_SIZE_TC);
	if (istart > beamlength) {
		// beam short enough to be drawn by one polygon
		// draw laser start
		tex.xstart = beamTex.xstart + starttex * (beamTex.xend - beamTex.xstart);

		va->AddVertexQTC(pos1 - (dir1 * size),     tex.xstart, tex.ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 + (dir1 * size),     tex.xstart, tex.yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 + (dir1 * size),     tex.xend,   tex.yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 - (dir1 * size),     tex.xend,   tex.ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 - (dir1 * coresize), tex.xstart, tex.ystart, proj->GetCoreColStart());
		va->AddVertexQTC(pos1 + (dir1 * coresize), tex.xstart, tex.yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 + (dir1 * coresize), tex.xend,   tex.yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 - (dir1 * coresize), tex.xend,   tex.ystart, proj->GetCoreColStart());
	} else {
		// beam longer than one polygon
		pos2 = pos1 + ddir * istart;

		// draw laser start
		tex.xstart = beamTex.xstart + (starttex * (beamTex.xend - beamTex.xstart));

		va->AddVertexQTC(pos1 - (dir1 * size),     tex.xstart, tex.ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 + (dir1 * size),     tex.xstart, tex.yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 + (dir1 * size),     tex.xend,   tex.yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 - (dir1 * size),     tex.xend,   tex.ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 - (dir1 * coresize), tex.xstart, tex.ystart, proj->GetCoreColStart());
		va->AddVertexQTC(pos1 + (dir1 * coresize), tex.xstart, tex.yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 + (dir1 * coresize), tex.xend,   tex.yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 - (dir1 * coresize), tex.xend,   tex.ystart, proj->GetCoreColStart());

		// draw continous beam
		tex.xstart = beamTex.xstart;
		float i;
		for (i = istart; i < iend; i += proj->GetTileLength()) //! CAUTION: loop count must match EnlargeArrays above
		{
			pos1 = proj->GetStartPos() + ddir * i;
			pos2 = proj->GetStartPos() + ddir * (i + proj->GetTileLength());

			va->AddVertexQTC(pos1 - (dir1 * size),     tex.xstart, tex.ystart, proj->GetKoColStart());
			va->AddVertexQTC(pos1 + (dir1 * size),     tex.xstart, tex.yend,   proj->GetKoColStart());
			va->AddVertexQTC(pos2 + (dir1 * size),     tex.xend,   tex.yend,   proj->GetKoColStart());
			va->AddVertexQTC(pos2 - (dir1 * size),     tex.xend,   tex.ystart, proj->GetKoColStart());
			va->AddVertexQTC(pos1 - (dir1 * coresize), tex.xstart, tex.ystart, proj->GetCoreColStart());
			va->AddVertexQTC(pos1 + (dir1 * coresize), tex.xstart, tex.yend,   proj->GetCoreColStart());
			va->AddVertexQTC(pos2 + (dir1 * coresize), tex.xend,   tex.yend,   proj->GetCoreColStart());
			va->AddVertexQTC(pos2 - (dir1 * coresize), tex.xend,   tex.ystart, proj->GetCoreColStart());
		}

		// draw laser end
		pos1 = proj->GetStartPos() + ddir * i;
		pos2 = proj->GetEndPos();
		tex.xend = tex.xstart + ((pos2 - pos1).Length() / proj->GetTileLength()) * texxsize;

		va->AddVertexQTC(pos1 - dir1 * size,     tex.xstart, tex.ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 + dir1 * size,     tex.xstart, tex.yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 + dir1 * size,     tex.xend,   tex.yend,   proj->GetKoColStart());
		va->AddVertexQTC(pos2 - dir1 * size,     tex.xend,   tex.ystart, proj->GetKoColStart());
		va->AddVertexQTC(pos1 - dir1 * coresize, tex.xstart, tex.ystart, proj->GetCoreColStart());
		va->AddVertexQTC(pos1 + dir1 * coresize, tex.xstart, tex.yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 + dir1 * coresize, tex.xend,   tex.yend,   proj->GetCoreColStart());
		va->AddVertexQTC(pos2 - dir1 * coresize, tex.xend,   tex.ystart, proj->GetCoreColStart());
	}

	const AtlasedTexture* tex2 = proj->GetWeaponDef()->visuals.texture2;
	//float midtexx = tex2->xstart + (tex2->xend-tex2->xstart)*0.5f;
	va->AddVertexQTC(pos2 - (dir1 * size),                         tex2->xstart, tex2->ystart, proj->GetKoColStart());
	va->AddVertexQTC(pos2 + (dir1 * size),                         tex2->xstart, tex2->yend,   proj->GetKoColStart());
	va->AddVertexQTC(pos2 + (dir1 * size) + (dir2 * size),         tex2->xend,   tex2->yend,   proj->GetKoColStart());
	va->AddVertexQTC(pos2 - (dir1 * size) + (dir2 * size),         tex2->xend,   tex2->ystart, proj->GetKoColStart());
	va->AddVertexQTC(pos2 - (dir1 * coresize),                     tex2->xstart, tex2->ystart, proj->GetCoreColStart());
	va->AddVertexQTC(pos2 + (dir1 * coresize),                     tex2->xstart, tex2->yend,   proj->GetCoreColStart());
	va->AddVertexQTC(pos2 + (dir1 * coresize) + (dir2 * coresize), tex2->xend,   tex2->yend,   proj->GetCoreColStart());
	va->AddVertexQTC(pos2 - (dir1 * coresize) + (dir2 * coresize), tex2->xend,   tex2->ystart, proj->GetCoreColStart());

	// draw muzzleflare
	starttex  = gu->modGameTime * proj->GetPulseSpeed();
	starttex -= (int)starttex;

		float muzzlesize = size * proj->GetFlareSize() * starttex;
		unsigned char corcol[4];
		unsigned char kocol[4];
		corcol[3] = 1;
		kocol[3] = 1;
		for (int i = 0; i < 3; i++) {
			corcol[i] = (int)(proj->GetCoreColStart()[i] * (1 - starttex));
			kocol[i]  = (int)(proj->GetKoColStart()[i]   * (1 - starttex));
		}

		pos1 = proj->GetStartPos() - ddir * (size * proj->GetFlareSize()) * 0.02f;

		va->AddVertexQTC(pos1 + (dir1 * muzzlesize),                       side.xstart, side.ystart, kocol);
		va->AddVertexQTC(pos1 + (dir1 * muzzlesize) + (ddir * muzzlesize), side.xend,   side.ystart, kocol);
		va->AddVertexQTC(pos1 - (dir1 * muzzlesize) + (ddir * muzzlesize), side.xend,   side.yend,   kocol);
		va->AddVertexQTC(pos1 - (dir1 * muzzlesize),                       side.xstart, side.yend,   kocol);
		muzzlesize = muzzlesize * 0.6f;
		va->AddVertexQTC(pos1 + (dir1 * muzzlesize),                       side.xstart, side.ystart, corcol);
		va->AddVertexQTC(pos1 + (dir1 * muzzlesize) + (ddir * muzzlesize), side.xend,   side.ystart, corcol);
		va->AddVertexQTC(pos1 - (dir1 * muzzlesize) + (ddir * muzzlesize), side.xend,   side.yend,   corcol);
		va->AddVertexQTC(pos1 - (dir1 * muzzlesize),                       side.xstart, side.yend,   corcol);

		starttex += 0.5f;
		if (starttex > 1) {
			starttex = starttex - 1;
		}
		for (int i = 0; i < 3; i++) {
			corcol[i] = (int)(proj->GetCoreColStart()[i] * (1 - starttex));
			kocol[i]  = (int)(proj->GetKoColStart()[i]   * (1 - starttex));
		}
		muzzlesize = size * proj->GetFlareSize() * starttex;
		va->AddVertexQTC(pos1 + (dir1 * muzzlesize),                       side.xstart, side.ystart, kocol);
		va->AddVertexQTC(pos1 + (dir1 * muzzlesize) + (ddir * muzzlesize), side.xend,   side.ystart, kocol);
		va->AddVertexQTC(pos1 - (dir1 * muzzlesize) + (ddir * muzzlesize), side.xend,   side.yend,   kocol);
		va->AddVertexQTC(pos1 - (dir1 * muzzlesize),                       side.xstart, side.yend,   kocol);
		muzzlesize = muzzlesize * 0.6f;
		va->AddVertexQTC(pos1 + (dir1 * muzzlesize),                       side.xstart, side.ystart, corcol);
		va->AddVertexQTC(pos1 + (dir1 * muzzlesize) + (ddir * muzzlesize), side.xend,   side.ystart, corcol);
		va->AddVertexQTC(pos1 - (dir1 * muzzlesize) + (ddir * muzzlesize), side.xend,   side.yend,   corcol);
		va->AddVertexQTC(pos1 - (dir1 * muzzlesize),                       side.xstart, side.yend,   corcol);

	// draw flare
	const AtlasedTexture* tex4 = proj->GetWeaponDef()->visuals.texture4;
	float fsize = size * proj->GetFlareSize();
	// move flare slightly in camera direction
	pos1 = proj->GetStartPos() - (camera->forward * 3);
	va->AddVertexQTC(pos1 - (camera->right * fsize - camera->up * fsize), tex4->xstart, tex4->ystart, proj->GetKoColStart());
	va->AddVertexQTC(pos1 + (camera->right * fsize - camera->up * fsize), tex4->xend,   tex4->ystart, proj->GetKoColStart());
	va->AddVertexQTC(pos1 + (camera->right * fsize + camera->up * fsize), tex4->xend,   tex4->yend,   proj->GetKoColStart());
	va->AddVertexQTC(pos1 - (camera->right * fsize + camera->up * fsize), tex4->xstart, tex4->yend,   proj->GetKoColStart());

	fsize = fsize * proj->GetCoreThickness();
	va->AddVertexQTC(pos1 - (camera->right * fsize - camera->up * fsize), tex4->xstart, tex4->ystart, proj->GetCoreColStart());
	va->AddVertexQTC(pos1 + (camera->right * fsize - camera->up * fsize), tex4->xend,   tex4->ystart, proj->GetCoreColStart());
	va->AddVertexQTC(pos1 + (camera->right * fsize + camera->up * fsize), tex4->xend,   tex4->yend,   proj->GetCoreColStart());
	va->AddVertexQTC(pos1 - (camera->right * fsize + camera->up * fsize), tex4->xstart, tex4->yend,   proj->GetCoreColStart());
}
