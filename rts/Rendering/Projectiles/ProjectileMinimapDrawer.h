/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef PROJECTILE_MINIMAP_DRAWER_H
#define PROJECTILE_MINIMAP_DRAWER_H

#include "Rendering/WorldObjectRenderer.h"

class CVertexArray;

class ProjectileMinimapDrawer : public WorldObjectRenderer
{
public:
	static void SetLines(CVertexArray* lines) {
		ProjectileMinimapDrawer::lines = lines;
	}
	static void SetPoints(CVertexArray* points) {
		ProjectileMinimapDrawer::points = points;
	}

protected:
	static CVertexArray* GetLines() { return lines; }
	static CVertexArray* GetPoints() { return points; }

private:
	static CVertexArray* lines;
	static CVertexArray* points;
};

#endif // PROJECTILE_MINIMAP_DRAWER_H
