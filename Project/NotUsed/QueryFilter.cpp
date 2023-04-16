
#include "QueryFilter.h"
#include "NavMeshQuery.h"

dtQueryFilter::dtQueryFilter() :
	m_includeFlags(0xffff),
	m_excludeFlags(0)
{
	for (int i = 0; i < DT_MAX_AREAS; ++i)
		m_areaCost[i] = 1.0f;
}

#ifdef DT_VIRTUAL_QUERYFILTER
bool dtQueryFilter::passFilter(const dtPolyRef /*ref*/,
	const dtMeshTile* /*tile*/,
	const dtPoly* poly) const
{
	return (poly->flags & m_includeFlags) != 0 && (poly->flags & m_excludeFlags) == 0;
}

float dtQueryFilter::getCost(const float* pa, const float* pb,
	const dtPolyRef /*prevRef*/, const dtMeshTile* /*prevTile*/, const dtPoly* /*prevPoly*/,
	const dtPolyRef /*curRef*/, const dtMeshTile* /*curTile*/, const dtPoly* curPoly,
	const dtPolyRef /*nextRef*/, const dtMeshTile* /*nextTile*/, const dtPoly* /*nextPoly*/) const
{
	return dtVdist(pa, pb) * m_areaCost[curPoly->getArea()];
}
#else
inline bool dtQueryFilter::passFilter(const dtPolyRef /*ref*/,
	const dtMeshTile* /*tile*/,
	const dtPoly* poly) const
{
	return (poly->flags & m_includeFlags) != 0 && (poly->flags & m_excludeFlags) == 0;
}

#endif	

dtNavMeshQuery* dtAllocNavMeshQuery()
{
	void* mem = dtAlloc(sizeof(dtNavMeshQuery), DT_ALLOC_PERM);
	if (!mem) return 0;
	return new(mem) dtNavMeshQuery;
}

dtPolyQuery::~dtPolyQuery()
{
	// Defined out of line to fix the weak v-tables warning
}