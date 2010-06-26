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

	/**
	 * Returns the next waypoint of the path.
	 *
	 * @param pathId
	 *        The path-id returned by RequestPath.
	 * @param callerPos
	 *        The current position of the user of the path.
	 *        This extra information is needed to keep the path connected to its
	 *        user.
	 * @param minDistance
	 *        Could be used to set a minimum required distance between callerPos
	 *        and the returned waypoint.
	 * @param numRetries
	 *        Dont set this, used internally
	 * @param ownerId
	 *        The id of the unit the path is used for, or 0.
	 * @param synced
	 *        Whether this evaluation has to run synced or unsynced.
	 *        If false, this call may not change any state of the path manager
	 *        that could alter paths requested in the future.
	 *        example: if (synced == false) turn of heat-mapping
	 * @return
	 *         the next waypoint of the path, or (-1,-1,-1) in case no new
	 *         waypoint could be found.
	 */
	virtual float3 NextWaypoint(unsigned int pathId, float3 callerPos,
		float minDistance = 0,int numRetries=0, int ownerId = 0,
		bool synced = true) const=0;

	/**
	 * Returns current estimated waypoints sorted by estimation levels
	 * @param pathId
	 *        The path-id returned by RequestPath.
	 * @param points
	 *        The list of estimated waypoints.
	 * @param starts
	 *        The list of starting indices for the different estimation levels
	 */
	virtual void GetEstimatedPath(unsigned int pathId,
		std::vector<float3>& points, std::vector<int>& starts) const=0;

	/**
	 * When a path is no longer used, call this function to release it from
	 * memory.
	 * @param pathId
	 *        The path-id returned by RequestPath.
	 */
	virtual void DeletePath(unsigned int pathId)=0;

	/**
	 * Generate a path from startPos to the target defined by
	 * (goalPos, goalRadius).
	 * If no complete path from startPos to goalPos could be found,
	 * then a path getting as "close" as possible to target is generated.
	 *
	 * @param moveData
	 *        Defining the move details of the unit to use the path.
	 * @param startPos
	 *        The starting location of the requested path.
	 * @param goalPos
	 *        The center of the path goal area.
	 * @param goalRadius
	 *        Use goalRadius to define a goal area within any square that could
	 *        be accepted as path goal.
	 *        If a singular goal position is wanted, use goalRadius = 0.
	 * @param caller
	 *        The unit or feature the path will be used for.
	 * @param synced
	 *        Whether this evaluation has to run synced or unsynced.
	 *        If false, this call may not change any state of the path manager
	 *        that could alter paths requested in the future.
	 *        example: if (synced == false) turn of heat-mapping
	 * @param frame
	 *        ??? FIXME looks liek we should get rid of this in the interface
	 * @return
	 *         a path-id >= 1 on success, 0 on failure
	 *         Failure means, no path getting "closer" to goalPos then startPos
	 *         could be found
	 */
	virtual unsigned int RequestPath(
		const MoveData* moveData,
		float3 startPos, float3 goalPos, float goalRadius = 8,
		CSolidObject* caller = 0,
		bool synced = true,
		const int frame=0
	)=0;

	/**
	 * Whenever there are any changes in the terrain
	 * (examples: explosions, new buildings, etc.)
	 * this function will be called.
	 * @param upperCorner
	 *        The upper corner defining the rectangular area affected by the
	 *        changes.
	 * @param lowerCorner
	 *        The lower corner defining the rectangular area affected by the
	 *        changes.
	 */
	virtual void TerrainChange(float3 upperCorner, float3 lowerCorner)=0;
	/**
	 * Whenever there are any changes in the terrain
	 * (examples: explosions, new buildings, etc.)
	 * this function will be called.
	 * @param x1
	 *        First corners X-axis value, defining the rectangular area
	 *        affected by the changes.
	 * @param z1
	 *        First corners Z-axis value, defining the rectangular area
	 *        affected by the changes.
	 * @param x2
	 *        Second corners X-axis value, defining the rectangular area
	 *        affected by the changes.
	 * @param z2
	 *        Second corners Z-axis value, defining the rectangular area
	 *        affected by the changes.
	 */
	virtual void TerrainChange(unsigned int x1, unsigned int z1,
		unsigned int x2, unsigned int z2)=0;

	enum { PATH_RESOLUTION = 2 * SQUARE_SIZE };
	/**
	 * Returns the Drawer of the PathManger
	 */
	virtual IPathDrawer* getDrawer()=0;
	/**
	 * Called every sim-frame (aprox 1/30 sec during runtime).
	 */
	virtual void Update()=0;
	//FIXME seems to be an implementation specific function, maybe this could be called by update()
	virtual void Update(int pathId, CUnit *owner)=0;
private:
	static IPathManager* pd;

};

#define pathManager (IPathManager::GetInstance())

#endif
