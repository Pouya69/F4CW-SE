#include "Serialization.h"
#include <cstdint>
#include <set>
#include <F4SE/Interfaces.h>
#include <RE/B/BGSKeyword.h>
#include <RE/B/BGSListForm.h>
#include <memory>
#include <REX/LOG.h>
#include <RE/T/TESDataHandler.h>
#include "IGlobalConfig.h"

namespace F4CWSerialization {
    std::int32_t playerSkillPoints = 0;
    std::set<std::uint32_t> taggedSkills;
    bool playerReadyToLevelUp = false;

    RepairListMap weaponRepairList;
    RepairListMap armorRepairList;

    auto F4CWSerialization::GetSingleton() -> F4CWSerialization*
    {
        static F4CWSerialization singleton{};
        return std::addressof(singleton);
    }
    void F4CWSerialization::Serialize(const F4SE::SerializationInterface* a_intfc)
    {
        REX::DEBUG("Serialize save data.");
        std::int32_t test = 12;

        a_intfc->WriteRecord('USKP', SerializationVersion, &test, sizeof(std::int32_t));

        std::uint32_t taggedSkillCount = taggedSkills.size();

        a_intfc->OpenRecord('UTSK', SerializationVersion);
        a_intfc->WriteRecordData(&taggedSkillCount, sizeof(uint32_t));
        for (auto& form : taggedSkills)
        {
            std::uint32_t formID = form;
            a_intfc->WriteRecordData(&formID, sizeof(uint32_t));
        }
        REX::DEBUG("Saving tagged skills.");

        a_intfc->WriteRecord('BPRL', SerializationVersion, &playerReadyToLevelUp, sizeof(bool));

        std::uint32_t weaponRepairListCount = weaponRepairList.size();
        a_intfc->OpenRecord('UWRL', SerializationVersion);
        a_intfc->WriteRecordData(&weaponRepairListCount, sizeof(std::uint32_t));

        for (auto& it : weaponRepairList) {
            std::uint32_t list = it.first->GetFormID();
            std::uint32_t keyword = it.second->GetFormID();

            a_intfc->WriteRecordData(&list, sizeof(list));
            a_intfc->WriteRecordData(&keyword, sizeof(list));
        }
        REX::DEBUG("Saving weapon repair list.");

        std::uint32_t armorRepairListCount = armorRepairList.size();
        a_intfc->OpenRecord('UARL', SerializationVersion);
        a_intfc->WriteRecordData(&armorRepairListCount, sizeof(std::uint32_t));

        for (auto& it : armorRepairList) {
            std::uint32_t list = it.first->GetFormID();
            std::uint32_t keyword = it.second->GetFormID();

            a_intfc->WriteRecordData(&list, sizeof(list));
            a_intfc->WriteRecordData(&keyword, sizeof(list));
        }
        REX::DEBUG("Saving weapon repair list.");
    }

    void F4CWSerialization::Deserialize(const F4SE::SerializationInterface * a_intfc)
    {
        REX::DEBUG("Deserialize save data.");
        std::uint32_t type;
        std::uint32_t version;
        std::uint32_t length;

        auto dataHandler = RE::TESDataHandler::GetSingleton();

        while (a_intfc->GetNextRecordInfo(type, version, length))
        {
            switch (type) {
                case 'BPRL':
                    REX::DEBUG("Found PlayerReadyToLevelUp data.");
                    a_intfc->ReadRecordData(&playerReadyToLevelUp, sizeof(bool));
                    break;

                case 'USKP':
                    REX::DEBUG("Found playerSkillPoints data.");
                    a_intfc->ReadRecordData(&playerSkillPoints, sizeof(int32_t));
                    REX::DEBUG("{}", playerSkillPoints);
                    break;

                case 'UTSK':
                {
                    REX::DEBUG("Found tagged skills data.");
                    std::uint32_t taggedSkillsCount = 0;
                    a_intfc->ReadRecordData(&taggedSkillsCount, sizeof(uint32_t));

                    for (std::uint32_t i = 0; i < taggedSkillsCount; i++)
                    {
                        std::uint32_t oldFormID = 0;
                        std::uint32_t newFormID = 0;

                        a_intfc->ReadRecordData(&oldFormID, sizeof(uint32_t));
                        newFormID = a_intfc->ResolveFormID(oldFormID).value_or(0);

                        if (!newFormID != 0)
                        {
                            taggedSkills.insert(newFormID);
                        }
                    }
                }
                    break;

                case 'UWRL':
                {
                    REX::DEBUG("Found WeaponRepairList data.");
                    std::uint32_t weaponRepairListCount = 0;
                    a_intfc->ReadRecordData(&weaponRepairListCount, sizeof(uint32_t));

                    for (std::uint32_t i = 0; i < weaponRepairListCount; i++) {
                        std::uint32_t oldFormID = 0;
                        std::uint32_t newFormID = 0;

                        std::uint32_t oldListID = 0;
                        std::uint32_t newListID = 0;

                        a_intfc->ReadRecordData(&oldFormID, sizeof(uint32_t));
                        newFormID = a_intfc->ResolveFormID(oldFormID).value_or(0);

                        a_intfc->ReadRecordData(&oldListID, sizeof(uint32_t));
                        newFormID = a_intfc->ResolveFormID(oldListID).value_or(0);

                        if (newFormID != 0 && newListID != 0)
                        {
                            RE::BGSListForm* list = dataHandler->LookupForm<RE::BGSListForm>(newListID, MOD_ESM);
                            RE::BGSKeyword* keyword = dataHandler->LookupForm<RE::BGSKeyword>(newFormID, MOD_ESM);

                            armorRepairList.emplace(list, keyword);
                        }
                    }
                }
                    break;

                case 'UARL':
                {
                    REX::DEBUG("Found ArmorRepairList data.");
                    std::uint32_t armorRepairListCount = 0;
                    a_intfc->ReadRecordData(&armorRepairListCount, sizeof(uint32_t));

                    for (std::uint32_t i = 0; i < armorRepairListCount; i++) {
                        std::uint32_t oldFormID = 0;
                        std::uint32_t newFormID = 0;

                        std::uint32_t oldListID = 0;
                        std::uint32_t newListID = 0;

                        a_intfc->ReadRecordData(&oldFormID, sizeof(uint32_t));
                        newFormID = a_intfc->ResolveFormID(oldFormID).value_or(0);

                        a_intfc->ReadRecordData(&oldListID, sizeof(uint32_t));
                        newFormID = a_intfc->ResolveFormID(oldListID).value_or(0);

                        if (newFormID != 0 && newListID != 0)
                        {
                            RE::BGSListForm* list = dataHandler->LookupForm<RE::BGSListForm>(newListID, MOD_ESM);
                            RE::BGSKeyword* keyword = dataHandler->LookupForm<RE::BGSKeyword>(newFormID, MOD_ESM);

                            armorRepairList.emplace(list, keyword);
                        }
                    }
                }
                    break;

            }
        }
    }

    void F4CWSerialization::Revert()
    {
        REX::DEBUG("Revert serialization data.");
        playerSkillPoints = 0;
        playerReadyToLevelUp = false;
        taggedSkills.clear();
        weaponRepairList.clear();
        armorRepairList.clear();
    }
}

std::int32_t F4CWSerialization::GetSkillPoints()
{
    return playerSkillPoints;
}

void F4CWSerialization::SetSkillPoints(std::int32_t setValue)
{
    playerSkillPoints = setValue;
}

void F4CWSerialization::ModSkillPoints(std::int32_t modValue)
{
    playerSkillPoints += modValue;
}

void F4CWSerialization::SetSkillTagged(std::uint32_t skillFormID)
{
    taggedSkills.insert(skillFormID);
}

void F4CWSerialization::RemoveSkillTagged(std::uint32_t skillFormID)
{
    taggedSkills.erase(skillFormID);
}

bool F4CWSerialization::IsSkillTagged(std::uint32_t skillFormID)
{
    return taggedSkills.find(skillFormID) != taggedSkills.end();
}

std::uint32_t F4CWSerialization::GetSkillsTagged()
{
    return taggedSkills.size();
}

bool F4CWSerialization::IsReadyToLevelUp()
{
    return playerReadyToLevelUp;
}

void F4CWSerialization::SetReadyToLevelUp(bool bReady)
{
    playerReadyToLevelUp = bReady;
}

RepairListMap F4CWSerialization::GetWeaponRepairList()
{
    return weaponRepairList;
}

bool F4CWSerialization::AddToWeaponRepairList(RE::BGSListForm* repairList, RE::BGSKeyword* keyword)
{
    auto search = weaponRepairList.find(repairList);
    if (search != weaponRepairList.end()) {
        return false;
    }
    else {
        weaponRepairList.emplace(repairList, keyword);
    }

    return true;
}

RepairListMap F4CWSerialization::GetArmorRepairList()
{
    return armorRepairList;
}

bool F4CWSerialization::AddToArmorRepairList(RE::BGSListForm* repairList, RE::BGSKeyword* keyword)
{
    auto search = armorRepairList.find(repairList);
    if (search != armorRepairList.end())
    {
        return false;
    }
    else
    {
        armorRepairList.emplace(repairList, keyword);
    }
    return true;
}

//=========================================================================================================================

    /*	Save/Load F4SE Data Stuff
    =========================================================================================================================*/

void F4CWSerialization::RevertCallback(const F4SE::SerializationInterface* f4seSerial)
{
    F4CWSerialization::GetSingleton()->Revert();
}

void F4CWSerialization::LoadCallback(const F4SE::SerializationInterface * f4seSerial)
{
    F4CWSerialization::GetSingleton()->Deserialize(f4seSerial);
}

void F4CWSerialization::SaveCallback(const F4SE::SerializationInterface* f4seSerial)
{
    F4CWSerialization::GetSingleton()->Serialize(f4seSerial);
}