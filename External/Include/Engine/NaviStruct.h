#pragma once

static const int DT_VERTS_PER_POLYGON = 6;

static const int DT_NODE_PARENT_BITS = 24;
static const int DT_NODE_STATE_BITS = 2;
static unsigned short MESH_NULL_IDX = 0xffff;

static const int DT_MAX_STATES_PER_NODE = 1 << DT_NODE_STATE_BITS;	// number of extra states per node. See dtNode::state

typedef unsigned int dtPolyRef;
typedef unsigned int dtTileRef;

struct BoundsItem
{
	float bmin[2];
	float bmax[2];
	int i;
};

struct dtNavMeshParams
{
	float orig[3];					///< The world space origin of the navigation mesh's tile space. [(x, y, z)]
	float tileWidth;				///< The width of each tile. (Along the x-axis.)
	float tileHeight;				///< The height of each tile. (Along the z-axis.)
	int maxTiles;					///< The maximum number of tiles the navigation mesh can contain. This and maxPolys are used to calculate how many bits are needed to identify tiles and polygons uniquely.
	int maxPolys;					///< The maximum number of polygons each tile can contain. This and maxTiles are used to calculate how many bits are needed to identify tiles and polygons uniquely.
};

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};


struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

struct dtMeshHeader
{
	int magic;				///< Tile magic number. (Used to identify the data format.)
	int version;			///< Tile data format version number.
	int x;					///< The x-position of the tile within the dtNavMesh tile grid. (x, y, layer)
	int y;					///< The y-position of the tile within the dtNavMesh tile grid. (x, y, layer)
	int layer;				///< The layer of the tile within the dtNavMesh tile grid. (x, y, layer)
	unsigned int userId;	///< The user defined id of the tile.
	int polyCount;			///< The number of polygons in the tile.
	int vertCount;			///< The number of vertices in the tile.
	int maxLinkCount;		///< The number of allocated links.
	int detailMeshCount;	///< The number of sub-meshes in the detail mesh.

	/// The number of unique vertices in the detail mesh. (In addition to the polygon vertices.)
	int detailVertCount;

	int detailTriCount;			///< The number of triangles in the detail mesh.
	int bvNodeCount;			///< The number of bounding volume nodes. (Zero if bounding volumes are disabled.)
	int offMeshConCount;		///< The number of off-mesh connections.
	int offMeshBase;			///< The index of the first polygon which is an off-mesh connection.
	float walkableHeight;		///< The height of the agents using the tile.
	float walkableRadius;		///< The radius of the agents using the tile.
	float walkableClimb;		///< The maximum climb height of the agents using the tile.
	float bmin[3];				///< The minimum bounds of the tile's AABB. [(x, y, z)]
	float bmax[3];				///< The maximum bounds of the tile's AABB. [(x, y, z)]

	/// The bounding volume quantization factor. 
	float bvQuantFactor;
};

/// Defines a polygon within a dtMeshTile object.
/// @ingroup detour
struct dtPoly
{
	/// Index to first link in linked list. (Or #DT_NULL_LINK if there is no link.)
	unsigned int firstLink;

	/// The indices of the polygon's vertices.
	/// The actual vertices are located in dtMeshTile::verts.
	unsigned short verts[DT_VERTS_PER_POLYGON];

	/// Packed data representing neighbor polygons references and flags for each edge.
	unsigned short neis[DT_VERTS_PER_POLYGON];

	/// The user defined polygon flags.
	unsigned short flags;

	/// The number of vertices in the polygon.
	unsigned char vertCount;

	/// The bit packed area id and polygon type.
	/// @note Use the structure's set and get methods to acess this value.
	unsigned char areaAndtype;

	/// Sets the user defined area id. [Limit: < #DT_MAX_AREAS]
	inline void setArea(unsigned char a) { areaAndtype = (areaAndtype & 0xc0) | (a & 0x3f); }

	/// Sets the polygon type. (See: #dtPolyTypes.)
	inline void setType(unsigned char t) { areaAndtype = (areaAndtype & 0x3f) | (t << 6); }

	/// Gets the user defined area id.
	inline unsigned char getArea() const { return areaAndtype & 0x3f; }

	/// Gets the polygon type. (See: #dtPolyTypes)
	inline unsigned char getType() const { return areaAndtype >> 6; }
};

struct dtLink
{
	dtPolyRef ref;					///< Neighbour reference. (The neighbor that is linked to.)
	unsigned int next;				///< Index of the next link.
	unsigned char edge;				///< Index of the polygon edge that owns this link.
	unsigned char side;				///< If a boundary link, defines on which side the link is.
	unsigned char bmin;				///< If a boundary link, defines the minimum sub-edge area.
	unsigned char bmax;				///< If a boundary link, defines the maximum sub-edge area.
};

/// Defines the location of detail sub-mesh data within a dtMeshTile.
struct dtPolyDetail
{
	unsigned int vertBase;			///< The offset of the vertices in the dtMeshTile::detailVerts array.
	unsigned int triBase;			///< The offset of the triangles in the dtMeshTile::detailTris array.
	unsigned char vertCount;		///< The number of vertices in the sub-mesh.
	unsigned char triCount;			///< The number of triangles in the sub-mesh.
};

/// Bounding volume node.
/// @note This structure is rarely if ever used by the end user.
/// @see dtMeshTile
struct dtBVNode
{
	unsigned short bmin[3];			///< Minimum bounds of the node's AABB. [(x, y, z)]
	unsigned short bmax[3];			///< Maximum bounds of the node's AABB. [(x, y, z)]
	int i;							///< The node's index. (Negative for escape sequence.)
};


/// Defines an navigation mesh off-mesh connection within a dtMeshTile object.
/// An off-mesh connection is a user defined traversable connection made up to two vertices.
struct dtOffMeshConnection
{
	/// The endpoints of the connection. [(ax, ay, az, bx, by, bz)]
	float pos[6];

	/// The radius of the endpoints. [Limit: >= 0]
	float rad;

	/// The polygon reference of the connection within the tile.
	unsigned short poly;

	/// Link flags. 
	/// @note These are not the connection's user defined flags. Those are assigned via the 
	/// connection's dtPoly definition. These are link flags used for internal purposes.
	unsigned char flags;

	/// End point side.
	unsigned char side;

	/// The id of the offmesh connection. (User assigned when the navigation mesh is built.)
	unsigned int userId;
};

struct dtMeshTile
{
	unsigned int salt;					///< Counter describing modifications to the tile.

	unsigned int linksFreeList;			///< Index to the next free link.
	dtMeshHeader* header;				///< The tile header.
	dtPoly* polys;						///< The tile polygons. [Size: dtMeshHeader::polyCount]
	float* verts;						///< The tile vertices. [(x, y, z) * dtMeshHeader::vertCount]
	dtLink* links;						///< The tile links. [Size: dtMeshHeader::maxLinkCount]
	dtPolyDetail* detailMeshes;			///< The tile's detail sub-meshes. [Size: dtMeshHeader::detailMeshCount]

	/// The detail mesh's unique vertices. [(x, y, z) * dtMeshHeader::detailVertCount]
	float* detailVerts;

	/// The detail mesh's triangles. [(vertA, vertB, vertC, triFlags) * dtMeshHeader::detailTriCount].
	/// See dtDetailTriEdgeFlags and dtGetDetailTriEdgeFlags.
	unsigned char* detailTris;

	/// The tile bounding volume nodes. [Size: dtMeshHeader::bvNodeCount]
	/// (Will be null if bounding volumes are disabled.)
	dtBVNode* bvTree;

	dtOffMeshConnection* offMeshCons;		///< The tile off-mesh connections. [Size: dtMeshHeader::offMeshConCount]

	unsigned char* data;					///< The tile data. (Not directly accessed under normal situations.)
	int dataSize;							///< Size of the tile data.
	int flags;								///< Tile flags. (See: #dtTileFlags)
	dtMeshTile* next;						///< The next free tile, or the next tile in the spatial grid.
private:
	dtMeshTile(const dtMeshTile&);
	dtMeshTile& operator=(const dtMeshTile&);
};


/// Provides information about raycast hit
/// filled by dtNavMeshQuery::raycast
/// @ingroup detour
struct dtRaycastHit
{
	/// The hit parameter. (FLT_MAX if no wall hit.)
	float t;

	/// hitNormal	The normal of the nearest wall hit. [(x, y, z)]
	float hitNormal[3];

	/// The index of the edge on the final polygon where the wall was hit.
	int hitEdgeIndex;

	/// Pointer to an array of reference ids of the visited polygons. [opt]
	dtPolyRef* path;

	/// The number of visited polygons. [opt]
	int pathCount;

	/// The maximum number of polygons the @p path array can hold.
	int maxPath;

	///  The cost of the path until hit.
	float pathCost;
};

struct ConvexVolume
{
	float verts[MAX_CONVEXVOL_PTS * 3];
	float hmin, hmax;
	int nverts;
	int area;
};

struct BuildSettings
{
	// Cell size in world units
	float cellSize;
	// Cell height in world units
	float cellHeight;
	// Agent height in world units
	float agentHeight;
	// Agent radius in world units
	float agentRadius;
	// Agent max climb in world units
	float agentMaxClimb;
	// Agent max slope in degrees
	float agentMaxSlope;
	// Region minimum size in voxels.
	// regionMinSize = sqrt(regionMinArea)
	float regionMinSize;
	// Region merge size in voxels.
	// regionMergeSize = sqrt(regionMergeArea)
	float regionMergeSize;
	// Edge max length in world units
	float edgeMaxLen;
	// Edge max error in voxels
	float edgeMaxError;
	float vertsPerPoly;
	// Detail sample distance in voxels
	float detailSampleDist;
	// Detail sample max error in voxel heights.
	float detailSampleMaxError;
	// Partition type, see SamplePartitionType
	int partitionType;
	// Bounds of the area to mesh
	float navMeshBMin[3];
	float navMeshBMax[3];
	// Size of the tiles in voxels
	float tileSize;
};

struct dtNode
{
	float pos[3];								///< Position of the node.
	float cost;									///< Cost from previous node to current node.
	float total;								///< Cost up to the node.
	unsigned int pidx : DT_NODE_PARENT_BITS;	///< Index to parent node.
	unsigned int state : DT_NODE_STATE_BITS;	///< extra state information. A polyRef can have multiple nodes with different extra info. see DT_MAX_STATES_PER_NODE
	unsigned int flags : 3;						///< Node flags. A combination of dtNodeFlags.
	dtPolyRef id;								///< Polygon ref the node corresponds to.
};

struct BVItem
{
	unsigned short bmin[3];
	unsigned short bmax[3];
	int i;
};
/// Represents the source data used to build an navigation mesh tile.
/// @ingroup detour
struct dtNavMeshCreateParams
{

	/// @name Polygon Mesh Attributes
	/// Used to create the base navigation graph.
	/// See #rcPolyMesh for details related to these attributes.
	/// @{

	const unsigned short* verts;			///< The polygon mesh vertices. [(x, y, z) * #vertCount] [Unit: vx]
	int vertCount;							///< The number vertices in the polygon mesh. [Limit: >= 3]
	const unsigned short* polys;			///< The polygon data. [Size: #polyCount * 2 * #nvp]
	const unsigned short* polyFlags;		///< The user defined flags assigned to each polygon. [Size: #polyCount]
	const unsigned char* polyAreas;			///< The user defined area ids assigned to each polygon. [Size: #polyCount]
	int polyCount;							///< Number of polygons in the mesh. [Limit: >= 1]
	int nvp;								///< Number maximum number of vertices per polygon. [Limit: >= 3]

	/// @}
	/// @name Height Detail Attributes (Optional)
	/// See #rcPolyMeshDetail for details related to these attributes.
	/// @{

	const unsigned int* detailMeshes;		///< The height detail sub-mesh data. [Size: 4 * #polyCount]
	const float* detailVerts;				///< The detail mesh vertices. [Size: 3 * #detailVertsCount] [Unit: wu]
	int detailVertsCount;					///< The number of vertices in the detail mesh.
	const unsigned char* detailTris;		///< The detail mesh triangles. [Size: 4 * #detailTriCount]
	int detailTriCount;						///< The number of triangles in the detail mesh.

	/// @}
	/// @name Off-Mesh Connections Attributes (Optional)
	/// Used to define a custom point-to-point edge within the navigation graph, an 
	/// off-mesh connection is a user defined traversable connection made up to two vertices, 
	/// at least one of which resides within a navigation mesh polygon.
	/// @{

	/// Off-mesh connection vertices. [(ax, ay, az, bx, by, bz) * #offMeshConCount] [Unit: wu]
	const float* offMeshConVerts;
	/// Off-mesh connection radii. [Size: #offMeshConCount] [Unit: wu]
	const float* offMeshConRad;
	/// User defined flags assigned to the off-mesh connections. [Size: #offMeshConCount]
	const unsigned short* offMeshConFlags;
	/// User defined area ids assigned to the off-mesh connections. [Size: #offMeshConCount]
	const unsigned char* offMeshConAreas;
	/// The permitted travel direction of the off-mesh connections. [Size: #offMeshConCount]
	///
	/// 0 = Travel only from endpoint A to endpoint B.<br/>
	/// #DT_OFFMESH_CON_BIDIR = Bidirectional travel.
	const unsigned char* offMeshConDir;
	/// The user defined ids of the off-mesh connection. [Size: #offMeshConCount]
	const unsigned int* offMeshConUserID;
	/// The number of off-mesh connections. [Limit: >= 0]
	int offMeshConCount;

	/// @}
	/// @name Tile Attributes
	/// @note The tile grid/layer data can be left at zero if the destination is a single tile mesh.
	/// @{

	unsigned int userId;	///< The user defined id of the tile.
	int tileX;				///< The tile's x-grid location within the multi-tile destination mesh. (Along the x-axis.)
	int tileY;				///< The tile's y-grid location within the multi-tile desitation mesh. (Along the z-axis.)
	int tileLayer;			///< The tile's layer within the layered destination mesh. [Limit: >= 0] (Along the y-axis.)
	float bmin[3];			///< The minimum bounds of the tile. [(x, y, z)] [Unit: wu]
	float bmax[3];			///< The maximum bounds of the tile. [(x, y, z)] [Unit: wu]

	/// @}
	/// @name General Configuration Attributes
	/// @{

	float walkableHeight;	///< The agent height. [Unit: wu]
	float walkableRadius;	///< The agent radius. [Unit: wu]
	float walkableClimb;	///< The agent maximum traversable ledge. (Up/Down) [Unit: wu]
	float cs;				///< The xz-plane cell size of the polygon mesh. [Limit: > 0] [Unit: wu]
	float ch;				///< The y-axis cell height of the polygon mesh. [Limit: > 0] [Unit: wu]

	/// True if a bounding volume tree should be built for the tile.
	/// @note The BVTree is not normally needed for layered navigation meshes.
	bool buildBvTree;

	/// @}
};