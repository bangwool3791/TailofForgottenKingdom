#pragma once


// 키 입력 관련 문제
// 1. 키 입력 사용이 편하게
// 2. 키 이벤트 체크 가능(눌린 순간, 떼는 순간)
// 3. 프레임 동기화
// 4. 프로세스 동기화( 프로세스가 포커싱이 아닐 때 입력 방지)

enum class KEY
{
	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	I,
	O,
	P,

	A,
	S,
	D,
	F,
	G,
	H,
	J,
	K,
	L,

	Z,
	X,
	C,
	V,
	B,
	N,
	M,

	ENTER,
	ESC,
	LSHIFT,
	LALT,
	LCTRL,
	SPACE,

	LEFT,
	RIGHT,
	UP,
	DOWN,

	LBTN,
	RBTN,

	NUM_0,
	NUM_1,
	NUM_2,
	NUM_3,
	NUM_4,
	NUM_5,
	NUM_6,
	NUM_7,
	NUM_8,
	NUM_9,

	N_0,
	N_1,
	N_2,
	N_3,
	N_4,
	N_5,
	N_6,
	N_7,
	N_8,
	N_9,


	END,
};



enum KEY_STATE
{
	TAP,
	PRESS,
	RELEASE,
	NONE,
};


struct tKeyInfo
{
	KEY  eType;
	KEY_STATE eState;
	bool	  bPrevPress;
};


class CKeyMgr
	:public CSingleton<CKeyMgr>
{
private:
	vector<tKeyInfo>	m_vecKey;
	Vec2				m_vMousePos;
	Vec2				m_vPrevMousePos;
	Vec2				m_vMouseDir;

public:
	void init();
	void tick();

public:
	KEY_STATE GetKeyState(KEY _key)
	{
		return m_vecKey[(UINT)_key].eState;
	}

	Vec2  GetMousePos() { return m_vMousePos; }
	Vec2&  GetMousePos_() { return m_vMousePos; }
	Vec2  GetMouseDir() { return m_vMouseDir; }
	Vec2& GetMouseDir_() { return m_vMouseDir; }
private:
	CKeyMgr();
	virtual ~CKeyMgr();
	friend class CSingleton<CKeyMgr>;
};

