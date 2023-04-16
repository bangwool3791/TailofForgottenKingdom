#pragma once

#include "GlobalNavi.h"
/// A navigation mesh based on tiles of convex polygons.
/// @ingroup detour
class dtNavMesh
{
public:
	dtNavMesh();
	~dtNavMesh();

	/// @{
	/// @name Initialization and Tile Management

	/// Initializes the navigation mesh for tiled use.
	///  @param[in]	params		Initialization parameters.
	/// @return The status flags for the operation.
	dtStatus init(const dtNavMeshParams* params);

	/// Initializes the navigation mesh for single tile use.
	///  @param[in]	data		Data of the new tile. (See: #dtCreateNavMeshData)
	///  @param[in]	dataSize	The data size of the new tile.
	///  @param[in]	flags		The tile flags. (See: #dtTileFlags)
	/// @return The status flags for the operation.
	///  @see dtCreateNavMeshData
	dtStatus init(unsigned char* data, const int dataSize, const int flags);

	/// The navigation mesh initialization params.
	const dtNavMeshParams* getParams() const;

	/// Adds a tile to the navigation mesh.
	///  @param[in]		data		Data for the new tile mesh. (See: #dtCreateNavMeshData)
	///  @param[in]		dataSize	Data size of the new tile mesh.
	///  @param[in]		flags		Tile flags. (See: #dtTileFlags)
	///  @param[in]		lastRef		The desired reference for the tile. (When reloading a tile.) [opt] [Default: 0]
	///  @param[out]	result		The tile reference. (If the tile was succesfully added.) [opt]
	/// @return The status flags for the operation.
	dtStatus addTile(unsigned char* data, int dataSize, int flags, dtTileRef lastRef, dtTileRef* result);

	/// Removes the specified tile from the navigation mesh.
	///  @param[in]		ref			The reference of the tile to remove.
	///  @param[out]	data		Data associated with deleted tile.
	///  @param[out]	dataSize	Size of the data associated with deleted tile.
	/// @return The status flags for the operation.
	dtStatus removeTile(dtTileRef ref, unsigned char** data, int* dataSize);

	/// @}

	/// @{
	/// @name Query Functions

	/// Calculates the tile grid location for the specified world position.
	///  @param[in]	pos  The world position for the query. [(x, y, z)]
	///  @param[out]	tx		The tile's x-location. (x, y)
	///  @param[out]	ty		The tile's y-location. (x, y)
	void calcTileLoc(const float* pos, int* tx, int* ty) const;

	/// Gets the tile at the specified grid location.
	///  @param[in]	x		The tile's x-location. (x, y, layer)
	///  @param[in]	y		The tile's y-location. (x, y, layer)
	///  @param[in]	layer	The tile's layer. (x, y, layer)
	/// @return The tile, or null if the tile does not exist.
	const dtMeshTile* getTileAt(const int x, const int y, const int layer) const;

	/// Gets all tiles at the specified grid location. (All layers.)
	///  @param[in]		x			The tile's x-location. (x, y)
	///  @param[in]		y			The tile's y-location. (x, y)
	///  @param[out]	tiles		A pointer to an array of tiles that will hold the result.
	///  @param[in]		maxTiles	The maximum tiles the tiles parameter can hold.
	/// @return The number of tiles returned in the tiles array.
	int getTilesAt(const int x, const int y,
		dtMeshTile const** tiles, const int maxTiles) const;

	/// Gets the tile reference for the tile at specified grid location.
	///  @param[in]	x		The tile's x-location. (x, y, layer)
	///  @param[in]	y		The tile's y-location. (x, y, layer)
	///  @param[in]	layer	The tile's layer. (x, y, layer)
	/// @return The tile reference of the tile, or 0 if there is none.
	dtTileRef getTileRefAt(int x, int y, int layer) const;

	/// Gets the tile reference for the specified tile.
	///  @param[in]	tile	The tile.
	/// @return The tile reference of the tile.
	dtTileRef getTileRef(const dtMeshTile* tile) const;

	/// Gets the tile for the specified tile reference.
	///  @param[in]	ref		The tile reference of the tile to retrieve.
	/// @return The tile for the specified reference, or null if the 
	///		reference is invalid.
	const dtMeshTile* getTileByRef(dtTileRef ref) const;

	/// The maximum number of tiles supported by the navigation mesh.
	/// @return The maximum number of tiles supported by the navigation mesh.
	int getMaxTiles() const;

	/// Gets the tile at the specified index.
	///  @param[in]	i		The tile index. [Limit: 0 >= index < #getMaxTiles()]
	/// @return The tile at the specified index.
	const dtMeshTile* getTile(int i) const;

	/// Gets the tile and polygon for the specified polygon reference.
	///  @param[in]		ref		The reference for the a polygon.
	///  @param[out]	tile	The tile containing the polygon.
	///  @param[out]	poly	The polygon.
	/// @return The status flags for the operation.
	dtStatus getTileAndPolyByRef(const dtPolyRef ref, const dtMeshTile** tile, const dtPoly** poly) const;

	/// Returns the tile and polygon for the specified polygon reference.
	///  @param[in]		ref		A known valid reference for a polygon.
	///  @param[out]	tile	The tile containing the polygon.
	///  @param[out]	poly	The polygon.
	void getTileAndPolyByRefUnsafe(const dtPolyRef ref, const dtMeshTile** tile, const dtPoly** poly) const;

	/// Checks the validity of a polygon reference.
	///  @param[in]	ref		The polygon reference to check.
	/// @return True if polygon reference is valid for the navigation mesh.
	bool isValidPolyRef(dtPolyRef ref) const;

	/// Gets the polygon reference for the tile's base polygon.
	///  @param[in]	tile		The tile.
	/// @return The polygon reference for the base polygon in the specified tile.
	dtPolyRef getPolyRefBase(const dtMeshTile* tile) const;

	/// Gets the endpoints for an off-mesh connection, ordered by "direction of travel".
	///  @param[in]		prevRef		The reference of the polygon before the connection.
	///  @param[in]		polyRef		The reference of the off-mesh connection polygon.
	///  @param[out]	startPos	The start position of the off-mesh connection. [(x, y, z)]
	///  @param[out]	endPos		The end position of the off-mesh connection. [(x, y, z)]
	/// @return The status flags for the operation.
	dtStatus getOffMeshConnectionPolyEndPoints(dtPolyRef prevRef, dtPolyRef polyRef, float* startPos, float* endPos) const;

	/// Gets the specified off-mesh connection.
	///  @param[in]	ref		The polygon reference of the off-mesh connection.
	/// @return The specified off-mesh connection, or null if the polygon reference is not valid.
	const dtOffMeshConnection* getOffMeshConnectionByRef(dtPolyRef ref) const;

	/// @}

	/// @{
	/// @name State Management
	/// These functions do not effect #dtTileRef or #dtPolyRef's. 

	/// Sets the user defined flags for the specified polygon.
	///  @param[in]	ref		The polygon reference.
	///  @param[in]	flags	The new flags for the polygon.
	/// @return The status flags for the operation.
	dtStatus setPolyFlags(dtPolyRef ref, unsigned short flags);

	/// Gets the user defined flags for the specified polygon.
	///  @param[in]		ref				The polygon reference.
	///  @param[out]	resultFlags		The polygon flags.
	/// @return The status flags for the operation.
	dtStatus getPolyFlags(dtPolyRef ref, unsigned short* resultFlags) const;

	/// Sets the user defined area for the specified polygon.
	///  @param[in]	ref		The polygon reference.
	///  @param[in]	area	The new area id for the polygon. [Limit: < #DT_MAX_AREAS]
	/// @return The status flags for the operation.
	dtStatus setPolyArea(dtPolyRef ref, unsigned char area);

	/// Gets the user defined area for the specified polygon.
	///  @param[in]		ref			The polygon reference.
	///  @param[out]	resultArea	The area id for the polygon.
	/// @return The status flags for the operation.
	dtStatus getPolyArea(dtPolyRef ref, unsigned char* resultArea) const;

	/// Gets the size of the buffer required by #storeTileState to store the specified tile's state.
	///  @param[in]	tile	The tile.
	/// @return The size of the buffer required to store the state.
	int getTileStateSize(const dtMeshTile* tile) const;

	/// Stores the non-structural state of the tile in the specified buffer. (Flags, area ids, etc.)
	///  @param[in]		tile			The tile.
	///  @param[out]	data			The buffer to store the tile's state in.
	///  @param[in]		maxDataSize		The size of the data buffer. [Limit: >= #getTileStateSize]
	/// @return The status flags for the operation.
	dtStatus storeTileState(const dtMeshTile* tile, unsigned char* data, const int maxDataSize) const;

	/// Restores the state of the tile.
	///  @param[in]	tile			The tile.
	///  @param[in]	data			The new state. (Obtained from #storeTileState.)
	///  @param[in]	maxDataSize		The size of the state within the data buffer.
	/// @return The status flags for the operation.
	dtStatus restoreTileState(dtMeshTile* tile, const unsigned char* data, const int maxDataSize);

	/// @}

	/// @{
	/// @name Encoding and Decoding
	/// These functions are generally meant for internal use only.

	/// Derives a standard polygon reference.
	///  @note This function is generally meant for internal use only.
	///  @param[in]	salt	The tile's salt value.
	///  @param[in]	it		The index of the tile.
	///  @param[in]	ip		The index of the polygon within the tile.
	inline dtPolyRef encodePolyId(unsigned int salt, unsigned int it, unsigned int ip) const
	{
#ifdef DT_POLYREF64
		return ((dtPolyRef)salt << (DT_POLY_BITS + DT_TILE_BITS)) | ((dtPolyRef)it << DT_POLY_BITS) | (dtPolyRef)ip;
#else
		return ((dtPolyRef)salt << (m_polyBits + m_tileBits)) | ((dtPolyRef)it << m_polyBits) | (dtPolyRef)ip;
#endif
	}

	/// Decodes a standard polygon reference.
	///  @note This function is generally meant for internal use only.
	///  @param[in]	ref   The polygon reference to decode.
	///  @param[out]	salt	The tile's salt value.
	///  @param[out]	it		The index of the tile.
	///  @param[out]	ip		The index of the polygon within the tile.
	///  @see #encodePolyId
	inline void decodePolyId(dtPolyRef ref, unsigned int& salt, unsigned int& it, unsigned int& ip) const
	{
#ifdef DT_POLYREF64
		const dtPolyRef saltMask = ((dtPolyRef)1 << DT_SALT_BITS) - 1;
		const dtPolyRef tileMask = ((dtPolyRef)1 << DT_TILE_BITS) - 1;
		const dtPolyRef polyMask = ((dtPolyRef)1 << DT_POLY_BITS) - 1;
		salt = (unsigned int)((ref >> (DT_POLY_BITS + DT_TILE_BITS)) & saltMask);
		it = (unsigned int)((ref >> DT_POLY_BITS) & tileMask);
		ip = (unsigned int)(ref & polyMask);
#else
		const dtPolyRef saltMask = ((dtPolyRef)1 << m_saltBits) - 1;
		const dtPolyRef tileMask = ((dtPolyRef)1 << m_tileBits) - 1;
		const dtPolyRef polyMask = ((dtPolyRef)1 << m_polyBits) - 1;
		salt = (unsigned int)((ref >> (m_polyBits + m_tileBits)) & saltMask);
		it = (unsigned int)((ref >> m_polyBits) & tileMask);
		ip = (unsigned int)(ref & polyMask);
#endif
	}

	/// Extracts a tile's salt value from the specified polygon reference.
	///  @note This function is generally meant for internal use only.
	///  @param[in]	ref		The polygon reference.
	///  @see #encodePolyId
	inline unsigned int decodePolyIdSalt(dtPolyRef ref) const
	{
#ifdef DT_POLYREF64
		const dtPolyRef saltMask = ((dtPolyRef)1 << DT_SALT_BITS) - 1;
		return (unsigned int)((ref >> (DT_POLY_BITS + DT_TILE_BITS)) & saltMask);
#else
		const dtPolyRef saltMask = ((dtPolyRef)1 << m_saltBits) - 1;
		return (unsigned int)((ref >> (m_polyBits + m_tileBits)) & saltMask);
#endif
	}

	/// Extracts the tile's index from the specified polygon reference.
	///  @note This function is generally meant for internal use only.
	///  @param[in]	ref		The polygon reference.
	///  @see #encodePolyId
	inline unsigned int decodePolyIdTile(dtPolyRef ref) const
	{
#ifdef DT_POLYREF64
		const dtPolyRef tileMask = ((dtPolyRef)1 << DT_TILE_BITS) - 1;
		return (unsigned int)((ref >> DT_POLY_BITS) & tileMask);
#else
		const dtPolyRef tileMask = ((dtPolyRef)1 << m_tileBits) - 1;
		return (unsigned int)((ref >> m_polyBits) & tileMask);
#endif
	}

	/// Extracts the polygon's index (within its tile) from the specified polygon reference.
	///  @note This function is generally meant for internal use only.
	///  @param[in]	ref		The polygon reference.
	///  @see #encodePolyId
	inline unsigned int decodePolyIdPoly(dtPolyRef ref) const
	{
#ifdef DT_POLYREF64
		const dtPolyRef polyMask = ((dtPolyRef)1 << DT_POLY_BITS) - 1;
		return (unsigned int)(ref & polyMask);
#else
		const dtPolyRef polyMask = ((dtPolyRef)1 << m_polyBits) - 1;
		return (unsigned int)(ref & polyMask);
#endif
	}

	/// @}

private:
	// Explicitly disabled copy constructor and copy assignment operator.
	dtNavMesh(const dtNavMesh&);
	dtNavMesh& operator=(const dtNavMesh&);

	/// Returns pointer to tile in the tile array.
	dtMeshTile* getTile(int i);

	/// Returns neighbour tile based on side.
	int getTilesAt(const int x, const int y,
		dtMeshTile** tiles, const int maxTiles) const;

	/// Returns neighbour tile based on side.
	int getNeighbourTilesAt(const int x, const int y, const int side,
		dtMeshTile** tiles, const int maxTiles) const;

	/// Returns all polygons in neighbour tile based on portal defined by the segment.
	int findConnectingPolys(const float* va, const float* vb,
		const dtMeshTile* tile, int side,
		dtPolyRef* con, float* conarea, int maxcon) const;

	/// Builds internal polygons links for a tile.
	void connectIntLinks(dtMeshTile* tile);
	/// Builds internal polygons links for a tile.
	void baseOffMeshLinks(dtMeshTile* tile);

	/// Builds external polygon links for a tile.
	void connectExtLinks(dtMeshTile* tile, dtMeshTile* target, int side);
	/// Builds external polygon links for a tile.
	void connectExtOffMeshLinks(dtMeshTile* tile, dtMeshTile* target, int side);

	/// Removes external links at specified side.
	void unconnectLinks(dtMeshTile* tile, dtMeshTile* target);


	// TODO: These methods are duplicates from dtNavMeshQuery, but are needed for off-mesh connection finding.

	/// Queries polygons within a tile.
	int queryPolygonsInTile(const dtMeshTile* tile, const float* qmin, const float* qmax,
		dtPolyRef* polys, const int maxPolys) const;
	/// Find nearest polygon within a tile.
	dtPolyRef findNearestPolyInTile(const dtMeshTile* tile, const float* center,
		const float* halfExtents, float* nearestPt) const;
	/// Returns whether position is over the poly and the height at the position if so.
	bool getPolyHeight(const dtMeshTile* tile, const dtPoly* poly, const float* pos, float* height) const;
	/// Returns closest point on polygon.
	void closestPointOnPoly(dtPolyRef ref, const float* pos, float* closest, bool* posOverPoly) const;

	dtNavMeshParams m_params;			///< Current initialization params. TODO: do not store this info twice.
	float m_orig[3];					///< Origin of the tile (0,0)
	float m_tileWidth, m_tileHeight;	///< Dimensions of each tile.
	int m_maxTiles;						///< Max number of tiles.
	int m_tileLutSize;					///< Tile hash lookup size (must be pot).
	int m_tileLutMask;					///< Tile hash lookup mask.

	dtMeshTile** m_posLookup;			///< Tile hash lookup.
	dtMeshTile* m_nextFree;				///< Freelist of tiles.
	dtMeshTile* m_tiles;				///< List of tiles.

#ifndef DT_POLYREF64
	unsigned int m_saltBits;			///< Number of salt bits in the tile ID.
	unsigned int m_tileBits;			///< Number of tile bits in the tile ID.
	unsigned int m_polyBits;			///< Number of poly bits in the tile ID.
#endif

	friend class dtNavMeshQuery;
};

/// Allocates a navigation mesh object using the Detour allocator.
/// @return A navigation mesh that is ready for initialization, or null on failure.
///  @ingroup detour
dtNavMesh* dtAllocNavMesh();

/// Frees the specified navigation mesh object using the Detour allocator.
///  @param[in]	navmesh		A navigation mesh allocated using #dtAllocNavMesh
///  @ingroup detour
void dtFreeNavMesh(dtNavMesh* navmesh);