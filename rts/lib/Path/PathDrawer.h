/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef PATH_DRAWER_H
#define PATH_DRAWER_H

#include "Rendering/IPathDrawer.h"

class CPathManager;
class CPathFinderDef;
class CPathFinder;
class CPathEstimator;

class CPathDrawer: public IPathDrawer {
public:
	static IPathDrawer* GetInstance();
	void Draw() const;
	void DrawHeat(int starty, int endy, int pwr2mapx_half, int offset, GLbyte* infoTexMem) const;
	void SetHeatMappingEnabled(bool enabled){
		benabled=enabled;
	};
private:
	void Draw(const CPathManager*) const;
	void Draw(const CPathFinderDef*) const;
	void Draw(const CPathFinder*) const;
	void Draw(const CPathEstimator*) const;
	bool benabled;
};

#endif
