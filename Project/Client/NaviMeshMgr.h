#pragma once
//#include <Engine\singleton.h>
//#include <Engine\globalNavi.h>
//
//class CGameObjectEx;
//
//class NaviMeshMgr
//	: public CSingleton<NaviMeshMgr>
//{
//private:
//	dtNavMeshParams m_params;			///< Current initialization params. TODO: do not store this info twice.
//	float m_orig[3];					///< Origin of the tile (0,0)
//	float m_tileWidth, m_tileHeight;	///< Dimensions of each tile.
//	int m_maxTiles;						///< Max number of tiles.
//	int m_tileLutSize;					///< Tile hash lookup size (must be pot).
//	int m_tileLutMask;					///< Tile hash lookup mask.
//
//	dtMeshTile** m_posLookup;			///< Tile hash lookup.
//	dtMeshTile* m_nextFree;				///< Freelist of tiles.
//	dtMeshTile* m_tiles;				///< List of tiles.
//
//#ifndef DT_POLYREF64
//	unsigned int m_saltBits;			///< Number of salt bits in the tile ID.
//	unsigned int m_tileBits;			///< Number of tile bits in the tile ID.
//	unsigned int m_polyBits;			///< Number of poly bits in the tile ID.
//#endif
//private:
//	dtStatus init(const dtNavMeshParams* params);
//	dtStatus addTile(unsigned char* data, int dataSize, int flags, dtTileRef lastRef, dtTileRef* result, dtMeshTile* pMeshTile);
//	const dtMeshTile* getTileAt(const int x, const int y, const int layer) const;
//	int getTilesAt(const int x, const int y, dtMeshTile** tiles, const int maxTiles) const;
//	
//	unsigned int decodePolyIdTile(dtPolyRef ref) const;
//	unsigned int decodePolyIdSalt(dtPolyRef ref) const;
//
//	/// Builds internal polygons links for a tile.
//	void connectIntLinks(dtMeshTile* tile);
//	/// Builds internal polygons links for a tile.
//	void baseOffMeshLinks(dtMeshTile* tile);
//	dtPolyRef getPolyRefBase(const dtMeshTile* tile) const;
//	/// Find nearest polygon within a tile.
//	dtPolyRef findNearestPolyInTile(const dtMeshTile* tile, const float* center,
//		const float* halfExtents, float* nearestPt) const;
//	int queryPolygonsInTile(const dtMeshTile* tile, const float* qmin, const float* qmax,
//		dtPolyRef* polys, const int maxPolys) const;
//	void closestPointOnPoly(dtPolyRef ref, const float* pos, float* closest, bool* posOverPoly) const;
//	int getTilesAt(const int x, const int y, dtMeshTile const** tiles, const int maxTiles) const;
//	void connectExtLinks(dtMeshTile* tile, dtMeshTile* target, int side);
//
//	/// Returns all polygons in neighbour tile based on portal defined by the segment.
//	int findConnectingPolys(const float* va, const float* vb,
//		const dtMeshTile* tile, int side,
//		dtPolyRef* con, float* conarea, int maxcon) const;
//	/// Returns neighbour tile based on side.
//	int getNeighbourTilesAt(const int x, const int y, const int side,
//		dtMeshTile** tiles, const int maxTiles) const;
//	dtTileRef getTileRef(const dtMeshTile* tile) const;
//
//	inline void decodePolyId(dtPolyRef ref, unsigned int& salt, unsigned int& it, unsigned int& ip) const
//	{
//#ifdef DT_POLYREF64
//		const dtPolyRef saltMask = ((dtPolyRef)1 << DT_SALT_BITS) - 1;
//		const dtPolyRef tileMask = ((dtPolyRef)1 << DT_TILE_BITS) - 1;
//		const dtPolyRef polyMask = ((dtPolyRef)1 << DT_POLY_BITS) - 1;
//		salt = (unsigned int)((ref >> (DT_POLY_BITS + DT_TILE_BITS)) & saltMask);
//		it = (unsigned int)((ref >> DT_POLY_BITS) & tileMask);
//		ip = (unsigned int)(ref & polyMask);
//#else
//		const dtPolyRef saltMask = ((dtPolyRef)1 << m_saltBits) - 1;
//		const dtPolyRef tileMask = ((dtPolyRef)1 << m_tileBits) - 1;
//		const dtPolyRef polyMask = ((dtPolyRef)1 << m_polyBits) - 1;
//		salt = (unsigned int)((ref >> (m_polyBits + m_tileBits)) & saltMask);
//		it = (unsigned int)((ref >> m_polyBits) & tileMask);
//		ip = (unsigned int)(ref & polyMask);
//#endif
//	}
//
//	/// Extracts the polygon's index (within its tile) from the specified polygon reference.
//	///  @note This function is generally meant for internal use only.
//	///  @param[in]	ref		The polygon reference.
//	///  @see #encodePolyId
//	inline unsigned int decodePolyIdPoly(dtPolyRef ref) const
//	{
//#ifdef DT_POLYREF64
//		const dtPolyRef polyMask = ((dtPolyRef)1 << DT_POLY_BITS) - 1;
//		return (unsigned int)(ref & polyMask);
//#else
//		const dtPolyRef polyMask = ((dtPolyRef)1 << m_polyBits) - 1;
//		return (unsigned int)(ref & polyMask);
//#endif
//	}
//	/// @}
//
///// @{
///// @name Encoding and Decoding
///// These functions are generally meant for internal use only.
//
///// Derives a standard polygon reference.
/////  @note This function is generally meant for internal use only.
/////  @param[in]	salt	The tile's salt value.
/////  @param[in]	it		The index of the tile.
/////  @param[in]	ip		The index of the polygon within the tile.
//	inline dtPolyRef encodePolyId(unsigned int salt, unsigned int it, unsigned int ip) const
//	{
//#ifdef DT_POLYREF64
//		return ((dtPolyRef)salt << (DT_POLY_BITS + DT_TILE_BITS)) | ((dtPolyRef)it << DT_POLY_BITS) | (dtPolyRef)ip;
//#else
//		return ((dtPolyRef)salt << (m_polyBits + m_tileBits)) | ((dtPolyRef)it << m_polyBits) | (dtPolyRef)ip;
//#endif
//	}
//
//	void connectExtOffMeshLinks(dtMeshTile* tile, dtMeshTile* target, int side);
//	void getTileAndPolyByRefUnsafe(const dtPolyRef ref, const dtMeshTile** tile, const dtPoly** poly) const;
//	bool getPolyHeight(const dtMeshTile* tile, const dtPoly* poly, const float* pos, float* height) const;
//public:
//	CGameObjectEx* loadAll(const wstring& path);
//public:
//	NaviMeshMgr();
//	~NaviMeshMgr();
//	friend class CSingleton<NaviMeshMgr>;
//};
