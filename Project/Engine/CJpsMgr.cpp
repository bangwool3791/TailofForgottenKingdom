#include "pch.h"
#include "CJpsMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"

CJpsMgr::CJpsMgr()
{

}

CJpsMgr::~CJpsMgr()
{

}

void CJpsMgr::Initialize(Ptr<CMesh> _Mesh)
{
	m_pTileObejct = CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(L"LevelTile");
	m_spCollision = std::make_shared<JPSCollision>();
	m_spMonsterCollision = std::make_shared<JPSCollision>();
	if (!m_spCollision)
		throw std::bad_alloc();


	//====================================================
// CREATE MAP 
//====================================================
	m_spCollision->Create(GridWidth, GridHeight);
	m_spMonsterCollision->Create(GridWidth, GridHeight);
	//====================================================
	// Find PATH
	//====================================================
	std::list<JPSCoord> ResultNodes;	// Result for JPS
	//이니셜 라이즈 여기서 
	m_JpsPath.Init(m_spCollision, _Mesh);
	m_JpsMonsterPath.Init(m_spMonsterCollision, _Mesh);
}

const vector<Vec3>& CJpsMgr::Update(Int32 x1, Int32 z1, Int32 x2, Int32 z2)
{
	m_vecResult.clear();
	// Start, End Position (시작점, 도착점)
	Int32 Sx = x1, Sy = z1;
	Int32 Ex = x2, Ey = z2;

	m_JpsPath.Search(Sx, Sy, Ex, Ey, ResultNodes);

	std::string results(GridHeight * (GridWidth + 1) + 1, ' ');

	for (Int32 y = 0; y < GridHeight; y++)
	{
		for (Int32 x = 0; x < GridWidth; x++)
		{
			results[(GridHeight - 1 - y) * (GridWidth + 1) + x] = !m_spCollision->IsCollision(x, y) ? ' ' : '@';
		}
		results[(GridHeight - 1 - y) * (GridWidth + 1) + GridWidth] = '\n';
	}

	if (ResultNodes.size() > 0)
	{
		auto iterS = ResultNodes.begin();
		auto iterE = ResultNodes.end();
		auto iterP = iterS;	++iterS;
		for (; iterS != iterE; iterP = iterS, ++iterS)
		{
			auto& PreCoord = (*iterP);
			auto& CurCoord = (*iterS);

			Int32 x = PreCoord.m_x;
			Int32 y = PreCoord.m_y;
			Int32 dx = core::clamp<Int32>(CurCoord.m_x - PreCoord.m_x, -1, 1);
			Int32 dy = core::clamp<Int32>(CurCoord.m_y - PreCoord.m_y, -1, 1);

			for (Int32 v = y, u = x; ; v += dy, u += dx)
			{
				results[(GridHeight - 1 - v) * (GridWidth + 1) + u] = '#';
				cout << "JPS 좌표 [x] " << u << " [z] " << v << endl;
				if (u  != -1)
					m_vecResult.push_back(m_JpsPath.GetCoord(u, v));

				if (u == CurCoord.m_x && v == CurCoord.m_y)
					break;

				Int32 deltax = core::clamp<Int32>(CurCoord.m_x - u, -1, 1);
				Int32 deltay = core::clamp<Int32>(CurCoord.m_y - v, -1, 1);
				if (deltax != dx || deltay != dy)
				{
					std::cout << "INVALID NODE\n";
					break;
				}
			}
			results[(GridHeight - 1 - CurCoord.m_y) * (GridWidth + 1) + CurCoord.m_x] = '#';
			cout << "JPS 좌표 [x] " << CurCoord.m_x << " [z] " << CurCoord.m_y << endl;

			if (CurCoord.m_x != -1)
				m_vecResult.push_back(m_JpsPath.GetCoord(CurCoord.m_x, CurCoord.m_y));
		}

		size_t start{}, end{};
		bool bCheck = false;
		for (size_t i{}; i < m_vecResult.size() && i + 1 < m_vecResult.size() && i + 2 < m_vecResult.size(); ++i)
		{
			float dx = abs(m_vecResult[i].x - m_vecResult[i + 1].x);
			float dz = abs(m_vecResult[i].z - m_vecResult[i + 2].z);

			if (m_vecResult[i].x == m_vecResult[i + 2].x && dx == TILECX * 0.5 && dz == TILECZ)
			{
				if (!bCheck)
				{
					bCheck = true;
					start = i;
				}
			}
			else if (bCheck && !(m_vecResult[i].x == m_vecResult[i + 2].x && dx == TILECX * 0.5 && dz == TILECZ))
			{
				end = i;
			}
		}
		auto startiter = m_vecResult.begin();
		vector<Vec3>::iterator enditer = m_vecResult.begin();
		vector<Vec3> vecdata{};

		for (size_t i{}; i < start; ++i)
			++startiter;
		for (size_t i{}; i < end; ++i)
			++enditer;
		cout << "A" << endl;
		for (; startiter != enditer;)
		{
			++startiter;
			if (startiter == m_vecResult.end())
				break;
			vecdata.push_back(*startiter);
			if (startiter == m_vecResult.end())
				break;
			++startiter;

			if (startiter == m_vecResult.end())
				break;
		}
		cout << "b" << endl;
		size_t i{};
		for (auto iter{ m_vecResult.begin() }; iter != m_vecResult.end();)
		{
			if (i >= vecdata.size())
				break;

			if (vecdata[i] == *iter)
			{
				iter = m_vecResult.erase(iter);
				++i;
			}
			else
				++iter;
		}
		cout << "c" << endl;
		// Mark Start & End Position ('S', 'E' 로 시작점 도착점을 표시합니다.)
		auto	iterStart = ResultNodes.begin();
		auto	iterEnd = ResultNodes.rbegin();
		auto& startCoord = (*iterStart);
		auto& endCoord = (*iterEnd);
		results[(GridHeight - 1 - startCoord.m_y) * (GridWidth + 1) + startCoord.m_x] = 'S';
		results[(GridHeight - 1 - endCoord.m_y) * (GridWidth + 1) + endCoord.m_x] = 'E';
		cout << "d" << endl;
	}

	for (size_t i{}; i < m_vecResult.size(); ++i)
	{
		cout << "[x] " << m_vecResult[i].x << " " << "[z] " << m_vecResult[i].z << endl;
	}

	return m_vecResult;
}


const vector<Vec3>& CJpsMgr::UpdateMonsterPath(Int32 x1, Int32 z1, Int32 x2, Int32 z2)
{
	m_vecResultMonster.clear();
	// Start, End Position (시작점, 도착점)
	Int32 Sx = x1, Sy = z1;
	Int32 Ex = x2, Ey = z2;

	m_JpsMonsterPath.Search(Sx, Sy, Ex, Ey, ResultNodes);

	if (ResultNodes.size() > 0)
	{
		auto iterS = ResultNodes.begin();
		auto iterE = ResultNodes.end();
		auto iterP = iterS;	++iterS;
		for (; iterS != iterE; iterP = iterS, ++iterS)
		{
			auto& PreCoord = (*iterP);
			auto& CurCoord = (*iterS);

			Int32 x = PreCoord.m_x;
			Int32 y = PreCoord.m_y;
			Int32 dx = core::clamp<Int32>(CurCoord.m_x - PreCoord.m_x, -1, 1);
			Int32 dy = core::clamp<Int32>(CurCoord.m_y - PreCoord.m_y, -1, 1);

			for (Int32 v = y, u = x; ; v += dy, u += dx)
			{
				cout << "JPS 좌표 [x] " << u << " [z] " << v << endl;
				if (u != -1)
					m_vecResultMonster.push_back(m_JpsPath.GetCoord(u, v));

				if (u == CurCoord.m_x && v == CurCoord.m_y)
					break;

				Int32 deltax = core::clamp<Int32>(CurCoord.m_x - u, -1, 1);
				Int32 deltay = core::clamp<Int32>(CurCoord.m_y - v, -1, 1);
				if (deltax != dx || deltay != dy)
				{
					std::cout << "INVALID NODE\n";
					break;
				}
			}

			cout << "JPS 좌표 [x] " << CurCoord.m_x << " [z] " << CurCoord.m_y << endl;

			if (CurCoord.m_x != -1)
				m_vecResultMonster.push_back(m_JpsPath.GetCoord(CurCoord.m_x, CurCoord.m_y));
		}

		size_t start{}, end{};
		bool bCheck = false;
		for (size_t i{}; i < m_vecResultMonster.size() && i + 1 < m_vecResultMonster.size() && i + 2 < m_vecResultMonster.size(); ++i)
		{
			float dx = abs(m_vecResultMonster[i].x - m_vecResultMonster[i + 1].x);
			float dz = abs(m_vecResultMonster[i].z - m_vecResultMonster[i + 2].z);

			if (m_vecResultMonster[i].x == m_vecResultMonster[i + 2].x && dx == TILECX * 0.5 && dz == TILECZ)
			{
				if (!bCheck)
				{
					bCheck = true;
					start = i;
				}
			}
			else if (bCheck && !(m_vecResultMonster[i].x == m_vecResultMonster[i + 2].x && dx == TILECX * 0.5 && dz == TILECZ))
			{
				end = i;
			}
		}
		auto startiter = m_vecResultMonster.begin();
		vector<Vec3>::iterator enditer = m_vecResultMonster.begin();
		vector<Vec3> vecdata{};

		for (size_t i{}; i < start; ++i)
			++startiter;
		for (size_t i{}; i < end; ++i)
			++enditer;
		for (; startiter != enditer;)
		{
			++startiter;
			if (startiter == m_vecResultMonster.end())
				break;
			vecdata.push_back(*startiter);
			if (startiter == m_vecResultMonster.end())
				break;
			++startiter;

			if (startiter == m_vecResultMonster.end())
				break;
		}

		size_t i{};
		for (auto iter{ m_vecResultMonster.begin() }; iter != m_vecResultMonster.end();)
		{
			if (i >= vecdata.size())
				break;

			if (vecdata[i] == *iter)
			{
				iter = m_vecResultMonster.erase(iter);
				++i;
			}
			else
				++iter;
		}

		// Mark Start & End Position ('S', 'E' 로 시작점 도착점을 표시합니다.)
		auto	iterStart = ResultNodes.begin();
		auto	iterEnd = ResultNodes.rbegin();
		auto& startCoord = (*iterStart);
		auto& endCoord = (*iterEnd);


	}


	for (size_t i{}; i < m_vecResultMonster.size(); ++i)
	{
		cout << "[x] " << m_vecResultMonster[i].x << " " << "[z] " << m_vecResultMonster[i].z << endl;
	}

	return m_vecResultMonster;
}


void CJpsMgr::SetCollision(Int32 x, Int32 z)
{
	m_spCollision->SetAt(x, z);
}

void CJpsMgr::ClearCollision(Int32 x, Int32 z)
{
	m_spCollision->ClrAt(x, z);
}

void CJpsMgr::SetMonsterCollision(Int32 x, Int32 z)
{
	m_spMonsterCollision->SetAt(x, z);
}

void CJpsMgr::ClearMonsterCollision(Int32 x, Int32 z)
{
	m_spMonsterCollision->ClrAt(x, z);
}