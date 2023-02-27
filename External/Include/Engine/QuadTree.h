#pragma once
#include "QuadNode.h"

class QuadTree
{
private:
	QuadNode* rootNode;
	int		  minSize;

public:
	QuadTree(int maxSize, int minSize);
	~QuadTree();

	void InsertObjectAABB(AABB aabb);
	void CollisionInspection(AABB aabb);

private:
	QuadNode* FindAABBIncludedNode(AABB aabb, QuadNode* currentNode);
	void	  CollisionInspectionToParrent(AABB aabb, QuadNode* currentNode);
	void	  CollisionInspectionToChild(AABB aabb, QuadNode* currentNode);
};