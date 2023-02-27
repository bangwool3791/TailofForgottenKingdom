#pragma once
#include "pch.h"

// Interface to the debug renderer

// Interface to the debug renderer

namespace debug_renderer
{
	void add_line(Vec3 point_a, Vec3 point_b, Vec4 color_a, Vec4 color_b);

	inline void add_line(Vec3 p, Vec3 q, Vec4 color) { add_line(p, q, color, color); }

	void clear_lines();

	const colored_vertex* get_line_verts();

	size_t get_line_vert_count();

	size_t get_line_vert_capacity();
}
