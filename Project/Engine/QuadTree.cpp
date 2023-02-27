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
		//AABB ��ü�� ��/2�� QuadTree �ּ� ������� �۴ٸ�
		/*
		* AABB ��ü�� �ڽ��� ���� ����, ���� ��������, AABB ���� �ּ� ������� �۾����� �ݺ����� �����.
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

	// �θ� ���鿡 ���� �浹 �˻�
	CollisionInspectionToParrent(aabb, IncludedNode);
	// �ڽ� ���鿡 ���� �浹 �˻�
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
			cout << "Object" << aabb.InstanceID << " �� Object" << currentNode->IncludedObjectAABB(i).InstanceID << " �� �浹�Ͽ����ϴ�." << endl;
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
				cout << "Object" << aabb.InstanceID << " �� Object" << childNode->IncludedObjectAABB(i).InstanceID << " �� �浹�Ͽ����ϴ�." << endl;
			}
			CollisionInspectionToChild(aabb, childNode);
		}
	}
}
