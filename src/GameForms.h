#pragma once
#include <RE/T/TESGlobal.h>
#include <RE/B/BGSPerk.h>

enum SPECIALFormIDs
{
	StrengthID = 706,
	PerceptionID,
	EnduranceID,
	CharismaID,
	IntelligenceID,
	AgilityID,
	LuckID,
	ExperienceID
};

#ifndef VanillaAVStruct
#define VanillaAVStruct
struct VanillaAV_Struct
{
	// Vanilla Actor Values
	RE::ActorValueInfo* Strength;
	RE::ActorValueInfo* Perception;
	RE::ActorValueInfo* Endurance;
	RE::ActorValueInfo* Charisma;
	RE::ActorValueInfo* Intelligence;
	RE::ActorValueInfo* Agility;
	RE::ActorValueInfo* Luck;
};
#endif
extern VanillaAV_Struct VanillaActorValues;


#ifndef CW_SkillsStruct
#define CW_SkillsStruct
struct CW_Skills_Struct
{
	// PA Skill Values

	//	CW Skill Values
	RE::ActorValueInfo* Barter;
	RE::ActorValueInfo* BigGuns;
	RE::ActorValueInfo* EnergyWeapons;
	RE::ActorValueInfo* Explosives;
	RE::ActorValueInfo* Lockpick;
	RE::ActorValueInfo* Medicine;
	RE::ActorValueInfo* MeleeWeapons;
	RE::ActorValueInfo* Repair;
	RE::ActorValueInfo* Science;
	RE::ActorValueInfo* SmallGuns;
	RE::ActorValueInfo* Sneak;
	RE::ActorValueInfo* Speech;
	RE::ActorValueInfo* Unarmed;

	//	CW Karma
	RE::ActorValueInfo* Karma;

	//	CW Resists
	RE::ActorValueInfo* WeaponCNDResist;

	// PA Resists
};
#endif
extern CW_Skills_Struct CW_Skills;

#ifndef CWGlobalsStruct
#define CWGlobalsStruct
struct CWGlobals_Struct
{
	// PA Globals
	RE::TESGlobal* Karma;
	RE::TESGlobal* AmmoDefaultDegradation;
	RE::TESGlobal* TutorialWPNCND;
};
#endif
extern CWGlobals_Struct CWGlobals;

#ifndef CWPerksStruct
#define CWPerksStruct
struct CWPerks_Struct
{
	//	CW Weapon Type Perks
	RE::BGSPerk* WeaponTypeBigGunsPerk;
	RE::BGSPerk* WeaponTypeEnergyWeaponsPerk;
	RE::BGSPerk* WeaponTypeExplosivesPerk;
	RE::BGSPerk* WeaponTypeMeleeWeaponsPerk;
	RE::BGSPerk* WeaponTypeSmallGunsPerk;
	RE::BGSPerk* WeaponTypeUnarmedPerk;

	//	CW Handler Perks
	RE::BGSPerk* WeaponConditionHandlerPerk;
};
#endif
extern CWPerks_Struct CWPerks;


#ifndef ItemDegredationStruct
#define ItemDegredationStruct
struct ItemDegredation_Struct
{
	//	these holds our weapon condition on the Weapon Form in Creation Kit
	RE::BGSDamageType* weaponConditionHealthMaxDMGT;
	RE::BGSDamageType* weaponConditionHealthStartingDMGT;

	//	Actor Values that hold information on Items
	RE::ActorValueInfo* itemConditionMaxHealth;
	RE::ActorValueInfo* itemConditionMinHealth;
	RE::ActorValueInfo* itemConditionStartCond;

	//	Reload Jam Stuff
	RE::BGSAction* actionReloadJam;

	/*
	//	Repair Lists for Weapons
	BGSListForm* repair10mmPistol;
	BGSListForm* repair10mmSub;
	BGSListForm* repair32CalPistol;
	BGSListForm* repairAlienBlaster;
	BGSListForm* repairAssaultRifle;
	BGSListForm* repairBaseballBat;
	BGSListForm* repairBBGun;
	BGSListForm* repairBrassKnuckles;
	BGSListForm* repairChineseAssault;
	BGSListForm* repairChineseSword;
	BGSListForm* repairChinesePistol;
	BGSListForm* repairDartGun;
	BGSListForm* repairFatman;
	BGSListForm* repairFlamer;
	BGSListForm* repairGatlingLaser;
	BGSListForm* repairHuntingRifle;
	BGSListForm* repairKnife;
	BGSListForm* repairKnifeCombat;
	BGSListForm* repairLaserPistol;
	BGSListForm* repairLaserRifle;
	BGSListForm* repairLeadPipe;
	BGSListForm* repairMesmetron;
	BGSListForm* repairMinigun;
	BGSListForm* repairMissileLauncher;
	BGSListForm* repairNailBoard;
	BGSListForm* repairPlasmaPistol;
	BGSListForm* repairPlasmaRifle;
	BGSListForm* repairPoliceBaton;
	BGSListForm* repairPoolCue;
	BGSListForm* repairPowerFist;
	BGSListForm* repairRailwayRifle;
	BGSListForm* repairRipper;
	BGSListForm* repairRockItLauncher;
	BGSListForm* repairRollingPin;
	BGSListForm* repairSawedOffShotgun;
	BGSListForm* repairScoped44Magnum;
	BGSListForm* repairShishkebab;
	BGSListForm* repairShotgunCombat;
	BGSListForm* repairSledgehammer;
	BGSListForm* repairSniperRifle;
	BGSListForm* repairSuperSledge;
	BGSListForm* repairSwitchblade;
	BGSListForm* repairTireIron;

	//	Repair Keywords for Weapons
	BGSKeyword* repairKW10mmPistol;
	BGSKeyword* repairKW10mmSub;
	BGSKeyword* repairKW32CalPistol;
	BGSKeyword* repairKWAlienBlaster;
	BGSKeyword* repairKWAssaultRifle;
	BGSKeyword* repairKWBaseballBat;
	BGSKeyword* repairKWBBGun;
	BGSKeyword* repairKWBrassKnuckles;
	BGSKeyword* repairKWChineseAssault;
	BGSKeyword* repairKWChineseSword;
	BGSKeyword* repairKWChinesePistol;
	BGSKeyword* repairKWDartGun;
	BGSKeyword* repairKWFatman;
	BGSKeyword* repairKWFlamer;
	BGSKeyword* repairKWGatlingLaser;
	BGSKeyword* repairKWHuntingRifle;
	BGSKeyword* repairKWKnife;
	BGSKeyword* repairKWKnifeCombat;
	BGSKeyword* repairKWLaserPistol;
	BGSKeyword* repairKWLaserRifle;
	BGSKeyword* repairKWLeadPipe;
	BGSKeyword* repairKWMesmetron;
	BGSKeyword* repairKWMinigun;
	BGSKeyword* repairKWMissileLauncher;
	BGSKeyword* repairKWNailBoard;
	BGSKeyword* repairKWPlasmaPistol;
	BGSKeyword* repairKWPlasmaRifle;
	BGSKeyword* repairKWPoliceBaton;
	BGSKeyword* repairKWPoolCue;
	BGSKeyword* repairKWPowerFist;
	BGSKeyword* repairKWRailwayRifle;
	BGSKeyword* repairKWRipper;
	BGSKeyword* repairKWRockItLauncher;
	BGSKeyword* repairKWRollingPin;
	BGSKeyword* repairKWSawedOffShotgun;
	BGSKeyword* repairKWScoped44Magnum;
	BGSKeyword* repairKWShishkebab;
	BGSKeyword* repairKWShotgunCombat;
	BGSKeyword* repairKWSledgehammer;
	BGSKeyword* repairKWSniperRifle;
	BGSKeyword* repairKWSuperSledge;
	BGSKeyword* repairKWSwitchblade;
	BGSKeyword* repairKWTireIron;
	*/

	//	Misc Forms
	RE::ActorValueInfo* tempConditionVariable;	// used by AddItemHealthPercent to temporarily add our wanted condition to weapon
};
#endif
extern ItemDegredation_Struct ItemDegredationForms;