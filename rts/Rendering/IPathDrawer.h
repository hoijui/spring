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
};

#define pathDrawer (IPathDrawer::GetInstance())

#endif

