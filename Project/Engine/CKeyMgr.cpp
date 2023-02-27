#include "pch.h"
#include "CKeyMgr.h"
#include "CEngine.h"

int vkarr[(UINT)KEY::END]
=
{
	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	'A',
	'S',
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',
	'Z',
	'X',
	'C',
	'V',
	'B',
	'N',
	'M',

	VK_RETURN,
	VK_ESCAPE,
	VK_LSHIFT,
	VK_LMENU,
	VK_LCONTROL,
	VK_SPACE,

	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,

	VK_LBUTTON,
	VK_RBUTTON,

	VK_NUMPAD0,
	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,

	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
};


CKeyMgr::CKeyMgr()
{

}

CKeyMgr::~CKeyMgr()
{

}




void CKeyMgr::init()
{
	std::back_insert_iterator<vector<tKeyInfo>> inserter{ m_vecKey };

	for (UINT i = 0; i < (UINT)KEY::END; ++i)
	{
		*inserter = tKeyInfo{ (KEY)i, KEY_STATE::NONE, false };
	}
}

void CKeyMgr::tick()
{
	if (GetFocus())
	{
		for (UINT i = 0; i < (UINT)KEY::END; ++i)
		{
			// 해당키가 현재 눌려있다.
			if (GetAsyncKeyState(vkarr[i]) & 0x8000)
			{
				// 이전 프레임에도 눌려 있었다.
				if (m_vecKey[i].bPrevPress)
				{
					m_vecKey[i].eState = KEY_STATE::PRESS;
				}
				else
				{
					m_vecKey[i].eState = KEY_STATE::TAP;
				}

				m_vecKey[i].bPrevPress = true;
			}

			// 해당키가 현재 안눌려있다.
			else
			{
				// 이전 프레임에는 눌려 있었다.
				if (m_vecKey[i].bPrevPress)
				{
					m_vecKey[i].eState = KEY_STATE::RELEASE;
				}

				// 이전 프레임에도 안눌려 있었다.
				else
				{
					m_vecKey[i].eState = KEY_STATE::NONE;
				}

				m_vecKey[i].bPrevPress = false;
			}
		}
		m_vPrevMousePos = m_vMousePos;
		POINT ptMouse = {};
		GetCursorPos(&ptMouse);
		ScreenToClient(CEngine::GetInst()->GetMainHwnd(), &ptMouse);
		m_vMousePos.x = (float)ptMouse.x;
		m_vMousePos.y = (float)ptMouse.y;

		m_vMouseDir = m_vMousePos - m_vPrevMousePos;
		m_vMouseDir.y *= -1.f;
	}

	else
	{
		for (UINT i = 0; i < (UINT)KEY::END; ++i)
		{
			if (KEY_STATE::TAP == m_vecKey[i].eState || KEY_STATE::PRESS == m_vecKey[i].eState)
				m_vecKey[i].eState = KEY_STATE::RELEASE;
			else if (KEY_STATE::RELEASE == m_vecKey[i].eState)
				m_vecKey[i].eState = KEY_STATE::NONE;

			m_vecKey[i].bPrevPress = false;
		}
	}
}
