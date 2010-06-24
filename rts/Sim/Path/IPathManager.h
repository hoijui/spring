/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef I_PATH_MANAGER_H
#define I_PATH_MANAGER_H

#include <boost/cstdint.hpp> /* Replace with <stdint.h> if appropriate */
#include "float3.h"
#include "Vec2.h"
#include "Map/ReadMap.h"
#include "Rendering/IPathDrawer.h"

struct MoveData;
class CSolidObject;

class IPathManager{
		static IPathManager* pd;
	public:
		static void Initialize();
		virtual ~IPathManager(){};
		static IPathManager* GetInstance(){
			return pd;
		}
		virtual boost::uint32_t GetPathCheckSum()=0;
		virtual void Update()=0;
		virtual float3 NextWaypoint(unsigned int pathId, float3 callerPos, float minDistance = 0,
			int numRetries=0, int ownerId = 0, bool synced = true)=0;
		virtual void GetEstimatedPath(unsigned int pathId,
			std::vector<float3>& points, std::vector<int>& starts)=0;
		virtual void DeletePath(unsigned int pathId)=0;
		virtual unsigned int RequestPath(
			const MoveData* moveData,
			float3 startPos, float3 goalPos, float goalRadius = 8,
			CSolidObject* caller = 0,
			bool synced = true,
			const int frame=0
		)=0;

		virtual void TerrainChange(float3 upperCorner, float3 lowerCorner)=0;
		virtual void TerrainChange(unsigned int x1, unsigned int z1, unsigned int x2, unsigned int z2)=0;

		enum { PATH_RESOLUTION = 2 * SQUARE_SIZE };
		virtual IPathDrawer* getDrawer()=0;
		virtual void Update(int pathId, CUnit *owner)=0;

};

#define pathManager (IPathManager::GetInstance())

#endif
