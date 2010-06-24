/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef I_PATH_DRAWER_H
#define I_PATH_DRAWER_H

#include <cstdlib>
#include <Rendering/GL/myGL.h>


class IPathDrawer {
		static IPathDrawer* pd;
	public:
		static void Initialize();
		static IPathDrawer* GetInstance();
		virtual ~IPathDrawer(){};
		virtual void Draw()=0;
		virtual void DrawHeat(int starty, int endy, int pwr2mapx_half, int offset, GLbyte* infoTexMem)=0;
};

#endif

