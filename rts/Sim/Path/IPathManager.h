/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef I_PATH_MANAGER_H
#define I_PATH_MANAGER_H

#include <boost/cstdint.hpp> /* Replace with <stdint.h> if appropriate */
#include "float3.h"
#include "Vec2.h"
#include "Map/ReadMap.h"

class IPathDrawer;
struct MoveData;
class CSolidObject;

class IPathManager{
public:
	static void Initialize();
	virtual ~IPathManager(){};
	static IPathManager* GetInstance(){
		return pd;
	}
	virtual boost::uint32_t GetPathCheckSum()=0;
	/*
	Gives the next waypoint of the path.
	Gives (-1,-1,-1) in case no new waypoint could be found.
	Param:
		pathId
			The path-id returned by RequestPath.

		callerPos
			The current position of the user of the path.
			This extra information is needed to keep the path connected to it's user.

		minDistance
			Could be used to set a minimum required distance between callerPos and
			the returned waypoint.
		numRetries
			Dont set this, used internally

	*/

	virtual float3 NextWaypoint(unsigned int pathId, float3 callerPos, float minDistance = 0,
		int numRetries=0, int ownerId = 0, bool synced = true) const=0;
	/*
	Returns current estimated waypoints sorted by estimation levels
	Param:
		pathId
			The path-id returned by RequestPath.
		points
			The list of estimated waypoints.
		starts
			The list of starting indices for the different estimation levels
	*/
	virtual void GetEstimatedPath(unsigned int pathId,
		std::vector<float3>& points, std::vector<int>& starts) const=0;

	/*
	When a path are no longer used, please call this function to release it from memory.
	Param:
		pathId
			The path-id returned by RequestPath.
	*/
	virtual void DeletePath(unsigned int pathId)=0;

	/*
	Generate a path from startPos to the target defined by either peDef or (goalPos, goalRadius).
	If no complete path from startPos to the path target could be found, then a path getting as
	"close" as possible to target is generated.
	Only if no path getting "closer" to the target could be found no path is created.
	If a path could be created, then a none-zero path-id is returned.
	If no path could be created, then 0 is returned as a indication of a failure.
	Param:
		moveData
			Defining the footprint to use the path.

		startPos
			The starting location of the requested path.

		peDef
			A CPathEstimatorDef-object defining the target of the search.

		goalPos
			The center of the path target area.

		goalRadius
			Use goalRadius to define a goal area within any square could be accepted as path target.
			If a singular goal position is wanted, then use goalRadius = 0.
	*/
	virtual unsigned int RequestPath(
		const MoveData* moveData,
		float3 startPos, float3 goalPos, float goalRadius = 8,
		CSolidObject* caller = 0,
		bool synced = true,
		const int frame=0
	)=0;

	/*
	Whenever there are any changes in the terrain (ex. explosions, new buildings, etc.)
	this function need to be called to keep the estimator a jour.
	Param:
		upperCorner, lowerCorner
			The two corners defining the rectangular area affected by the changes.

		x1, z1, x2, z2
			Square coordinates defining the rectangular area affected by the changes.
	*/
	virtual void TerrainChange(float3 upperCorner, float3 lowerCorner)=0;
	virtual void TerrainChange(unsigned int x1, unsigned int z1, unsigned int x2, unsigned int z2)=0;

	enum { PATH_RESOLUTION = 2 * SQUARE_SIZE };
	/*
	returns the Drawer of the PathManger
	*/
	virtual IPathDrawer* getDrawer()=0;
	/*
	called every frame
	*/
	virtual void Update()=0;
	//FIXME seems to be an implementation specific function, maybe this could be called by update()
	virtual void Update(int pathId, CUnit *owner)=0;
private:
	static IPathManager* pd;

};

#define pathManager (IPathManager::GetInstance())

#endif
