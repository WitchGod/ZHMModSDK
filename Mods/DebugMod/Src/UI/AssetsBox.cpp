#include "DebugMod.h"
#include "imgui_internal.h"

#include <Glacier/ZContentKitManager.h>

void DebugMod::DrawAssetsBox(bool p_HasFocus)
{
    if (!p_HasFocus || !m_AssetsMenuActive)
    {
        return;
    }

    ImGui::PushFont(SDK()->GetImGuiBlackFont());
    const auto s_Showing = ImGui::Begin("ASSETS", &m_AssetsMenuActive);
    ImGui::PushFont(SDK()->GetImGuiRegularFont());

    if (s_Showing && p_HasFocus)
    {
        if (m_RepositoryProps.size() == 0)
        {
            LoadRepositoryProps();
        }

        ZContentKitManager* s_ContentKitManager = Globals::ContentKitManager;

        static char s_PropTitle[36] { "" };
        static char s_PropAssemblyPath[512] { "" };
        static char s_NumberOfPropsToSpawn[5] { "1" };
        static char s_NumberOfPropsToSpawn2[5] { "1" };
        static char s_NumberOfPropsToSpawn3[5] { "1" };
        static int s_Button = 1;
        static char s_NpcName[100] {};

        ImGui::Text("Convenience Props");
        ImGui::Text("");

        if (ImGui::Button("Spawn Convenience Props in Inventory"))
        {
            std::vector<ZRepositoryID> m_ConvenientProps {
                ZRepositoryID("4404fc8d-ffc0-472d-a63a-6480be973e74"), // Crowbar
                ZRepositoryID("92d9acf6-fd79-4818-bda6-c4c28b123d8c"), // Screwdriver
                ZRepositoryID("494e74b2-f3c0-4c77-be15-8f22a6eed97b"), // Wrench
                ZRepositoryID("d3387f28-866d-4262-88cb-6e5b1076bac0"), // Lockpick
                ZRepositoryID("351c144c-8687-426a-a6f0-c4abd7021062"), // Sieker
                ZRepositoryID("808ebdcb-aafe-496a-9541-5903bf03c12e"), // Kalmer
            };
            for (auto& m_Prop : m_ConvenientProps)
                SpawnRepositoryProp(m_Prop, false);

            std::vector<ZRepositoryID> m_ConvenientProps20 {
                    ZRepositoryID("93f34bf9-2bd4-4aff-85c7-7e4a9921dfe7"), // EMP Explosive
                    ZRepositoryID("c2beb035-74af-488e-bfb5-a7f7fb8e0bcc"), // Lethal Poison
                    ZRepositoryID("882fe39c-4395-4c29-87a3-a378fada5c67"), // Lethal Syringe
                    ZRepositoryID("a10cf0cb-266d-498b-ac29-49ab10c4e575"), // Emetic Poison
                    ZRepositoryID("15295f15-b72d-49ce-9c36-fbb45c68c72a"), // Emetic Syringe
                    ZRepositoryID("765b2c7d-8554-463a-9ee4-de7b20822161"), // Remote Taser
                    ZRepositoryID("0bc37bb7-dcd8-4348-a338-22fd8676a416"), // Remote Duck
                    ZRepositoryID("293af6cc-dd8d-4641-b650-14cdfd00f1de"), // Breaching Charge
                    ZRepositoryID("dda002e9-02b1-4208-82a5-cf059f3c79cf"), // Coin
                    ZRepositoryID("8b114fce-586b-4b06-b446-75d0bb4a4cfb"), // Keycard Hacker
            };
            for (auto& m_Prop : m_ConvenientProps20)
            {
                for (int i = 0; i < 20; ++i)
                    SpawnRepositoryProp(m_Prop, false);
            }
        }
        ImGui::Separator();

        ImGui::Text("Repository Props");
        ImGui::Text("");
        ImGui::Text("Prop Title");
        ImGui::SameLine();

        const bool s_IsInputTextEnterPressed = ImGui::InputText("##PropRepositoryID", s_PropTitle, sizeof(s_PropTitle), ImGuiInputTextFlags_EnterReturnsTrue);
        const bool s_IsInputTextActive = ImGui::IsItemActive();

        if (ImGui::IsItemActivated())
        {
            ImGui::OpenPopup("##popup");
        }

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

        if (ImGui::BeginPopup("##popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
        {
            for (auto it = m_RepositoryProps.begin(); it != m_RepositoryProps.end(); ++it)
            {
                const char* s_PropTitle2 = it->first.c_str();

                if (!strstr(s_PropTitle2, s_PropTitle))
                {
                    continue;
                }

                if (ImGui::Selectable(s_PropTitle2))
                {
                    ImGui::ClearActiveID();
                    strcpy_s(s_PropTitle, s_PropTitle2);

                    const int s_NumberOfPropsToSpawn22 = std::atoi(s_NumberOfPropsToSpawn);

                    for (int i = 0; i < s_NumberOfPropsToSpawn22; ++i)
                    {
                        SpawnRepositoryProp(it->second, s_Button == 1);
                    }
                }
            }

            if (s_IsInputTextEnterPressed || (!s_IsInputTextActive && !ImGui::IsWindowFocused()))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (ImGui::RadioButton("Add To World", s_Button == 1))
        {
            s_Button = 1;
        }

        ImGui::SameLine();

        if (ImGui::RadioButton("Add To Inventory", s_Button == 2))
        {
            s_Button = 2;
        }

        ImGui::Text("Number Of Props To Spawn");
        ImGui::SameLine();

        ImGui::InputText("##NumberOfPropsToSpawn", s_NumberOfPropsToSpawn, sizeof(s_NumberOfPropsToSpawn));

        ImGui::Separator();
        ImGui::Text("Non Repository Props");
        ImGui::Text("");
        ImGui::Text("Prop Assembly Path");
        ImGui::SameLine();

        ImGui::InputText("##Prop Assembly Path", s_PropAssemblyPath, sizeof(s_PropAssemblyPath));
        ImGui::SameLine();

        if (ImGui::Button("Spawn Prop"))
        {
            const int s_NumberOfPropsToSpawn33 = std::atoi(s_NumberOfPropsToSpawn2);

            for (int i = 0; i < s_NumberOfPropsToSpawn33; ++i)
            {
                SpawnNonRepositoryProp(s_PropAssemblyPath);
            }
        }

        ImGui::Text("Number Of Props To Spawn");
        ImGui::SameLine();

        ImGui::InputText("##NumberOfPropsToSpawn2", s_NumberOfPropsToSpawn2, sizeof(s_NumberOfPropsToSpawn2));
        ImGui::Separator();

        ImGui::Text("NPCs");
        ImGui::Text("");
        ImGui::Text("NPC Name");
        ImGui::SameLine();

        ImGui::InputText("##NPCName", s_NpcName, sizeof(s_NpcName));

        static char outfitName[256] { "" };

        ImGui::Text("Outfit");
        ImGui::SameLine();

        const bool s_IsInputTextEnterPressed2 = ImGui::InputText("##OutfitName", outfitName, sizeof(outfitName), ImGuiInputTextFlags_EnterReturnsTrue);
        const bool s_IsInputTextActive2 = ImGui::IsItemActive();

        if (ImGui::IsItemActivated())
        {
            ImGui::OpenPopup("##popup2");
        }

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

        static ZRepositoryID s_RepositoryId = ZRepositoryID("");
        static TEntityRef<ZGlobalOutfitKit>* s_GlobalOutfitKit = nullptr;
        static char s_CurrentCharacterSetIndex[3] { "0" };
        static const char* s_CurrentcharSetCharacterType = "HeroA";
        static char s_CurrentOutfitVariationIndex[3] { "0" };

        if (ImGui::BeginPopup("##popup2", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
        {
            for (auto it = s_ContentKitManager->m_repositoryGlobalOutfitKits.begin(); it != s_ContentKitManager->m_repositoryGlobalOutfitKits.end(); ++it)
            {
                TEntityRef<ZGlobalOutfitKit>* s_GlobalOutfitKit2 = &it->second;
                const char* outfitName2 = s_GlobalOutfitKit2->m_pInterfaceRef->m_sCommonName.c_str();

                if (!strstr(outfitName2, outfitName))
                {
                    continue;
                }

                if (ImGui::Selectable(outfitName2))
                {
                    ImGui::ClearActiveID();
                    strcpy_s(outfitName, outfitName2);

                    s_RepositoryId = it->first;
                    s_GlobalOutfitKit = s_GlobalOutfitKit2;
                }
            }

            if (s_IsInputTextEnterPressed2 || (!s_IsInputTextActive2 && !ImGui::IsWindowFocused()))
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
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Text("Number Of Props To Spawn");
        ImGui::SameLine();

        ImGui::InputText("##NumberOfPropsToSpawn3", s_NumberOfPropsToSpawn3, sizeof(s_NumberOfPropsToSpawn3));

        if (ImGui::Button("Spawn NPC"))
        {
            const int s_NumberOfPropsToSpawn4 = std::atoi(s_NumberOfPropsToSpawn3);

            for (int i = 0; i < s_NumberOfPropsToSpawn4; ++i)
            {
                SpawnNPC(s_NpcName, s_RepositoryId, s_GlobalOutfitKit, s_CurrentCharacterSetIndex, s_CurrentcharSetCharacterType, s_CurrentOutfitVariationIndex);
            }
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}
