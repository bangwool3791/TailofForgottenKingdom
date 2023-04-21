#include "pch.h"
#include "NavMeshTestMgr.h"
#include <Engine\CResMgr.h>

#include <Engine\CTransform.h>
#include <Engine\CMeshRender.h>
#include "CGameObjectEx.h"


NavMeshTestMgr::NavMeshTestMgr()
{

}

NavMeshTestMgr::~NavMeshTestMgr()
{
	dtFree(m_tiles);
	dtFree(m_posLookup);
}

dtStatus NavMeshTestMgr::init(const dtNavMeshParams* params)
{
	memcpy(&m_params, params, sizeof(dtNavMeshParams));
	dtVcopy(m_orig, params->orig);
	m_tileWidth = params->tileWidth;
	m_tileHeight = params->tileHeight;

	// Init tiles
	m_maxTiles = params->maxTiles;
	m_tileLutSize = dtNextPow2(params->maxTiles / 4);
	if (!m_tileLutSize) m_tileLutSize = 1;
	m_tileLutMask = m_tileLutSize - 1;

	m_tiles = (dtMeshTile*)dtAlloc(sizeof(dtMeshTile) * m_maxTiles, DT_ALLOC_PERM);
	if (!m_tiles)
		return DT_FAILURE | DT_OUT_OF_MEMORY;
	m_posLookup = (dtMeshTile**)dtAlloc(sizeof(dtMeshTile*) * m_tileLutSize, DT_ALLOC_PERM);
	if (!m_posLookup)
		return DT_FAILURE | DT_OUT_OF_MEMORY;
	memset(m_tiles, 0, sizeof(dtMeshTile) * m_maxTiles);
	memset(m_posLookup, 0, sizeof(dtMeshTile*) * m_tileLutSize);
	m_nextFree = 0;
	for (int i = m_maxTiles - 1; i >= 0; --i)
	{
		m_tiles[i].salt = 1;
		m_tiles[i].next = m_nextFree;
		m_nextFree = &m_tiles[i];
	}

	// Init ID generator values.
#ifndef DT_POLYREF64
	m_tileBits = dtIlog2(dtNextPow2((unsigned int)params->maxTiles));
	m_polyBits = dtIlog2(dtNextPow2((unsigned int)params->maxPolys));
	// Only allow 31 salt bits, since the salt mask is calculated using 32bit uint and it will overflow.
	m_saltBits = dtMin((unsigned int)31, 32 - m_tileBits - m_polyBits);

	if (m_saltBits < 10)
		return DT_FAILURE | DT_INVALID_PARAM;
#endif

	return DT_SUCCESS;
}
CGameObjectEx* NavMeshTestMgr::loadAll(const wstring& path)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += path;

	FILE* fp = nullptr;
	_wfopen_s(&fp, strFilePath.c_str(), L"rb");
	if (!fp) return nullptr;

	// Read header.
	NavMeshSetHeader header;
	dtMeshTile* pMeshTile = nullptr;

	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1)
	{
		fclose(fp);
		return nullptr;
	}
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return nullptr;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return nullptr;
	}

	dtStatus status = init(&header.params);

	if (dtStatusFailed(status))
	{
		fclose(fp);
		return nullptr;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			return nullptr;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			return nullptr;
		}

		addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0, pMeshTile);
		dtFree(data);
	}

	CGameObjectEx*	pObject = new CGameObjectEx;
	pObject->SetName(L"Arene_Stage_Navi");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	pObject->Transform()->SetRelativeScale(Vec3(1.f, 1.f, 1.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"AreneStage_Navi_Mesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ColorMtrl"), 0);

	fclose(fp);

	return pObject;
}

/// @par
///
/// The add operation will fail if the data is in the wrong format, the allocated tile
/// space is full, or there is a tile already at the specified reference.
///
/// The lastRef parameter is used to restore a tile with the same tile
/// reference it had previously used.  In this case the #dtPolyRef's for the
/// tile will be restored to the same values they were before the tile was 
/// removed.
///
/// The nav mesh assumes exclusive access to the data passed and will make
/// changes to the dynamic portion of the data. For that reason the data
/// should not be reused in other nav meshes until the tile has been successfully
/// removed from this nav mesh.
///
/// @see dtCreateNavMeshData, #removeTile
dtStatus NavMeshTestMgr::addTile(unsigned char* data, int dataSize, int flags,
	dtTileRef lastRef, dtTileRef* result, dtMeshTile* pMeshTile)
{
	// Make sure the data is in right format.
	dtMeshHeader* header = (dtMeshHeader*)data;
	if (header->magic != DT_NAVMESH_MAGIC)
		return DT_FAILURE | DT_WRONG_MAGIC;
	if (header->version != DT_NAVMESH_VERSION)
		return DT_FAILURE | DT_WRONG_VERSION;

	// Make sure the location is free.
	if (getTileAt(header->x, header->y, header->layer))
		return DT_FAILURE | DT_ALREADY_OCCUPIED;

	// Allocate a tile.
	if (!lastRef)
	{
		if (m_nextFree)
		{
			pMeshTile = m_nextFree;
			m_nextFree = pMeshTile->next;
			pMeshTile->next = 0;
		}
	}
	else
	{
		// Try to relocate the tile to specific index with same salt.
		int tileIndex = (int)decodePolyIdTile((dtPolyRef)lastRef);
		if (tileIndex >= m_maxTiles)
			return DT_FAILURE | DT_OUT_OF_MEMORY;
		// Try to find the specific tile id from the free list.
		dtMeshTile* target = &m_tiles[tileIndex];
		dtMeshTile* prev = 0;
		pMeshTile = m_nextFree;
		while (pMeshTile && pMeshTile != target)
		{
			prev = pMeshTile;
			pMeshTile = pMeshTile->next;
		}
		// Could not find the correct location.
		if (pMeshTile != target)
			return DT_FAILURE | DT_OUT_OF_MEMORY;
		// Remove from freelist
		if (!prev)
			m_nextFree = pMeshTile->next;
		else
			prev->next = pMeshTile->next;

		// Restore salt.
		pMeshTile->salt = decodePolyIdSalt((dtPolyRef)lastRef);
	}

	// Make sure we could allocate a tile.
	if (!pMeshTile)
		return DT_FAILURE | DT_OUT_OF_MEMORY;

	// Insert tile into the position lut.
	int h = computeTileHash(header->x, header->y, m_tileLutMask);
	pMeshTile->next = m_posLookup[h];
	m_posLookup[h] = pMeshTile;

	// Patch header pointers.
	const int headerSize = dtAlign4(sizeof(dtMeshHeader));
	const int vertsSize = dtAlign4(sizeof(float) * 3 * header->vertCount);
	const int polysSize = dtAlign4(sizeof(dtPoly) * header->polyCount);
	const int linksSize = dtAlign4(sizeof(dtLink) * (header->maxLinkCount));
	const int detailMeshesSize = dtAlign4(sizeof(dtPolyDetail) * header->detailMeshCount);
	const int detailVertsSize = dtAlign4(sizeof(float) * 3 * header->detailVertCount);
	const int detailTrisSize = dtAlign4(sizeof(unsigned char) * 4 * header->detailTriCount);
	const int bvtreeSize = dtAlign4(sizeof(dtBVNode) * header->bvNodeCount);
	const int offMeshLinksSize = dtAlign4(sizeof(dtOffMeshConnection) * header->offMeshConCount);

	unsigned char* d = data + headerSize;
	pMeshTile->verts = dtGetThenAdvanceBufferPointer<float>(d, vertsSize);
	pMeshTile->polys = dtGetThenAdvanceBufferPointer<dtPoly>(d, polysSize);
	pMeshTile->links = dtGetThenAdvanceBufferPointer<dtLink>(d, linksSize);
	pMeshTile->detailMeshes = dtGetThenAdvanceBufferPointer<dtPolyDetail>(d, detailMeshesSize);
	pMeshTile->detailVerts = dtGetThenAdvanceBufferPointer<float>(d, detailVertsSize);
	pMeshTile->detailTris = dtGetThenAdvanceBufferPointer<unsigned char>(d, detailTrisSize);
	pMeshTile->bvTree = dtGetThenAdvanceBufferPointer<dtBVNode>(d, bvtreeSize);
	pMeshTile->offMeshCons = dtGetThenAdvanceBufferPointer<dtOffMeshConnection>(d, offMeshLinksSize);

	// If there are no items in the bvtree, reset the tree pointer.
	if (!bvtreeSize)
		pMeshTile->bvTree = 0;

	// Build links freelist
	pMeshTile->linksFreeList = 0;
	pMeshTile->links[header->maxLinkCount - 1].next = DT_NULL_LINK;
	for (int i = 0; i < header->maxLinkCount - 1; ++i)
		pMeshTile->links[i].next = i + 1;

	// Init tile.
	pMeshTile->header = header;
	pMeshTile->data = data;
	pMeshTile->dataSize = dataSize;
	pMeshTile->flags = flags;

	connectIntLinks(pMeshTile);

	// Base off-mesh connections to their starting polygons and connect connections inside the tile.
	baseOffMeshLinks(pMeshTile);
	connectExtOffMeshLinks(pMeshTile, pMeshTile, -1);

	// Create connections with neighbour tiles.
	static const int MAX_NEIS = 32;
	dtMeshTile* neis[MAX_NEIS];
	int nneis;

	// Connect with layers in current tile.
	nneis = getTilesAt(header->x, header->y, neis, MAX_NEIS);
	for (int j = 0; j < nneis; ++j)
	{
		if (neis[j] == pMeshTile)
			continue;

		connectExtLinks(pMeshTile, neis[j], -1);
		connectExtLinks(neis[j], pMeshTile, -1);
		connectExtOffMeshLinks(pMeshTile, neis[j], -1);
		connectExtOffMeshLinks(neis[j], pMeshTile, -1);
	}

	// Connect with neighbour tiles.
	for (int i = 0; i < 8; ++i)
	{
		nneis = getNeighbourTilesAt(header->x, header->y, i, neis, MAX_NEIS);
		for (int j = 0; j < nneis; ++j)
		{
			connectExtLinks(pMeshTile, neis[j], i);
			connectExtLinks(neis[j], pMeshTile, dtOppositeTile(i));
			connectExtOffMeshLinks(pMeshTile, neis[j], i);
			connectExtOffMeshLinks(neis[j], pMeshTile, dtOppositeTile(i));
		}
	}

	if (result)
		*result = getTileRef(pMeshTile);

	CResMgr::GetInst()->CreateNaviMesh(pMeshTile);

	return DT_SUCCESS;
}


const dtMeshTile* NavMeshTestMgr::getTileAt(const int x, const int y, const int layer) const
{
	// Find tile based on hash.
	int h = computeTileHash(x, y, m_tileLutMask);
	dtMeshTile* tile = m_posLookup[h];
	while (tile)
	{
		if (tile->header &&
			tile->header->x == x &&
			tile->header->y == y &&
			tile->header->layer == layer)
		{
			return tile;
		}
		tile = tile->next;
	}
	return 0;
}

dtTileRef NavMeshTestMgr::getTileRef(const dtMeshTile* tile) const
{
	if (!tile) return 0;
	const unsigned int it = (unsigned int)(tile - m_tiles);
	return (dtTileRef)encodePolyId(tile->salt, it, 0);
}

int NavMeshTestMgr::getNeighbourTilesAt(const int x, const int y, const int side, dtMeshTile** tiles, const int maxTiles) const
{
	int nx = x, ny = y;
	switch (side)
	{
	case 0: nx++; break;
	case 1: nx++; ny++; break;
	case 2: ny++; break;
	case 3: nx--; ny++; break;
	case 4: nx--; break;
	case 5: nx--; ny--; break;
	case 6: ny--; break;
	case 7: nx++; ny--; break;
	};

	return getTilesAt(nx, ny, tiles, maxTiles);
}

/// Extracts the tile's index from the specified polygon reference.
///  @note This function is generally meant for internal use only.
///  @param[in]	ref		The polygon reference.
///  @see #encodePolyId
unsigned int NavMeshTestMgr::decodePolyIdTile(dtPolyRef ref) const
{
#ifdef DT_POLYREF64
	const dtPolyRef tileMask = ((dtPolyRef)1 << DT_TILE_BITS) - 1;
	return (unsigned int)((ref >> DT_POLY_BITS) & tileMask);
#else
	const dtPolyRef tileMask = ((dtPolyRef)1 << m_tileBits) - 1;
	return (unsigned int)((ref >> m_polyBits) & tileMask);
#endif
}

/// Extracts a tile's salt value from the specified polygon reference.
///  @note This function is generally meant for internal use only.
///  @param[in]	ref		The polygon reference.
///  @see #encodePolyId
unsigned int NavMeshTestMgr::decodePolyIdSalt(dtPolyRef ref) const
{
#ifdef DT_POLYREF64
	const dtPolyRef saltMask = ((dtPolyRef)1 << DT_SALT_BITS) - 1;
	return (unsigned int)((ref >> (DT_POLY_BITS + DT_TILE_BITS)) & saltMask);
#else
	const dtPolyRef saltMask = ((dtPolyRef)1 << m_saltBits) - 1;
	return (unsigned int)((ref >> (m_polyBits + m_tileBits)) & saltMask);
#endif
}

void NavMeshTestMgr::connectIntLinks(dtMeshTile* tile)
{
	if (!tile) return;

	dtPolyRef base = getPolyRefBase(tile);

	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		dtPoly* poly = &tile->polys[i];
		poly->firstLink = DT_NULL_LINK;

		if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
			continue;

		// Build edge links backwards so that the links will be
		// in the linked list from lowest index to highest.
		for (int j = poly->vertCount - 1; j >= 0; --j)
		{
			// Skip hard and non-internal edges.
			if (poly->neis[j] == 0 || (poly->neis[j] & DT_EXT_LINK)) continue;

			unsigned int idx = allocLink(tile);
			if (idx != DT_NULL_LINK)
			{
				dtLink* link = &tile->links[idx];
				link->ref = base | (dtPolyRef)(poly->neis[j] - 1);
				link->edge = (unsigned char)j;
				link->side = 0xff;
				link->bmin = link->bmax = 0;
				// Add to linked list.
				link->next = poly->firstLink;
				poly->firstLink = idx;
			}
		}
	}
}

void NavMeshTestMgr::baseOffMeshLinks(dtMeshTile* tile)
{
	if (!tile) return;

	dtPolyRef base = getPolyRefBase(tile);

	// Base off-mesh connection start points.
	for (int i = 0; i < tile->header->offMeshConCount; ++i)
	{
		dtOffMeshConnection* con = &tile->offMeshCons[i];
		dtPoly* poly = &tile->polys[con->poly];

		const float halfExtents[3] = { con->rad, tile->header->walkableClimb, con->rad };

		// Find polygon to connect to.
		const float* p = &con->pos[0]; // First vertex
		float nearestPt[3];
		dtPolyRef ref = findNearestPolyInTile(tile, p, halfExtents, nearestPt);
		if (!ref) continue;
		// findNearestPoly may return too optimistic results, further check to make sure. 
		if (dtSqr(nearestPt[0] - p[0]) + dtSqr(nearestPt[2] - p[2]) > dtSqr(con->rad))
			continue;
		// Make sure the location is on current mesh.
		float* v = &tile->verts[poly->verts[0] * 3];
		dtVcopy(v, nearestPt);

		// Link off-mesh connection to target poly.
		unsigned int idx = allocLink(tile);
		if (idx != DT_NULL_LINK)
		{
			dtLink* link = &tile->links[idx];
			link->ref = ref;
			link->edge = (unsigned char)0;
			link->side = 0xff;
			link->bmin = link->bmax = 0;
			// Add to linked list.
			link->next = poly->firstLink;
			poly->firstLink = idx;
		}

		// Start end-point is always connect back to off-mesh connection. 
		unsigned int tidx = allocLink(tile);
		if (tidx != DT_NULL_LINK)
		{
			const unsigned short landPolyIdx = (unsigned short)decodePolyIdPoly(ref);
			dtPoly* landPoly = &tile->polys[landPolyIdx];
			dtLink* link = &tile->links[tidx];
			link->ref = base | (dtPolyRef)(con->poly);
			link->edge = 0xff;
			link->side = 0xff;
			link->bmin = link->bmax = 0;
			// Add to linked list.
			link->next = landPoly->firstLink;
			landPoly->firstLink = tidx;
		}
	}
}

/// @par
///
/// Example use case:
/// @code
///
/// const dtPolyRef base = navmesh->getPolyRefBase(tile);
/// for (int i = 0; i < tile->header->polyCount; ++i)
/// {
///     const dtPoly* p = &tile->polys[i];
///     const dtPolyRef ref = base | (dtPolyRef)i;
///     
///     // Use the reference to access the polygon data.
/// }
/// @endcode
dtPolyRef NavMeshTestMgr::getPolyRefBase(const dtMeshTile* tile) const
{
	if (!tile) return 0;
	const unsigned int it = (unsigned int)(tile - m_tiles);
	return encodePolyId(tile->salt, it, 0);
}

void NavMeshTestMgr::connectExtOffMeshLinks(dtMeshTile* tile, dtMeshTile* target, int side)
{
	if (!tile) return;

	// Connect off-mesh links.
	// We are interested on links which land from target tile to this tile.
	const unsigned char oppositeSide = (side == -1) ? 0xff : (unsigned char)dtOppositeTile(side);

	for (int i = 0; i < target->header->offMeshConCount; ++i)
	{
		dtOffMeshConnection* targetCon = &target->offMeshCons[i];
		if (targetCon->side != oppositeSide)
			continue;

		dtPoly* targetPoly = &target->polys[targetCon->poly];
		// Skip off-mesh connections which start location could not be connected at all.
		if (targetPoly->firstLink == DT_NULL_LINK)
			continue;

		const float halfExtents[3] = { targetCon->rad, target->header->walkableClimb, targetCon->rad };

		// Find polygon to connect to.
		const float* p = &targetCon->pos[3];
		float nearestPt[3];
		dtPolyRef ref = findNearestPolyInTile(tile, p, halfExtents, nearestPt);
		if (!ref)
			continue;
		// findNearestPoly may return too optimistic results, further check to make sure. 
		if (dtSqr(nearestPt[0] - p[0]) + dtSqr(nearestPt[2] - p[2]) > dtSqr(targetCon->rad))
			continue;
		// Make sure the location is on current mesh.
		float* v = &target->verts[targetPoly->verts[1] * 3];
		dtVcopy(v, nearestPt);

		// Link off-mesh connection to target poly.
		unsigned int idx = allocLink(target);
		if (idx != DT_NULL_LINK)
		{
			dtLink* link = &target->links[idx];
			link->ref = ref;
			link->edge = (unsigned char)1;
			link->side = oppositeSide;
			link->bmin = link->bmax = 0;
			// Add to linked list.
			link->next = targetPoly->firstLink;
			targetPoly->firstLink = idx;
		}

		// Link target poly to off-mesh connection.
		if (targetCon->flags & DT_OFFMESH_CON_BIDIR)
		{
			unsigned int tidx = allocLink(tile);
			if (tidx != DT_NULL_LINK)
			{
				const unsigned short landPolyIdx = (unsigned short)decodePolyIdPoly(ref);
				dtPoly* landPoly = &tile->polys[landPolyIdx];
				dtLink* link = &tile->links[tidx];
				link->ref = getPolyRefBase(target) | (dtPolyRef)(targetCon->poly);
				link->edge = 0xff;
				link->side = (unsigned char)(side == -1 ? 0xff : side);
				link->bmin = link->bmax = 0;
				// Add to linked list.
				link->next = landPoly->firstLink;
				landPoly->firstLink = tidx;
			}
		}
	}

}

dtPolyRef NavMeshTestMgr::findNearestPolyInTile(const dtMeshTile* tile,
	const float* center, const float* halfExtents,
	float* nearestPt) const
{
	float bmin[3], bmax[3];
	dtVsub(bmin, center, halfExtents);
	dtVadd(bmax, center, halfExtents);

	// Get nearby polygons from proximity grid.
	dtPolyRef polys[128];
	int polyCount = queryPolygonsInTile(tile, bmin, bmax, polys, 128);

	// Find nearest polygon amongst the nearby polygons.
	dtPolyRef nearest = 0;
	float nearestDistanceSqr = FLT_MAX;
	for (int i = 0; i < polyCount; ++i)
	{
		dtPolyRef ref = polys[i];
		float closestPtPoly[3];
		float diff[3];
		bool posOverPoly = false;
		float d;
		closestPointOnPoly(ref, center, closestPtPoly, &posOverPoly);

		// If a point is directly over a polygon and closer than
		// climb height, favor that instead of straight line nearest point.
		dtVsub(diff, center, closestPtPoly);
		if (posOverPoly)
		{
			d = dtAbs<float>(diff[1]) - tile->header->walkableClimb;
			d = d > 0 ? d * d : 0;
		}
		else
		{
			d = dtVlenSqr(diff);
		}

		if (d < nearestDistanceSqr)
		{
			dtVcopy(nearestPt, closestPtPoly);
			nearestDistanceSqr = d;
			nearest = ref;
		}
	}

	return nearest;
}

int NavMeshTestMgr::queryPolygonsInTile(const dtMeshTile* tile, const float* qmin, const float* qmax,
	dtPolyRef* polys, const int maxPolys) const
{
	if (tile->bvTree)
	{
		const dtBVNode* node = &tile->bvTree[0];
		const dtBVNode* end = &tile->bvTree[tile->header->bvNodeCount];
		const float* tbmin = tile->header->bmin;
		const float* tbmax = tile->header->bmax;
		const float qfac = tile->header->bvQuantFactor;

		// Calculate quantized box
		unsigned short bmin[3], bmax[3];
		// dtClamp query box to world box.
		float minx = dtClamp(qmin[0], tbmin[0], tbmax[0]) - tbmin[0];
		float miny = dtClamp(qmin[1], tbmin[1], tbmax[1]) - tbmin[1];
		float minz = dtClamp(qmin[2], tbmin[2], tbmax[2]) - tbmin[2];
		float maxx = dtClamp(qmax[0], tbmin[0], tbmax[0]) - tbmin[0];
		float maxy = dtClamp(qmax[1], tbmin[1], tbmax[1]) - tbmin[1];
		float maxz = dtClamp(qmax[2], tbmin[2], tbmax[2]) - tbmin[2];
		// Quantize
		bmin[0] = (unsigned short)(qfac * minx) & 0xfffe;
		bmin[1] = (unsigned short)(qfac * miny) & 0xfffe;
		bmin[2] = (unsigned short)(qfac * minz) & 0xfffe;
		bmax[0] = (unsigned short)(qfac * maxx + 1) | 1;
		bmax[1] = (unsigned short)(qfac * maxy + 1) | 1;
		bmax[2] = (unsigned short)(qfac * maxz + 1) | 1;

		// Traverse tree
		dtPolyRef base = getPolyRefBase(tile);
		int n = 0;
		while (node < end)
		{
			const bool overlap = dtOverlapQuantBounds(bmin, bmax, node->bmin, node->bmax);
			const bool isLeafNode = node->i >= 0;

			if (isLeafNode && overlap)
			{
				if (n < maxPolys)
					polys[n++] = base | (dtPolyRef)node->i;
			}

			if (overlap || isLeafNode)
				node++;
			else
			{
				const int escapeIndex = -node->i;
				node += escapeIndex;
			}
		}

		return n;
	}
	else
	{
		float bmin[3], bmax[3];
		int n = 0;
		dtPolyRef base = getPolyRefBase(tile);
		for (int i = 0; i < tile->header->polyCount; ++i)
		{
			dtPoly* p = &tile->polys[i];
			// Do not return off-mesh connection polygons.
			if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
				continue;
			// Calc polygon bounds.
			const float* v = &tile->verts[p->verts[0] * 3];
			dtVcopy(bmin, v);
			dtVcopy(bmax, v);
			for (int j = 1; j < p->vertCount; ++j)
			{
				v = &tile->verts[p->verts[j] * 3];
				dtVmin(bmin, v);
				dtVmax(bmax, v);
			}
			if (dtOverlapBounds(qmin, qmax, bmin, bmax))
			{
				if (n < maxPolys)
					polys[n++] = base | (dtPolyRef)i;
			}
		}
		return n;
	}
}

void NavMeshTestMgr::closestPointOnPoly(dtPolyRef ref, const float* pos, float* closest, bool* posOverPoly) const
{
	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	getTileAndPolyByRefUnsafe(ref, &tile, &poly);

	dtVcopy(closest, pos);
	if (getPolyHeight(tile, poly, pos, &closest[1]))
	{
		if (posOverPoly)
			*posOverPoly = true;
		return;
	}

	if (posOverPoly)
		*posOverPoly = false;

	// Off-mesh connections don't have detail polygons.
	if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
	{
		const float* v0 = &tile->verts[poly->verts[0] * 3];
		const float* v1 = &tile->verts[poly->verts[1] * 3];
		float t;
		dtDistancePtSegSqr2D(pos, v0, v1, t);
		dtVlerp(closest, v0, v1, t);
		return;
	}

	// Outside poly that is not an offmesh connection.
	closestPointOnDetailEdges<true>(tile, poly, pos, closest);
}

/// @par
///
/// This function will not fail if the tiles array is too small to hold the
/// entire result set.  It will simply fill the array to capacity.
int NavMeshTestMgr::getTilesAt(const int x, const int y, dtMeshTile const** tiles, const int maxTiles) const
{
	int n = 0;

	// Find tile based on hash.
	int h = computeTileHash(x, y, m_tileLutMask);
	dtMeshTile* tile = m_posLookup[h];
	while (tile)
	{
		if (tile->header &&
			tile->header->x == x &&
			tile->header->y == y)
		{
			if (n < maxTiles)
				tiles[n++] = tile;
		}
		tile = tile->next;
	}

	return n;
}

/// Returns neighbour tile based on side.
int NavMeshTestMgr::getTilesAt(const int x, const int y,
	dtMeshTile** tiles, const int maxTiles) const
{
	int n = 0;

	// Find tile based on hash.
	int h = computeTileHash(x, y, m_tileLutMask);
	dtMeshTile* tile = m_posLookup[h];
	while (tile)
	{
		if (tile->header &&
			tile->header->x == x &&
			tile->header->y == y)
		{
			if (n < maxTiles)
				tiles[n++] = tile;
		}
		tile = tile->next;
	}

	return n;
}
void NavMeshTestMgr::connectExtLinks(dtMeshTile* tile, dtMeshTile* target, int side)
{
	if (!tile) return;

	// Connect border links.
	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		dtPoly* poly = &tile->polys[i];

		// Create new links.
//		unsigned short m = DT_EXT_LINK | (unsigned short)side;

		const int nv = poly->vertCount;
		for (int j = 0; j < nv; ++j)
		{
			// Skip non-portal edges.
			if ((poly->neis[j] & DT_EXT_LINK) == 0)
				continue;

			const int dir = (int)(poly->neis[j] & 0xff);
			if (side != -1 && dir != side)
				continue;

			// Create new links
			const float* va = &tile->verts[poly->verts[j] * 3];
			const float* vb = &tile->verts[poly->verts[(j + 1) % nv] * 3];
			dtPolyRef nei[4];
			float neia[4 * 2];
			int nnei = findConnectingPolys(va, vb, target, dtOppositeTile(dir), nei, neia, 4);
			for (int k = 0; k < nnei; ++k)
			{
				unsigned int idx = allocLink(tile);
				if (idx != DT_NULL_LINK)
				{
					dtLink* link = &tile->links[idx];
					link->ref = nei[k];
					link->edge = (unsigned char)j;
					link->side = (unsigned char)dir;

					link->next = poly->firstLink;
					poly->firstLink = idx;

					// Compress portal limits to a byte value.
					if (dir == 0 || dir == 4)
					{
						float tmin = (neia[k * 2 + 0] - va[2]) / (vb[2] - va[2]);
						float tmax = (neia[k * 2 + 1] - va[2]) / (vb[2] - va[2]);
						if (tmin > tmax)
							dtSwap(tmin, tmax);
						link->bmin = (unsigned char)roundf(dtClamp(tmin, 0.0f, 1.0f) * 255.0f);
						link->bmax = (unsigned char)roundf(dtClamp(tmax, 0.0f, 1.0f) * 255.0f);
					}
					else if (dir == 2 || dir == 6)
					{
						float tmin = (neia[k * 2 + 0] - va[0]) / (vb[0] - va[0]);
						float tmax = (neia[k * 2 + 1] - va[0]) / (vb[0] - va[0]);
						if (tmin > tmax)
							dtSwap(tmin, tmax);
						link->bmin = (unsigned char)roundf(dtClamp(tmin, 0.0f, 1.0f) * 255.0f);
						link->bmax = (unsigned char)roundf(dtClamp(tmax, 0.0f, 1.0f) * 255.0f);
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
int NavMeshTestMgr::findConnectingPolys(const float* va, const float* vb,
	const dtMeshTile* tile, int side,
	dtPolyRef* con, float* conarea, int maxcon) const
{
	if (!tile) return 0;

	float amin[2], amax[2];
	calcSlabEndPoints(va, vb, amin, amax, side);
	const float apos = getSlabCoord(va, side);

	// Remove links pointing to 'side' and compact the links array. 
	float bmin[2], bmax[2];
	unsigned short m = DT_EXT_LINK | (unsigned short)side;
	int n = 0;

	dtPolyRef base = getPolyRefBase(tile);

	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		dtPoly* poly = &tile->polys[i];
		const int nv = poly->vertCount;
		for (int j = 0; j < nv; ++j)
		{
			// Skip edges which do not point to the right side.
			if (poly->neis[j] != m) continue;

			const float* vc = &tile->verts[poly->verts[j] * 3];
			const float* vd = &tile->verts[poly->verts[(j + 1) % nv] * 3];
			const float bpos = getSlabCoord(vc, side);

			// Segments are not close enough.
			if (dtAbs<float>(apos - bpos) > 0.01f)
				continue;

			// Check if the segments touch.
			calcSlabEndPoints(vc, vd, bmin, bmax, side);

			if (!overlapSlabs(amin, amax, bmin, bmax, 0.01f, tile->header->walkableClimb)) continue;

			// Add return value.
			if (n < maxcon)
			{
				conarea[n * 2 + 0] = dtMax(amin[0], bmin[0]);
				conarea[n * 2 + 1] = dtMin(amax[0], bmax[0]);
				con[n] = base | (dtPolyRef)i;
				n++;
			}
			break;
		}
	}
	return n;
}

void NavMeshTestMgr::getTileAndPolyByRefUnsafe(const dtPolyRef ref, const dtMeshTile** tile, const dtPoly** poly) const
{
	unsigned int salt, it, ip;
	decodePolyId(ref, salt, it, ip);
	*tile = &m_tiles[it];
	*poly = &m_tiles[it].polys[ip];
}

bool NavMeshTestMgr::getPolyHeight(const dtMeshTile* tile, const dtPoly* poly, const float* pos, float* height) const
{
	// Off-mesh connections do not have detail polys and getting height
	// over them does not make sense.
	if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
		return false;

	const unsigned int ip = (unsigned int)(poly - tile->polys);
	const dtPolyDetail* pd = &tile->detailMeshes[ip];

	float verts[DT_VERTS_PER_POLYGON * 3];
	const int nv = poly->vertCount;
	for (int i = 0; i < nv; ++i)
		dtVcopy(&verts[i * 3], &tile->verts[poly->verts[i] * 3]);

	if (!dtPointInPolygon(pos, verts, nv))
		return false;

	if (!height)
		return true;

	// Find height at the location.
	for (int j = 0; j < pd->triCount; ++j)
	{
		const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];
		const float* v[3];
		for (int k = 0; k < 3; ++k)
		{
			if (t[k] < poly->vertCount)
				v[k] = &tile->verts[poly->verts[t[k]] * 3];
			else
				v[k] = &tile->detailVerts[(pd->vertBase + (t[k] - poly->vertCount)) * 3];
		}
		float h;
		if (dtClosestHeightPointTriangle(pos, v[0], v[1], v[2], h))
		{
			*height = h;
			return true;
		}
	}

	// If all triangle checks failed above (can happen with degenerate triangles
	// or larger floating point values) the point is on an edge, so just select
	// closest. This should almost never happen so the extra iteration here is
	// ok.
	float closest[3];
	closestPointOnDetailEdges<false>(tile, poly, pos, closest);
	*height = closest[1];
	return true;
}
