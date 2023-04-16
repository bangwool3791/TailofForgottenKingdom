#include "pch.h"
#include "AnimationUI.h"
#include "CGameObjectEx.h"

AnimationUI::AnimationUI()
	: UI("AnimationUI")
	, m_pParentObj{}
{

}

AnimationUI::~AnimationUI()
{
}

void AnimationUI::update()
{

}

void AnimationUI::render_update()
{
    assert(m_pParentObj);

    ImVec2 vListBoxSize{ 200.f, 200.f };
    static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };

    Render_Text(HSV_SKY_GREY, Vec2(230.f, 30.f), "Animation");

    ImGui::PushItemWidth(200.f);
    if (ImGui::InputText("Key##1", m_szAnimation, IM_ARRAYSIZE(m_szAnimation)))
        m_strInputKey = m_szAnimation;

    ImGui::SameLine();

    if(ImGui::Button("Play##1", ImVec2(40.f, 20.f)))
    {
        if (!m_strAniCurKey.empty())
        {
            wstring str = wstring(m_strAniCurKey.begin(), m_strAniCurKey.end());
            Play(str);
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Puase##1", ImVec2(40.f, 20.f)))
    {
        if (!m_strAniCurKey.empty())
        {
            wstring str = wstring(m_strAniCurKey.begin(), m_strAniCurKey.end());
            Pause();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Release##1", ImVec2(40.f, 20.f)))
    {
        if (!m_strAniCurKey.empty())
        {
            wstring str = wstring(m_strAniCurKey.begin(), m_strAniCurKey.end());
            Release();
        }
    }

    ImGui::SameLine();
    if (ImGui::Checkbox("Repeat##1", &m_tFrame.bRepeat))
    {
        Update_Engine_Frame();
    }

    if (ImGui::Button("Add Frame##1", ImVec2(80.f, 20.f)))
    {
        if (!m_strInputKey.empty())
        {
            wstring str = wstring(m_strInputKey.begin(), m_strInputKey.end());
            CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

            if (m_tFrame.iStart == 0 && m_tFrame.iEnd == 0)
            {
                MessageBox(nullptr, L"Start End Frame both 0", L"Error", MB_OK);;
            }
            else if (m_tFrame.iStart > m_tFrame.iEnd)
            {
                MessageBox(nullptr, L"Invalid Frame", L"Error", MB_OK);;
            }
            else
            {
                pAniCom->Add_Frame(str, m_tFrame);
                Play(str);
                Update_FrameListUI();
            }
        }
        else
        {
            MessageBox(nullptr, L"Key를 먼저 입력 하세요.", L"Error", MB_OK);;
        }
    }

    ImGui::SameLine();
    //Erase and Get New CurKey, Not 0 Frame
    if (ImGui::Button("Delete   ##1", ImVec2(80.f, 20.f)))
    {
        if (!m_strAniCurKey.empty())
        {
            wstring str = wstring(m_strAniCurKey.begin(), m_strAniCurKey.end());
            Delete(str);
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Add Frame All##1", ImVec2(100.f, 20.f)))
    {
        if (!m_strInputKey.empty())
        {
            vector<CAnimator3D*> vec{};

            wstring str = wstring(m_strInputKey.begin(), m_strInputKey.end());
            vec.push_back(m_pParentObj->Animator3D());

            const vector<CGameObject*>& childs = m_pParentObj->GetChilds();

            for (UINT i = 0; i < childs.size(); ++i)
            {
                if (childs[i]->Animator3D())
                    vec.push_back(childs[i]->Animator3D());
            }

            if (m_tFrame.iStart == 0 && m_tFrame.iEnd == 0)
            {
                MessageBox(nullptr, L"Start End Frame both 0", L"Error", MB_OK);;
            }
            else if (m_tFrame.iStart > m_tFrame.iEnd)
            {
                MessageBox(nullptr, L"Invalid Frame", L"Error", MB_OK);;
            }
            else
            {
                for (UINT i = 0; i < vec.size(); ++i)
                    vec[i]->Add_Frame(str, m_tFrame);
                
                PlayAll (str);
                Update_FrameListUI();
            }
        }
        else
        {
            MessageBox(nullptr, L"Key를 먼저 입력 하세요.", L"Error", MB_OK);;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete All##2", ImVec2(100.f, 20.f)))
    {
        if (!m_strAniCurKey.empty())
        {
            wstring str = wstring(m_strAniCurKey.begin(), m_strAniCurKey.end());

            DeleteAll(str);
        }
    }

    if (ImGui::BeginListBox("##KeyListBox", vListBoxSize))
    {
        //Ref String vector
        for (size_t i = 0; i < m_vecAniKey.size(); ++i)
        {
            bool Selectable = (m_iAniCurKey == i);
            if (ImGui::Selectable(m_vecAniKey[i].c_str(), Selectable))
            {
                //인덱스 하나더 만들고
                m_iAniCurKey = (UINT)i;
                m_strAniCurKey = m_vecAniKey[i];
                PlayAll(wstring(m_strAniCurKey.begin(), m_strAniCurKey.end()));
                Update_GUI_Frame();
            }

            if (Selectable)
            {
                ImGui::SetItemDefaultFocus();
            }

            // 해당 아이템이 더블클릭 되었다.
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
            }
        }

        ImGui::EndListBox();
    }

    ImGui::SameLine();

    if (ImGui::BeginListBox("##ChildsListBox", vListBoxSize))
    {
        //Ref String vector
        
        static vector<CGameObjectEx*> temp;

        const vector<CGameObject*>& vecChilds = m_pParentObj->GetChilds();

        temp.clear();
        vector<CGameObjectEx*> garbage;
        temp.swap(garbage);

        temp.push_back(m_pParentObj);

        for (UINT i = 0; i < vecChilds.size(); ++i)
            temp.push_back((CGameObjectEx*)vecChilds[i]);

        for (size_t i = 0; i < temp.size(); ++i)
        {
            bool Selectable = (m_iCurChild == i);
            
            string str = string(temp[i]->GetName().begin(), temp[i]->GetName().end());

            if (ImGui::Selectable(str.c_str(), Selectable))
            {
                CAnimator3D* pAnimation = temp[i]->Animator3D();
                if (nullptr != pAnimation)
                {
                    m_iCurChild = (UINT)i;
                    m_pCurrentObj = temp[i];
                    Update_FrameListUI();
                    Update_GUI_Frame();
                    PlayAll(wstring(m_strAniCurKey.begin(), m_strAniCurKey.end()));
                }
            }

            if (Selectable)
            {
                ImGui::SetItemDefaultFocus();
            }

            // 해당 아이템이 더블클릭 되었다.
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
            }
        }

        ImGui::EndListBox();
    }

    wstring str = wstring(m_strAniCurKey.begin(), m_strAniCurKey.end());
    CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

    int iCurFrame = pAniCom->GetCurFrameIdx();

    ImGui::PushItemWidth(500.f);
    if(!m_bPuase)
        ImGui::SliderInt("Cur Frame  ##1", (int*)&iCurFrame, 0.0f, m_iMaxFarme, "%d", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
    else
    {
        ImGui::SliderInt("Cur Frame  ##1", (int*)&iCurFrame, 0.0f, m_iMaxFarme, "%d");

        ImGui::SameLine();
        ImGui::PushItemWidth(200.f);

        ImGui::InputInt("Cur Frame##2", &iCurFrame, 1, 5);

        CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();
        pAniCom->SetCurFrameIdx(iCurFrame);
    }

    ImGui::PushItemWidth(500.f);
    if (ImGui::SliderInt("Start Frame##1", (int*)&m_tFrame.iStart, 0.0f, m_iMaxFarme))
    {
        //Update_Engine_Frame();
    }

    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
        ImGui::SetTooltip("%d", m_tFrame.iStart);      

    ImGui::SameLine();
    ImGui::PushItemWidth(200.f);
    if (ImGui::InputInt("Start Frame##2", &m_tFrame.iStart, 1, 5))
    {
        //Update_Engine_Frame();
    }

    ImGui::PushItemWidth(500.f);
    if (ImGui::SliderInt("End Frame  ##1", (int*)&m_tFrame.iEnd, 0.0f, m_iMaxFarme))
    {
       // Update_Engine_Frame();
    }

    if (ImGui::IsItemActive() || ImGui::IsItemHovered())    
        ImGui::SetTooltip("%d", m_tFrame.iEnd);
    
    ImGui::SameLine();
    ImGui::PushItemWidth(200.f);
    if (ImGui::InputInt("End Frame  ##2", &m_tFrame.iEnd, 1, 5))
    {
       // Update_Engine_Frame();
    }

    ImGui::PushItemWidth(200.f);
    if (ImGui::InputFloat("Time Scale##1", &m_fTimeScale, 0.1f, 0.5f))
    {
        SetTimeScale();
    }

}

#include "CEditor.h"

void AnimationUI::SetTargetObj(CGameObjectEx* _pObj)
{
    m_pCurrentObj = m_pParentObj = _pObj;

    if (0 < CEditor::GetInst()->Animation3D_Object_Size())
        CEditor::GetInst()->Delete_Animation3D_Object();
    
    CEditor::GetInst()->Add_Editobject(EDIT_MODE::ANIMATOR, _pObj);
    Update_FrameListUI();
    Update_GUI_Frame();

    wstring wstr = StringToWString(m_strAniCurKey);
    PlayAll(wstr);
}

void AnimationUI::Update_FrameListUI()
{
    static int iCount = 0;
    if (iCount > 0)
        assert(m_pParentObj);
    else
        ++iCount;

    m_vecAniKey.clear();

    CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

    m_iMaxFarme = pAniCom->GetEndFrame();
    assert(0 != m_iMaxFarme);

    const std::unordered_map<wstring, tAnim3DFrm>& mapFrames = pAniCom->GetFrames();

    if (0 < mapFrames.size())
    {
        m_strAniCurKey = string(mapFrames.begin()->first.begin(), mapFrames.begin()->first.end());
        ++m_iAniCurKey;
        for (auto iter{ mapFrames.begin() }; iter != mapFrames.end(); ++iter)
        {
            string str = string(iter->first.begin(), iter->first.end());
            m_vecAniKey.push_back(str);
        }
    }
    else if(0 == mapFrames.size())
    {
        m_tFrame.iStart = 0;
        m_tFrame.iEnd = m_iMaxFarme - 1;
        m_tFrame.bRepeat = true;
    }
    else
    {
        assert(false);
    }

    if (0 != m_iAniCurKey)
        m_iAniCurKey -= 1;
}

void AnimationUI::Render_Text(ImVec4 _vColor, Vec2 _vSize, string _str)
{
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, _vColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _vColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, _vColor);
    ImGui::Button(_str.c_str(), ImVec2(_vSize.x, _vSize.y));
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}

void AnimationUI::Play(const wstring& str)
{
    CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

    m_iMaxFarme = pAniCom->GetEndFrame();

    assert(0 != m_iMaxFarme);

    const std::unordered_map<wstring, tAnim3DFrm>& mapFrames = pAniCom->GetFrames();

    if (0 < mapFrames.size())
    {
        auto iter = mapFrames.begin();

        for (; iter != mapFrames.end(); ++iter)
        {
            if (!lstrcmp(str.c_str(), iter->first.c_str()))
            {
                m_strAniCurKey = string(iter->first.begin(), iter->first.end());
                pAniCom->SetCurFrameKey(iter->first);
                break;
            }
        }

        if (iter == mapFrames.end())
        {
            MessageBox(nullptr, L"키를 찾을 수 없습니다.", L"Warning", S_OK);
        }
    }
}

void AnimationUI::PlayAll(const wstring& str)
{
    vector<CAnimator3D*> vecAnimations{};

    vecAnimations.push_back(m_pParentObj->Animator3D());

    const vector<CGameObject*> vecChilds = m_pParentObj->GetChilds();

    for (UINT i = 0; i < vecChilds.size(); ++i)
    {
        CAnimator3D* pAniCom = vecChilds[i]->Animator3D();

        if (nullptr != pAniCom)
            vecAnimations.push_back(pAniCom);
    }

    for (UINT i = 0; i < vecAnimations.size(); ++i)
        vecAnimations[i]->SetCurFrameKey(str);
}

void AnimationUI::Pause()
{
    CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

    assert(0 != m_iMaxFarme);

    pAniCom->SetPuase(true);

    m_bPuase = true;
}

void AnimationUI::Release()
{
    CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

    assert(0 != m_iMaxFarme);

    pAniCom->SetPuase(false);

    m_bPuase = false;
}

void AnimationUI::Delete(const wstring& str)
{
    CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

    assert(0 != m_iMaxFarme);

    const std::unordered_map<wstring, tAnim3DFrm>& mapFrames = pAniCom->GetFrames();

    if (0 < mapFrames.size() && 1 != mapFrames.size())
    {
        auto iter = mapFrames.begin();

        bool bCheck = false;
        for (; iter != mapFrames.end(); ++iter)
        {
            if (!lstrcmp(str.c_str(), iter->first.c_str()))
            {
                bCheck = true;
                m_strAniCurKey = string(iter->first.begin(), iter->first.end());
                pAniCom->Delete(str);
                break;
            }
        }

        if (false == bCheck)
            MessageBox(nullptr, L"키를 찾을 수 없습니다.", L"Warning", S_OK);

        //Ui List 삭제
        for (auto iter = m_vecAniKey.begin(); iter != m_vecAniKey.end(); ++iter)
        {
            wstring temp = wstring((*iter).begin(), (*iter).end());
            if (!lstrcmp(temp.c_str(), str.c_str()))
            {
                m_vecAniKey.erase(iter);
                break;
            }
        }
    }
}

void AnimationUI::DeleteAll(const wstring& str)
{
    vector<CAnimator3D*> vec{};

    vec.push_back(m_pParentObj->Animator3D());

    const vector<CGameObject*>& childs = m_pParentObj->GetChilds();

    for (UINT i = 0; i < childs.size(); ++i)
    {
        if (childs[i]->Animator3D())
            vec.push_back(childs[i]->Animator3D());
    }

    for (UINT i = 0; i < vec.size(); ++i)
        m_strAniCurKey = vec[i]->Delete(str);
}

void AnimationUI::SetTimeScale()
{
    CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

    pAniCom->SetTimeScale(m_fTimeScale);
}

void AnimationUI::Update_Engine_Frame()
{
    if (!m_strAniCurKey.empty())
    {
        wstring str = wstring(m_strAniCurKey.begin(), m_strAniCurKey.end());
        CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

        pAniCom->SetCurFrame(str, m_tFrame);
        Play(str);
    }
    else
    {
        MessageBox(nullptr, L"키를 먼저 선택하세요.", L"Warning", S_OK);
    }
}

void AnimationUI::Update_GUI_Frame()
{
    CAnimator3D* pAniCom = m_pCurrentObj->Animator3D();

    const std::unordered_map<wstring, tAnim3DFrm>& map = pAniCom->GetFrames();

    for (auto iter = map.begin(); iter != map.end(); ++iter)
    {
        if (!lstrcmp(iter->first.c_str(), wstring(m_strAniCurKey.begin(), m_strAniCurKey.end()).c_str()))
        {
            m_tFrame = iter->second;
            m_iMaxFarme = pAniCom->GetEndFrame() - 1;
        }
    }
}