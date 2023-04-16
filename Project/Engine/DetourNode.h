#pragma once

#include "GlobalNavi.h"

class dtNodePool
{
public:
	dtNodePool(int maxNodes, int hashSize);
	~dtNodePool();
	void clear();

	// Get a dtNode by ref and extra state information. If there is none then - allocate
	// There can be more than one node for the same polyRef but with different extra state information
	dtNode* getNode(dtPolyRef id, unsigned char state = 0);
	dtNode* findNode(dtPolyRef id, unsigned char state);
	unsigned int findNodes(dtPolyRef id, dtNode** nodes, const int maxNodes);

	inline unsigned int getNodeIdx(const dtNode* node) const
	{
		if (!node) return 0;
		return (unsigned int)(node - m_nodes) + 1;
	}

	inline dtNode* getNodeAtIdx(unsigned int idx)
	{
		if (!idx) return 0;
		return &m_nodes[idx - 1];
	}

	inline const dtNode* getNodeAtIdx(unsigned int idx) const
	{
		if (!idx) return 0;
		return &m_nodes[idx - 1];
	}

	inline int getMemUsed() const
	{
		return sizeof(*this) +
			sizeof(dtNode) * m_maxNodes +
			sizeof(dtNodeIndex) * m_maxNodes +
			sizeof(dtNodeIndex) * m_hashSize;
	}

	inline int getMaxNodes() const { return m_maxNodes; }

	inline int getHashSize() const { return m_hashSize; }
	inline dtNodeIndex getFirst(int bucket) const { return m_first[bucket]; }
	inline dtNodeIndex getNext(int i) const { return m_next[i]; }
	inline int getNodeCount() const { return m_nodeCount; }

private:
	// Explicitly disabled copy constructor and copy assignment operator.
	dtNodePool(const dtNodePool&);
	dtNodePool& operator=(const dtNodePool&);

	dtNode* m_nodes;
	dtNodeIndex* m_first;
	dtNodeIndex* m_next;
	const int m_maxNodes;
	const int m_hashSize;
	int m_nodeCount;
};


class dtNodeQueue
{
public:
	dtNodeQueue(int n);
	~dtNodeQueue();

	inline void clear() { m_size = 0; }

	inline dtNode* top() { return m_heap[0]; }

	inline dtNode* pop()
	{
		dtNode* result = m_heap[0];
		m_size--;
		trickleDown(0, m_heap[m_size]);
		return result;
	}

	inline void push(dtNode* node)
	{
		m_size++;
		bubbleUp(m_size - 1, node);
	}

	inline void modify(dtNode* node)
	{
		for (int i = 0; i < m_size; ++i)
		{
			if (m_heap[i] == node)
			{
				bubbleUp(i, node);
				return;
			}
		}
	}

	inline bool empty() const { return m_size == 0; }

	inline int getMemUsed() const
	{
		return sizeof(*this) +
			sizeof(dtNode*) * (m_capacity + 1);
	}

	inline int getCapacity() const { return m_capacity; }

private:
	// Explicitly disabled copy constructor and copy assignment operator.
	dtNodeQueue(const dtNodeQueue&);
	dtNodeQueue& operator=(const dtNodeQueue&);

	void bubbleUp(int i, dtNode* node);
	void trickleDown(int i, dtNode* node);

	dtNode** m_heap;
	const int m_capacity;
	int m_size;
};

