#include "Forms.h"
#include "Config.h"


namespace Forms
{
	void Validator::LoadEnchantmentList()
	{
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		auto& enchantments = dataHandler->GetFormArray<RE::EnchantmentItem>();

		for (auto& enchantment : enchantments) {
			if (auto* base = BaseEnchantment(enchantment);
				base && !base->fullName.empty() && !_enchantments.count(base)) {
				_enchantments.emplace(base);

				if (auto* file = base->GetFile()->fileName;
					!_stats.try_emplace(file, 1).second) {
					++_stats.at(file);
				}
			}
		}

		DEBUG("Enchantments preloaded"sv);
	}


	void Validator::LoadKeywordList()
	{
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		auto& armorList = dataHandler->GetFormArray<RE::TESObjectARMO>();
		auto& weaponList = dataHandler->GetFormArray<RE::TESObjectWEAP>();

		LoadKeywordList<RE::TESObjectARMO>(armorList);
		LoadKeywordList<RE::TESObjectWEAP>(weaponList);

		_keywords.erase(RE::TESForm::LookupByID<RE::BGSKeyword>(0x000C27BD));

		DEBUG("Keywords preloaded"sv);
	}


	template <typename T>
	void Validator::LoadKeywordList(RE::BSTArray<T*>& a_array)
	{
		auto* factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSKeyword>();
		auto* emptyForm = factory->Create();

		for (auto* form : a_array) {
			if (!form->GetNumKeywords()) {
				continue;
			}

			for (std::uint32_t index = 0; index < form->GetNumKeywords(); ++index) {
				// Recalculate count each cycle to avoid UB.
				auto count = form->GetNumKeywords();
				if (!form->GetKeywordAt(index).has_value()) {
					continue;
				}

				// DE option.
				// Only remove the keyword if disenchant everything is enabled and the object has a name.
				if (*Config::EnableDE &&
					form->keywords[index]->GetFormID() == 0x000C27BD &&
					!form->fullName.empty()) {
					// If we are not at the end of the list:
					if (count - index > 1) {
						// Swap with the end of the list.
						form->keywords[index] = form->keywords[count - 1];

					} else {
						// Otherwise set the value to empty form.
						form->keywords[index] = emptyForm;
					}
					// Truncate the array, removing our position if this is the end, or the position we swapped the bad value to.
					--form->numKeywords;
					continue;
				}

				_keywords.insert(form->keywords[index]);
			}
		}
	}


	// nested validator to find the very base form
	// patch from jsa2352
	auto Validator::BaseEnchantment(const ENIT a_ench)
		-> ENIT
	{
		auto ench = a_ench;
		while (true) {
			if (!ench->data.baseEnchantment)
				return ench;
			if (ench == ench->data.baseEnchantment)
				return ench;
			ench = ench->data.baseEnchantment;
		}
	}


	void Validator::DumpStats()
	{
		INFO("Stat report:"sv);
		INFO("Enchantments processed: {}", GetEnchantmentsAmount());
		INFO("Keywords processed: {}", GetKeywordsAmount());

		for (auto& [file, count] : _stats) {
			INFO("{:3d} ({:04.1f}%)[{}]", count, static_cast<float>(count) / GetEnchantmentsAmount() * 100, file);
		}

		DEBUG("Form dump:"sv);

		for (auto& keyword : _keywords) {
			DEBUG("KWDA({:X})[{}]", keyword->GetFormID(), keyword->GetFormEditorID());
		}

		for (auto& enchantment : _enchantments) {
			DEBUG("ENIT({:X})[{}]", enchantment->GetFormID(), enchantment->GetFullName());
		}
	}


	void PatchAll()
	{
		Validator validator;

		validator.LoadEnchantmentList();
		validator.LoadKeywordList();

		auto* factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSListForm>();
		auto* list = factory->Create();

		list->forms.resize(validator.GetKeywordsAmount());
		for (auto& keyword : validator.GetLoadedKeywords()) {
			list->forms.emplace_back(std::move(keyword));
		}

		for (auto& ench : validator.GetLoadedEnchantments()) {
			ench->data.wornRestrictions = list;
		}

		INFO("Forms patched"sv);
	}
}
