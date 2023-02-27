#pragma once

union COLLIDER_ID
{
	struct {
		UINT LEFT_ID;
		UINT RIGHT_ID;
	};
	ULONGLONG ID;
};

class CLevel;
class CCollider2D;
class CCollider3D;

class CCollisionMgr
	:public CSingleton<CCollisionMgr>
{
private:
	UINT m_matrix[MAX_LAYER];
	map<ULONGLONG, bool> m_mapColInfo;

public:
	void tick();

private:
	void CollisionBtwLayer(CLevel* _pCurScene, int _iLeft, int _iRight);
	void CollisionBtwCollider(CCollider3D* _pLeft, CCollider3D* _pRight);
	bool IsCollision(CCollider3D* _pLeft, CCollider3D* _pRight);
public:
	void clear()
	{
		memset(m_matrix, 0, sizeof(WORD) * MAX_LAYER);
		m_mapColInfo.clear();
	}

	void CollisionLayerCheck(int _iLeft, int _iRight);
	void CollisionLayerRelease(int _iLeft, int _iRight);
	void SaveToFile(FILE* _pFIle);
	void LoadToFile(FILE* _pFIle);
private:
	CCollisionMgr();
	virtual ~CCollisionMgr();
	friend class CSingleton<CCollisionMgr>;
};