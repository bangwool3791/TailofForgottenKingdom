#pragma once

struct tRGBA
{
	int R;
	int G;
	int B;
	int A;
};

struct Vtx
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;

	Vec3 vTangent;
	Vec3 vBinormal;
	Vec3 vNormal;

	Vec4 vWeights;
	Vec4 vIndices;
};

struct tEvent
{
	EVENT_TYPE eType;
	DWORD_PTR  wParam;
	DWORD_PTR  lParam;
	DWORD_PTR  oParam;
};

// ==============
// TileMap 구조체
// ==============
struct tTile
{
	Vec3		vPos;
	UINT        iIndex;
	UINT		iInfo;
};

struct tDebugShapeInfo
{
	DEBUG_SHAPE eShape;
	Vec4		vColor;
	Vec3		vPosition;
	Vec3		vScale;
	Vec3		vRot;
	float		fRadius;
	float		fDuration;
	float		fCurTime;
};

// ================
// Shader Parameter
// ================
/*
* Shader 생성 시, 사용 Scalar, Texture Param Setting
* Material UI -> Material -> GetShader -> GetScalarParam, TextureParam
*/
struct tScalarParam
{
	SCALAR_PARAM eParam;
	string		 strName;
};

struct tTextureParam
{
	TEX_PARAM	 eParam;
	string		 strName;
};

// ================
// 상수버퍼용 구조체
// ================
struct tTransform
{
	Matrix matWorld;
	Matrix matWorldInv;
	Matrix matView;
	Matrix matViewInv;
	Matrix matProj;
	Matrix matProjInv;

	Matrix matWV;
	Matrix matWVP;

	Matrix matReflect;
};

// Material 계수
struct tMtrlData
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmv;
};

struct tMtrlConst
{
	tMtrlData mtrl;
	int iArr[4];
	float fArr[4];
	Vec2 v2Arr[4];
	Vec4 v4Arr[4];
	Matrix matArr[4];

	int HasTex[(UINT)TEX_PARAM::TEX_END];

	int	arrAnimData[4];	// 3D Animation 정보
};

struct tLightInfo
{
	Vec4		vDiff;		// 난반사 광	
	Vec4		vSpec;
	Vec4		vAmb;		// 환경광

	Vec4		vWorldPos;	// 광원 월드 위치
	Vec4		vWorldDir;	// 광원의 방향
	LIGHT_TYPE	iLightType;	// 광원 타입
	float		fRadius;	// 광원 반경 (POINT, SPOT)
	float		fAngle;		// 광원 각도 (SPOT)
	int			pad;
};

struct tGlobalData
{
	Vec2	vRenderResolution;
	Vec2	vNoiseResolution;
	int		iLight2DCount;
	int		iLight3DCount;

	float	fAccTime;
	float	fDT;
};

struct tParticle
{
	Vec4 vRelativePos;
	Vec4 vDir;

	float fMaxTime;
	float fCurTime;
	float fSpeed;
	UINT iActive;
};

struct tParticleShare
{
	UINT iAliveCount;
	UINT iPadding[3];
};

struct tLightShare
{
	UINT32 R;
	UINT32 G;
	UINT32 B;
	UINT32 A;
};

struct tWoodWall
{
	bool bChecked = false;
	bool arr[8] = { false, };
};

struct tDxatlas
{
	UINT  iFrame{};
	float x = 0.f;
	float y = 0.f;
	float w = 0.f;
	float h = 0.f;

	float pX = 0.f;
	float pY = 0.f;
	float oX = 0.f;
	float oY = 0.f;

	float oW = 0.f;
	float oH = 0.f;
};

struct tBlock
{
	int x;
	int z;
};

struct tTextInfo
{
	wchar_t sz[50];
	Vec2 vPos;
	float fSize;
	Vec4 vColor;
};

struct tFrustum
{
	Vec3 Origin;
	Vec4 Orientation;

	float RightSlope;
	float LeftSlope;
	float TopSlope;
	float BottomSlope;
	float Near, Far;
};

struct colored_vertex
{
	Vec3 pos = { 0.0f, 0.0f, 0.0f };
	Vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	colored_vertex() = default;
	colored_vertex(const colored_vertex&) = default;

	inline colored_vertex(const Vec3& p, const Vec4& c) : pos{ p }, color{ c } {}
	inline colored_vertex(const Vec3& p, const Vec3& c) : pos{ p }, color{ c.x, c.y, c.z, 1.0f } {}
	inline colored_vertex(const Vec3& p, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : pos{ p }, color{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f } {}
};


// 광선 구조체
struct tRay
{
	Vec3 vStart;
	Vec3 vDir;
};

// Raycast 결과를 받을 구조체
struct tRaycastOut
{
	Vec2 vUV;
	Vec3 vPos;
	int  iDist;
	int  bSuccess;
};

struct tWeight_4
{
	float arrWeight[4];
};

// ============
// Animation 2D
// ============
struct tAnim2DFrm
{
	Vec2 vLeftTop;
	Vec2 vSlice;
	Vec2 vOffset;
	Vec2 vFullSize;
	float fDuration;
};

// ============
// Animation 3D
// ============
struct tAnim3DFrm
{
	int iStart;
	int iEnd;
	UINT   iFrameCount;
	bool   bRepeat;
};

struct tAnim2DInfo
{
	Vec2 vLeftTop;
	Vec2 vSlice;
	Vec2 vOffset;
	Vec2 vFullSize;

	int iAnim2DUse;
	/*
	* .fx 파일에서 16바이트 단위로 데이터 전달 필요
	*/
	int iPaading[3];
};

struct tObjectRender
{
	tTransform transform;
	tMtrlConst mtrl;
	tAnim2DInfo animation;
};

// ============
// Animation 3D
// ============

struct tFrameTrans
{
	Vec4	vTranslate;
	Vec4	vScale;
	Vec4	qRot;
};

struct tMTKeyFrame
{
	//시간
	double  dTime;
	//프레임
	int	    iFrame;
	//이동
	Vec3	vTranslate;
	//크기
	Vec3	vScale;
	//회전
	Vec4	qRot;
};

struct tMTBone
{
	//뼈 이름
	wstring				strBoneName;
	//뼈 깊이
	int					iDepth;
	//부모 인덱스
	int					iParentIndx;
	//부모->자신 행렬 곱
	Matrix				matOffset;
	//미 사용
	Matrix				matBone;
	//프레임 정보
	vector<tMTKeyFrame> vecKeyFrame;
};

//애니메이션 정보
struct tMTAnimClip
{
	//애니메이션 이름
	wstring			strAnimName;
	//시작 프레임
	int				iStartFrame;
	//끝 프레임
	int				iEndFrame;
	//프레임 길이
	int				iFrameLength;
	//시작 시간
	double			dStartTime;
	//끝 시간
	double			dEndTime;
	//시간 길이
	double			dTimeLength;
	float			fUpdateTime;
	//애니메이션 프레임 정보
	FbxTime::EMode	eMode;
};

// ===========
// Instancing
// ===========
//인스턴싱 unorderd_map Key
union uInstID
{
	struct {
		UINT iMesh;
		WORD iMtrl;
		WORD iMtrlIdx;
	};
	ULONG64 llID;
};

class CGameObject;
//인스턴싱에서 사용
struct tInstObj
{
	//월드행렬, Animator3D 가져오는 용도
	CGameObject* pObj;
	//
	UINT		 iMtrlIdx;
};

struct tInstancingData
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
	int    iRowIdx;
};

extern tGlobalData g_global;
extern  tTransform g_transform;
extern  std::vector<tObjectRender>	g_vecInfoObject;