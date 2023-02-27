#include "pch.h"
#include "EditAnimationUI.h"

#include <Engine\CDevice.h>
#include <Engine/CResMgr.h>
#include <Engine/CTimeMgr.h>
#include <Engine\CCamera.h>
#include <Engine\CKeyMgr.h>
#include <Engine\CResMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\CMeshRender.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CTransform.h>

#include <Script\CEditorMouseScript.h>
#include "CEditor.h"

#include "CImGuiMgr.h"
#include "ListUI.h"

#include "CGameObjectEx.h"

static const int dx[] = { -1, 0, 1, 0 };
static const int dy[] = { 0, 1, 0, -1 };
#define HSV_SKY_GREY ImColor::HSV(0.f, 0.f, 0.5f)
/*
* Editor
* Rect Mesh Link
* * UI 클릭 ->
* * Start Pos, End Pos
* Animator Link
* * Pixel Get
*/
EditAnimationUI::EditAnimationUI()
    : ComponentUI("Animator2D", COMPONENT_TYPE::ANIMATOR2D)
    , m_fMaxX{}
    , m_fMaxY{}
    , m_fMinX{}
    , m_fMinY{}
    , m_iSelectedIdx{}
    , m_MouseObject{}
    , m_bfs_visited{}
    , m_dfs_visited{}
    , m_fDuration{}
    , m_fileDialogInfo{}
    , m_iCurIdx{}
    , m_iPixel_Height{}
    , m_iPixel_Width{}
    , m_pAnimator{}
    , m_pCameraObject{}
    , m_pGameObject{}
    , m_pImage{}
    , m_pPixels{}
    , m_pPointObject{}
    , m_szAnimation{}
    , m_fileDialogOpen{}
    , m_eEditMode{ EDIT_ANIMATION_MODE::SCEN_MODE }
{
}

EditAnimationUI::~EditAnimationUI()
{
    CEditor::GetInst()->PopByName(L"Dummy Object");

    for (int i{}; i < m_iPixel_Height; ++i)
    {
        delete* (m_dfs_visited + i);
        delete* (m_bfs_visited + i);
    }
    delete m_dfs_visited;
    delete m_bfs_visited;
}
#include <string.h>
#include "TreeUI.h"
#include <stack>

void EditAnimationUI::begin()
{
    Initialize_Animation_Info();

    Initialize_Aimation_Pixel();

    m_MouseObject = CEditor::GetInst()->FindByName(L"MouseObject");
    m_pCameraObject = CEditor::GetInst()->FindByName(L"Editor Camera");
    m_pPointObject = CEditor::GetInst()->FindByName(L"AnimationToolDrag");
}

void EditAnimationUI::update()
{
    if (m_pGameObject && m_pGameObject->IsDead())
        m_pGameObject = nullptr;

    ComponentUI::update();
}

void EditAnimationUI::render_update()
{
    ComponentUI::render_update();

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##OutlinerTree"))
        {
            TreeNode* pNode = (TreeNode*)payload->Data;
            m_pGameObject = (CGameObject*)pNode->GetData();

            if (!m_pGameObject->Transform())
            {
                m_pGameObject = nullptr;
                return;
            }
            if (!m_pGameObject->Animator2D())
            {
                m_pGameObject = nullptr;
                return;
            }
            if (!m_pGameObject->MeshRender())
            {
                m_pGameObject = nullptr;
                return;
            }

            if (dynamic_cast<CGameObject*>(m_pGameObject))
            {
                m_pAnimator = dynamic_cast<CGameObject*>(m_pGameObject)->Animator2D();

                if (nullptr == m_pAnimator)
                {
                   // cout << "Dummy Object has not Animation2D Component" << endl;
                    ImGui::EndDragDropTarget();
                    return;
                }

                m_pAtlasTexture = m_pAnimator->GetTexture();
                float width = (float)m_pAtlasTexture->GetWidth();
                float height = (float)m_pAtlasTexture->GetHeight();
                Refresh_Animation(width, (float)m_pAtlasTexture->GetHeight());
                m_pImage = m_pAtlasTexture->GetSRV().Get();
                //더미 오브젝트 넣고, 툴 환시 더미 오브젝트 빼기 ?
                CEditor::GetInst()->UpdateAnimationObject(m_pGameObject);
                CGameObjectEx* pGameObject = CEditor::GetInst()->FindByName(L"AnimationTool");
                pGameObject->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_PARAM::TEX_0, m_pAtlasTexture);

                const vector<wstring>& vec = m_pAnimator->GetKeys();

                m_vecAnimation.clear();

                for (size_t i{}; i < vec.size(); ++i)
                {
                    m_vecAnimation.push_back(string(vec[i].begin(), vec[i].end()));
                }
                Initialize_Aimation_Pixel();
            }
        }
        ImGui::EndDragDropTarget();
    }

    Render_Text(HSV_SKY_GREY, Vec2(230.f, 30.f), "Texture");

    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.f, 0.1f));

    if (ImGui::Button("Open", ImVec2(65.f, 40.f)))
    {
        m_fileDialogOpen = true;
        m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
        m_fileDialogInfo.title = "Open File";
        m_fileDialogInfo.fileName = "test.png";
        //m_fileDialogInfo.directoryPath = std::filesystem::current_path();
        string path = CPathMgr::GetInst()->GetSingleContentPath();
        std::filesystem::path _path(path + "\\texture");
        m_fileDialogInfo.directoryPath = _path;
        //m_fileDialogInfo.directoryPath.filename().append("\\content\\texture");
    }

    if (m_fileDialogOpen)
    {
        // Any place in draw loop
        if (ImGui::FileDialog(&m_fileDialogOpen, &m_fileDialogInfo))
        {
            SetTextureUI();
        }

    }

    ImGui::SameLine();
    if (ImGui::Button("Sprite\r\nMode", ImVec2(65.f, 40.f)))
        m_eEditMode = EDIT_ANIMATION_MODE::SPRITE_MODE;

    ImGui::SameLine();

    if (ImGui::Button("Scene\r\nMode", ImVec2(65.f, 40.f)))
        m_eEditMode = EDIT_ANIMATION_MODE::SCEN_MODE;

    ImGui::PopStyleColor(3);
    ImGui::PopID();

    Vec2 vMinUV = m_uvLeftTop - m_uvDiff;
    Vec2 vMaxUV = m_uvRightBottom + m_uvDiff;

    Render_Pos();

    Render_Text(HSV_SKY_GREY, Vec2(230.f, 30.f), "FullSize");
    ImGui::Image(m_pImage, ImVec2(212.f, 212.f), ImVec2(vMinUV.x, vMinUV.y), ImVec2(vMaxUV.x, vMaxUV.y));
    Render_Text(HSV_SKY_GREY, Vec2(230.f, 30.f), "UV");
    ImGui::Image(m_pImage, ImVec2(212.f, 212.f), ImVec2(m_uvLeftTop.x, m_uvLeftTop.y), ImVec2(m_uvRightBottom.x, m_uvRightBottom.y));

    float f = 0.f;
    Render_Text(HSV_SKY_GREY, Vec2(230.f, 30.f), "Frame");

    ImGui::Button("Pos", ImVec2(40.f, 20.f)); ImGui::SameLine();
    ImGui::Button("x", ImVec2(20.f, 20.f));
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::InputFloat("##1", &m_vLeftTop.x))
        m_pAnimator->SetLeftTopX(m_vLeftTop.x, m_iCurIdx);
    ImGui::SameLine();
    ImGui::Button("y", ImVec2(20.f, 20.f));
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::InputFloat("##2", &m_vLeftTop.y))
        m_pAnimator->SetLeftTopY(m_vLeftTop.y, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("Apply##1", ImVec2(40.f, 20.f)))
        m_pAnimator->SetLeftTop(m_vLeftTop, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("All##1", ImVec2(40.f, 20.f)))
        for (int i{}; i < m_vecFrameIndex.size(); ++i)
            m_pAnimator->SetLeftTop(m_vLeftTop, i);

    ImGui::Button("Slice", ImVec2(40.f, 20.f));
    ImGui::SameLine();
    ImGui::Button("x", ImVec2(20.f, 20.f));
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::InputFloat("##3", &m_vSlice.x))
        m_pAnimator->SetSliceX(m_vSlice.x, m_iCurIdx);
    ImGui::SameLine();
    ImGui::Button("y", ImVec2(20.f, 20.f));
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::InputFloat("##4", &m_vSlice.y))
        m_pAnimator->SetSliceY(m_vSlice.y, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("Apply##2", ImVec2(40.f, 20.f)))
        m_pAnimator->SetSlice(m_vSlice, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("All##2", ImVec2(40.f, 20.f)))
        for (int i{}; i < m_vecFrameIndex.size(); ++i)
            m_pAnimator->SetSlice(m_vSlice, i);


    if (ImGui::Button("FullSize", ImVec2(40.f, 20.f)))
        m_pAnimator->SetFullSize(m_vFullSize, m_iCurIdx);
    ImGui::SameLine();
    ImGui::Button("x", ImVec2(20.f, 20.f));
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::InputFloat("##6", &m_vFullSize.x))
        m_pAnimator->SetFullSizeX(m_vFullSize.x, m_iCurIdx);
    ImGui::SameLine();
    ImGui::Button("y", ImVec2(20.f, 20.f));
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::InputFloat("##7", &m_vFullSize.y))
        m_pAnimator->SetFullSizeY(m_vFullSize.y, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("Apply##3", ImVec2(40.f, 20.f)))
        m_pAnimator->SetFullSize(m_vFullSize, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("All##3", ImVec2(40.f, 20.f)))
        for (int i{}; i < m_vecFrameIndex.size(); ++i)
            m_pAnimator->SetFullSize(m_vFullSize, i);

    ImGui::Button("OffSet", ImVec2(40.f, 20.f));
    ImGui::SameLine();
    ImGui::Button("x", ImVec2(20.f, 20.f));
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::InputFloat("##8", &m_vOffset.x))
        m_pAnimator->SetOffsetX(m_vOffset.x, m_iCurIdx);
    ImGui::SameLine();
    ImGui::Button("y", ImVec2(20.f, 20.f));
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::InputFloat("##9", &m_vOffset.y))
        m_pAnimator->SetOffsetY(m_vOffset.y, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("Apply##4", ImVec2(40.f, 20.f)))
        m_pAnimator->SetOffset(m_vOffset, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("All##4", ImVec2(40.f, 20.f)))
        for (int i{}; i < m_vecFrameIndex.size(); ++i)
            m_pAnimator->SetOffset(m_vOffset, i);

    ImGui::Button("Duration", ImVec2(40.f, 20.f));
    ImGui::SameLine();
    if (ImGui::InputFloat("##10", &m_fDuration))
        m_pAnimator->SetDuration(m_fDuration, m_iCurIdx);
    ImGui::SameLine();
    ImGui::PushItemWidth(50.f);
    if (ImGui::Button("All##5", ImVec2(40.f, 20.f)))
        for (int i{}; i < m_vecFrameIndex.size(); ++i)
            m_pAnimator->SetDuration(m_fDuration, i);

    ImGui::Separator();
    if (ImGui::Button("Add##1", ImVec2(40.f, 20.f)))
    {
        m_iCurIdx = m_pAnimator->Add_Animation2D(m_vLeftTop, m_vSlice, m_fDuration, m_vFullSize);
        Refresh_Animation((float)m_pAtlasTexture->GetWidth(), (float)m_pAtlasTexture->GetHeight());
    }

    ImGui::SameLine();
    if (ImGui::Button("Edit##1", ImVec2(40.f, 20.f)))
        m_pAnimator->SetState(ANIMATION_STATE::PAUSE);

    ImGui::SameLine();
    if (ImGui::Button("Del##1", ImVec2(40.f, 20.f)))
    {
        if(m_iCurIdx >= 0)
            m_iCurIdx = m_pAnimator->Delete_Animation2D(m_iCurIdx);

        Refresh_Animation((float)m_pAtlasTexture->GetWidth(), (float)m_pAtlasTexture->GetHeight());
    }
    ImGui::SameLine();
    if (ImGui::Button("Play", ImVec2(40.f, 20.f)))
        m_pAnimator->SetState(ANIMATION_STATE::PLAY);

    ImVec2 vSize{ 200.f, 200.f };
    if (ImGui::BeginListBox("##ListBox1", vSize))
    {
        for (size_t i = 0; i < m_vecFrameIndex.size(); ++i)
        {
            bool Selectable = (m_iCurIdx == i);
            if (ImGui::Selectable(m_vecFrameIndex[i].c_str(), Selectable))
            {
                m_iCurIdx = (int)i;
                Vec2 vLeftTop = m_pAnimator->GetLeftTop(m_iCurIdx);
                Vec2 vSlice = m_pAnimator->GetSlice(m_iCurIdx);
                m_uvLeftTop.x = vLeftTop.x / m_iPixel_Width;
                m_uvLeftTop.y = vLeftTop.y / m_iPixel_Height;
                m_uvRightBottom.x = (vLeftTop.x + vSlice.x) / m_iPixel_Width;
                m_uvRightBottom.y = (vLeftTop.y + vSlice.y) / m_iPixel_Height;
            }

            if (Selectable)
                ImGui::SetItemDefaultFocus();

            // 해당 아이템이 더블클릭 되었다.
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                strcpy_s(m_szAnimation, m_vecAnimation[m_iCurIdx].c_str());
                m_strAnimationName = m_vecAnimation[m_iCurIdx];
            }
        }

        ImGui::EndListBox();
    }

    Render_Text(HSV_SKY_GREY, Vec2(230.f, 30.f), "Animation");

    if (ImGui::InputText("##5", m_szAnimation, IM_ARRAYSIZE(m_szAnimation)))
        m_strEditName = m_szAnimation;

    ImGui::SameLine();
    if (ImGui::Button("Add##2", ImVec2(40.f, 20.f)))
    {
        if (!m_strAnimationName.empty())
        {
            //const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS
            wstring strKey = wstring(m_strAnimationName.begin(), m_strAnimationName.end());
            CAnimation2D* pAnimator = m_pAnimator->Add_Animation(strKey, m_pAtlasTexture, m_vLeftTop, m_vSlice, 0, (int)m_vecFrameIndex.size(), 1.f / m_fDuration);

            if (nullptr != pAnimator)
            {
                m_pAnimator->Play(pAnimator->GetName(), m_pAnimator->GetRepeat());
                m_strAnimationName = string(pAnimator->GetName().begin(), pAnimator->GetName().end());

                if (!m_strAnimationName.empty())
                {
                    strcpy_s(m_szAnimation, m_strAnimationName.c_str());
                    m_vecAnimation.push_back(m_strAnimationName);
                    ++m_iSelectedIdx;
                }
            }
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Del##2", ImVec2(40.f, 20.f)))
    {
        if (!m_strAnimationName.empty() && 1 < m_vecAnimation.size())
        {
            CAnimation2D* pAnimator = m_pAnimator->Delete_Animation(wstring(m_strAnimationName.begin(), m_strAnimationName.end()));

            if (nullptr != pAnimator)
            {
                m_pAnimator->Play(pAnimator->GetName(), m_pAnimator->GetRepeat());

                for (auto iter{ m_vecAnimation.begin() }; iter != m_vecAnimation.end();)
                {
                    if (*iter == m_strAnimationName)
                    {
                        iter = m_vecAnimation.erase(iter);
                        break;
                    }
                    else
                    {
                        ++iter;
                    }
                }

                /*추가 애니메이션 텍스쳐 자동 업데이트*/
                m_pAtlasTexture = m_pAnimator->GetTexture();
                m_pImage = m_pAtlasTexture->GetSRV().Get();
                CGameObjectEx* pGameObject = CEditor::GetInst()->FindByName(L"AnimationTool");
                pGameObject->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_PARAM::TEX_0, m_pAtlasTexture);
                Initialize_Aimation_Pixel();

                decltype(m_pAnimator->GetFames()) vecFrames = m_pAnimator->GetFames();

                assert(vecFrames.size());

                m_vecFrameIndex.clear();

                for (int i{}; i < vecFrames.size(); ++i)
                {
                    if (10 > i)
                    {
                        m_vecFrameIndex.push_back("0" + std::to_string(i));
                    }
                    else
                    {
                        m_vecFrameIndex.push_back(std::to_string(i));
                    }
                }

                if (vecFrames.size() > 0)
                    m_iCurIdx = (int)vecFrames.size() - 1;
                else
                    m_iCurIdx = 0;

                if (!m_strAnimationName.empty())
                {
                    strcpy_s(m_szAnimation, m_strAnimationName.c_str());

                    if (m_vecAnimation.size() > 0 && m_iSelectedIdx != 0)
                        --m_iSelectedIdx;
                }
            }
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Edit##2", ImVec2(40.f, 20.f)))
    {
        if (!m_strEditName.empty())
        {
            CAnimation2D* pAnimator = m_pAnimator->EditAnimation(wstring(m_strAnimationName.begin(), m_strAnimationName.end()), wstring(m_strEditName.begin(), m_strEditName.end()));
            
            for (auto iter{ m_vecAnimation.begin() }; iter != m_vecAnimation.end();)
            {
                if (!strcmp(m_strAnimationName.data(), iter->data()))
                {
                    iter = m_vecAnimation.erase(iter);
                    break;
                }
                else
                {
                    ++iter;
                }
            }

            m_strAnimationName = string(pAnimator->GetName().begin(), pAnimator->GetName().end());

            if (!m_strAnimationName.empty())
            {
                m_vecAnimation.push_back(m_strAnimationName);
                strcpy_s(m_szAnimation, m_strAnimationName.c_str());
            }
        }
    }

    if (ImGui::Button("Clone##1", ImVec2(40.f, 20.f)))
    {
        if (!m_strFileData.empty())
        {
            m_pAnimator->CloneAnimation(m_strFileData, *m_pAnimator);

            m_vecAnimation.clear();
            const vector<wstring> vec =  m_pAnimator->GetKeys();

            for (size_t i{}; i < vec.size(); ++i)
            {
                m_vecAnimation.push_back(string(vec[i].begin(), vec[i].end()));
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Copy##1", ImVec2(40.f, 20.f)))
    {
        if (!m_strFileData.empty())
        {
            m_pAnimator->CopyAnimation(m_strFileData, *m_pAnimator);

            string strTemp = string(m_strFileData.begin(), m_strFileData.end());
            auto iter = m_vecAnimation.begin();

            for (; iter != m_vecAnimation.end(); ++iter)
            {
                if (!strcmp(iter->c_str(), strTemp.c_str()))
                    break;
            }

            if (iter == m_vecAnimation.end())
                m_vecAnimation.push_back(strTemp);
        }
    }

    if (ImGui::BeginListBox("##ListBox2", vSize))
    {
        //Ref String vector
        for (size_t i = 0; i < m_vecFileData.size(); ++i)
        {
            bool Selectable = (m_iFileData == i);
            if (ImGui::Selectable(m_vecFileData[i].c_str(), Selectable))
            {
                //인덱스 하나더 만들고
                m_iFileData = (int)i;
                m_strFileData = wstring(m_vecFileData[i].begin(), m_vecFileData[i].end());
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

    if (ImGui::BeginListBox("##ListBox3", vSize))
    {
        for (size_t i = 0; i < m_vecAnimation.size(); ++i)
        {
            bool Selectable = (m_iSelectedIdx == i);
            if (ImGui::Selectable(m_vecAnimation[i].c_str(), Selectable))
            {
                m_iSelectedIdx = (int)i;
                m_strAnimationName = m_vecAnimation[i];
                strcpy_s(m_szAnimation, m_vecAnimation[i].c_str());
                m_pAnimator->Play(wstring(m_vecAnimation[i].begin(), m_vecAnimation[i].end()), m_pAnimator->GetRepeat());

                /*추가 애니메이션 텍스쳐 자동 업데이트*/
                m_pAtlasTexture = m_pAnimator->GetTexture();
                m_pImage = m_pAtlasTexture->GetSRV().Get();
                CGameObjectEx* pGameObject = CEditor::GetInst()->FindByName(L"AnimationTool");
                pGameObject->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_PARAM::TEX_0, m_pAtlasTexture);
                Initialize_Aimation_Pixel();

                decltype(m_pAnimator->GetFames()) vecFrames = m_pAnimator->GetFames();

                assert(vecFrames.size());

                m_vecFrameIndex.clear();

                for (int i{}; i < vecFrames.size(); ++i)
                {
                    if (10 > i)
                    {
                        m_vecFrameIndex.push_back("0" + std::to_string(i));
                    }
                    else
                    {
                        m_vecFrameIndex.push_back(std::to_string(i));
                    }
                }

                if (vecFrames.size() > 0)
                    m_iCurIdx = (int)vecFrames.size() - 1;
                else
                    m_iCurIdx = 0;
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

    static float fDeltaTime = DT;
    static bool bCheck = false;
    fDeltaTime += DT;

    if (EDIT_ANIMATION_MODE::SPRITE_MODE == m_eEditMode)
    {
        if (0.3f < fDeltaTime)
        {
            if (KEY_PRESSED(KEY::LBTN))
            {
                if (Click_Pixel_LBtn())
                    bCheck = true;
                fDeltaTime = fDeltaTime - (fDeltaTime / 0.3f) * 0.3f;
            }

            if (KEY_PRESSED(KEY::LEFT))
            {
                Click_Pixel_KeyBoard(KEY::LEFT);
                fDeltaTime = fDeltaTime - (fDeltaTime / 0.3f) * 0.3f;
                bCheck = true;
            }
            else if (KEY_PRESSED(KEY::RIGHT))
            {
                Click_Pixel_KeyBoard(KEY::RIGHT);
                fDeltaTime = fDeltaTime - (fDeltaTime / 0.3f) * 0.3f;
                bCheck = true;
            }
            else if (KEY_PRESSED(KEY::UP))
            {
                Click_Pixel_KeyBoard(KEY::UP);
                fDeltaTime = fDeltaTime - (fDeltaTime / 0.3f) * 0.3f;
                bCheck = true;
            }
            else if (KEY_PRESSED(KEY::DOWN))
            {
                Click_Pixel_KeyBoard(KEY::DOWN);
                fDeltaTime = fDeltaTime - (fDeltaTime / 0.3f) * 0.3f;
                bCheck = true;
            }
        }

        if (bCheck)
        {
            m_pAnimator->SetLeftTop(Vec2(m_fMinX, m_fMinY), m_iCurIdx);
            m_pAnimator->SetSlice(Vec2(m_fMaxX - m_fMinX, m_fMaxY - m_fMinY), m_iCurIdx);
            //두개의 Vec2 멤버
           //클릭시 처리
           //리스트 클릭 시 처리
            m_uvLeftTop = Vec2{ (float)m_fMinX / m_iPixel_Width ,m_fMinY / m_iPixel_Height };
            m_uvRightBottom = Vec2{ (float)m_fMaxX / m_iPixel_Width ,m_fMaxY / m_iPixel_Height };

            m_uvSlice = Vec2{ abs(m_fMaxX - m_fMinX),abs(m_fMaxY - m_fMinY) };
            bCheck = false;
        }
    }
}

void EditAnimationUI::Click_Pixel_KeyBoard(KEY _Key)
{
    if (_Key == KEY::LEFT || _Key == KEY::RIGHT || _Key == KEY::UP || _Key == KEY::DOWN)
    {
        static int x{};
        static int y{};

        if (_Key == KEY::LEFT)
        {
            x = (int)m_pPointObject->Transform()->GetRelativePos().x - (int)m_pPointObject->Transform()->GetRelativeScale().x;
            y = (int)m_pPointObject->Transform()->GetRelativePos().z;
        }
        else if (_Key == KEY::RIGHT)
        {
            x = (int)m_pPointObject->Transform()->GetRelativePos().x + (int)m_pPointObject->Transform()->GetRelativeScale().x;
            y = (int)m_pPointObject->Transform()->GetRelativePos().z;
        }
        else if (_Key == KEY::UP)
        {
            x = (int)m_pPointObject->Transform()->GetRelativePos().x;
            y = (int)m_pPointObject->Transform()->GetRelativePos().z - (int)m_pPointObject->Transform()->GetRelativeScale().z;
        }
        else if (_Key == KEY::DOWN)
        {
            x = (int)m_pPointObject->Transform()->GetRelativePos().x + (int)m_pPointObject->Transform()->GetRelativeScale().x;
            y = (int)m_pPointObject->Transform()->GetRelativePos().z + (int)m_pPointObject->Transform()->GetRelativeScale().z;
        }

        UINT _ux = (UINT)(x + m_iPixel_Width * 0.5f);
        UINT _uy = (UINT)(-1.f * y + m_iPixel_Height * 0.5f);

        Set_Texture_Pixel(_ux, _uy);
    }
}

bool EditAnimationUI::Click_Pixel_LBtn()
{
    CGameObjectEx* pGameObject = CEditor::GetInst()->FindByName(L"AnimationTool");

    Ray _ray = m_MouseObject->GetScript<CEditorMouseScript>()->GetRay();

    Vec3 vCameraPos = m_pCameraObject->Transform()->GetRelativePos();

    Vec3 vMousePos = pGameObject->Transform()->Picking(_ray);

    if (vMousePos.x == -1.f && vMousePos.y == -1.f && vMousePos.z == -1.f)
        return false;

    BoundingFrustum fr(m_pCameraObject->Camera()->GetProjMat());

    //[23.02.03] 수정
    Vec2 vRes = CDevice::GetInst()->GetRenderResolution();
    float fDelta = m_pCameraObject->Camera()->GetFar() - m_pCameraObject->Camera()->GetNear();
    BoundingBox box(vMousePos - vCameraPos, Vec3(vRes.x, vRes.y, fDelta));


    if (!fr.Contains(box))
    {
        return false;
    }
    else
    {
        int b = 0;
    }
    //cout << "[X] : " << vMousePos.x << " [Y] : " << vMousePos.y << endl;
    Vec3 vPos = vMousePos;

     Vec3 vSacle{};

     Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
     vResolution *= m_pCameraObject->Camera()->GetOrthographicScale();

     vSacle.x = m_pCameraObject->Camera()->Transform()->GetRelativePos().x;
     vSacle.z = m_pCameraObject->Camera()->Transform()->GetRelativePos().z;
    
    //vMousePos *= m_pCameraObject->Camera()->GetOrthographicScale();
    //vPos.x = vMousePos.x + vResolution.x * 0.5f;
    //vPos.y = -1.f * vMousePos.y + vResolution.y * 0.5f;

    vPos.x += m_iPixel_Width * 0.5f;
    vPos.z = -1.f * vMousePos.z + m_iPixel_Height * 0.5f;

    if (0 > vPos.x || 0 > vPos.z || m_iPixel_Width <= vPos.x || m_iPixel_Height <= vPos.z)
        return false;

    tRGBA tInfo = GetRGBA((int)vPos.x, (int)vPos.z);

    cout << "[X] : " << vMousePos.x << " [Y] : " << vMousePos.z << endl;
    cout << "R : " << tInfo.R << " G : " << tInfo.G << " B : " << tInfo.B << " A : " << tInfo.A << endl;


    UINT x = (UINT)vPos.x;
    UINT y = (UINT)vPos.z;
    Set_Texture_Pixel(x, y);
    return true;

}

void EditAnimationUI::Set_Texture_Pixel(UINT x, UINT y)
{
    int index = 4 * (x + y * m_iPixel_Width);

    if (true == (m_pPixels[index] == 0.f &&
        m_pPixels[index + 1] == 0.f &&
        m_pPixels[index + 2] == 0.f && m_pPixels[index + 3] == 0))
    {
        //bfs사용
        Vec2 vPixel = Get_Pixel_Bfs(x, y);

        if (vPixel.x == -1.f && vPixel.y == -1.f)
            return;

        x = (UINT)vPixel.x;
        y = (UINT)vPixel.y;
        return;
    }

    float fMinX = (float)m_iPixel_Width;
    float fMinY = (float)m_iPixel_Height;

    float fMaxX = -1;
    float fMaxY = -1;

    static std::stack<std::pair<int, int>> StackPiexel;
    StackPiexel.push(make_pair(x, y)); // pair를 만들어서 stack에 넣습니다.

    // 처음 x,y를 방문 했기때문에
    if (0 > x || 0 > y || (UINT)m_iPixel_Width <= x || (UINT)m_iPixel_Height <= y)
        return;

    m_dfs_visited[x][y] = true;

    while (!StackPiexel.empty()) {

        x = StackPiexel.top().first;
        y = StackPiexel.top().second;
        StackPiexel.pop();

        // 해당 위치의 주변을 확인
        for (int i = 0; i < 4; i++)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];

            // 지도를 벗어나지 않고
            if (0 <= nx && nx < m_iPixel_Width && 0 <= ny && ny < m_iPixel_Height)
            {
                // 방문하지 않았다면 -> 방문
                if (m_dfs_visited[ny][nx] == false)
                {
                    m_dfs_visited[ny][nx] = true;

                    int index = 4 * (nx + ny * m_iPixel_Width);
                    if (false == (m_pPixels[index] == 0.f &&
                        m_pPixels[index + 1] == 0.f &&
                        m_pPixels[index + 2] == 0.f && m_pPixels[index + 3] == 0))
                    {
                        if (nx < fMinX)
                            fMinX = (float)nx;

                        if (ny < fMinY)
                            fMinY = (float)ny;

                        if (nx > fMaxX) {
                            fMaxX = (float)nx;
                        }

                        if (ny > fMaxY)
                            fMaxY = (float)ny;

                        StackPiexel.push(make_pair(x, y)); // push하는 경우이기 때문에 현재 위치도 넣어준다.
                        StackPiexel.push(make_pair(nx, ny)); // 스택에 현재 nx,ny도 푸시
                    }
                }
            }
        }
    }

    fMinX = fMinX - m_iPixel_Width * 0.5f;
    fMaxX = fMaxX - m_iPixel_Width * 0.5f;

    fMinY = (fMinY - m_iPixel_Height * 0.5f) * -1.f;
    fMaxY = (fMaxY - m_iPixel_Height * 0.5f) * -1.f;

    cout << "m_minx " << fMinX << "m_miny " << fMinY << "m_maxx " << fMaxX << "m_maxy " << fMaxY << endl;
    Vec3 vPos = Vec3{ (fMaxX + fMinX) * 0.5f, 0.f, (fMaxY + fMinY) * 0.5f };
    Vec3 vScale = Vec3{ abs(fMaxX - fMinX), 1.f, abs(fMaxY - fMinY) };
    m_pPointObject->Transform()->SetRelativePos(vPos);
    m_pPointObject->Transform()->SetRelativeScale(vScale);

    Clear_Visited(m_dfs_visited, m_iPixel_Width, m_iPixel_Height);

    m_fMinX = fMinX + m_iPixel_Width * 0.5f;
    m_fMaxX = fMaxX + m_iPixel_Width * 0.5f;

    m_fMinY = -1.f * fMinY + m_iPixel_Height * 0.5f;
    m_fMaxY = -1.f * fMaxY + m_iPixel_Height * 0.5f;
}

Vec2 EditAnimationUI::Get_Pixel_Bfs(UINT x, UINT y)
{
    float fMinX = (float)m_iPixel_Width;
    float fMinY = (float)m_iPixel_Height;

    float fMaxX = -1;
    float fMaxY = -1;

    std::queue<std::pair<int, int>> QueuePixel;
    QueuePixel.push(make_pair(x, y)); // pair를 만들어서 stack에 넣습니다.

    // 처음 x,y를 방문 했기때문에

    if (x < 0 || x >(UINT)(m_iPixel_Width - 1) || y < 0 || y >(UINT)m_iPixel_Height)
        return Vec2(-1.f, -1.f);

    m_bfs_visited[x][y] = true;

    while (!QueuePixel.empty()) {

        x = QueuePixel.front().first;
        y = QueuePixel.front().second;

        QueuePixel.pop();

        // 해당 위치의 주변을 확인
        for (int i = 0; i < 4; i++)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];

            // 지도를 벗어나지 않고
            if (0 <= nx && nx < m_iPixel_Width && 0 <= ny && ny < m_iPixel_Height)
            {
                // 방문하지 않았다면 -> 방문
                if (m_bfs_visited[ny][nx] == false)
                {
                    m_bfs_visited[ny][nx] = true;

                    int index = 4 * (nx + ny * m_iPixel_Width);
                    bool bChecked = m_pPixels[index] == 0.f && m_pPixels[index + 1] == 0.f && m_pPixels[index + 2] == 0.f && m_pPixels[index + 3] == 0.f;

                    if (bChecked)
                    {

                        QueuePixel.push(make_pair(x, y)); // push하는 경우이기 때문에 현재 위치도 넣어준다.
                        QueuePixel.push(make_pair(nx, ny)); // 스택에 현재 nx,ny도 푸시
                    }
                    else
                    {
                        Clear_Visited(m_bfs_visited, m_iPixel_Width, m_iPixel_Height);
                        return Vec2{ nx,ny };
                    }
                }
            }
        }
    }
    return Vec2(-1.f, -1.f);
}

void EditAnimationUI::Clear_Visited(bool** _visited, size_t width, size_t height)
{
    for (size_t i{}; i < height; ++i)
    {
        for (size_t j{}; j < width; ++j)
        {
            _visited[i][j] = false;
        }
    }
}

void EditAnimationUI::Initialize_Aimation_Pixel()
{
    if (m_dfs_visited)
    {
        for (int i{}; i < m_iPixel_Height; ++i)
        {
            if (m_dfs_visited[i])
                delete m_dfs_visited[i];
        }
        delete m_dfs_visited;
        m_dfs_visited = nullptr;
    }
    if (m_bfs_visited)
    {
        for (int i{}; i < m_iPixel_Height; ++i)
        {
            if (m_bfs_visited[i])
                delete m_bfs_visited[i];
        }
        delete m_bfs_visited;
        m_bfs_visited = nullptr;
    }
    m_pPixels = m_pAtlasTexture->GetPixel();
    m_iPixel_Width = m_pAtlasTexture->GetWidth();
    m_iPixel_Height = m_pAtlasTexture->GetHeight();

    m_dfs_visited = (bool**)malloc(sizeof(bool*) * m_iPixel_Height);
    m_bfs_visited = (bool**)malloc(sizeof(bool*) * m_iPixel_Height);
    for (int i{}; i < m_iPixel_Height; ++i)
    {
        if (m_dfs_visited[i])
        {
            m_dfs_visited[i] = (bool*)malloc(sizeof(bool) * m_iPixel_Width);
            memset(m_dfs_visited[i], 0, sizeof(bool) * m_iPixel_Width);
        }

        if (m_bfs_visited[i])
        {
            m_bfs_visited[i] = (bool*)malloc(sizeof(bool) * m_iPixel_Width);
            memset(m_bfs_visited[i], 0, sizeof(bool) * m_iPixel_Width);
        }
    }
}

tRGBA EditAnimationUI::GetRGBA(int _x, int _y)
{

    tRGBA tInfo;

    static const size_t RGBA = 4;

    //_y = m_iPixel_Height - _y;
    size_t index = RGBA * (_y * m_iPixel_Width + _x);

    tInfo.R = m_pPixels[index + 0];
    tInfo.G = m_pPixels[index + 1];
    tInfo.B = m_pPixels[index + 2];
    tInfo.A = m_pPixels[index + 3];

    return tInfo;
}

void EditAnimationUI::SetTextureUI()
{
    //Texture 파일 경로 찾기
    wstring wstrFilePath = CPathMgr::GetInst()->GetContentPath();
    m_fileDialogInfo.fileName;
    string strFilePath = string(wstrFilePath.begin(), wstrFilePath.end());
    string strFullPath = m_fileDialogInfo.resultPath.string();

    std::size_t ind = strFullPath.find(strFilePath); // Find the starting position of substring in the string

    if (ind != std::string::npos) {
        strFullPath.erase(ind, strFilePath.length()); // erase function takes two parameter, the starting index in the string from where you want to erase characters and total no of characters you want to erase.


        wstring wstrFileName = wstring(strFullPath.begin(), strFullPath.end());
        CResMgr::GetInst()->Load<CTexture>(wstrFileName, wstrFileName);

        //Aniamtion 객체
        //Dummy 객체 Anmation component, 
        //Default Anmation component (초기 값 일 때)
        m_pAtlasTexture = CResMgr::GetInst()->FindRes<CTexture>(wstrFileName);
        m_pAnimator->SetTexture(m_pAtlasTexture);
        m_pImage = m_pAtlasTexture->GetSRV().Get();

        CGameObjectEx* pGameObject = CEditor::GetInst()->FindByName(L"AnimationTool");
        pGameObject->MeshRender()->GetDynamicMaterial()->SetTexParam(TEX_PARAM::TEX_0, m_pAtlasTexture);

        Initialize_Aimation_Pixel();
    }
}
void EditAnimationUI::Initialize_Animation_Info()
{
    m_pAnimator = (CAnimator2D*)CEditor::GetInst()->GetArrComponent(COMPONENT_TYPE::ANIMATOR2D);
    const map<wstring, CAnimation2D*> map = m_pAnimator->GetConstRef();

    for (auto iter{ map.begin() }; iter != map.end(); ++iter)
    {
        m_vecFileData.push_back(string(iter->first.begin(), iter->first.end()));
    }

    m_uvFullSize = m_pAnimator->GetAniFrame().vFullSize;
    m_uvSlice = m_pAnimator->GetAniFrame().vSlice;
    m_uvOffset = m_pAnimator->GetAniFrame().vOffset;
    m_uvDiff = (m_uvFullSize - m_uvSlice) / 2.f;

    m_pAtlasTexture = m_pAnimator->GetTexture();
    m_pImage = m_pAtlasTexture->GetSRV().Get();
}

void EditAnimationUI::Render_Text(ImVec4 _vColor, Vec2 _vSize, string _str)
{
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, _vColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _vColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, _vColor);
    ImGui::Button(_str.c_str(), ImVec2(_vSize.x, _vSize.y));
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}

void EditAnimationUI::Render_Pos()
{
    static string strLeftop = "Pos : (";
    strLeftop = "Pos : (";
    float fLeft = m_pPointObject->Transform()->GetRelativePos().x - m_pPointObject->Transform()->GetRelativeScale().x * 0.5f;
    float fTop = m_pPointObject->Transform()->GetRelativePos().y + m_pPointObject->Transform()->GetRelativeScale().y * 0.5f;
    fLeft += m_iPixel_Width * 0.5f;
    fTop = -1.f * fTop + m_iPixel_Height * 0.5f;
    char szLeft[50] = {};
    char szTop[50] = {};
    sprintf_s(szLeft, "%7.1f", fLeft);
    sprintf_s(szTop, "%7.1f", fTop);
    strLeftop += szLeft;
    strLeftop += ", ";
    strLeftop += szTop;
    strLeftop += ")";
    ImGui::Text(strLeftop.c_str());

    static string strSize = "Size : (";
    strSize = "Size : (";
    float fSizex = m_pPointObject->Transform()->GetRelativeScale().x;
    float fSizey = m_pPointObject->Transform()->GetRelativeScale().y;
    char szSizex[50] = {};
    char szSizey[50] = {};
    sprintf_s(szSizex, "%7.1f", fSizex);
    sprintf_s(szSizey, "%7.1f", abs(fSizey));
    strSize += szSizex;
    strSize += ", ";
    strSize += szSizey;
    strSize += ")";
    ImGui::Text(strSize.c_str());
}

void EditAnimationUI::Refresh_Animation(float width, float height)
{
    const vector<tAnim2DFrm> vecFrames = m_pAnimator->GetFames();

    m_vecFrameIndex.clear();

    m_vLeftTop.x = m_pAnimator->GetAniFrame().vLeftTop.x * width;
    m_vLeftTop.y = m_pAnimator->GetAniFrame().vLeftTop.y * height;
    m_vSlice.x = m_pAnimator->GetAniFrame().vSlice.x * width;
    m_vSlice.y = m_pAnimator->GetAniFrame().vSlice.y * height;
    m_vFullSize.x = m_pAnimator->GetAniFrame().vFullSize.x * width;
    m_vFullSize.y = m_pAnimator->GetAniFrame().vFullSize.y * height;
    m_vOffset.x = m_pAnimator->GetAniFrame().vOffset.x * width;
    m_vOffset.y = m_pAnimator->GetAniFrame().vOffset.y * height;

    m_uvLeftTop = m_pAnimator->GetAniFrame().vLeftTop;
    m_uvSlice = m_pAnimator->GetAniFrame().vSlice;
    m_uvRightBottom = m_uvLeftTop + m_uvSlice;
    m_uvFullSize = m_pAnimator->GetAniFrame().vFullSize;
    m_uvOffset = m_pAnimator->GetAniFrame().vOffset;
    m_fDuration = m_pAnimator->GetAniFrame().fDuration;

    for (int i{}; i < vecFrames.size(); ++i)
    {
        if (10 > i)
        {
            m_vecFrameIndex.push_back("0" + std::to_string(i));
        }
        else
        {
            m_vecFrameIndex.push_back(std::to_string(i));
        }
    }
}