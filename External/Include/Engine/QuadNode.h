#pragma once
#include "AABB.h"

class QuadNode
{
private:
	QuadNode* parentNode;
	array<QuadNode*, 4> childNodes;

	AABB aabb;
	vector<AABB> includedObjectAABB;
	int includedObjectAABBCount;
	bool isHaveChlids;

public:
	QuadNode(AABB _aabb, QuadNode* _ParentNode);
	~QuadNode();

public:
	void InsertObjectAABB(AABB _aabb);
	void SplitBy4();
	bool ObjectAABBContains(AABB _aabb);

	QuadNode* GetParentNode() { return parentNode; }
	QuadNode* GetChildNode(int _index) { return childNodes[_index]; }
	AABB GetAABB() { return aabb; }
	int IncludedObjectAABBCount() { return includedObjectAABBCount; }
	AABB IncludedObjectAABB(int i) { return includedObjectAABB[i]; }
	bool IsHaveChilds() { return isHaveChlids; }
};