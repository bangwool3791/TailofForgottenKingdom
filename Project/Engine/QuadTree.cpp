#include "pch.h"
#include "QuadTree.h"


QuadTree::QuadTree(int _maxSize, int _minSize)
{
	rootNode = new QuadNode{ AABB{0, _maxSize, 0, _maxSize}, nullptr };
	minSize = _minSize;
}

QuadTree::~QuadTree()
{
	if (rootNode)
	{
		delete rootNode;
	}
}

void QuadTree::InsertObjectAABB(AABB aabb)
{
	QuadNode* currentNode = rootNode;

	while (true)
	{
		//AABB 객체의 폭/2가 QuadTree 최소 사이즈보다 작다면
		/*
		* AABB 객체는 자식을 만들 수록, 폭이 좁아지고, AABB 검출 최소 사이즈보다 작아지면 반복문을 멈춘다.
		*/
		if (currentNode->GetAABB().Width() /2 < minSize)
		{
			break;
		}

		if (currentNode->GetAABB().IsIncluded(aabb))
		{
			if (!currentNode->IsHaveChilds())
			{
				currentNode->SplitBy4();
			}

			int notIncludeCount = 0;

			for (int i{ 0 }; i < 4; ++i)
			{
				if (currentNode->GetChildNode(i)->GetAABB().IsIncluded(aabb))
				{
					currentNode = currentNode->GetChildNode(i);
				}
				notIncludeCount++;
			}

			if (notIncludeCount == 4)
				break;
		}
	}
	currentNode->InsertObjectAABB(aabb);
}

void QuadTree::CollisionInspection(AABB aabb)
{
	QuadNode* IncludedNode = FindAABBIncludedNode(aabb, rootNode);

	// 부모 노드들에 대한 충돌 검사
	CollisionInspectionToParrent(aabb, IncludedNode);
	// 자식 노드들에 대한 충돌 검사
	CollisionInspectionToChild(aabb, IncludedNode);

	cout << endl;
}

QuadNode* QuadTree::FindAABBIncludedNode(AABB aabb, QuadNode* currentNode)
{
	if (currentNode->ObjectAABBContains(aabb))
	{
		return currentNode;
	}

	QuadNode* IncludeNode = nullptr;

	if (currentNode->IsHaveChilds())
	{
		for (int i{ 0 }; i < 4 && IncludeNode == nullptr; ++i)
		{
			IncludeNode = FindAABBIncludedNode(aabb, currentNode->GetChildNode(i));
		}
	}
	return IncludeNode;
}


void QuadTree::CollisionInspectionToParrent(AABB aabb, QuadNode* currentNode)
{
	if (currentNode == nullptr)
		return;

	for (int i{ 0 }; i < currentNode->IncludedObjectAABBCount(); ++i)
	{
		if (aabb.InstanceID == currentNode->GetAABB().InstanceID)
			continue;

		if (aabb.IsIntersect(currentNode->IncludedObjectAABB(i)))
		{
			cout << "Object" << aabb.InstanceID << " 와 Object" << currentNode->IncludedObjectAABB(i).InstanceID << " 는 충돌하였습니다." << endl;
		}
	}

	CollisionInspectionToParrent(aabb, currentNode->GetParentNode());
}

void QuadTree::CollisionInspectionToChild(AABB aabb, QuadNode* currentNode)
{
	if (!currentNode->IsHaveChilds())
		return;

	for(int i {}; i < currentNode->IncludedObjectAABBCount(); ++i)
	{
		QuadNode* childNode = currentNode->GetChildNode(i);

		for (int j{}; j < childNode->IncludedObjectAABBCount(); ++j)
		{
			if (aabb.IsIntersect(childNode->IncludedObjectAABB(i)))
			{
				cout << "Object" << aabb.InstanceID << " 와 Object" << childNode->IncludedObjectAABB(i).InstanceID << " 는 충돌하였습니다." << endl;
			}
			CollisionInspectionToChild(aabb, childNode);
		}
	}
}
