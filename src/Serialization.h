#pragma once
#include <map>
#include <RE/B/BGSKeyword.h>
#include <RE/B/BGSListForm.h>
#include <cstdint>
#include <F4SE/Interfaces.h>

typedef std::map<RE::BGSListForm*, RE::BGSKeyword*>	RepairListMap;

namespace F4CWSerialization
{
	class F4CWSerialization {
	public:
		~F4CWSerialization() = default;

		static auto GetSingleton() -> F4CWSerialization*;

		void Serialize(const F4SE::SerializationInterface* a_intfc);

		void Deserialize(const F4SE::SerializationInterface* a_intfc);

		void Revert();

	private:
		static constexpr std::uint32_t SerializationVersion = 1;
		static constexpr std::uint32_t SerializationType = 'F4CW';
	};

	//	Skill Point Functions
	std::int32_t GetSkillPoints();
	void SetSkillPoints(std::int32_t setValue);
	void ModSkillPoints(std::int32_t modValue);

	//	Tag Skill Functions
	void SetSkillTagged(std::uint32_t skillFormID);
	void RemoveSkillTagged(std::uint32_t skillFormID);
	bool IsSkillTagged(std::uint32_t skillFormID);
	std::uint32_t GetSkillsTagged();

	//	Level Up Functions
	bool IsReadyToLevelUp();
	void SetReadyToLevelUp(bool bReady);

	//	Repair List Functions
	RepairListMap GetWeaponRepairList();
	bool AddToWeaponRepairList(RE::BGSListForm* repairList, RE::BGSKeyword* keyword);
	RepairListMap GetArmorRepairList();
	bool AddToArmorRepairList(RE::BGSListForm* repairList, RE::BGSKeyword* keyword);

	//	Serialization Functions
	void RevertCallback(const F4SE::SerializationInterface* f4seSerial);
	void LoadCallback(const F4SE::SerializationInterface* f4seSerial);
	void SaveCallback(const F4SE::SerializationInterface* f4seSerial);
}