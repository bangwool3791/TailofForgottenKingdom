#pragma once

typedef unsigned int dtStatus;


typedef unsigned short dtNodeIndex;
static const dtNodeIndex DT_NULL_IDX = (dtNodeIndex)~0;


enum dtNodeFlags
{
	DT_NODE_OPEN = 0x01,
	DT_NODE_CLOSED = 0x02,
	DT_NODE_PARENT_DETACHED = 0x04 // parent of the node is not adjacent. Found using raycast.
};

/// The maximum number of user defined area ids.
/// @ingroup detour
static const int DT_MAX_AREAS = 64;
static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

/// Limit raycasting during any angle pahfinding
/// The limit is given as a multiple of the character radius
static const float DT_RAY_CAST_LIMIT_PROPORTIONS = 50.0f;
/// A flag that indicates that an off-mesh connection can be traversed in both directions. (Is bidirectional.)
static const unsigned int DT_OFFMESH_CON_BIDIR = 1;
/// @{
/// @name Tile Serialization Constants
/// These constants are used to detect whether a navigation tile's data
/// and state format is compatible with the current build.
///

/// A magic number used to detect compatibility of navigation tile data.
static const int DT_NAVMESH_MAGIC = 'D' << 24 | 'N' << 16 | 'A' << 8 | 'V';

/// A version number used to detect compatibility of navigation tile data.
static const int DT_NAVMESH_VERSION = 7;

/// A magic number used to detect the compatibility of navigation tile states.
static const int DT_NAVMESH_STATE_MAGIC = 'D' << 24 | 'N' << 16 | 'M' << 8 | 'S';

/// A version number used to detect compatibility of navigation tile states.
static const int DT_NAVMESH_STATE_VERSION = 1;

static const int MAX_CONVEXVOL_PTS = 12;

/// A memory deallocation function.
///  @param[in]		ptr		A pointer to a memory block previously allocated using #dtAllocFunc.
/// @see dtAllocSetCustom
typedef void (dtFreeFunc)(void* ptr);

typedef unsigned int dtStatus;

// High level status.
static const unsigned int DT_FAILURE = 1u << 31;			// Operation failed.
static const unsigned int DT_SUCCESS = 1u << 30;			// Operation succeed.
static const unsigned int DT_IN_PROGRESS = 1u << 29;		// Operation still in progress.

// Detail information for status.
static const unsigned int DT_STATUS_DETAIL_MASK = 0x0ffffff;
static const unsigned int DT_WRONG_MAGIC = 1 << 0;		// Input data is not recognized.
static const unsigned int DT_WRONG_VERSION = 1 << 1;	// Input data is in wrong version.
static const unsigned int DT_OUT_OF_MEMORY = 1 << 2;	// Operation ran out of memory.
static const unsigned int DT_INVALID_PARAM = 1 << 3;	// An input parameter was invalid.
static const unsigned int DT_BUFFER_TOO_SMALL = 1 << 4;	// Result buffer for the query was too small to store all results.
static const unsigned int DT_OUT_OF_NODES = 1 << 5;		// Query ran out of nodes during search.
static const unsigned int DT_PARTIAL_RESULT = 1 << 6;	// Query did not reach the end location, returning best guess. 
static const unsigned int DT_ALREADY_OCCUPIED = 1 << 7;	// A tile has already been assigned to the given x,y coordinate


/// A value that indicates the entity does not link to anything.
static const unsigned int DT_NULL_LINK = 0xffffffff;

/// @}

/// A flag that indicates that an entity links to an external entity.
/// (E.g. A polygon edge is a portal that links to another polygon.)
static const unsigned short DT_EXT_LINK = 0x8000;

enum dtAllocHint
{
	DT_ALLOC_PERM,		///< Memory persist after a function call.
	DT_ALLOC_TEMP		///< Memory used temporarily within a function.
};

enum dtTileFlags
{
	/// The navigation mesh owns the tile memory and is responsible for freeing it.
	DT_TILE_FREE_DATA = 0x01
};

enum dtPolyTypes
{
	/// The polygon is a standard convex polygon that is part of the surface of the mesh.
	DT_POLYTYPE_GROUND = 0,
	/// The polygon is an off-mesh connection consisting of two vertices.
	DT_POLYTYPE_OFFMESH_CONNECTION = 1
};

enum dtDetailTriEdgeFlags
{
	DT_DETAIL_EDGE_BOUNDARY = 0x01		///< Detail triangle edge is part of the poly boundary
};

/// Options for dtNavMeshQuery::initSlicedFindPath and updateSlicedFindPath
enum dtFindPathOptions
{
	DT_FINDPATH_ANY_ANGLE = 0x02		///< use raycasts during pathfind to "shortcut" (raycast still consider costs)
};

enum dtRaycastOptions
{
	DT_RAYCAST_USE_COSTS = 0x01		///< Raycast should calculate movement cost along the ray and fill RaycastHit::cost
};

/// Vertex flags returned by dtNavMeshQuery::findStraightPath.
enum dtStraightPathFlags
{
	DT_STRAIGHTPATH_START = 0x01,				///< The vertex is the start position in the path.
	DT_STRAIGHTPATH_END = 0x02,					///< The vertex is the end position in the path.
	DT_STRAIGHTPATH_OFFMESH_CONNECTION = 0x04	///< The vertex is the start of an off-mesh connection.
};

/// Options for dtNavMeshQuery::findStraightPath.
enum dtStraightPathOptions
{
	DT_STRAIGHTPATH_AREA_CROSSINGS = 0x01,	///< Add a vertex at every polygon edge crossing where area changes.
	DT_STRAIGHTPATH_ALL_CROSSINGS = 0x02	///< Add a vertex at every polygon edge crossing.
};


