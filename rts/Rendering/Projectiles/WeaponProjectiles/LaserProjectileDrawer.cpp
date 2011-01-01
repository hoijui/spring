/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "LaserProjectileDrawer.h"

#include "Sim/Projectiles/WeaponProjectiles/LaserProjectile.h"
#include "Sim/Weapons/WeaponDef.h"
#include "Game/Camera.h"
#include "Rendering/GL/VertexArray.h"
#include "Rendering/Textures/TextureAtlas.h"

void CLaserProjectileDrawer::Render(const CWorldObject* object) const
{
	const CLaserProjectile* proj = (const CLaserProjectile*) object;

	if (proj->GetModel()) {
		// dont draw if a 3d model has been defined for us
		return;
	}

	CProjectile::inArray = true;

	float3 dif(proj->GetPos() - camera->pos);
	const float camDist = dif.Length();
	dif /= camDist;
	float3 dir1(dif.cross(proj->GetDir()));
	dir1.Normalize();
	float3 dir2(dif.cross(dir1));

	unsigned char col[4];
	col[0]  = (unsigned char) (proj->GetColor().x  * proj->GetIntensity() * 255);
	col[1]  = (unsigned char) (proj->GetColor().y  * proj->GetIntensity() * 255);
	col[2]  = (unsigned char) (proj->GetColor().z  * proj->GetIntensity() * 255);
	col[3]  = 1; //proj->GetIntensity()*255;
	unsigned char col2[4];
	col2[0] = (unsigned char) (proj->GetColor2().x * proj->GetIntensity() * 255);
	col2[1] = (unsigned char) (proj->GetColor2().y * proj->GetIntensity() * 255);
	col2[2] = (unsigned char) (proj->GetColor2().z * proj->GetIntensity() * 255);
	col2[3] = 1; //proj->GetIntensity()*255;

	const float size = proj->GetWeaponDef()->thickness;
	const float coresize = size * proj->GetWeaponDef()->corethickness;

	// FIXME: hack
	CVertexArray* va = CProjectile::va;

	va->EnlargeArrays(32, 0, VA_SIZE_TC);

	const AtlasedTexture* tex1 = proj->GetWeaponDef()->visuals.texture1;
	const float3& drawPos = proj->GetDrawPos();
	if (camDist < proj->GetWeaponDef()->lodDistance) {
		const AtlasedTexture* tex2 = proj->GetWeaponDef()->visuals.texture2;
		float midTexx = (tex2->xstart + (tex2->xend - tex2->xstart) * 0.5f);

		const float3 pos2 = drawPos - (proj->GetDir() * proj->GetCurLength());
		float texStartOffset;
		float texEndOffset;
		if (proj->GetCheckCol()) { // expanding or contracting?
			texStartOffset = 0;
			texEndOffset   = (1.0f - (proj->GetCurLength() / proj->GetLength())) * (tex1->xstart - tex1->xend);
		} else {
			texStartOffset = (-1.0f + (proj->GetCurLength() / proj->GetLength()) + ((float)proj->GetStayTime() * (proj->GetSpeedF() / proj->GetLength()))) * (tex1->xstart - tex1->xend);
			texEndOffset   = ((float)proj->GetStayTime() * (proj->GetSpeedF() / proj->GetLength())) * (tex1->xstart - tex1->xend);
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
		const float3 pos1 = drawPos + (proj->GetDir() * (size * 0.5f));
		const float3 pos2 = pos1 - (proj->GetDir() * (proj->GetCurLength() + size));
		float texStartOffset;
		float texEndOffset;
		if (proj->GetCheckCol()) { // expanding or contracting?
			texStartOffset = 0;
			texEndOffset   = (1.0f - (proj->GetCurLength() / proj->GetLength())) * (tex1->xstart - tex1->xend);
		} else {
			texStartOffset = (-1.0f + (proj->GetCurLength() / proj->GetLength()) + ((float)proj->GetStayTime() * (proj->GetSpeedF() / proj->GetLength()))) * (tex1->xstart - tex1->xend);
			texEndOffset   = ((float)proj->GetStayTime() * (proj->GetSpeedF() / proj->GetLength())) * (tex1->xstart - tex1->xend);
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
