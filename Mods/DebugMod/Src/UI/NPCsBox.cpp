#include "DebugMod.h"

#include <Glacier/ZActor.h>
#include <Glacier/ZApplicationEngineWin32.h>
#include <Glacier/ZCameraEntity.h>
#include <Glacier/ZContentKitManager.h>
#include <Glacier/ZFreeCamera.h>
#include <Glacier/ZHM5InputManager.h>
#include <Glacier/ZSpatialEntity.h>

#include "imgui_internal.h"

void DebugMod::DrawNPCsBox(bool p_HasFocus)
{
    if (!p_HasFocus || !m_NPCsMenuActive)
    {
        return;
    }

    ImGui::PushFont(SDK()->GetImGuiBlackFont());
    const auto s_Showing = ImGui::Begin("NPCs", &m_NPCsMenuActive);
    ImGui::PushFont(SDK()->GetImGuiRegularFont());

    if (s_Showing && p_HasFocus)
    {
        ZContentKitManager* s_ContentKitManager = Globals::ContentKitManager;
        static size_t s_Selected = 0;

        ImGui::BeginChild("left pane", ImVec2(300, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

        static char s_NpcName[256] { "" };

        ImGui::Text("NPC Name");
        ImGui::SameLine();

        ImGui::InputText("##NPCName", s_NpcName, sizeof(s_NpcName));

        for (int i = 0; i < *Globals::NextActorId; ++i)
        {
            const ZActor* s_Actor = Globals::ActorManager->m_aActiveActors[i].m_pInterfaceRef;
            std::string s_NpcName2 = s_Actor->m_sActorName.c_str();

            if (!strstr(s_NpcName2.c_str(), s_NpcName))
            {
                continue;
            }

            if (ImGui::Selectable(s_NpcName2.c_str(), s_Selected == i))
            {
                s_Selected = i;
            }
        }

        ImGui::EndChild();
        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        ZActor* s_Actor = Globals::ActorManager->m_aActiveActors[s_Selected].m_pInterfaceRef;
        static char s_OutfitName[256] { "" };

        ImGui::Text("Outfit");
        ImGui::SameLine();

        const bool s_IsInputTextEnterPressed = ImGui::InputText("##OutfitName", s_OutfitName, sizeof(s_OutfitName), ImGuiInputTextFlags_EnterReturnsTrue);
        const bool s_IsInputTextActive = ImGui::IsItemActive();

        if (ImGui::IsItemActivated())
        {
            ImGui::OpenPopup("##popup");
        }

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

        static TEntityRef<ZGlobalOutfitKit>* s_GlobalOutfitKit = nullptr;
        static char s_CurrentCharacterSetIndex[3] { "0" };
        static const char* s_CurrentcharSetCharacterType = "Actor";
        static const char* s_CurrentcharSetCharacterType2 = "Actor";
        static char s_CurrentOutfitVariationIndex[3] { "0" };

        if (ImGui::BeginPopup("##popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
        {
            for (auto it = s_ContentKitManager->m_repositoryGlobalOutfitKits.begin(); it != s_ContentKitManager->m_repositoryGlobalOutfitKits.end(); ++it)
            {
                TEntityRef<ZGlobalOutfitKit>* s_GlobalOutfitKit2 = &it->second;
                const char* s_OutfitName2 = s_GlobalOutfitKit2->m_pInterfaceRef->m_sCommonName.c_str();

                if (!strstr(s_OutfitName2, s_OutfitName))
                {
                    continue;
                }

                if (ImGui::Selectable(s_OutfitName2))
                {
                    ImGui::ClearActiveID();
                    strcpy_s(s_OutfitName, s_OutfitName2);

                    EquipOutfit(it->second, std::stoi(s_CurrentCharacterSetIndex), s_CurrentcharSetCharacterType, std::stoi(s_CurrentOutfitVariationIndex), s_Actor);

                    s_GlobalOutfitKit = s_GlobalOutfitKit2;
                }
            }

            if (s_IsInputTextEnterPressed || (!s_IsInputTextActive && !ImGui::IsWindowFocused()))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Text("Character Set Index");
        ImGui::SameLine();

        if (ImGui::BeginCombo("##CharacterSetIndex", s_CurrentCharacterSetIndex))
        {
            if (s_GlobalOutfitKit)
            {
                for (size_t i = 0; i < s_GlobalOutfitKit->m_pInterfaceRef->m_aCharSets.size(); ++i)
                {
                    std::string s_CharacterSetIndex = std::to_string(i);
                    const bool s_IsSelected = s_CurrentCharacterSetIndex == s_CharacterSetIndex.c_str();

                    if (ImGui::Selectable(s_CharacterSetIndex.c_str(), s_IsSelected))
                    {
                        strcpy_s(s_CurrentCharacterSetIndex, s_CharacterSetIndex.c_str());

                        if (s_GlobalOutfitKit)
                        {
                            EquipOutfit(*s_GlobalOutfitKit, std::stoi(s_CurrentCharacterSetIndex), s_CurrentcharSetCharacterType, std::stoi(s_CurrentOutfitVariationIndex), s_Actor);
                        }
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Text("CharSet Character Type");
        ImGui::SameLine();

        if (ImGui::BeginCombo("##CharSetCharacterType", s_CurrentcharSetCharacterType))
        {
            if (s_GlobalOutfitKit)
            {
                for (size_t i = 0; i < 3; ++i)
                {
                    const bool s_IsSelected = s_CurrentcharSetCharacterType == m_CharSetCharacterTypes[i];

                    if (ImGui::Selectable(m_CharSetCharacterTypes[i], s_IsSelected))
                    {
                        s_CurrentcharSetCharacterType = m_CharSetCharacterTypes[i];

                        if (s_GlobalOutfitKit)
                        {
                            EquipOutfit(*s_GlobalOutfitKit, std::stoi(s_CurrentCharacterSetIndex), s_CurrentcharSetCharacterType, std::stoi(s_CurrentOutfitVariationIndex), s_Actor);
                        }
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Text("Outfit Variation");
        ImGui::SameLine();

        if (ImGui::BeginCombo("##OutfitVariation", s_CurrentOutfitVariationIndex))
        {
            if (s_GlobalOutfitKit)
            {
                const unsigned int s_CurrentCharacterSetIndex2 = std::stoi(s_CurrentCharacterSetIndex);
                const size_t s_VariationCount = s_GlobalOutfitKit->m_pInterfaceRef->m_aCharSets[s_CurrentCharacterSetIndex2].m_pInterfaceRef->m_aCharacters[0].m_pInterfaceRef->m_aVariations.size();

                for (size_t i = 0; i < s_VariationCount; ++i)
                {
                    std::string s_OutfitVariationIndex = std::to_string(i);
                    const bool s_IsSelected = s_CurrentOutfitVariationIndex == s_OutfitVariationIndex.c_str();

                    if (ImGui::Selectable(s_OutfitVariationIndex.c_str(), s_IsSelected))
                    {
                        strcpy_s(s_CurrentOutfitVariationIndex, s_OutfitVariationIndex.c_str());

                        if (s_GlobalOutfitKit)
                        {
                            EquipOutfit(*s_GlobalOutfitKit, std::stoi(s_CurrentCharacterSetIndex), s_CurrentcharSetCharacterType, std::stoi(s_CurrentOutfitVariationIndex), s_Actor);
                        }
                    }
                }
            }

            ImGui::EndCombo();
        }

        if (s_GlobalOutfitKit)
        {
            ImGui::Checkbox("Weapons Allowed", &s_GlobalOutfitKit->m_pInterfaceRef->m_bWeaponsAllowed);
            ImGui::Checkbox("Authority Figure", &s_GlobalOutfitKit->m_pInterfaceRef->m_bAuthorityFigure);
        }

        ImGui::Separator();

        static char s_NpcName2[256] { "" };

        ImGui::Text("NPC Name");
        ImGui::SameLine();

        ImGui::InputText("##NPCName", s_NpcName2, sizeof(s_NpcName2));
        ImGui::SameLine();

        if (ImGui::Button("Get NPC Outfit"))
        {
            const ZActor* s_Actor2 = Globals::ActorManager->GetActorByName(s_NpcName2);

            if (s_Actor2)
            {
                EquipOutfit(s_Actor2->m_rOutfit, s_Actor2->m_nOutfitCharset, s_CurrentcharSetCharacterType2, s_Actor2->m_nOutfitVariation, s_Actor);
            }
        }

        if (ImGui::Button("Get Nearest NPC's Outfit"))
        {
            ZEntityRef s_Ref;

            s_Actor->GetID(&s_Ref);

            ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

            for (int i = 0; i < *Globals::NextActorId; ++i)
            {
                ZActor* s_Actor2 = Globals::ActorManager->m_aActiveActors[i].m_pInterfaceRef;
                ZEntityRef s_Ref;

                s_Actor2->GetID(&s_Ref);

                const ZSpatialEntity* s_ActorSpatialEntity2 = s_Ref.QueryInterface<ZSpatialEntity>();

                const SVector3 s_Temp = s_ActorSpatialEntity->m_mTransform.Trans - s_ActorSpatialEntity2->m_mTransform.Trans;
                const float s_Distance = sqrt(s_Temp.x * s_Temp.x + s_Temp.y * s_Temp.y + s_Temp.z * s_Temp.z);

                if (s_Distance <= 3.0f)
                {
                    EquipOutfit(s_Actor2->m_rOutfit, s_Actor2->m_nOutfitCharset, s_CurrentcharSetCharacterType2, s_Actor2->m_nOutfitVariation, s_Actor);

                    break;
                }
            }
        }

        ImGui::Text("CharSet Character Type");
        ImGui::SameLine();

        if (ImGui::BeginCombo("##CharSetCharacterType", s_CurrentcharSetCharacterType2))
        {
            if (s_GlobalOutfitKit)
            {
                for (size_t i = 0; i < 3; ++i)
                {
                    const bool s_IsSelected = s_CurrentcharSetCharacterType2 == m_CharSetCharacterTypes[i];

                    if (ImGui::Selectable(m_CharSetCharacterTypes[i], s_IsSelected))
                    {
                        s_CurrentcharSetCharacterType2 = m_CharSetCharacterTypes[i];
                    }
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Teleport NPC To Player"))
        {
            TEntityRef<ZHitman5> s_LocalHitman;
            Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

            if (s_LocalHitman)
            {
                ZEntityRef s_Ref;
                s_Actor->GetID(&s_Ref);

                ZSpatialEntity* s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();
                ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                s_ActorSpatialEntity->SetWorldMatrix(s_HitmanSpatialEntity->GetWorldMatrix());
            }
        }
        ImGui::SameLine();

        if (ImGui::Button("Teleport Player To NPC"))
        {
            TEntityRef<ZHitman5> s_LocalHitman;
            Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

            if (s_LocalHitman)
            {
                ZEntityRef s_Ref;
                s_Actor->GetID(&s_Ref);

                if (s_Actor)
                {
                    ZSpatialEntity* s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();
                    ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                    s_HitmanSpatialEntity->SetWorldMatrix(s_ActorSpatialEntity->GetWorldMatrix());
                }
            }
        }

        ImGui::Separator();

        if (ImGui::Button("Mark Current Position"))
        {
            TEntityRef<ZHitman5> s_LocalHitman;
            Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

            if (s_LocalHitman)
            {
                ZSpatialEntity* s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();
                m_MarkPosition = s_HitmanSpatialEntity->GetWorldMatrix();
            }
        }
        ImGui::SameLine();

        if (ImGui::Button("Teleport Player To Mark"))
        {
            TEntityRef<ZHitman5> s_LocalHitman;
            Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

            if (s_LocalHitman)
            {
                ZEntityRef s_Ref;
                s_Actor->GetID(&s_Ref);

                ZSpatialEntity* s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();

                s_HitmanSpatialEntity->SetWorldMatrix(m_MarkPosition);
            }
        }

        ImGui::Text("PosX");
        ImGui::SameLine();
        ImGui::Text("%f", m_MarkPosition.Trans.x);
        ImGui::SameLine();

        ImGui::Text("PosY");
        ImGui::SameLine();
        ImGui::Text("%f", m_MarkPosition.Trans.y);
        ImGui::SameLine();

        ImGui::Text("PosZ");
        ImGui::SameLine();
        ImGui::Text("%f", m_MarkPosition.Trans.z);

        if (ImGui::Button("Add NPC to Marked Targets"))
            m_MarkTargets.push_back(s_Actor);
        ImGui::SameLine();

        if (ImGui::Button("Clear Marked Targets"))
            m_MarkTargets.clear();

        const float m_ClipSize = static_cast<float>(min(m_MarkTargets.size(), 5) + 1);
        static ImGuiTableFlags flags =
            /* ImGuiTableFlags_Resizable | */ ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable // | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingStretchProp
            | ImGuiTableFlags_ScrollY;
        if (ImGui::BeginTable("EntityTable", 5, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * m_ClipSize), 0.0f))
        {
            /*ImGui::TableSetupColumn("Commands", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, 0.0, 0);
            ImGui::TableSetupColumn("NPC Name", ImGuiTableColumnFlags_WidthStretch, 0.0f, 1);
            ImGui::TableSetupColumn("Pos.x", ImGuiTableColumnFlags_WidthStretch, 0.0, 2);
            ImGui::TableSetupColumn("Pos.y", ImGuiTableColumnFlags_WidthStretch, 0.0, 3);
            ImGui::TableSetupColumn("Pos.z", ImGuiTableColumnFlags_WidthStretch, 0.0, 4);*/
			ImGui::TableSetupColumn("Commands", ImGuiTableColumnFlags_NoSort, 0.0, 0);
			ImGui::TableSetupColumn("NPC Name", 0, 0.0f, 1);
			ImGui::TableSetupColumn("Pos.x", 0, 0.0, 2);
			ImGui::TableSetupColumn("Pos.y", 0, 0.0, 3);
			ImGui::TableSetupColumn("Pos.z", 0, 0.0, 4);
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(m_MarkTargets.size()));
            while (clipper.Step())
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    auto m_MarkTarget = m_MarkTargets[row_n];
					auto m_MarkTargetActorName = m_MarkTarget->m_sActorName.c_str();
                    ZEntityRef s_Ref;
                    m_MarkTarget->GetID(&s_Ref);

                    TEntityRef<ZHitman5> s_LocalHitman;
                    Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

                    ImGui::PushID(m_MarkTargetActorName);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    if (ImGui::SmallButton("Summon"))
                    {
                        if (s_LocalHitman)
                        {
                            ZEntityRef s_Ref;
                            m_MarkTarget->GetID(&s_Ref);

                            ZSpatialEntity* s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();
                            ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                            s_ActorSpatialEntity->SetWorldMatrix(s_HitmanSpatialEntity->GetWorldMatrix());
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::SmallButton("Banish"))
                    {
                        if (s_LocalHitman)
                        {
                            ZEntityRef s_Ref;
                            m_MarkTarget->GetID(&s_Ref);

                            ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                            m_VoidPosition.Trans.z = -10000.0f;
                            s_ActorSpatialEntity->SetWorldMatrix(m_VoidPosition);
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::SmallButton("Move"))
                    {
                        if (s_LocalHitman)
                        {
                            ZEntityRef s_Ref;
                            m_MarkTarget->GetID(&s_Ref);

                            ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();
                            s_ActorSpatialEntity->SetWorldMatrix(m_MarkPosition);
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::SmallButton("Goto"))
                    {
                        if (s_LocalHitman)
                        {
                            ZEntityRef s_Ref;
                            m_MarkTarget->GetID(&s_Ref);

                            if (m_MarkTarget)
                            {
                                ZSpatialEntity* s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();
                                ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                                s_HitmanSpatialEntity->SetWorldMatrix(s_ActorSpatialEntity->GetWorldMatrix());
                            }
                        }
                    }
                    ImGui::SameLine();

                    if (!m_CurrentlyPeeking)
                    {
                        if (ImGui::SmallButton("Peek"))
                        {
                            if (s_LocalHitman)
                            {
                                ZEntityRef s_Ref;
                                m_MarkTarget->GetID(&s_Ref);

                                if (m_MarkTarget)
                                {
                                    Functions::ZEngineAppCommon_CreateFreeCamera->Call(&(*Globals::ApplicationEngineWin32)->m_pEngineAppCommon);
                                    (*Globals::ApplicationEngineWin32)->m_pEngineAppCommon.m_pFreeCameraControl01.m_pInterfaceRef->SetActive(true);

                                    auto s_Camera = (*Globals::ApplicationEngineWin32)->m_pEngineAppCommon.m_pFreeCamera01;
                                    TEntityRef<IRenderDestinationEntity> s_RenderDest;
                                    Functions::ZCameraManager_GetActiveRenderDestinationEntity->Call(Globals::CameraManager, &s_RenderDest);

                                    m_OriginalCam = *s_RenderDest.m_pInterfaceRef->GetSource();

                                    ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();
                                    ZCameraEntity* s_ActorCameraEntity = s_Ref.QueryInterface<ZCameraEntity>();

                                    const auto s_CurrentCamera = Functions::GetCurrentCamera->Call();
                                    SMatrix s_CameraTrans = s_CurrentCamera->GetWorldMatrix();
                                    SMatrix s_MarkTargetTrans = s_ActorSpatialEntity->GetWorldMatrix();

                                    // We'll hack it by changing the XYZ in the s_MarkTargetTrans to that of s_CameraTrans.
                                    // The hope is we'll be able to replicate the general bearing of 47 and literally just move
                                    // the camera to that position.
                                    s_CameraTrans.Trans.x = s_MarkTargetTrans.mat[3].x;
                                    s_CameraTrans.Trans.y = s_MarkTargetTrans.mat[3].y;
                                    s_CameraTrans.Trans.z = s_MarkTargetTrans.mat[3].z + 2.05f;

                                    s_Camera.m_pInterfaceRef->SetWorldMatrix(s_CameraTrans);

                                    Logger::Debug("Camera trans: {}", fmt::ptr(&s_Camera.m_pInterfaceRef->m_mTransform.Trans));
                                    s_RenderDest.m_pInterfaceRef->SetSource(&s_Camera.m_ref);

                                    auto* s_InputControl = Functions::ZHM5InputManager_GetInputControlForLocalPlayer->Call(Globals::InputManager);

                                    if (s_InputControl)
                                        s_InputControl->m_bActive = false;

                                    m_CurrentlyPeeking = true;
                                }
                            }
                        }
                    }

                    if (m_CurrentlyPeeking)
                    {
                        if (ImGui::SmallButton("Stop Peeking"))
                        {
                            if (s_LocalHitman)
                            {
                                TEntityRef<IRenderDestinationEntity> s_RenderDest;
                                Functions::ZCameraManager_GetActiveRenderDestinationEntity->Call(Globals::CameraManager, &s_RenderDest);

                                s_RenderDest.m_pInterfaceRef->SetSource(&m_OriginalCam);

                                auto* s_InputControl = Functions::ZHM5InputManager_GetInputControlForLocalPlayer->Call(Globals::InputManager);

                                if (s_InputControl)
                                    s_InputControl->m_bActive = true;

                                m_CurrentlyPeeking = false;
                            }
                        }
                    }

                    if (m_MarkTarget->IsPacified() || m_MarkTarget->IsDead())
                    {
                        ImGui::SameLine();
                        if (ImGui::SmallButton("Revive"))
                        {
                            if (s_LocalHitman)
                            {
                                ZEntityRef s_Ref;
                                m_MarkTarget->GetID(&s_Ref);

                                Functions::ZActor_ReviveActor->Call(m_MarkTarget);
                            }
                        }
                    }

                    ImGui::TableNextColumn();

                    ImGui::Text("%s", m_MarkTargetActorName);
                    ImGui::TableNextColumn();

                    ImGui::Text("%.04f", s_Ref.QueryInterface<ZSpatialEntity>()->GetWorldMatrix().Trans.x);
                    ImGui::TableNextColumn();

                    ImGui::Text("%.04f", s_Ref.QueryInterface<ZSpatialEntity>()->GetWorldMatrix().Trans.y);
                    ImGui::TableNextColumn();

                    ImGui::Text("%.04f", s_Ref.QueryInterface<ZSpatialEntity>()->GetWorldMatrix().Trans.z);

                    ImGui::PopID();
                }
            ImGui::EndTable();
        }


        //for (auto it = m_MarkTargets.begin(); it != m_MarkTargets.end(); ++it)
        /*for (int i = 0; i < m_MarkTargets.size(); i++)
        {
            ZActor* s_MarkedActor = m_MarkTargets.at(i);
            ImGui::Text(s_MarkedActor->m_sActorName.c_str());
            ImGui::SameLine();

            if (ImGui::SmallButton("Teleport NPC To Mark"))
            {
                TEntityRef<ZHitman5> s_LocalHitman;
                Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

                if (s_LocalHitman)
                {
                    ZEntityRef s_Ref;
                    s_MarkedActor->GetID(&s_Ref);

                    ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                    s_ActorSpatialEntity->SetWorldMatrix(m_MarkPosition);
                }
            }
            ImGui::SameLine();

            if (ImGui::SmallButton("Teleport NPC To The Void"))
            {
                TEntityRef<ZHitman5> s_LocalHitman;
                Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

                if (s_LocalHitman)
                {
                    ZEntityRef s_Ref;
                    s_MarkedActor->GetID(&s_Ref);

                    ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                    m_VoidPosition.Trans.z = -1000.0f;
                    s_ActorSpatialEntity->SetWorldMatrix(m_VoidPosition);
                }
            }
            ImGui::SameLine();

            if (ImGui::SmallButton("Teleport NPC To Player"))
            {
                TEntityRef<ZHitman5> s_LocalHitman;
                Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

                if (s_LocalHitman)
                {
                    ZEntityRef s_Ref;
                    s_MarkedActor->GetID(&s_Ref);

                    ZSpatialEntity* s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();
                    ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                    s_ActorSpatialEntity->SetWorldMatrix(s_HitmanSpatialEntity->GetWorldMatrix());
                }
            }
            ImGui::SameLine();

            if (ImGui::SmallButton("Teleport Player To NPC"))
            {
                TEntityRef<ZHitman5> s_LocalHitman;
                Functions::ZPlayerRegistry_GetLocalPlayer->Call(Globals::PlayerRegistry, &s_LocalHitman);

                if (s_LocalHitman)
                {
                    ZEntityRef s_Ref;
                    s_MarkedActor->GetID(&s_Ref);

                    if (s_MarkedActor)
                    {
                        ZSpatialEntity* s_HitmanSpatialEntity = s_LocalHitman.m_ref.QueryInterface<ZSpatialEntity>();
                        ZSpatialEntity* s_ActorSpatialEntity = s_Ref.QueryInterface<ZSpatialEntity>();

                        s_HitmanSpatialEntity->SetWorldMatrix(s_ActorSpatialEntity->GetWorldMatrix());
                    }
                }
            }
        }*/

        ImGui::EndChild();
        ImGui::EndGroup();
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}
