/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "IPathManager.h"
#include "lib/Path/PathManager.h"

IPathManager* IPathManager::pd = NULL;

void IPathManager::Initialize(){
	pd= new CPathManager();
}

