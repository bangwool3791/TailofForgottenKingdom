#pragma once

#include "pch.h"
#include "NaviDefine.h"
#include "NaviStruct.h"

inline float dtMathFabsf(float x) { return fabsf(x); }
inline float dtMathSqrtf(float x) { return sqrtf(x); }
inline float dtMathFloorf(float x) { return floorf(x); }
inline float dtMathCeilf(float x) { return ceilf(x); }
inline float dtMathCosf(float x) { return cosf(x); }
inline float dtMathSinf(float x) { return sinf(x); }
inline float dtMathAtan2f(float y, float x) { return atan2f(y, x); }
inline bool dtMathIsfinite(float x) { return isfinite(x); }

/// Used to ignore a function parameter.  VS complains about unused parameters
/// and this silences the warning.
template<class T> void dtIgnoreUnused(const T&) { }
/// Swaps the values of the two parameters.
///  @param[in,out]	a	Value A
///  @param[in,out]	b	Value B
template<class T> inline void dtSwap(T& a, T& b) { T t = a; a = b; b = t; }

/// Returns the minimum of two values.
///  @param[in]		a	Value A
///  @param[in]		b	Value B
///  @return The minimum of the two values.
template<class T> inline T dtMin(T a, T b) { return a < b ? a : b; }

/// Returns the maximum of two values.
///  @param[in]		a	Value A
///  @param[in]		b	Value B
///  @return The maximum of the two values.
template<class T> inline T dtMax(T a, T b) { return a > b ? a : b; }

/// Returns the absolute value.
///  @param[in]		a	The value.
///  @return The absolute value of the specified value.
template<class T> inline T dtAbs(T a) { return a < 0 ? -a : a; }

/// Returns the square of the value.
///  @param[in]		a	The value.
///  @return The square of the value.
template<class T> inline T dtSqr(T a) { return a * a; }

static int compareItemX(const void* va, const void* vb)
{
	const BVItem* a = (const BVItem*)va;
	const BVItem* b = (const BVItem*)vb;
	if (a->bmin[0] < b->bmin[0])
		return -1;
	if (a->bmin[0] > b->bmin[0])
		return 1;
	return 0;
}

static int compareItemY(const void* va, const void* vb)
{
	const BVItem* a = (const BVItem*)va;
	const BVItem* b = (const BVItem*)vb;
	if (a->bmin[1] < b->bmin[1])
		return -1;
	if (a->bmin[1] > b->bmin[1])
		return 1;
	return 0;
}

static int compareItemZ(const void* va, const void* vb)
{
	const BVItem* a = (const BVItem*)va;
	const BVItem* b = (const BVItem*)vb;
	if (a->bmin[2] < b->bmin[2])
		return -1;
	if (a->bmin[2] > b->bmin[2])
		return 1;
	return 0;
}

/// Selects the minimum value of each element from the specified vectors.
///  @param[in,out]	mn	A vector.  (Will be updated with the result.) [(x, y, z)]
///  @param[in]	v	A vector. [(x, y, z)]
inline void dtVmin(float* mn, const float* v)
{
	mn[0] = dtMin(mn[0], v[0]);
	mn[1] = dtMin(mn[1], v[1]);
	mn[2] = dtMin(mn[2], v[2]);
}

/// Selects the maximum value of each element from the specified vectors.
///  @param[in,out]	mx	A vector.  (Will be updated with the result.) [(x, y, z)]
///  @param[in]		v	A vector. [(x, y, z)]
inline void dtVmax(float* mx, const float* v)
{
	mx[0] = dtMax(mx[0], v[0]);
	mx[1] = dtMax(mx[1], v[1]);
	mx[2] = dtMax(mx[2], v[2]);
}
/// Determines if two axis-aligned bounding boxes overlap.
///  @param[in]		amin	Minimum bounds of box A. [(x, y, z)]
///  @param[in]		amax	Maximum bounds of box A. [(x, y, z)]
///  @param[in]		bmin	Minimum bounds of box B. [(x, y, z)]
///  @param[in]		bmax	Maximum bounds of box B. [(x, y, z)]
/// @return True if the two AABB's overlap.
/// @see dtOverlapBounds
inline bool dtOverlapQuantBounds(const unsigned short amin[3], const unsigned short amax[3],
	const unsigned short bmin[3], const unsigned short bmax[3])
{
	bool overlap = true;
	overlap = (amin[0] > bmax[0] || amax[0] < bmin[0]) ? false : overlap;
	overlap = (amin[1] > bmax[1] || amax[1] < bmin[1]) ? false : overlap;
	overlap = (amin[2] > bmax[2] || amax[2] < bmin[2]) ? false : overlap;
	return overlap;
}

// Returns true of status is failure.
inline bool dtStatusFailed(dtStatus status)
{
	return (status & DT_FAILURE) != 0;
}

/// A memory allocation function.
//  @param[in]		size			The size, in bytes of memory, to allocate.
//  @param[in]		rcAllocHint	A hint to the allocator on how long the memory is expected to be in use.
//  @return A pointer to the beginning of the allocated memory block, or null if the allocation failed.
///  @see dtAllocSetCustom
typedef void* (dtAllocFunc)(size_t size, dtAllocHint hint);

/// Determines if two axis-aligned bounding boxes overlap.
///  @param[in]		amin	Minimum bounds of box A. [(x, y, z)]
///  @param[in]		amax	Maximum bounds of box A. [(x, y, z)]
///  @param[in]		bmin	Minimum bounds of box B. [(x, y, z)]
///  @param[in]		bmax	Maximum bounds of box B. [(x, y, z)]
/// @return True if the two AABB's overlap.
/// @see dtOverlapQuantBounds
inline bool dtOverlapBounds(const float* amin, const float* amax,
	const float* bmin, const float* bmax)
{
	bool overlap = true;
	overlap = (amin[0] > bmax[0] || amax[0] < bmin[0]) ? false : overlap;
	overlap = (amin[1] > bmax[1] || amax[1] < bmin[1]) ? false : overlap;
	overlap = (amin[2] > bmax[2] || amax[2] < bmin[2]) ? false : overlap;
	return overlap;
}


/// @par
///
/// All points are projected onto the xz-plane, so the y-values are ignored.
inline bool dtPointInPolygon(const float* pt, const float* verts, const int nverts)
{
	// TODO: Replace pnpoly with triArea2D tests?
	int i, j;
	bool c = false;
	for (i = 0, j = nverts - 1; i < nverts; j = i++)
	{
		const float* vi = &verts[i * 3];
		const float* vj = &verts[j * 3];
		if (((vi[2] > pt[2]) != (vj[2] > pt[2])) &&
			(pt[0] < (vj[0] - vi[0]) * (pt[2] - vi[2]) / (vj[2] - vi[2]) + vi[0]))
			c = !c;
	}
	return c;
}

inline unsigned int allocLink(class dtMeshTile* tile)
{
	if (tile->linksFreeList == DT_NULL_LINK)
		return DT_NULL_LINK;
	unsigned int link = tile->linksFreeList;
	tile->linksFreeList = tile->links[link].next;
	return link;
}


inline unsigned int dtNextPow2(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

inline unsigned int dtIlog2(unsigned int v)
{
	unsigned int r;
	unsigned int shift;
	r = (v > 0xffff) << 4; v >>= r;
	shift = (v > 0xff) << 3; v >>= shift; r |= shift;
	shift = (v > 0xf) << 2; v >>= shift; r |= shift;
	shift = (v > 0x3) << 1; v >>= shift; r |= shift;
	r |= (v >> 1);
	return r;
}

/// Performs a vector copy.
///  @param[out]	dest	The result. [(x, y, z)]
///  @param[in]		a		The vector to copy. [(x, y, z)]
inline void dtVcopy(float* dest, const float* a)
{
	dest[0] = a[0];
	dest[1] = a[1];
	dest[2] = a[2];
}

static void* dtAllocDefault(size_t size, dtAllocHint)
{
	return malloc(size);
}

static void dtFreeDefault(void* ptr)
{
	free(ptr);
}

static dtAllocFunc* sAllocFunc = dtAllocDefault;
static dtFreeFunc* sFreeFunc = dtFreeDefault;


inline float dtDistancePtSegSqr2D(const float* pt, const float* p, const float* q, float& t)
{
	float pqx = q[0] - p[0];
	float pqz = q[2] - p[2];
	float dx = pt[0] - p[0];
	float dz = pt[2] - p[2];
	float d = pqx * pqx + pqz * pqz;
	t = pqx * dx + pqz * dz;
	if (d > 0) t /= d;
	if (t < 0) t = 0;
	else if (t > 1) t = 1;
	dx = p[0] + t * pqx - pt[0];
	dz = p[2] + t * pqz - pt[2];
	return dx * dx + dz * dz;
}


/// Performs a linear interpolation between two vectors. (@p v1 toward @p v2)
///  @param[out]	dest	The result vector. [(x, y, x)]
///  @param[in]		v1		The starting vector.
///  @param[in]		v2		The destination vector.
///	 @param[in]		t		The interpolation factor. [Limits: 0 <= value <= 1.0]
inline void dtVlerp(float* dest, const float* v1, const float* v2, const float t)
{
	dest[0] = v1[0] + (v2[0] - v1[0]) * t;
	dest[1] = v1[1] + (v2[1] - v1[1]) * t;
	dest[2] = v1[2] + (v2[2] - v1[2]) * t;
}

//void dtAllocSetCustom(dtAllocFunc* allocFunc, dtFreeFunc* freeFunc)
//{
//	sAllocFunc = allocFunc ? allocFunc : dtAllocDefault;
//	sFreeFunc = freeFunc ? freeFunc : dtFreeDefault;
//}

inline void* dtAlloc(size_t size, dtAllocHint hint)
{
	return sAllocFunc(size, hint);
}

inline void dtFree(void* ptr)
{
	if (ptr)
		sFreeFunc(ptr);
}

inline int longestAxis(unsigned short x, unsigned short y, unsigned short z)
{
	int	axis = 0;
	unsigned short maxVal = x;
	if (y > maxVal)
	{
		axis = 1;
		maxVal = y;
	}
	if (z > maxVal)
	{
		axis = 2;
	}
	return axis;
}

static void calcExtends(BVItem* items, const int /*nitems*/, const int imin, const int imax,
	unsigned short* bmin, unsigned short* bmax)
{
	bmin[0] = items[imin].bmin[0];
	bmin[1] = items[imin].bmin[1];
	bmin[2] = items[imin].bmin[2];

	bmax[0] = items[imin].bmax[0];
	bmax[1] = items[imin].bmax[1];
	bmax[2] = items[imin].bmax[2];

	for (int i = imin + 1; i < imax; ++i)
	{
		const BVItem& it = items[i];
		if (it.bmin[0] < bmin[0]) bmin[0] = it.bmin[0];
		if (it.bmin[1] < bmin[1]) bmin[1] = it.bmin[1];
		if (it.bmin[2] < bmin[2]) bmin[2] = it.bmin[2];

		if (it.bmax[0] > bmax[0]) bmax[0] = it.bmax[0];
		if (it.bmax[1] > bmax[1]) bmax[1] = it.bmax[1];
		if (it.bmax[2] > bmax[2]) bmax[2] = it.bmax[2];
	}
}


static void subdivide(BVItem* items, int nitems, int imin, int imax, int& curNode, dtBVNode* nodes)
{
	int inum = imax - imin;
	int icur = curNode;

	dtBVNode& node = nodes[curNode++];

	if (inum == 1)
	{
		// Leaf
		node.bmin[0] = items[imin].bmin[0];
		node.bmin[1] = items[imin].bmin[1];
		node.bmin[2] = items[imin].bmin[2];

		node.bmax[0] = items[imin].bmax[0];
		node.bmax[1] = items[imin].bmax[1];
		node.bmax[2] = items[imin].bmax[2];

		node.i = items[imin].i;
	}
	else
	{
		// Split
		calcExtends(items, nitems, imin, imax, node.bmin, node.bmax);

		int	axis = longestAxis(node.bmax[0] - node.bmin[0],
			node.bmax[1] - node.bmin[1],
			node.bmax[2] - node.bmin[2]);

		if (axis == 0)
		{
			// Sort along x-axis
			qsort(items + imin, inum, sizeof(BVItem), compareItemX);
		}
		else if (axis == 1)
		{
			// Sort along y-axis
			qsort(items + imin, inum, sizeof(BVItem), compareItemY);
		}
		else
		{
			// Sort along z-axis
			qsort(items + imin, inum, sizeof(BVItem), compareItemZ);
		}

		int isplit = imin + inum / 2;

		// Left
		subdivide(items, nitems, imin, isplit, curNode, nodes);
		// Right
		subdivide(items, nitems, isplit, imax, curNode, nodes);

		int iescape = curNode - icur;
		// Negative index means escape.
		node.i = -iescape;
	}
}

inline int computeTileHash(int x, int y, const int mask)
{
	const unsigned int h1 = 0x8da6b343; // Large multiplicative constants;
	const unsigned int h2 = 0xd8163841; // here arbitrarily chosen primes
	unsigned int n = h1 * x + h2 * y;
	return (int)(n & mask);
}

inline int dtAlign4(int x) { return (x + 3) & ~3; }

inline int dtOppositeTile(int side) { return (side + 4) & 0x7; }

/// Performs a vector subtraction. (@p v1 - @p v2)
///  @param[out]	dest	The result vector. [(x, y, z)]
///  @param[in]		v1		The base vector. [(x, y, z)]
///  @param[in]		v2		The vector to subtract from @p v1. [(x, y, z)]
inline void dtVsub(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[0] - v2[0];
	dest[1] = v1[1] - v2[1];
	dest[2] = v1[2] - v2[2];
}

/// Performs a vector addition. (@p v1 + @p v2)
///  @param[out]	dest	The result vector. [(x, y, z)]
///  @param[in]		v1		The base vector. [(x, y, z)]
///  @param[in]		v2		The vector to add to @p v1. [(x, y, z)]
inline void dtVadd(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[0] + v2[0];
	dest[1] = v1[1] + v2[1];
	dest[2] = v1[2] + v2[2];
}


inline bool dtClosestHeightPointTriangle(const float* p, const float* a, const float* b, const float* c, float& h)
{
	const float EPS = 1e-6f;
	float v0[3], v1[3], v2[3];

	dtVsub(v0, c, a);
	dtVsub(v1, b, a);
	dtVsub(v2, p, a);

	// Compute scaled barycentric coordinates
	float denom = v0[0] * v1[2] - v0[2] * v1[0];
	if (fabsf(denom) < EPS)
		return false;

	float u = v1[2] * v2[0] - v1[0] * v2[2];
	float v = v0[0] * v2[2] - v0[2] * v2[0];

	if (denom < 0) {
		denom = -denom;
		u = -u;
		v = -v;
	}

	// If point lies inside the triangle, return interpolated ycoord.
	if (u >= 0.0f && v >= 0.0f && (u + v) <= denom) {
		h = a[1] + (v0[1] * u + v1[1] * v) / denom;
		return true;
	}
	return false;
}

/// Derives the square of the scalar length of the vector. (len * len)
///  @param[in]		v The vector. [(x, y, z)]
/// @return The square of the scalar length of the vector.
inline float dtVlenSqr(const float* v)
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

inline bool overlapSlabs(const float* amin, const float* amax,
	const float* bmin, const float* bmax,
	const float px, const float py)
{
	// Check for horizontal overlap.
	// The segment is shrunken a little so that slabs which touch
	// at end points are not connected.
	const float minx = dtMax(amin[0] + px, bmin[0] + px);
	const float maxx = dtMin(amax[0] - px, bmax[0] - px);
	if (minx > maxx)
		return false;

	// Check vertical overlap.
	const float ad = (amax[1] - amin[1]) / (amax[0] - amin[0]);
	const float ak = amin[1] - ad * amin[0];
	const float bd = (bmax[1] - bmin[1]) / (bmax[0] - bmin[0]);
	const float bk = bmin[1] - bd * bmin[0];
	const float aminy = ad * minx + ak;
	const float amaxy = ad * maxx + ak;
	const float bminy = bd * minx + bk;
	const float bmaxy = bd * maxx + bk;
	const float dmin = bminy - aminy;
	const float dmax = bmaxy - amaxy;

	// Crossing segments always overlap.
	if (dmin * dmax < 0)
		return true;

	// Check for overlap at endpoints.
	const float thr = dtSqr(py * 2);
	if (dmin * dmin <= thr || dmax * dmax <= thr)
		return true;

	return false;
}

static void calcSlabEndPoints(const float* va, const float* vb, float* bmin, float* bmax, const int side)
{
	if (side == 0 || side == 4)
	{
		if (va[2] < vb[2])
		{
			bmin[0] = va[2];
			bmin[1] = va[1];
			bmax[0] = vb[2];
			bmax[1] = vb[1];
		}
		else
		{
			bmin[0] = vb[2];
			bmin[1] = vb[1];
			bmax[0] = va[2];
			bmax[1] = va[1];
		}
	}
	else if (side == 2 || side == 6)
	{
		if (va[0] < vb[0])
		{
			bmin[0] = va[0];
			bmin[1] = va[1];
			bmax[0] = vb[0];
			bmax[1] = vb[1];
		}
		else
		{
			bmin[0] = vb[0];
			bmin[1] = vb[1];
			bmax[0] = va[0];
			bmax[1] = va[1];
		}
	}
}

inline void dtSwapByte(unsigned char* a, unsigned char* b)
{
	unsigned char tmp = *a;
	*a = *b;
	*b = tmp;
}

inline void dtSwapEndian(unsigned short* v)
{
	unsigned char* x = (unsigned char*)v;
	dtSwapByte(x + 0, x + 1);
}

inline void dtSwapEndian(short* v)
{
	unsigned char* x = (unsigned char*)v;
	dtSwapByte(x + 0, x + 1);
}

inline void dtSwapEndian(unsigned int* v)
{
	unsigned char* x = (unsigned char*)v;
	dtSwapByte(x + 0, x + 3); dtSwapByte(x + 1, x + 2);
}

inline void dtSwapEndian(int* v)
{
	unsigned char* x = (unsigned char*)v;
	dtSwapByte(x + 0, x + 3); dtSwapByte(x + 1, x + 2);
}

inline void dtSwapEndian(float* v)
{
	unsigned char* x = (unsigned char*)v;
	dtSwapByte(x + 0, x + 3); dtSwapByte(x + 1, x + 2);
}

inline void freeLink(dtMeshTile* tile, unsigned int link)
{
	tile->links[link].next = tile->linksFreeList;
	tile->linksFreeList = link;
}

static float getSlabCoord(const float* va, const int side)
{
	if (side == 0 || side == 4)
		return va[0];
	else if (side == 2 || side == 6)
		return va[2];
	return 0;
}

/// Get flags for edge in detail triangle.
/// @param[in]	triFlags		The flags for the triangle (last component of detail vertices above).
/// @param[in]	edgeIndex		The index of the first vertex of the edge. For instance, if 0,
///								returns flags for edge AB.
inline int dtGetDetailTriEdgeFlags(unsigned char triFlags, int edgeIndex)
{
	return (triFlags >> (edgeIndex * 2)) & 0x3;
}

/// Returns the distance between two points.
///  @param[in]		v1	A point. [(x, y, z)]
///  @param[in]		v2	A point. [(x, y, z)]
/// @return The distance between the two points.
inline float dtVdist(const float* v1, const float* v2)
{
	const float dx = v2[0] - v1[0];
	const float dy = v2[1] - v1[1];
	const float dz = v2[2] - v1[2];
	return dtMathSqrtf(dx * dx + dy * dy + dz * dz);
}


/// Checks that the specified vector's components are all finite.
///  @param[in]		v	A point. [(x, y, z)]
/// @return True if all of the point's components are finite, i.e. not NaN
/// or any of the infinities.
inline bool dtVisfinite(const float* v)
{
	bool result =
		dtMathIsfinite(v[0]) &&
		dtMathIsfinite(v[1]) &&
		dtMathIsfinite(v[2]);

	return result;
}

/// Clamps the value to the specified range.
///  @param[in]		v	The value to clamp.
///  @param[in]		mn	The minimum permitted return value.
///  @param[in]		mx	The maximum permitted return value.
///  @return The value, clamped to the specified range.
template<class T> inline T dtClamp(T v, T mn, T mx) { return v < mn ? mn : (v > mx ? mx : v); }

template<typename TypeToRetrieveAs>
TypeToRetrieveAs* dtGetThenAdvanceBufferPointer(const unsigned char*& buffer, const size_t distanceToAdvance)
{
	TypeToRetrieveAs* returnPointer = reinterpret_cast<TypeToRetrieveAs*>(buffer);
	buffer += distanceToAdvance;
	return returnPointer;
}


template<typename TypeToRetrieveAs>
TypeToRetrieveAs* dtGetThenAdvanceBufferPointer(unsigned char*& buffer, const size_t distanceToAdvance)
{
	TypeToRetrieveAs* returnPointer = reinterpret_cast<TypeToRetrieveAs*>(buffer);
	buffer += distanceToAdvance;
	return returnPointer;
}

namespace
{
	template<bool onlyBoundary>
	void closestPointOnDetailEdges(const dtMeshTile* tile, const dtPoly* poly, const float* pos, float* closest)
	{
		const unsigned int ip = (unsigned int)(poly - tile->polys);
		const dtPolyDetail* pd = &tile->detailMeshes[ip];

		float dmin = FLT_MAX;
		float tmin = 0;
		const float* pmin = 0;
		const float* pmax = 0;

		for (int i = 0; i < pd->triCount; i++)
		{
			const unsigned char* tris = &tile->detailTris[(pd->triBase + i) * 4];
			const int ANY_BOUNDARY_EDGE =
				(DT_DETAIL_EDGE_BOUNDARY << 0) |
				(DT_DETAIL_EDGE_BOUNDARY << 2) |
				(DT_DETAIL_EDGE_BOUNDARY << 4);
			if (onlyBoundary && (tris[3] & ANY_BOUNDARY_EDGE) == 0)
				continue;

			const float* v[3];
			for (int j = 0; j < 3; ++j)
			{
				if (tris[j] < poly->vertCount)
					v[j] = &tile->verts[poly->verts[tris[j]] * 3];
				else
					v[j] = &tile->detailVerts[(pd->vertBase + (tris[j] - poly->vertCount)) * 3];
			}

			for (int k = 0, j = 2; k < 3; j = k++)
			{
				if ((dtGetDetailTriEdgeFlags(tris[3], j) & DT_DETAIL_EDGE_BOUNDARY) == 0 &&
					(onlyBoundary || tris[j] < tris[k]))
				{
					// Only looking at boundary edges and this is internal, or
					// this is an inner edge that we will see again or have already seen.
					continue;
				}

				float t;
				float d = dtDistancePtSegSqr2D(pos, v[j], v[k], t);
				if (d < dmin)
				{
					dmin = d;
					tmin = t;
					pmin = v[j];
					pmax = v[k];
				}
			}
		}

		dtVlerp(closest, pmin, pmax, tmin);
	}
}

static int createBVTree(dtNavMeshCreateParams* params, dtBVNode* nodes, int /*nnodes*/)
{
	// Build tree
	float quantFactor = 1 / params->cs;
	BVItem* items = (BVItem*)dtAlloc(sizeof(BVItem) * params->polyCount, DT_ALLOC_TEMP);
	for (int i = 0; i < params->polyCount; i++)
	{
		BVItem& it = items[i];
		it.i = i;
		// Calc polygon bounds. Use detail meshes if available.
		if (params->detailMeshes)
		{
			int vb = (int)params->detailMeshes[i * 4 + 0];
			int ndv = (int)params->detailMeshes[i * 4 + 1];
			float bmin[3];
			float bmax[3];

			const float* dv = &params->detailVerts[vb * 3];
			dtVcopy(bmin, dv);
			dtVcopy(bmax, dv);

			for (int j = 1; j < ndv; j++)
			{
				dtVmin(bmin, &dv[j * 3]);
				dtVmax(bmax, &dv[j * 3]);
			}

			// BV-tree uses cs for all dimensions
			it.bmin[0] = (unsigned short)dtClamp((int)((bmin[0] - params->bmin[0]) * quantFactor), 0, 0xffff);
			it.bmin[1] = (unsigned short)dtClamp((int)((bmin[1] - params->bmin[1]) * quantFactor), 0, 0xffff);
			it.bmin[2] = (unsigned short)dtClamp((int)((bmin[2] - params->bmin[2]) * quantFactor), 0, 0xffff);

			it.bmax[0] = (unsigned short)dtClamp((int)((bmax[0] - params->bmin[0]) * quantFactor), 0, 0xffff);
			it.bmax[1] = (unsigned short)dtClamp((int)((bmax[1] - params->bmin[1]) * quantFactor), 0, 0xffff);
			it.bmax[2] = (unsigned short)dtClamp((int)((bmax[2] - params->bmin[2]) * quantFactor), 0, 0xffff);
		}
		else
		{
			const unsigned short* p = &params->polys[i * params->nvp * 2];
			it.bmin[0] = it.bmax[0] = params->verts[p[0] * 3 + 0];
			it.bmin[1] = it.bmax[1] = params->verts[p[0] * 3 + 1];
			it.bmin[2] = it.bmax[2] = params->verts[p[0] * 3 + 2];

			for (int j = 1; j < params->nvp; ++j)
			{
				if (p[j] == MESH_NULL_IDX) break;
				unsigned short x = params->verts[p[j] * 3 + 0];
				unsigned short y = params->verts[p[j] * 3 + 1];
				unsigned short z = params->verts[p[j] * 3 + 2];

				if (x < it.bmin[0]) it.bmin[0] = x;
				if (y < it.bmin[1]) it.bmin[1] = y;
				if (z < it.bmin[2]) it.bmin[2] = z;

				if (x > it.bmax[0]) it.bmax[0] = x;
				if (y > it.bmax[1]) it.bmax[1] = y;
				if (z > it.bmax[2]) it.bmax[2] = z;
			}
			// Remap y
			it.bmin[1] = (unsigned short)dtMathFloorf((float)it.bmin[1] * params->ch / params->cs);
			it.bmax[1] = (unsigned short)dtMathCeilf((float)it.bmax[1] * params->ch / params->cs);
		}
	}

	int curNode = 0;
	subdivide(items, params->polyCount, 0, params->polyCount, curNode, nodes);

	dtFree(items);

	return curNode;
}

static char* parseRow(char* buf, char* bufEnd, char* row, int len)
{
	bool start = true;
	bool done = false;
	int n = 0;
	while (!done && buf < bufEnd)
	{
		char c = *buf;
		buf++;
		// multirow
		switch (c)
		{
		case '\\':
			break;
		case '\n':
			if (start) break;
			done = true;
			break;
		case '\r':
			break;
		case '\t':
		case ' ':
			if (start) break;
			// else falls through
		default:
			start = false;
			row[n++] = c;
			if (n >= len - 1)
				done = true;
			break;
		}
	}
	row[n] = '\0';
	return buf;
}

static int parseFace(char* row, int* data, int n, int vcnt)
{
	int j = 0;
	while (*row != '\0')
	{
		// Skip initial white space
		while (*row != '\0' && (*row == ' ' || *row == '\t'))
			row++;
		char* s = row;
		// Find vertex delimiter and terminated the string there for conversion.
		while (*row != '\0' && *row != ' ' && *row != '\t')
		{
			if (*row == '/') *row = '\0';
			row++;
		}
		if (*s == '\0')
			continue;
		int vi = atoi(s);
		data[j++] = vi < 0 ? vi + vcnt : vi - 1;
		if (j >= n) return j;
	}
	return j;
}

/// @name General helper functions
/// @{

/// Swaps the values of the two parameters.
/// @param[in,out]	a	Value A
/// @param[in,out]	b	Value B
template<class T> inline void rcSwap(T& a, T& b) { T t = a; a = b; b = t; }

/// Returns the minimum of two values.
/// @param[in]		a	Value A
/// @param[in]		b	Value B
/// @return The minimum of the two values.
template<class T> inline T rcMin(T a, T b) { return a < b ? a : b; }

/// Returns the maximum of two values.
/// @param[in]		a	Value A
/// @param[in]		b	Value B
/// @return The maximum of the two values.
template<class T> inline T rcMax(T a, T b) { return a > b ? a : b; }

/// Returns the absolute value.
/// @param[in]		a	The value.
/// @return The absolute value of the specified value.
template<class T> inline T rcAbs(T a) { return a < 0 ? -a : a; }

/// Returns the square of the value.
/// @param[in]		a	The value.
/// @return The square of the value.
template<class T> inline T rcSqr(T a) { return a * a; }

/// Clamps the value to the specified range.
/// @param[in]		value			The value to clamp.
/// @param[in]		minInclusive	The minimum permitted return value.
/// @param[in]		maxInclusive	The maximum permitted return value.
/// @return The value, clamped to the specified range.
template<class T> inline T rcClamp(T value, T minInclusive, T maxInclusive)
{
	return value < minInclusive ? minInclusive : (value > maxInclusive ? maxInclusive : value);
}

/// Returns the square root of the value.
///  @param[in]		x	The value.
///  @return The square root of the vlaue.
float rcSqrt(float x);

/// @}
/// @name Vector helper functions.
/// @{

/// Derives the cross product of two vectors. (@p v1 x @p v2)
/// @param[out]		dest	The cross product. [(x, y, z)]
/// @param[in]		v1		A Vector [(x, y, z)]
/// @param[in]		v2		A vector [(x, y, z)]
inline void rcVcross(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[1] * v2[2] - v1[2] * v2[1];
	dest[1] = v1[2] * v2[0] - v1[0] * v2[2];
	dest[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

/// Derives the dot product of two vectors. (@p v1 . @p v2)
/// @param[in]		v1	A Vector [(x, y, z)]
/// @param[in]		v2	A vector [(x, y, z)]
/// @return The dot product.
inline float rcVdot(const float* v1, const float* v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

/// Performs a scaled vector addition. (@p v1 + (@p v2 * @p s))
/// @param[out]		dest	The result vector. [(x, y, z)]
/// @param[in]		v1		The base vector. [(x, y, z)]
/// @param[in]		v2		The vector to scale and add to @p v1. [(x, y, z)]
/// @param[in]		s		The amount to scale @p v2 by before adding to @p v1.
inline void rcVmad(float* dest, const float* v1, const float* v2, const float s)
{
	dest[0] = v1[0] + v2[0] * s;
	dest[1] = v1[1] + v2[1] * s;
	dest[2] = v1[2] + v2[2] * s;
}

/// Performs a vector addition. (@p v1 + @p v2)
/// @param[out]		dest	The result vector. [(x, y, z)]
/// @param[in]		v1		The base vector. [(x, y, z)]
/// @param[in]		v2		The vector to add to @p v1. [(x, y, z)]
inline void rcVadd(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[0] + v2[0];
	dest[1] = v1[1] + v2[1];
	dest[2] = v1[2] + v2[2];
}

/// Performs a vector subtraction. (@p v1 - @p v2)
/// @param[out]		dest	The result vector. [(x, y, z)]
/// @param[in]		v1		The base vector. [(x, y, z)]
/// @param[in]		v2		The vector to subtract from @p v1. [(x, y, z)]
inline void rcVsub(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[0] - v2[0];
	dest[1] = v1[1] - v2[1];
	dest[2] = v1[2] - v2[2];
}

/// Selects the minimum value of each element from the specified vectors.
/// @param[in,out]	mn	A vector.  (Will be updated with the result.) [(x, y, z)]
/// @param[in]		v	A vector. [(x, y, z)]
inline void rcVmin(float* mn, const float* v)
{
	mn[0] = rcMin(mn[0], v[0]);
	mn[1] = rcMin(mn[1], v[1]);
	mn[2] = rcMin(mn[2], v[2]);
}

/// Selects the maximum value of each element from the specified vectors.
/// @param[in,out]	mx	A vector.  (Will be updated with the result.) [(x, y, z)]
/// @param[in]		v	A vector. [(x, y, z)]
inline void rcVmax(float* mx, const float* v)
{
	mx[0] = rcMax(mx[0], v[0]);
	mx[1] = rcMax(mx[1], v[1]);
	mx[2] = rcMax(mx[2], v[2]);
}

/// Performs a vector copy.
/// @param[out]		dest	The result. [(x, y, z)]
/// @param[in]		v		The vector to copy. [(x, y, z)]
inline void rcVcopy(float* dest, const float* v)
{
	dest[0] = v[0];
	dest[1] = v[1];
	dest[2] = v[2];
}

/// Returns the distance between two points.
/// @param[in]		v1	A point. [(x, y, z)]
/// @param[in]		v2	A point. [(x, y, z)]
/// @return The distance between the two points.
inline float rcVdist(const float* v1, const float* v2)
{
	float dx = v2[0] - v1[0];
	float dy = v2[1] - v1[1];
	float dz = v2[2] - v1[2];
	return rcSqrt(dx * dx + dy * dy + dz * dz);
}

/// Returns the square of the distance between two points.
/// @param[in]		v1	A point. [(x, y, z)]
/// @param[in]		v2	A point. [(x, y, z)]
/// @return The square of the distance between the two points.
inline float rcVdistSqr(const float* v1, const float* v2)
{
	float dx = v2[0] - v1[0];
	float dy = v2[1] - v1[1];
	float dz = v2[2] - v1[2];
	return dx * dx + dy * dy + dz * dz;
}

/// Normalizes the vector.
/// @param[in,out]	v	The vector to normalize. [(x, y, z)]
inline void rcVnormalize(float* v)
{
	float d = 1.0f / rcSqrt(rcSqr(v[0]) + rcSqr(v[1]) + rcSqr(v[2]));
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
}

static bool intersectSegmentTriangle(const float* sp, const float* sq,
	const float* a, const float* b, const float* c,
	float& t)
{
	float v, w;
	float ab[3], ac[3], qp[3], ap[3], norm[3], e[3];
	rcVsub(ab, b, a);
	rcVsub(ac, c, a);
	rcVsub(qp, sp, sq);

	// Compute triangle normal. Can be precalculated or cached if
	// intersecting multiple segments against the same triangle
	rcVcross(norm, ab, ac);

	// Compute denominator d. If d <= 0, segment is parallel to or points
	// away from triangle, so exit early
	float d = rcVdot(qp, norm);
	if (d <= 0.0f) return false;

	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle
	rcVsub(ap, sp, a);
	t = rcVdot(ap, norm);
	if (t < 0.0f) return false;
	if (t > d) return false; // For segment; exclude this code line for a ray test

	// Compute barycentric coordinate components and test if within bounds
	rcVcross(e, qp, ap);
	v = rcVdot(ac, e);
	if (v < 0.0f || v > d) return false;
	w = -rcVdot(ab, e);
	if (w < 0.0f || v + w > d) return false;

	// Segment/ray intersects triangle. Perform delayed division
	t /= d;

	return true;
}

/// Checks that the specified vector's 2D components are finite.
///  @param[in]		v	A point. [(x, y, z)]
inline bool dtVisfinite2D(const float* v)
{
	bool result = dtMathIsfinite(v[0]) && dtMathIsfinite(v[2]);
	return result;
}

/// Returns the square of the distance between two points.
///  @param[in]		v1	A point. [(x, y, z)]
///  @param[in]		v2	A point. [(x, y, z)]
/// @return The square of the distance between the two points.
inline float dtVdistSqr(const float* v1, const float* v2)
{
	const float dx = v2[0] - v1[0];
	const float dy = v2[1] - v1[1];
	const float dz = v2[2] - v1[2];
	return dx * dx + dy * dy + dz * dz;
}

/// Performs a 'sloppy' colocation check of the specified points.
///  @param[in]		p0	A point. [(x, y, z)]
///  @param[in]		p1	A point. [(x, y, z)]
/// @return True if the points are considered to be at the same location.
///
/// Basically, this function will return true if the specified points are 
/// close enough to eachother to be considered colocated.
inline bool dtVequal(const float* p0, const float* p1)
{
	static const float thr = dtSqr(1.0f / 16384.0f);
	const float d = dtVdistSqr(p0, p1);
	return d < thr;
}

inline float vperpXZ(const float* a, const float* b) { return a[0] * b[2] - a[2] * b[0]; }


/// @}
/// @name Computational geometry helper functions.
/// @{

/// Derives the signed xz-plane area of the triangle ABC, or the relationship of line AB to point C.
///  @param[in]		a		Vertex A. [(x, y, z)]
///  @param[in]		b		Vertex B. [(x, y, z)]
///  @param[in]		c		Vertex C. [(x, y, z)]
/// @return The signed xz-plane area of the triangle.
inline float dtTriArea2D(const float* a, const float* b, const float* c)
{
	const float abx = b[0] - a[0];
	const float abz = b[2] - a[2];
	const float acx = c[0] - a[0];
	const float acz = c[2] - a[2];
	return acx * abz - abx * acz;
}

/// Sets the vector elements to the specified values.
///  @param[out]	dest	The result vector. [(x, y, z)]
///  @param[in]		x		The x-value of the vector.
///  @param[in]		y		The y-value of the vector.
///  @param[in]		z		The z-value of the vector.
inline void dtVset(float* dest, const float x, const float y, const float z)
{
	dest[0] = x; dest[1] = y; dest[2] = z;
}

/// Derives the xz-plane 2D perp product of the two vectors. (uz*vx - ux*vz)
///  @param[in]		u		The LHV vector [(x, y, z)]
///  @param[in]		v		The RHV vector [(x, y, z)]
/// @return The perp dot product on the xz-plane.
///
/// The vectors are projected onto the xz-plane, so the y-values are ignored.
inline float dtVperp2D(const float* u, const float* v)
{
	return u[2] * v[0] - u[0] * v[2];
}


/// Performs a scaled vector addition. (@p v1 + (@p v2 * @p s))
///  @param[out]	dest	The result vector. [(x, y, z)]
///  @param[in]		v1		The base vector. [(x, y, z)]
///  @param[in]		v2		The vector to scale and add to @p v1. [(x, y, z)]
///  @param[in]		s		The amount to scale @p v2 by before adding to @p v1.
inline void dtVmad(float* dest, const float* v1, const float* v2, const float s)
{
	dest[0] = v1[0] + v2[0] * s;
	dest[1] = v1[1] + v2[1] * s;
	dest[2] = v1[2] + v2[2] * s;
}


/// Normalizes the vector.
///  @param[in,out]	v	The vector to normalize. [(x, y, z)]
inline void dtVnormalize(float* v)
{
	float d = 1.0f / dtMathSqrtf(dtSqr(v[0]) + dtSqr(v[1]) + dtSqr(v[2]));
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
}

/// Scales the vector by the specified value. (@p v * @p t)
///  @param[out]	dest	The result vector. [(x, y, z)]
///  @param[in]		v		The vector to scale. [(x, y, z)]
///  @param[in]		t		The scaling factor.
inline void dtVscale(float* dest, const float* v, const float t)
{
	dest[0] = v[0] * t;
	dest[1] = v[1] * t;
	dest[2] = v[2] * t;
}

/// Derives the dot product of two vectors on the xz-plane. (@p u . @p v)
///  @param[in]		u		A vector [(x, y, z)]
///  @param[in]		v		A vector [(x, y, z)]
/// @return The dot product on the xz-plane.
///
/// The vectors are projected onto the xz-plane, so the y-values are ignored.
inline float dtVdot2D(const float* u, const float* v)
{
	return u[0] * v[0] + u[2] * v[2];
}

static void projectPoly(const float* axis, const float* poly, const int npoly,
	float& rmin, float& rmax)
{
	rmin = rmax = dtVdot2D(axis, &poly[0]);
	for (int i = 1; i < npoly; ++i)
	{
		const float d = dtVdot2D(axis, &poly[i * 3]);
		rmin = dtMin(rmin, d);
		rmax = dtMax(rmax, d);
	}
}

inline bool overlapRange(const float amin, const float amax,
	const float bmin, const float bmax,
	const float eps)
{
	return ((amin + eps) > bmax || (amax - eps) < bmin) ? false : true;
}

/// @par
///
/// All vertices are projected onto the xz-plane, so the y-values are ignored.
