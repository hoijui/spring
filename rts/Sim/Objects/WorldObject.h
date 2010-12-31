/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H

#include "Object.h"
#include "float3.h"

struct S3DModel;

class CWorldObject: public CObject
{
public:
	CR_DECLARE(CWorldObject);

	CWorldObject(const float3& pos = ZeroVector)
		: id(0)
		, pos(pos)
		, radius(0.0f)
		, sqRadius(0.0f)
		, drawRadius(0.0f)
		, useAirLos(false)
		, alwaysVisible(false)
		, model(NULL)
	{}

	void SetRadius(float r);
	virtual ~CWorldObject();

	int GetId() const { return id; }
	const float3& GetPos() const { return pos; }
	float GetRadius() const { return radius; }
	float GetSqRadius() const { return sqRadius; }
	/// used to see if in LOS
	float GetDrawRadius() const { return drawRadius; }
	bool GetUseAirLos() const { return useAirLos; }
	bool GetAlwaysVisible() const { return alwaysVisible; }
	const S3DModel* GetModel() const { return model; }

	int id;

	float3 pos;
	float radius;       ///< used for collisions
	float sqRadius;

	float drawRadius;   ///< used to see if in los
	/// if true, object's visibility is checked against airLosMap[allyteam]
	bool useAirLos;
	bool alwaysVisible;

	S3DModel* model;
};

#endif /* WORLD_OBJECT_H */
