/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "IPathDrawer.h"
#include "lib/Path/PathDrawer.h"

IPathDrawer* IPathDrawer::pd = NULL;

void IPathDrawer::Initialize(){
	pd= new CPathDrawer();
}

IPathDrawer* IPathDrawer::GetInstance(){
	return pd;
}
