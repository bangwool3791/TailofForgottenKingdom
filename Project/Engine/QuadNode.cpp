#include "pch.h"
#include "QuadNode.h"

QuadNode::QuadNode(AABB _aabb, QuadNode* _ParentNode)
{
    aabb = _aabb;
    parentNode = _ParentNode;
    includedObjectAABBCount = 0;
    isHaveChlids = false;
}

QuadNode::~QuadNode()
{
    for (auto iter{ childNodes.begin() }; iter < childNodes.end(); ++iter)
    {
        if (*iter)
        {
            delete (*iter);
        }
    }
}

void QuadNode::InsertObjectAABB(AABB _aabb)
{
    includedObjectAABB.push_back(_aabb);
    includedObjectAABBCount++;
}

void QuadNode::SplitBy4()
{
    int CenterX = aabb.X0 + (aabb.X1 - aabb.X0) / 2;
    int CenterY = aabb.Y0 + (aabb.Y1 + aabb.Y0) / 2;

    childNodes[0] = new QuadNode(AABB{ aabb.X0, CenterX, aabb.Y0, CenterY }, this);
    childNodes[1] = new QuadNode(AABB{ CenterX, aabb.X1, aabb.Y0, CenterY }, this);
    childNodes[2] = new QuadNode(AABB{ aabb.X0, CenterX, CenterY, aabb.Y1 }, this);
    childNodes[3] = new QuadNode(AABB{ CenterX, aabb.X0, CenterY, aabb.Y1 }, this);

    isHaveChlids = true;
}


bool QuadNode::ObjectAABBContains(AABB _aabb)
{
    for (auto iter{ childNodes.begin() }; iter < childNodes.end(); ++iter)
    {
        if ((*iter)->aabb.InstanceID == _aabb.InstanceID)
        {
            return true;
        }
    }

    return false;
}
