#pragma once
#include <cstdarg>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <RE/A/AlarmPackage.h>
#include <RE/A/AlchemyItem.h>
#include <RE/A/ArrowProjectile.h>
#include <RE/B/BarrierProjectile.h>
#include <RE/B/BeamProjectile.h>
#include <RE/B/BGSAcousticSpace.h>
#include <RE/B/BGSAddonNode.h>
#include <RE/B/BGSAimModel.h>
#include <RE/B/BGSArtObject.h>
#include <RE/B/BGSAssociationType.h>
#include <RE/B/BGSAudioEffectChain.h>
#include <RE/B/BGSBendableSpline.h>
#include <RE/B/BGSBodyPartData.h>
#include <RE/B/BGSCameraPath.h>
#include <RE/B/BGSCameraShot.h>
#include <RE/B/BGSCollisionLayer.h>
#include <RE/B/BGSColorForm.h>
#include <RE/B/BGSComponent.h>
#include <RE/B/BGSConstructibleObject.h>
#include <RE/B/BGSDamageType.h>
#include <RE/B/BGSDebris.h>
#include <RE/B/BGSDualCastData.h>
#include <RE/B/BGSEncounterZone.h>
#include <RE/B/BGSEquipSlot.h>
#include <RE/B/BGSExplosion.h>
#include <RE/B/BGSFootstep.h>
#include <RE/B/BGSFootstepSet.h>
#include <RE/B/BGSGodRays.h>
#include <RE/B/BGSHazard.h>
#include <RE/B/BGSIdleMarker.h>
#include <RE/B/BGSImpactData.h>
#include <RE/B/BGSImpactDataSet.h>
#include <RE/B/BGSInstanceNamingRules.h>
#include <RE/B/BGSLensFlare.h>
#include <RE/B/BGSLightingTemplate.h>
#include <RE/B/BGSListForm.h>
#include <RE/B/BGSLocation.h>
#include <RE/B/BGSMaterialObject.h>
#include <RE/B/BGSMaterialSwap.h>
#include <RE/B/BGSMaterialType.h>
#include <RE/B/BGSMessage.h>
#include <RE/B/BGSMod.h>
#include <RE/B/BGSMovableStatic.h>
#include <RE/B/BGSMovementType.h>
#include <RE/B/BGSMusicTrackFormWrapper.h>
#include <RE/B/BGSNote.h>
#include <RE/B/BGSOutfit.h>
#include <RE/B/BGSPerk.h>
#include <RE/B/BGSProjectile.h>
#include <RE/B/BGSReferenceEffect.h>
#include <RE/B/BGSRelationship.h>
#include <RE/B/BGSReverbParameters.h>
#include <RE/B/BGSShaderParticleGeometryData.h>
#include <RE/B/BGSSoundCategory.h>
#include <RE/B/BGSSoundCategorySnapshot.h>
#include <RE/B/BGSSoundKeywordMapping.h>
#include <RE/B/BGSSoundOutput.h>
#include <RE/B/BGSSoundTagSet.h>
#include <RE/B/BGSStaticCollection.h>
#include <RE/B/BGSTalkingActivator.h>
#include <RE/B/BGSTerminal.h>
#include <RE/B/BGSTextureSet.h>
#include <RE/B/BGSTransform.h>
#include <RE/B/BGSZoomData.h>
#include <RE/B/BSSpinLock.h>
#include <RE/C/ConeProjectile.h>
#include <RE/D/DialoguePackage.h>
#include <RE/E/EffectSetting.h>
#include <RE/E/EnchantmentItem.h>
#include <RE/E/Explosion.h>
#include <RE/F/FlameProjectile.h>
#include <RE/F/FleePackage.h>
#include <RE/G/GrenadeProjectile.h>
#include <RE/H/Hazard.h>
#include <RE/I/IngredientItem.h>
#include <RE/M/MissileProjectile.h>
#include <RE/P/PlayerCharacter.h>
#include <RE/P/Projectile.h>
#include <RE/S/Script.h>
#include <RE/S/ScrollItem.h>
#include <RE/S/SpectatorPackage.h>
#include <RE/S/SpellItem.h>
#include <RE/T/TESAmmo.h>
#include <RE/T/TESClass.h>
#include <RE/T/TESClimate.h>
#include <RE/T/TESCombatStyle.h>
#include <RE/T/TESEffectShader.h>
#include <RE/T/TESEyes.h>
#include <RE/T/TESFaction.h>
#include <RE/T/TESFlora.h>
#include <RE/T/TESForm.h>
#include <RE/T/TESFurniture.h>
#include <RE/T/TESGrass.h>
#include <RE/T/TESImageSpace.h>
#include <RE/T/TESKey.h>
#include <RE/T/TESLandTexture.h>
#include <RE/T/TESLevCharacter.h>
#include <RE/T/TESLevItem.h>
#include <RE/T/TESLevSpell.h>
#include <RE/T/TESLoadScreen.h>
#include <RE/T/TESNPC.h>
#include <RE/T/TESObjectACTI.h>
#include <RE/T/TESObjectARMA.h>
#include <RE/T/TESObjectARMO.h>
#include <RE/T/TESObjectBOOK.h>
#include <RE/T/TESObjectCONT.h>
#include <RE/T/TESObjectDOOR.h>
#include <RE/T/TESObjectLIGH.h>
#include <RE/T/TESObjectMISC.h>
#include <RE/T/TESObjectSTAT.h>
#include <RE/T/TESObjectTREE.h>
#include <RE/T/TESObjectWEAP.h>
#include <RE/T/TESPackage.h>
#include <RE/T/TESRegion.h>
#include <RE/T/TESShout.h>
#include <RE/T/TESSoulGem.h>
#include <RE/T/TESTopicInfo.h>
#include <RE/T/TESWaterForm.h>
#include <RE/T/TESWeather.h>
#include <RE/T/TESWordOfPower.h>
#include <RE/T/TrespassPackage.h>
#include <REL/Relocation.h>
#include <REX/LOG.h>

namespace F4CW {
	namespace Patches {
		using namespace RE;

		class LoadEditorIDs
		{
		private:
			template <class Form>
			static void InstallHook()
			{
				REL::Relocation<std::uintptr_t> vtbl{ Form::VTABLE[0] };
				_GetFormEditorID = vtbl.write_vfunc(0x3A, GetFormEditorID);
				_SetFormEditorID = vtbl.write_vfunc(0x3B, SetFormEditorID);
			}

			static const char* GetFormEditorID(TESForm* a_this)
			{
				auto it = rmap.find(a_this->formID);
				if (it != rmap.end())
				{
					return it->second.c_str();
				}

				return _GetFormEditorID(a_this);
			}

			static bool SetFormEditorID(TESForm* a_this, const char* a_editor)
			{
				auto edid = std::string_view{ a_editor };
				if (a_this->formID < 0xFF000000 && !edid.empty())
				{
					AddToGameMap(a_this, a_editor);
				}

				return _SetFormEditorID(a_this, a_editor);
			}

		private:
			static void AddToGameMap(TESForm* a_this, const char* a_editorID)
			{
				const auto& [map, lock] = TESForm::GetAllFormsByEditorID();
				const BSAutoWriteLock locker{ lock.get() };
				if (map)
				{
					auto iter = map->find(a_editorID);
					if (iter != map->end())
					{
						if (iter->second->GetFormID() != a_this->GetFormID())
						{
							REX::WARN(
								"EditorID Conflict: {:08X} and {:08X} are both {:s}."sv,
								iter->second->GetFormID(),
								a_this->GetFormID(),
								a_editorID);
						}

						return;
					}

					map->emplace(a_editorID, a_this);
					rmap.emplace(a_this->formID, a_editorID);
				}
			}

			inline static std::unordered_map<std::uint32_t, std::string> rmap;

			inline static REL::Relocation<decltype(&TESForm::SetFormEditorID)> _SetFormEditorID;
			inline static REL::Relocation<decltype(&TESForm::GetFormEditorID)> _GetFormEditorID;

		public:
			static void Install()
			{
				// InstallHook<BGSKeyword>();
				// InstallHook<BGSLocationRefType>();
				// InstallHook<BGSAction>();
				InstallHook<BGSTransform>();
				InstallHook<BGSComponent>();
				InstallHook<BGSTextureSet>();
				// InstallHook<BGSMenuIcon>();
				// InstallHook<TESGlobal>();
				InstallHook<BGSDamageType>();
				InstallHook<TESClass>();
				InstallHook<TESFaction>();
				// InstallHook<BGSHeadPart>();
				InstallHook<TESEyes>();
				// InstallHook<TESRace>();
				// InstallHook<TESSound>();
				InstallHook<BGSAcousticSpace>();
				InstallHook<EffectSetting>();
				InstallHook<Script>();
				InstallHook<TESLandTexture>();
				InstallHook<EnchantmentItem>();
				InstallHook<SpellItem>();
				InstallHook<ScrollItem>();
				InstallHook<TESObjectACTI>();
				InstallHook<BGSTalkingActivator>();
				InstallHook<TESObjectARMO>();
				InstallHook<TESObjectBOOK>();
				InstallHook<TESObjectCONT>();
				InstallHook<TESObjectDOOR>();
				InstallHook<IngredientItem>();
				InstallHook<TESObjectLIGH>();
				InstallHook<TESObjectMISC>();
				InstallHook<TESObjectSTAT>();
				InstallHook<BGSStaticCollection>();
				InstallHook<BGSMovableStatic>();
				InstallHook<TESGrass>();
				InstallHook<TESObjectTREE>();
				InstallHook<TESFlora>();
				InstallHook<TESFurniture>();
				InstallHook<TESObjectWEAP>();
				InstallHook<TESAmmo>();
				InstallHook<TESNPC>();
				InstallHook<TESLevCharacter>();
				InstallHook<TESKey>();
				InstallHook<AlchemyItem>();
				InstallHook<BGSIdleMarker>();
				InstallHook<BGSNote>();
				InstallHook<BGSProjectile>();
				InstallHook<BGSHazard>();
				InstallHook<BGSBendableSpline>();
				InstallHook<TESSoulGem>();
				InstallHook<BGSTerminal>();
				InstallHook<TESLevItem>();
				InstallHook<TESWeather>();
				InstallHook<TESClimate>();
				InstallHook<BGSShaderParticleGeometryData>();
				InstallHook<BGSReferenceEffect>();
				InstallHook<TESRegion>();
				// InstallHook<NavMeshInfoMap>();
				// InstallHook<TESObjectCELL>();
				// InstallHook<TESObjectREFR>();
				InstallHook<Explosion>();
				InstallHook<Projectile>();
				//InstallHook<Actor>();
				InstallHook<PlayerCharacter>();
				InstallHook<MissileProjectile>();
				InstallHook<ArrowProjectile>();
				InstallHook<GrenadeProjectile>();
				InstallHook<BeamProjectile>();
				InstallHook<FlameProjectile>();
				InstallHook<ConeProjectile>();
				InstallHook<BarrierProjectile>();
				InstallHook<Hazard>();
				// InstallHook<TESWorldSpace>();
				// InstallHook<TESObjectLAND>();
				// InstallHook<NavMesh>();
				InstallHook<TESTopicInfo>();
				// InstallHook<TESQuest>();
				// InstallHook<TESIdleForm>();
				InstallHook<TESPackage>();
				InstallHook<AlarmPackage>();
				InstallHook<DialoguePackage>();
				InstallHook<FleePackage>();
				InstallHook<SpectatorPackage>();
				InstallHook<TrespassPackage>();
				InstallHook<TESCombatStyle>();
				InstallHook<TESLoadScreen>();
				InstallHook<TESLevSpell>();
				// InstallHook<TESObjectANIO>();
				InstallHook<TESWaterForm>();
				InstallHook<TESEffectShader>();
				InstallHook<BGSExplosion>();
				InstallHook<BGSDebris>();
				InstallHook<TESImageSpace>();
				// InstallHook<TESImageSpaceModifier>();
				InstallHook<BGSListForm>();
				InstallHook<BGSPerk>();
				InstallHook<BGSBodyPartData>();
				InstallHook<BGSAddonNode>();
				// InstallHook<ActorValueInfo>();
				InstallHook<BGSCameraShot>();
				InstallHook<BGSCameraPath>();
				// InstallHook<BGSVoiceType>();
				InstallHook<BGSMaterialType>();
				InstallHook<BGSImpactData>();
				InstallHook<BGSImpactDataSet>();
				InstallHook<TESObjectARMA>();
				InstallHook<BGSEncounterZone>();
				InstallHook<BGSLocation>();
				InstallHook<BGSMessage>();
				// InstallHook<BGSDefaultObjectManager>();
				// InstallHook<BGSDefaultObject>();
				InstallHook<BGSLightingTemplate>();
				// InstallHook<BGSMusicType>();
				InstallHook<BGSFootstep>();
				InstallHook<BGSFootstepSet>();
				// InstallHook<BGSStoryManagerBranchNode>();
				// InstallHook<BGSStoryManagerQuestNode>();
				// InstallHook<BGSStoryManagerEventNode>();
				//InstallHook<BGSDialogueBranch>();
				InstallHook<BGSMusicTrackFormWrapper>();
				InstallHook<TESWordOfPower>();
				InstallHook<TESShout>();
				InstallHook<BGSEquipSlot>();
				InstallHook<BGSRelationship>();
				//InstallHook<BGSScene>();
				InstallHook<BGSAssociationType>();
				InstallHook<BGSOutfit>();
				InstallHook<BGSArtObject>();
				InstallHook<BGSMaterialObject>();
				InstallHook<BGSMovementType>();
				// InstallHook<BGSSoundDescriptorForm>();
				InstallHook<BGSDualCastData>();
				InstallHook<BGSSoundCategory>();
				InstallHook<BGSSoundOutput>();
				InstallHook<BGSCollisionLayer>();
				InstallHook<BGSColorForm>();
				InstallHook<BGSReverbParameters>();
				// InstallHook<BGSPackIn>();
				InstallHook<BGSAimModel>();
				InstallHook<BGSConstructibleObject>();
				InstallHook<BGSMod::Attachment::Mod>();
				InstallHook<BGSMaterialSwap>();
				InstallHook<BGSZoomData>();
				InstallHook<BGSInstanceNamingRules>();
				InstallHook<BGSSoundKeywordMapping>();
				InstallHook<BGSAudioEffectChain>();
				// InstallHook<BGSAttractionRule>();
				InstallHook<BGSSoundCategorySnapshot>();
				InstallHook<BGSSoundTagSet>();
				InstallHook<BGSLensFlare>();
				InstallHook<BGSGodRays>();

				REX::DEBUG("Installed Patch: 'LoadEditorIDs'.");
			}
		};
	}
}