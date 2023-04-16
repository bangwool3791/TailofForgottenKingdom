#pragma once
#include "GlobalNavi.h"
#include "NavMesh.h"
/// Provides custom polygon query behavior.
/// Used by dtNavMeshQuery::queryPolygons.
/// @ingroup detour
class dtPolyQuery
{
public:
	virtual ~dtPolyQuery();

	/// Called for each batch of unique polygons touched by the search area in dtNavMeshQuery::queryPolygons.
	/// This can be called multiple times for a single query.
	virtual void process(const dtMeshTile* tile, dtPoly** polys, dtPolyRef* refs, int count) = 0;
};

/// Defines polygon filtering and traversal costs for navigation mesh query operations.
/// @ingroup detour
class dtQueryFilter
{
	float m_areaCost[DT_MAX_AREAS];		///< Cost per area type. (Used by default implementation.)
	unsigned short m_includeFlags;		///< Flags for polygons that can be visited. (Used by default implementation.)
	unsigned short m_excludeFlags;		///< Flags for polygons that should not be visted. (Used by default implementation.)

public:
	dtQueryFilter();
	~dtQueryFilter();

	/// Returns true if the polygon can be visited.  (I.e. Is traversable.)
	///  @param[in]		ref		The reference id of the polygon test.
	///  @param[in]		tile	The tile containing the polygon.
	///  @param[in]		poly  The polygon to test.
#ifdef DT_VIRTUAL_QUERYFILTER
	virtual bool passFilter(const dtPolyRef ref,
		const dtMeshTile* tile,
		const dtPoly* poly) const;
#else
	bool passFilter(const dtPolyRef ref,
		const dtMeshTile* tile,
		const dtPoly* poly) const;
#endif

	/// Returns cost to move from the beginning to the end of a line segment
	/// that is fully contained within a polygon.
	///  @param[in]		pa			The start position on the edge of the previous and current polygon. [(x, y, z)]
	///  @param[in]		pb			The end position on the edge of the current and next polygon. [(x, y, z)]
	///  @param[in]		prevRef		The reference id of the previous polygon. [opt]
	///  @param[in]		prevTile	The tile containing the previous polygon. [opt]
	///  @param[in]		prevPoly	The previous polygon. [opt]
	///  @param[in]		curRef		The reference id of the current polygon.
	///  @param[in]		curTile		The tile containing the current polygon.
	///  @param[in]		curPoly		The current polygon.
	///  @param[in]		nextRef		The refernece id of the next polygon. [opt]
	///  @param[in]		nextTile	The tile containing the next polygon. [opt]
	///  @param[in]		nextPoly	The next polygon. [opt]

	inline float getCost(const float* pa, const float* pb,
		const dtPolyRef /*prevRef*/, const dtMeshTile* /*prevTile*/, const dtPoly* /*prevPoly*/,
		const dtPolyRef /*curRef*/, const dtMeshTile* /*curTile*/, const dtPoly* curPoly,
		const dtPolyRef /*nextRef*/, const dtMeshTile* /*nextTile*/, const dtPoly* /*nextPoly*/) const
	{
		return dtVdist(pa, pb) * m_areaCost[curPoly->getArea()];
	}

	/// @name Getters and setters for the default implementation data.
	///@{

	/// Returns the traversal cost of the area.
	///  @param[in]		i		The id of the area.
	/// @returns The traversal cost of the area.
	inline float getAreaCost(const int i) const { return m_areaCost[i]; }

	/// Sets the traversal cost of the area.
	///  @param[in]		i		The id of the area.
	///  @param[in]		cost	The new cost of traversing the area.
	inline void setAreaCost(const int i, const float cost) { m_areaCost[i] = cost; }

	/// Returns the include flags for the filter.
	/// Any polygons that include one or more of these flags will be
	/// included in the operation.
	inline unsigned short getIncludeFlags() const { return m_includeFlags; }

	/// Sets the include flags for the filter.
	/// @param[in]		flags	The new flags.
	inline void setIncludeFlags(const unsigned short flags) { m_includeFlags = flags; }

	/// Returns the exclude flags for the filter.
	/// Any polygons that include one ore more of these flags will be
	/// excluded from the operation.
	inline unsigned short getExcludeFlags() const { return m_excludeFlags; }

	/// Sets the exclude flags for the filter.
	/// @param[in]		flags		The new flags.
	inline void setExcludeFlags(const unsigned short flags) { m_excludeFlags = flags; }

	///@}

};
