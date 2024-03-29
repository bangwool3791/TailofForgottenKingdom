//***************************************************************************************
// HillsDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs the calculations for the wave simulation.  After the simulation has been
// updated, the client must copy the current solution into vertex buffers for rendering.
// This class only does the calculations, it does not do any drawing.
//***************************************************************************************
#pragma once

#include <Windows.h>

class Waves
{
public:
	Waves();
	~Waves();

	UINT RowCount()const;
	UINT ColumnCount()const;
	UINT VertexCount()const;
	UINT TriangleCount()const;

	// Returns the solution at the ith grid point.
	const Vec3& operator[](int i)const { return mCurrSolution[i]; }

	void Init(UINT m, UINT n, float dx, float dt, float speed, float damping);
	void Update(float dt);
	void Disturb(UINT i, UINT j, float magnitude);

private:
	UINT mNumRows;
	UINT mNumCols;

	UINT mVertexCount;
	UINT mTriangleCount;

	// Simulation constants we can precompute.
	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	vector<Vec3> mPrevSolution;
	vector<Vec3> mCurrSolution;
};
