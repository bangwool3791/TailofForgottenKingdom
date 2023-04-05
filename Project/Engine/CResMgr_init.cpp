#include "pch.h"
#include "CResMgr.h"
#include "CDevice.h"
#include "CTransform.h"

#include "CTexture.h"
#include "CMeshRender.h"
#include "CPaintShader.h"

#include "CSound.h"

void CResMgr::init()
{
	initSound();

	CreateDefaultMesh();

	CreateDefaultTexture();

	CreateDefaultGraphicsShader();

	CreateDefaultComputeShader();

	CreateDefaultMaterial();

	CreateDefaultSound();

}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _eFormat, UINT _iBindFlag)
{
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);

	if(nullptr != pTex)
		return pTex;
	//assert(!pTex.Get());

	pTex = new CTexture(true);

	pTex->Create(_iWidth, _iHeight, _eFormat, _iBindFlag);

	AddRes<CTexture>(_strKey, pTex.Get());
	return pTex;
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D)
{
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(!pTex.Get());

	pTex = new CTexture(true);

	pTex->Create(_Tex2D);

	AddRes<CTexture>(_strKey, pTex.Get());
	return pTex;
}

Ptr<CTexture> CResMgr::CreateCubeTexture(const wstring& _strKey, const vector<Ptr<CTexture>> vecTex)
{
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	if (pTex.Get())
	{
		MessageBox(nullptr, L"에러", L"큐브 텍스쳐 이미 생성", MB_OK);
		return nullptr;
	}

	pTex = new CTexture(true);

	pTex->CreateCubeTexture(vecTex);

	AddRes<CTexture>(_strKey, pTex.Get());
	return pTex;
}

void CResMgr::CreateDefaultMesh()
{
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;

	back_insert_iterator iterVtx{ vecVtx };
	back_insert_iterator iterIdx{ vecIdx };
	
	Vtx v;

	CMesh* pMesh = nullptr;

	// 사각형 메쉬 만들기
	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	*iterVtx = v;

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	*iterVtx = v;

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	*iterVtx = v;

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	*iterVtx = v;

	*iterIdx = 0;
	*iterIdx = 1;
	*iterIdx = 2;

	*iterIdx = 2;
	*iterIdx = 3;
	*iterIdx = 0;

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"RectMesh", pMesh);
	//vecIdx.clear();

	//*iterIdx = 0;
	//*iterIdx = 1;
	//*iterIdx = 2;
	//*iterIdx = 3;
	//*iterIdx = 0;

	pMesh = new CMesh(true);
	pMesh->SetName(L"RectMesh_Debug");
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"RectMesh_Debug", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	// 원형메쉬 만들기
	// 중심점	
	v.vPos = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	*iterVtx = v;

	int iSlice = 40;
	float fRadius = 0.5f;
	float fTheta = XM_2PI / (float)iSlice;

	for (int i = 0; i < iSlice; ++i)
	{
		v.vPos = Vec3(fRadius * cosf(fTheta * (float)i), fRadius * sinf(fTheta * (float)i), 0.f);
		v.vNormal = Vec3(0.f, 0.f, -1.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		*iterVtx = v;
	}

	for (UINT i = 0; i < (UINT)iSlice; ++i)
	{
		*iterIdx = 0;
		if (i == iSlice - 1)
		{
			*iterIdx = 1;
		}
		else
		{
			*iterIdx = i + 2;
		}
		*iterIdx = i + 1;
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"CircleMesh", pMesh);
	vecIdx.clear();

	for (UINT i = 0; i < (UINT)iSlice; ++i)
	{
		*iterIdx = i + 1;
	}
	*iterIdx = 1;
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"CircleMesh_Debug", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	v.vPos = Vec3(-0.5f, 0.f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.5f);
	*iterVtx = v;
	v.vPos = Vec3(0.f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(0.5f, 0.f);
	*iterVtx = v;
	v.vPos = Vec3(0.5f, 0.f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 0.5f);
	*iterVtx = v;
	v.vPos = Vec3(0.f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(0.5f, 1.f);
	*iterVtx = v;

	*iterIdx = 0;
	*iterIdx = 1;
	*iterIdx = 2;

	*iterIdx = 0;
	*iterIdx = 2;
	*iterIdx = 3;

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"Tile", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	/*정점 메쉬
	*/
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);

	UINT idx = 0;

	pMesh = new CMesh(true);
	pMesh->Create(&v, 1, &idx, 1);
	AddRes<CMesh>(L"PointMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	int iOffset = TERRAINX * 64 >> 1;
	for (int i = 0; i < TERRAINX; ++i)
	{
		for (int j = 0; j < TERRAINZ; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				//top view
				//float	fX = (64 >> 1) * (j - i) + iOffset;
				//float	fZ = (64 >> 1) * (j + i) + (64 >> 1);
				float	fX = (TERRAINCX * j) + ((i % 2) * (TERRAINCX * 0.5f));
				float	fZ = (TERRAINCZ * 0.5f) * i;

				if (k % 4 == 0)
					fX -= TERRAINCX * 0.5f;
				else if (k % 4 == 1)
					fZ += TERRAINCZ * 0.5f;
				else if (k % 4 == 2)
					fX += TERRAINCX * 0.5f;
				else if (k % 4 == 3)
					fZ -= TERRAINCZ * 0.5f;

				v.vPos = Vec3{ fX , 0.f, fZ };
				//cout << "[x][z] " << v.vPos.x << " " << v.vPos.z << endl;
				v.vColor = Vec4(0.f, 0.f, 0.f, 1.f);
				*iterVtx = v;
			}
		}
	}


	for (int j = 0; j < TERRAINX * TERRAINZ; ++j)
	{
		*iterIdx = 0 + j * 4;
		*iterIdx = 1 + j * 4;
		*iterIdx = 2 + j * 4;
		*iterIdx = 0 + j * 4;
		*iterIdx = 2 + j * 4;
		*iterIdx = 3 + j * 4;

		vecVtx[0 + j * 4].vUV = Vec2(0.f, 0.f);
		vecVtx[1 + j * 4].vUV = Vec2(1.f, 0.f);
		vecVtx[2 + j * 4].vUV = Vec2(1.f, 1.f);
		vecVtx[3 + j * 4].vUV = Vec2(0.f, 1.f);
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"TerrainMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	for (int i = 0; i < TILEX; ++i)
	{
		for (int j = 0; j < TILEZ; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				float	fX = (TILECX * j) + ((i % 2) * (TILECX * 0.5f));
				float	fZ = (TILECZ * 0.5f) * i;

				if (k % 4 == 0)
					fX -= TILECX * 0.5f;
				else if (k % 4 == 1)
					fZ += TILECZ * 0.5f;
				else if (k % 4 == 2)
					fX += TILECX * 0.5f;
				else if (k % 4 == 3)
					fZ -= TILECZ * 0.5f;

				v.vPos = Vec3{ fX, 0.f, fZ };
				v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
				*iterVtx = v;
			}
		}
	}

	for (int j = 0; j < TILEX * TILEZ; ++j)
	{
		*iterIdx = 0 + j * 4;
		*iterIdx = 1 + j * 4;

		*iterIdx = 1 + j * 4;
		*iterIdx = 2 + j * 4;

		*iterIdx = 2 + j * 4;
		*iterIdx = 3 + j * 4;

		*iterIdx = 3 + j * 4;
		*iterIdx = 0 + j * 4;

		vecVtx[0 + j * 4].vUV = Vec2(0.f, 0.f);
		vecVtx[1 + j * 4].vUV = Vec2(1.f, 0.f);
		vecVtx[2 + j * 4].vUV = Vec2(1.f, 1.f);
		vecVtx[3 + j * 4].vUV = Vec2(0.f, 0.f);

	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"BorderMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	iOffset = TILEX * 64 >> 1;
	for (int i = 0; i < TILEX; ++i)
	{
		for (int j = 0; j < TILEZ; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				//top view
				//float	fX = (64 >> 1) * (j - i) + iOffset;
				//float	fZ = (64 >> 1) * (j + i) + (64 >> 1);
				float	fX = (TILECX * j) + ((i % 2) * (TILECX * 0.5f));
				float	fZ = (TILECZ * 0.5f) * i;

				if (k % 4 == 0)
					fX -= (TILECX * 0.5f - 5.f);
				else if (k % 4 == 1)
					fZ += (TILECZ * 0.5f - 5.f);
				else if (k % 4 == 2)
					fX += (TILECX * 0.5f - 5.f);
				else if (k % 4 == 3)
					fZ -= (TILECZ * 0.5f - 5.f);

				v.vPos = Vec3{ fX, 0.f, fZ };
				v.vColor = Vec4(0.f, 0.f, 0.f, 1.f);
				v.vColor = Vec4(i * (float)TILEZ + j, 0.f, 0.f, 0.f);
				*iterVtx = v;
			}
		}
	}


	for (int j = 0; j < TILEX * TILEZ; ++j)
	{
		*iterIdx = 0 + j * 4;
		*iterIdx = 1 + j * 4;
		*iterIdx = 2 + j * 4;
		*iterIdx = 0 + j * 4;
		*iterIdx = 2 + j * 4;
		*iterIdx = 3 + j * 4;

		vecVtx[0 + j * 4].vUV = Vec2(0.f, 0.f);
		vecVtx[1 + j * 4].vUV = Vec2(1.f, 0.f);
		vecVtx[2 + j * 4].vUV = Vec2(1.f, 1.f);
		vecVtx[3 + j * 4].vUV = Vec2(0.f, 1.f);
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"TileMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();

		// Cube Mesh
	Vtx arrCube[24] = {};
	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV = Vec2(0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[0].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[0].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV = Vec2(1.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[1].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[1].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV = Vec2(1.f, 1.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[2].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[2].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV = Vec2(0.f, 1.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[3].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[3].vBinormal = Vec3(0.f, 0.f, -1.f);

	// 아랫 면	
	arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUV = Vec2(0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[4].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[4].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUV = Vec2(1.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[5].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[5].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUV = Vec2(1.f, 1.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[6].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[6].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUV = Vec2(0.f, 1.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[7].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[7].vBinormal = Vec3(0.f, 0.f, 1.f);

	// 왼쪽 면
	arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV = Vec2(0.f, 0.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[8].vTangent = Vec3(0.f, 1.f, 0.f);
	arrCube[8].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV = Vec2(1.f, 0.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[9].vTangent = Vec3(0.f, 1.f, 0.f);
	arrCube[9].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV = Vec2(1.f, 1.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[10].vTangent = Vec3(0.f, 1.f, 0.f);
	arrCube[10].vBinormal = Vec3(0.f, 0.f, 1.f);

	arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV = Vec2(0.f, 1.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[11].vTangent = Vec3(0.f, 1.f, 0.f);
	arrCube[11].vBinormal = Vec3(0.f, 0.f, 1.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV = Vec2(0.f, 0.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[12].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[12].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV = Vec2(1.f, 0.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[13].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[13].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV = Vec2(1.f, 1.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[14].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[14].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV = Vec2(0.f, 1.f);
	arrCube[15].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[15].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[15].vBinormal = Vec3(0.f, 1.f, 0.f);

	// 뒷 면
	arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUV = Vec2(0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[16].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[16].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUV = Vec2(1.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[17].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[17].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUV = Vec2(1.f, 1.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[18].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[18].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUV = Vec2(0.f, 1.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[19].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[19].vBinormal = Vec3(0.f, 1.f, 0.f);

	// 앞 면
	arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);;
	arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUV = Vec2(0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[20].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[20].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUV = Vec2(1.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[21].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[21].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUV = Vec2(1.f, 1.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[22].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[22].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUV = Vec2(0.f, 1.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[23].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[23].vBinormal = Vec3(0.f, 1.f, 0.f);

	// 인덱스
	for (int i = 0; i < 12; i += 2)
	{
		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 1);
		vecIdx.push_back(i * 2 + 2);

		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 2);
		vecIdx.push_back(i * 2 + 3);
	}

	pMesh = new CMesh(true);
	pMesh->Create(arrCube, 24, vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"CubeMesh", pMesh);
	pMesh = new CMesh(true);
	pMesh->Create(arrCube, 24, vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"FrustumMesh", pMesh);
	vecIdx.clear();

	// ===========
	// Sphere Mesh
	// ===========
	fRadius = 0.5f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	// Body
	UINT iStackCount = 40; // 가로 분할 개수
	UINT iSliceCount = 40; // 세로 분할 개수

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
			v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent.Normalize();

			v.vTangent.Cross(v.vNormal, v.vBinormal);
			v.vBinormal.Normalize();

			vecVtx.push_back(v);
		}
	}

	// Bottom
	v.vPos = Vec3(0.f, -fRadius, 0.f);
	v.vUV = Vec2(0.5f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();

	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// 인덱스
	// 북극점
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 몸통
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	// 남극점
	UINT iBottomIdx = (UINT)vecVtx.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"SphereMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	pMesh = new CMesh(true);
	CreateCylinder(0.5f, 0.00000001f, 1.f, 50, 30, vecVtx, vecIdx);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"ConeMesh", pMesh);
	vecVtx.clear();
	vecVtx.shrink_to_fit();
	vecIdx.clear();
	vecIdx.shrink_to_fit();

	CreateConeMesh(0.5f, 1.f, 20);
}

void CResMgr::CreateDefaultTexture()
{
	Load<CTexture>(L"texture\\Player.bmp", L"texture\\Player.bmp");
	Load<CTexture>(L"texture\\smokeparticle.png", L"texture\\smokeparticle.png");
	Load<CTexture>(L"texture\\Character.png", L"texture\\Character.png");
	Load<CTexture>(L"texture\\link.png", L"texture\\link.png");

	Load<CTexture>(L"texture\\beheaded.png", L"texture\\beheaded.png");
	Load<CTexture>(L"texture\\beheaded_n.png", L"texture\\beheaded_n.png");

	Load<CTexture>(L"texture\\particle\\smokeparticle.png", L"texture\\particle\\smokeparticle.png");
	Load<CTexture>(L"texture\\particle\\CartoonSmoke.png", L"texture\\particle\\CartoonSmoke.png");
	Load<CTexture>(L"texture\\particle\\Bubbles50px.png", L"texture\\particle\\Bubbles50px.png");
	// NoiseTexture
	Load<CTexture>(L"texture\\noise\\noise_01.png", L"texture\\noise\\noise_01.png");
	Load<CTexture>(L"texture\\noise\\noise_02.png", L"texture\\noise\\noise_02.png");
	Load<CTexture>(L"texture\\noise\\noise_03.jpg", L"texture\\noise\\noise_03.jpg");

	Load<CTexture>(L"texture\\Heightmap3.dds", L"texture\\Heightmap3.dds");
	
	CreateTexture(L"UAVTex", 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_UNORDERED_ACCESS);

	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();


	CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTexCopy"
		, vRenderResolution.x, vRenderResolution.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_UNORDERED_ACCESS);

	Load<CTexture>(L"texture\\blue01.dds", L"texture\\blue01.dds");
}

void CResMgr::CreateDefaultSound()
{

}

#include "CShadowMapShader.h"

void CResMgr::CreateDefaultGraphicsShader()
{
	AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "POSITION");
	AddInputLayout(DXGI_FORMAT_R32G32B32A32_FLOAT, "COLOR");
	AddInputLayout(DXGI_FORMAT_R32G32_FLOAT, "TEXCOORD");
	AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "TANGENT");
	AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "BINORMAL");
	AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "NORMAL");
	AddInputLayout(DXGI_FORMAT_R32G32B32A32_FLOAT, "BLENDWEIGHT");
	AddInputLayout(DXGI_FORMAT_R32G32B32A32_FLOAT, "BLENDINDICES");

	CGraphicsShader* pShader = nullptr;

	pShader = new CGraphicsShader();
	pShader->CreateVertexShader(L"shader\\test.fx", "VS_DeadCell");
	pShader->CreatePixelShader(L"shader\\test.fx", "PS_DeadCell");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	AddRes<CGraphicsShader>(L"TestShader", pShader);

	pShader = new CGraphicsShader();
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	pShader->AddScalarParam(INT_0, "Test Int");
	pShader->AddScalarParam(FLOAT_2, "Test Float");
	pShader->AddScalarParam(VEC2_3, "Test Vec2");
	pShader->AddScalarParam(VEC4_2, "Test Vec4");
	pShader->AddTexureParam(TEX_0, "Output Texture 1");
	pShader->AddTexureParam(TEX_1, "Output Texture 2");
	pShader->AddTexureParam(TEX_2, "Output Texture 3");

	AddRes<CGraphicsShader>(L"Std2DShader", pShader);

	pShader = new CGraphicsShader();
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_AlphaBlend");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes<CGraphicsShader>(L"Std2DAlphaBlendShader", pShader);

	pShader = new CGraphicsShader();
	pShader->CreateVertexShader(L"shader\\editor.fx", "VS_Grid");
	pShader->CreatePixelShader(L"shader\\editor.fx", "PS_Grid");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes<CGraphicsShader>(L"EditorShader", pShader);

	pShader = new CGraphicsShader();
	pShader->CreateVertexShader(L"shader\\debugdraw.fx", "VS_DebugDraw");
	pShader->CreatePixelShader(L"shader\\debugdraw.fx", "PS_DebugDraw");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->AddScalarParam(VEC4_0, "Color");
	AddRes<CGraphicsShader>(L"DebugDrawShader", pShader);

	// ParticleRenderShader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\particlerender.fx", "VS_ParticleRender");
	pShader->CreateGeometryShader(L"shader\\particlerender.fx", "GS_ParticleRender");
	pShader->CreatePixelShader(L"shader\\particlerender.fx", "PS_ParticleRender");
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes<CGraphicsShader>(L"ParticleRenderShader", pShader);

	// Instancing Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\objectrenderer.fx", "VS_ObjectRender");
	pShader->CreatePixelShader(L"shader\\objectrenderer.fx", "PS_ObjectRender");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	pShader->AddScalarParam(INT_0, "Use Atlas");
	pShader->AddScalarParam(VEC2_0, "UV LeftTop");
	pShader->AddScalarParam(VEC2_1, "UV Slice");
	pShader->AddTexureParam(TEX_0, "Output Texture 1");

	AddRes<CGraphicsShader>(L"ObjectRenderShader", pShader);

	//Opaque
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\opaque.fx", "VS_Opaque");
	pShader->CreatePixelShader(L"shader\\opaque.fx", "PS_Opaque");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
	AddRes<CGraphicsShader>(L"OpaqueShader", pShader);

	// PostProcess Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_PostProcess");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_PostProcess");

	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POST_PROCESS);

	AddRes<CGraphicsShader>(L"PostProcessShader", pShader);

	// TileMap Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\terrain.fx", "VS_TileMap");
	//pShader->CreateGeometryShader(L"shader\\tilemap.fx", "GS_TileMap");
	pShader->CreatePixelShader(L"shader\\terrain.fx", "PS_TileMap");
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	AddRes<CGraphicsShader>(L"TileMapShader", pShader);

	//Tile Map Border Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\bordermap.fx", "VS_Border");
	pShader->CreatePixelShader(L"shader\\bordermap.fx", "PS_Border");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	AddRes<CGraphicsShader>(L"BorderShader", pShader);

	//Building Red, Green
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\building.fx", "VS_BuildRender");
	pShader->CreatePixelShader(L"shader\\building.fx", "PS_BuildRender");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	AddRes<CGraphicsShader>(L"BuildShader", pShader);

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\std3d.fx", "VS_Std3D");
	pShader->CreatePixelShader(L"shader\\std3d.fx", "PS_Std3D");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);
	pShader->AddScalarParam(FLOAT_0, "Specular Coefficient");
	pShader->AddTexureParam(TEX_0, "ObjectColor");
	pShader->AddTexureParam(TEX_1, "NormalMap");
	pShader->AddTexureParam(TEX_2, "Spec");
	AddRes<CGraphicsShader>(L"Std3DShader", pShader);

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\std3d_deffered.fx", "VS_Std3D_Deferred");
	pShader->CreatePixelShader(L"shader\\std3d_deffered.fx", "PS_Std3D_Deferred");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE);

	pShader->AddScalarParam(FLOAT_0, "Specular Coefficient");
	pShader->AddTexureParam(TEX_0, "ObjectColor");
	pShader->AddTexureParam(TEX_1, "NormalMap");
	pShader->AddTexureParam(TEX_2, "Spec");
	AddRes<CGraphicsShader>(L"Std3DDeferredShader", pShader);

	// DirLightShader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\light.fx", "VS_DirLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_DirLightShader");

	pShader->SetRSType(RS_TYPE::SHADOW);
	//C * F 에서 F(1, 1, 1, 1)
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes<CGraphicsShader>(L"DirLightShader", pShader);

	// PointLightShader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\light.fx", "VS_PointLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_PointLightShader");

	/*
	* 카메라가 PointLight 외부/내부 상관 없이
	* 레스터 라이저가 픽셀 셰이더를 호출할려면 CULL_FRONT 
	*/
	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes<CGraphicsShader>(L"PointLightShader", pShader);

	// SpotLightShader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\light.fx", "VS_SpotLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_SpotLightShader");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes<CGraphicsShader>(L"SpotLightShader", pShader);
	// Merge Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\merge.fx", "VS_Merge");
	pShader->CreatePixelShader(L"shader\\merge.fx", "PS_Merge");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::NONE);
	pShader->AddScalarParam(FLOAT_1, "Middle Grey");
	pShader->AddScalarParam(FLOAT_2, "Lum white sqr");
	AddRes<CGraphicsShader>(L"MergeShader", pShader);

	// Decal Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\decal.fx", "VS_Decal");
	pShader->CreatePixelShader(L"shader\\decal.fx", "PS_Decal");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DECAL);

	AddRes<CGraphicsShader>(L"DecalShader", pShader);

	// Decal Defferd Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\decal.fx", "VS_Decal_Defferd");
	pShader->CreatePixelShader(L"shader\\decal.fx", "PS_Decal_Defferd");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL);

	AddRes<CGraphicsShader>(L"DeferredDecalShader", pShader);

	// SkyBox Shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\skybox.fx", "VS_SkyBox");
	pShader->CreatePixelShader(L"shader\\skybox.fx", "PS_SkyBox");
	/*
	* Skybox -> 큐브 안
	* 깊이 판정 Far(1) 강제 변환(Shader)
	* 배경 위에 사물 그려지도록.
	*/
	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	AddRes<CGraphicsShader>(L"SkyBoxShader", pShader);

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\blur.fx", "VS_Blur");
	pShader->CreatePixelShader(L"shader\\blur.fx", "PS_Blurx");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::NONE);

	AddRes<CGraphicsShader>(L"BlurShaderX", pShader);

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\blur.fx", "VS_Blur");
	pShader->CreatePixelShader(L"shader\\blur.fx", "PS_Blury");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::NONE);

	AddRes<CGraphicsShader>(L"BlurShaderY", pShader);

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\bloom.fx", "VS_Bloom");
	pShader->CreatePixelShader(L"shader\\bloom.fx", "PS_Bloom");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::NONE);
	pShader->AddScalarParam(VEC4_0, "Bloom White");
	pShader->AddScalarParam(FLOAT_0, "Bloom Cos");
	AddRes<CGraphicsShader>(L"BloomShader", pShader);

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\bloom_update.fx", "VS_BloomUpdate");
	pShader->CreatePixelShader(L"shader\\bloom_update.fx", "PS_BloomUpdate");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::NONE);

	AddRes<CGraphicsShader>(L"BloomUpdateShader", pShader);

	// TessShader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\tess.fx", "VS_Tess");
	pShader->CreateHullShader(L"shader\\tess.fx", "HS_Tess");
	pShader->CreateDomainShader(L"shader\\tess.fx", "DS_Tess");
	pShader->CreatePixelShader(L"shader\\tess.fx", "PS_Tess");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	pShader->AddScalarParam(INT_0, "Tess Factor");
	pShader->AddScalarParam(INT_1, "Tess Factor");
	pShader->AddScalarParam(INT_2, "Tess Factor");
	pShader->AddScalarParam(INT_3, "Tess Factor");

	AddRes<CGraphicsShader>(L"TessShader", pShader);

	//reflect shader
	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\reflection.fx", "ReflectionVertexShader");
	pShader->CreatePixelShader(L"shader\\reflection.fx", "ReflectionPixelShader");
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes<CGraphicsShader>(L"ReflectShader", pShader);

	pShader = new CGraphicsShader();
	pShader->CreateVertexShader(L"shader\\reflection2.fx", "VS_EnvMappedScene_NoTexture");
	pShader->CreatePixelShader(L"shader\\reflection2.fx", "PS_EnvMappedScene_NoTexture");
	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	AddRes<CGraphicsShader>(L"ReflectionCubeShader", pShader);

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\color.fx", "VS");
	pShader->CreatePixelShader(L"shader\\color.fx", "PS");
	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	AddRes<CGraphicsShader>(L"ColorShader", pShader);

	// DepthmapShader
	pShader = new CGraphicsShader;
	pShader->SetDomain(SHADER_DOMAIN::NONE);
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_DepthMap");
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_DepthMap");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	AddRes<CGraphicsShader>(L"DepthMapShader", pShader);
}

#include "CComputeShader.h"
#include "CParticleUpdateShader.h"
#include "CSLight.h"
#include "CAnimation3DShader.h"

void CResMgr::CreateDefaultComputeShader()
{
	CComputeShader* pShader = nullptr;
	pShader = new CPaintShader;
	pShader->CreateComputeShader(L"shader\\compute.fx", "CS_Paint");
	AddRes<CComputeShader>(L"PaintShader", pShader);

	pShader = new CSLight;
	pShader->CreateComputeShader(L"shader\\compute_light.fx", "CS_Light");
	AddRes<CComputeShader>(L"CSLight", pShader);

	// Animation3D Update Shader	
	pShader = new CAnimation3DShader;
	pShader->CreateComputeShader(L"shader\\animation3d.fx", "CS_Animation3D");
	AddRes<CComputeShader>(L"Animation3DUpdateShader", pShader);
}

void CResMgr::CreateDefaultMaterial()
{

	CMaterial* pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"TestShader"));
	AddRes(L"TestMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"Std2DShader"));
	AddRes(L"Std2DMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"Std2DAlphaBlendShader"));
	AddRes(L"Std2DAlphaBlendMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"EditorShader"));
	AddRes(L"EditMaterial", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"ParticleRenderShader"));
	AddRes(L"ParticleRenderMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"ParticleRenderShader"));
	AddRes(L"ParticleRenderWoodMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"ObjectRenderShader"));
	AddRes(L"ObjectMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"PostProcessShader"));
	AddRes<CMaterial>(L"PostProcessMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"DebugDrawShader"));
	Vec4 vColor{ 0.f, 1.f, 0.f, 1.f };
	pMaterial->SetScalarParam(VEC4_0, &vColor);
	AddRes<CMaterial>(L"DebugDrawMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"TileMapShader"));
	AddRes<CMaterial>(L"TileMapMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"BorderShader"));
	AddRes<CMaterial>(L"BorderMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"OpaqueShader"));
	AddRes<CMaterial>(L"OpaqueMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"BuildShader"));
	AddRes<CMaterial>(L"BuildMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"Std3DShader"));
	AddRes<CMaterial>(L"Std3DMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"Std3DDeferredShader"));
	AddRes<CMaterial>(L"Std3DDeferredMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"MergeShader"));
	AddRes<CMaterial>(L"MergeMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"DirLightShader"));
	AddRes<CMaterial>(L"DirLightMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"PointLightShader"));
	AddRes<CMaterial>(L"PointLightMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"SpotLightShader"));
	AddRes<CMaterial>(L"SpotLightMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"DecalShader"));
	AddRes<CMaterial>(L"DecalMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"DeferredDecalShader"));
	AddRes<CMaterial>(L"DeferredDecalMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"SkyBoxShader"));
	AddRes<CMaterial>(L"SkyBoxMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"BlurShaderX"));
	AddRes<CMaterial>(L"BlurMtrlX", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"BlurShaderY"));
	AddRes<CMaterial>(L"BlurMtrlY", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"BloomShader"));
	AddRes<CMaterial>(L"BloomMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"BloomUpdateShader"));
	AddRes<CMaterial>(L"BloomUpdateMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"TessShader"));
	AddRes<CMaterial>(L"TessMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"ReflectShader"));
	AddRes<CMaterial>(L"ReflectMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"EnvCubeShader"));
	AddRes<CMaterial>(L"EnvCubeMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"ReflectionCubeShader"));
	AddRes<CMaterial>(L"ReflectionCubeMtrl", pMaterial);

	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"ColorShader"));
	AddRes<CMaterial>(L"ColorMtrl", pMaterial);

	// DepthMap Material
	pMaterial = new CMaterial(true);
	pMaterial->SetShader(FindRes<CGraphicsShader>(L"DepthMapShader"));
	AddRes<CMaterial>(L"DepthMapMtrl", pMaterial);
}

int GetSizeofFormat(DXGI_FORMAT _eFormat)
{
	int iRetByte = 0;
	switch (_eFormat)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		iRetByte = 128;
		break;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		iRetByte = 96;
		break;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		iRetByte = 64;
		break;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		iRetByte = 16;
		break;
	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		iRetByte = 8;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		iRetByte = 128;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		iRetByte = 64;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		iRetByte = 32;
		break;
		// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_UNKNOWN:
	default:
		return -1;
	}

	return iRetByte / 8;
}

void CResMgr::AddInputLayout(DXGI_FORMAT _eFormat, const char* _strSemanticName)
{
	D3D11_INPUT_ELEMENT_DESC LayoutDesc = {};
	LayoutDesc.AlignedByteOffset = m_iLayoutOffset;
	LayoutDesc.Format = _eFormat;
	LayoutDesc.InputSlot = 0;
	LayoutDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc.SemanticName = _strSemanticName;
	LayoutDesc.SemanticIndex = 0;
	m_vecLayoutInfo.push_back(LayoutDesc);

	m_iLayoutOffset += GetSizeofFormat(_eFormat);
}

void CResMgr::initSound()
{
	//FMOD::System_Create(&CSound::g_pFMOD);

	//if (nullptr == CSound::g_pFMOD)
		//assert(nullptr);

	//CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}


void CResMgr::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertices, vector<UINT>& Indices)
{
	Vertices.clear();
	Indices.clear();

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ringCount; ++i)
	{
		float y = -1.f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		// vertices of ring
		float dTheta = 2.0f * XM_PI / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			Vtx vertex;

			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			vertex.vPos = XMFLOAT3(r * c, y, r * s);

			vertex.vUV.x = (float)j / sliceCount;
			vertex.vUV.y = 1.0f - (float)i / stackCount;

			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.
			//   Let r0 be the bottom radius and let r1 be the top radius.
			//   y(v) = h - hv for v in [0,1].
			//   r(v) = r1 + (r0-r1)v
			//
			//   x(t, v) = r(v)*cos(t)
			//   y(t, v) = h - hv
			//   z(t, v) = r(v)*sin(t)
			// 
			//  dx/dt = -r(v)*sin(t)
			//  dy/dt = 0
			//  dz/dt = +r(v)*cos(t)
			//
			//  dx/dv = (r0-r1)*cos(t)
			//  dy/dv = -h
			//  dz/dv = (r0-r1)*sin(t)

			// This is unit length.
			vertex.vTangent = XMFLOAT3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			XMFLOAT3 bitangent(dr * c, -height, dr * s);

			XMVECTOR T = XMLoadFloat3(&vertex.vTangent);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
			XMStoreFloat3(&vertex.vNormal, N);

			Vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			Indices.push_back(i * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j + 1);

			Indices.push_back(i * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j + 1);
			Indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, Vertices, Indices);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, Vertices, Indices);
}

void CResMgr::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertices, vector<UINT>& Indices)
{
	UINT baseIndex = (UINT)Vertices.size();

	float y = -1.f * height;
	float dTheta = 2.0f * XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		/*
		* 	Vec3 vPos;
			Vec4 vColor;
			Vec2 vUV;

			Vec3 vTangent;
			Vec3 vBinormal;
			Vec3 vNormal;
		*/
		//Vertices.push_back(Vtx({x, y, z}, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
		Vertices.push_back(Vtx({ x, y, z }, {}, { u, v }, { 1.0f, 0.0f, 0.0f }, {0.f, 0.f, 1.f}, { 0.0f, 1.0f, 0.0f }));
	}

	// Cap center vertex.
	Vertices.push_back(Vtx({ 0.0f, y, 0.0f }, {}, { 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, {0.f, 0.f, 1.f}, { 0.0f, 1.0f, 0.0f }));

	// Index of center vertex.
	UINT centerIndex = (UINT)Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		Indices.push_back(centerIndex);
		Indices.push_back(baseIndex + i + 1);
		Indices.push_back(baseIndex + i);
	}
}

void CResMgr::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,UINT sliceCount, UINT stackCount, vector<Vtx>& Vertices, vector<UINT>& Indices)
{
	// 
	// Build bottom cap.
	//

	UINT baseIndex = (UINT)Vertices.size();
	float y = -1.f * height;

	// vertices of ring
	float dTheta = 2.0f * XM_PI / sliceCount;
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius * cosf(i * dTheta);
		float z = bottomRadius * sinf(i * dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		Vertices.push_back(Vtx({ x, y, z }, {}, { u, v }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, {0.f , 0.f, -1.f}));
	}

	// Cap center vertex.
	Vertices.push_back(Vtx({ 0.0f, y, 0.0f }, {}, { 0.5f, 0.5f, }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.f , 0.f, -1.f }));

	// Cache the index of center vertex.
	UINT centerIndex = (UINT)Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		Indices.push_back(centerIndex);
		Indices.push_back(baseIndex + i);
		Indices.push_back(baseIndex + i + 1);
	}
}
