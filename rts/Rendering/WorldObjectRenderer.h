/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef WORLD_OBJECT_RENDERER_H
#define WORLD_OBJECT_RENDERER_H

class CWorldObject;

class WorldObjectRenderer
{
public:
	/**
	 * @brief Renders a world object.
	 * Renders a world object, for example in 3D OpenGL,
	 * on a 2D mini-map or the sound of it.
	 * This method is called once each render-frame.
	 */
	virtual void Render(const CWorldObject* object) const = 0;
};

#endif // WORLD_OBJECT_RENDERER_H
