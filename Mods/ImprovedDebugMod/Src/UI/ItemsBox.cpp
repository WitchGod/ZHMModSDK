#include "ImprovedDebugMod.h"

#include <Glacier/ZAction.h>
#include <Glacier/ZItem.h>

void ImprovedDebugMod::DrawItemsBox(bool p_HasFocus)
{
    if (!p_HasFocus || !m_ItemsMenuActive)
    {
        return;
    }

    ImGui::PushFont(SDK()->GetImGuiBlackFont());
    const auto s_Showing = ImGui::Begin("ITEMS", &m_ItemsMenuActive);
    ImGui::PushFont(SDK()->GetImGuiRegularFont());

    if (s_Showing && p_HasFocus)
    {
        THashMap<ZRepositoryID, ZDynamicObject, TDefaultHashMapPolicy<ZRepositoryID>>* repositoryData = nullptr;

        if (m_RepositoryResource.m_nResourceIndex == -1)
        {
            const auto s_ID = ResId<"[assembly:/repository/pro.repo].pc_repo">;

            Globals::ResourceManager->GetResourcePtr(m_RepositoryResource, s_ID, 0);
        }

        if (m_RepositoryResource.GetResourceInfo().status == RESOURCE_STATUS_VALID)
        {
            repositoryData = static_cast<THashMap<ZRepositoryID, ZDynamicObject, TDefaultHashMapPolicy<ZRepositoryID>>*>(m_RepositoryResource.GetResourceData());
        }
        else
        {
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopFont();

            return;
        }

        const ZHM5ActionManager* s_Hm5ActionManager = Globals::HM5ActionManager;
        std::vector<ZHM5Action*> s_Actions;
        std::vector<ZHM5Action*> s_FilteredActions;
        std::vector<const ZHM5Item*> s_ItemsForTeleport;

        if (s_Hm5ActionManager->m_Actions.size() == 0)
        {
            ImGui::PopFont();
            ImGui::End();
            ImGui::PopFont();

            return;
        }

        for (unsigned int i = 0; i < s_Hm5ActionManager->m_Actions.size(); ++i)
        {
            ZHM5Action* s_Action = s_Hm5ActionManager->m_Actions[i];

            if (s_Action && s_Action->m_eActionType == EActionType::AT_PICKUP)
            {
                s_Actions.push_back(s_Action);
            }
        }

        static const char* s_CurrentComboItem = NULL;
        static std::vector<std::string> s_ComboItems;

        for (size_t i = 0; i < s_Actions.size(); i++)
        {
            ZHM5Action* s_Action = s_Actions[i];
            const ZHM5Item* s_Item = s_Action->m_Object.QueryInterface<ZHM5Item>();

            m_UniqueInventoryCategories.insert(GetItemDynamicObjectValueByKey(repositoryData, s_Item, "InventoryCategoryName"));
        }

        s_ComboItems.assign(m_UniqueInventoryCategories.begin(), m_UniqueInventoryCategories.end());

        ImGui::PushItemWidth(200);
        if (ImGui::BeginCombo("##categorycombo", s_CurrentComboItem))
        {
            for (int n = 0; n < s_ComboItems.size(); ++n)
            {
                const bool s_IsSelected = (s_CurrentComboItem == s_ComboItems[n].c_str());
                if (ImGui::Selectable(s_ComboItems[n].c_str(), s_IsSelected))
                {
                    s_CurrentComboItem = s_ComboItems[n].c_str();
                    s_FilteredActions.clear();
                    s_ItemsForTeleport.clear();
                }
                if (s_IsSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        for (size_t i = 0; i < s_Actions.size(); i++)
        {
            ZHM5Action* s_Action = s_Actions[i];
            const ZHM5Item* s_Item = s_Action->m_Object.QueryInterface<ZHM5Item>();
            std::string s_InventoryCategory = GetItemDynamicObjectValueByKey(repositoryData, s_Item, "InventoryCategoryName");
            std::string s_ItemType = GetItemDynamicObjectValueByKey(repositoryData, s_Item, "ItemType");

            s_FilteredActions.push_back(s_Action);

            if (s_CurrentComboItem != NULL)
                if (strcmp(s_InventoryCategory.c_str(), s_CurrentComboItem) == 0 || strcmp(s_InventoryCategory.c_str(), s_CurrentComboItem) == 0)
                {
                    // Check if "poison" items are of type eCC_Bottle. Don't teleport if so.
                    if (strcmp(s_InventoryCategory.c_str(), "poison") == 0 && strcmp(s_ItemType.c_str(), "eCC_Bottle") == 0)
                        continue;
                    s_ItemsForTeleport.push_back(s_Item);
                }
        }

        if (ImGui::Button("Teleport Item Group to Player"))
        {
            for (size_t i = 0; i < s_ItemsForTeleport.size(); i++)
                TeleportItemToHitman(s_ItemsForTeleport[i]);
        }

        static size_t s_Selected = 0;
        size_t count = s_FilteredActions.size();

        ImGui::BeginChild("left pane", ImVec2(300, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

        for (size_t i = 0; i < count; i++)
        {
            ZHM5Action* s_Action = s_FilteredActions[i];
            const ZHM5Item* s_Item = s_Action->m_Object.QueryInterface<ZHM5Item>();
            std::string s_Title = std::format("{} {}", s_Item->m_pItemConfigDescriptor->m_sTitle.c_str(), i + 1);

            if (ImGui::Selectable(s_Title.c_str(), s_Selected == i))
            {
                s_Selected = i;
                m_TextureResourceData.clear();
            }
        }

        ImGui::EndChild();
        ImGui::SameLine();

        ImGui::BeginGroup();

        ZHM5Action* s_Action = s_FilteredActions[s_Selected];
        const ZHM5Item* s_Item = s_Action->m_Object.QueryInterface<ZHM5Item>();
        //const ZDynamicObject* s_DynamicObject = &repositoryData->find(s_Item->m_pItemConfigDescriptor->m_RepositoryId)->second;
        //const auto s_Entries = s_DynamicObject->As<TArray<SDynamicObjectKeyValuePair>>();
        const auto s_Entries = GetItemDynamicObjectEntries(repositoryData, s_Item);
        std::string s_Image;

        if (ImGui::Button("Teleport Item To Player"))
            TeleportItemToHitman(s_Item);
        ImGui::SameLine();

        if (ImGui::Button("Teleport Player To Item"))
            TeleportHitmanToItem(s_Item);

        ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        s_Image = GetItemDynamicObjectValueByKey(s_Entries, "Image");

        /*
        if (m_TextureResourceData.size() == 0)
        {
            const unsigned long long s_DdsTextureHash = GetDDSTextureHash(s_Image);

            LoadResourceData(s_DdsTextureHash, m_TextureResourceData);

            SDK()->LoadTextureFromMemory(m_TextureResourceData, &m_TextureSrvGpuHandle, m_Width, m_Height);
        }

        ImGui::Image(reinterpret_cast<ImTextureID>(m_TextureSrvGpuHandle.ptr), ImVec2(static_cast<float>(m_Width / 2), static_cast<float>(m_Height / 2)));
        */

        for (unsigned int i = 0; i < s_Entries->size(); ++i)
        {
            std::string s_Key = std::format("{}:", s_Entries->operator[](i).sKey.c_str());
            const IType* s_Type = s_Entries->operator[](i).value.m_pTypeID->typeInfo();

            if (strcmp(s_Type->m_pTypeName, "TArray<ZDynamicObject>") == 0)
            {
                s_Key += " [";

                ImGui::Text(s_Key.c_str());

                const TArray<ZDynamicObject>* s_Array = s_Entries->operator[](i).value.As<TArray<ZDynamicObject>>();

                for (unsigned int j = 0; j < s_Array->size(); ++j)
                {
                    std::string s_Value = ConvertDynamicObjectValueToString(&s_Array->operator[](j));

                    if (!s_Value.empty())
                    {
                        ImGui::Text(std::format("\t{}", s_Value).c_str());
                    }
                }

                ImGui::Text("]");
            }
            else
            {
                ImGui::Text(s_Key.c_str());

                std::string s_Value = ConvertDynamicObjectValueToString(&s_Entries->operator[](i).value);

                ImGui::SameLine();
                ImGui::Text(s_Value.c_str());
            }
        }

        ImGui::EndChild();
        ImGui::EndGroup();
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}
