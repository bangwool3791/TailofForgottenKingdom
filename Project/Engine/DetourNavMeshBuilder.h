//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef DETOURNAVMESHBUILDER_H
#define DETOURNAVMESHBUILDER_H

#include "globalNavi.h"

/// Builds navigation mesh tile data from the provided tile creation data.
/// @ingroup detour
///  @param[in]		params		Tile creation data.
///  @param[out]	outData		The resulting tile data.
///  @param[out]	outDataSize	The size of the tile data array.
/// @return True if the tile data was successfully created.
bool dtCreateNavMeshData(dtNavMeshCreateParams* params, unsigned char** outData, int* outDataSize);

/// Swaps the endianess of the tile data's header (#dtMeshHeader).
///  @param[in,out]	data		The tile data array.
///  @param[in]		dataSize	The size of the data array.
bool dtNavMeshHeaderSwapEndian(unsigned char* data, const int dataSize);

/// Swaps endianess of the tile data.
///  @param[in,out]	data		The tile data array.
///  @param[in]		dataSize	The size of the data array.
bool dtNavMeshDataSwapEndian(unsigned char* data, const int dataSize);

#endif // DETOURNAVMESHBUILDER_H

// This section contains detailed documentation for members that don't have
// a source file. It reduces clutter in the main section of the header.

/**

@struct dtNavMeshCreateParams
@par

This structure is used to marshal data between the Recast mesh generation pipeline and Detour navigation components.

See the rcPolyMesh and rcPolyMeshDetail documentation for detailed information related to mesh structure.

Units are usually in voxels (vx) or world units (wu). The units for voxels, grid size, and cell size
are all based on the values of #cs and #ch.

The standard navigation mesh build process is to create tile data using dtCreateNavMeshData, then add the tile
to a navigation mesh using either the dtNavMesh single tile <tt>init()</tt> function or the dtNavMesh::addTile()
function.

@see dtCreateNavMeshData

*/

