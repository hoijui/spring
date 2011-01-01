/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "MissileProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/MissileProjectile.h"
#include "Sim/Weapons/WeaponDef.h"
#include "Game/Camera.h"
#include "Rendering/GlobalRendering.h"
#include "Rendering/ProjectileDrawer.hpp"
#include "Rendering/GL/myGL.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Models/3DModel.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "System/myMath.h"

void CMissileProjectileDrawer::Render(const CWorldObject* object) const
{
	const CMissileProjectile* proj = (const CMissileProjectile*) object;

	CProjectile::inArray = true;

	const float age2 = (proj->GetAge() & 7) + globalRendering->timeOffset;

	unsigned char col[4];

	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	va->EnlargeArrays(8 + (4 * proj->GetNumParts()), 0, VA_SIZE_TC);

	if (proj->GetWeaponDef()->visuals.smokeTrail) {
		const float color = 0.6f;
		if (proj->GetDrawTrail()) {
			// draw the trail as a single quad
			float3 dif(proj->GetDrawPos() - camera->pos);
			dif.ANormalize();
			float3 dir1(dif.cross(proj->GetDir()));
			dir1.ANormalize();
			float3 dif2(proj->GetOldSmoke() - camera->pos);
			dif2.ANormalize();
			float3 dir2(dif2.cross(proj->GetOldDir()));
			dir2.ANormalize();

			float a1 = (1.0f / (CMissileProjectile::SMOKE_TIME)) * 255;
			a1 *= 0.7f + fabs(dif.dot(proj->GetDir()));
			const float alpha1 = std::min(255.0f, std::max(0.0f, a1));
			col[0] = (unsigned char) (color * alpha1);
			col[1] = (unsigned char) (color * alpha1);
			col[2] = (unsigned char) (color * alpha1);
			col[3] = (unsigned char) alpha1;

			unsigned char col2[4];
			float a2 = (1 - float(age2) / (CMissileProjectile::SMOKE_TIME)) * 255;

			if (proj->GetAge() < 8) {
				a2 = 0;
			}

			a2 *= 0.7f + fabs(dif2.dot(proj->GetOldDir()));
			const float alpha2 = std::min(255.0f, std::max(0.0f, a2));
			col2[0] = (unsigned char) (color * alpha2);
			col2[1] = (unsigned char) (color * alpha2);
			col2[2] = (unsigned char) (color * alpha2);
			col2[3] = (unsigned char) alpha2;

			const AtlasedTexture* tex2 = proj->GetWeaponDef()->visuals.texture2;
			const float size = 1.0f;
			const float size2 = (1 + (age2 * (1 / CMissileProjectile::SMOKE_TIME)) * 7);
			const float txs = tex2->xend - (tex2->xend - tex2->xstart) * (age2 / 8.0f);

			va->AddVertexQTC(proj->GetDrawPos()  - dir1 * size,  txs,        tex2->ystart, col);
			va->AddVertexQTC(proj->GetDrawPos()  + dir1 * size,  txs,        tex2->yend,   col);
			va->AddVertexQTC(proj->GetOldSmoke() + dir2 * size2, tex2->xend, tex2->yend,   col2);
			va->AddVertexQTC(proj->GetOldSmoke() - dir2 * size2, tex2->xend, tex2->ystart, col2);
		} else {
			// draw the trail as particles
			const float dist = proj->GetPos().distance(proj->GetOldSmoke());
			const float3 dirpos1 = proj->GetPos() - proj->GetDir() * dist * 0.33f;
			const float3 dirpos2 = proj->GetOldSmoke() + proj->GetOldDir() * dist * 0.33f;

			for (int a = 0; a < proj->GetNumParts(); ++a) { //! CAUTION: loop count must match EnlargeArrays above
				const float alpha = 255.0f;
				col[0] = (unsigned char) (color * alpha);
				col[1] = (unsigned char) (color * alpha);
				col[2] = (unsigned char) (color * alpha);
				col[3] = (unsigned char) alpha;

				const float size = (1 + (a * (1 / CMissileProjectile::SMOKE_TIME)) * 7);
				float3 pos1 = CalcBeizer(float(a) / (proj->GetNumParts()), proj->GetPos(), dirpos1, dirpos2, proj->GetOldSmoke());

				#define st projectileDrawer->smoketex[0]
				va->AddVertexQTC(pos1 + ( camera->up + camera->right) * size, st->xstart, st->ystart, col);
				va->AddVertexQTC(pos1 + ( camera->up - camera->right) * size, st->xend,   st->ystart, col);
				va->AddVertexQTC(pos1 + (-camera->up - camera->right) * size, st->xend,   st->ystart, col);
				va->AddVertexQTC(pos1 + (-camera->up + camera->right) * size, st->xstart, st->ystart, col);
				#undef st
			}
		}
	}

	// rocket flare
	col[0] = 255;
	col[1] = 210;
	col[2] = 180;
	col[3] = 1;
	const float fsize = proj->GetRadius() * 0.4f;
	const AtlasedTexture* tex1 = proj->GetWeaponDef()->visuals.texture1;
	va->AddVertexQTC(proj->GetDrawPos() - camera->right * fsize-camera->up * fsize, tex1->xstart, tex1->ystart, col);
	va->AddVertexQTC(proj->GetDrawPos() + camera->right * fsize-camera->up * fsize, tex1->xend,   tex1->ystart, col);
	va->AddVertexQTC(proj->GetDrawPos() + camera->right * fsize+camera->up * fsize, tex1->xend,   tex1->yend,   col);
	va->AddVertexQTC(proj->GetDrawPos() - camera->right * fsize+camera->up * fsize, tex1->xstart, tex1->yend,   col);
}
