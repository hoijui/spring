#ifndef QTPFS_DEFINE_HDR
#define QTPFS_DEFINE_HDR

// #define QTPFS_NO_LOADSCREEN
// #define QTPFS_IGNORE_DEAD_PATHS
#define QTPFS_LIMIT_TEAM_SEARCHES
#define QTPFS_SUPPORT_PARTIAL_SEARCHES
// #define QTPFS_TRACE_PATH_SEARCHES
// #define QTPFS_SEARCH_SHARED_PATHS
#define QTPFS_SMOOTH_PATHS
// #define QTPFS_WEIGHTED_HEURISTIC_COST
#define QTPFS_IGNORE_MAP_EDGES
// #define QTPFS_CONSERVATIVE_NODE_SPLITS
// #define QTPFS_DEBUG_NODE_HEAP
#define QTPFS_CORNER_CONNECTED_NODES
// #define QTPFS_COPY_NEIGHBOR_NODES
// #define QTPFS_SLOW_ACCURATE_TESSELATION
// #define QTPFS_OPENMP_ENABLED
// #define QTPFS_ORTHOPROJECTED_EDGE_TRANSITIONS
#define QTPFS_STAGGERED_LAYER_UPDATES
// NOTE: incompatible with QTPFS_ORTHOPROJECTED_EDGE_TRANSITIONS
#define QTPFS_CACHED_EDGE_TRANSITION_POINTS
// #define QTPFS_VIRTUAL_NODE_FUNCTIONS

#define QTPFS_CACHE_VERSION 1
#define QTPFS_CACHE_BASEDIR "cache/QTPFS/"

#define QTPFS_POSITIVE_INFINITY (std::numeric_limits<float>::infinity())

#endif

