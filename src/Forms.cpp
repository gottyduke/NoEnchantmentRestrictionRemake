#include "Forms.h"
#include "Config.h"

namespace Forms
{
	void Validator::ResolveExclusion()
	{
		auto* dataHandler = RE::TESDataHandler::GetSingleton();

		for (auto& ex : Config::Exclusion.get_collection()) {
			auto list = dku::string::split(ex, "|"sv);
			if (list.size() < 2) {
				continue;
			}

			auto* file = dataHandler->LookupLoadedModByName(list.back()) ?
			                 dataHandler->LookupLoadedModByName(list.back()) :
			                 dataHandler->LookupLoadedLightModByName(list.back());
			if (!file) {
				continue;
			} else {
				list.pop_back();
			}

			if (list.front() == "*") {
				_excludedFile.emplace(file);
				INFO("Excluding plugin: {}", file->fileName);
			} else {
				for (auto& idStr : list) {
					auto id = dku::string::lexical_cast<RE::FormID>(idStr, true);
					if (auto* form = dataHandler->LookupForm(id, file->fileName); form && form->As<ENIT>()) {
						_excludedEnch.emplace(form->As<ENIT>());
						INFO("Excluding entry: {:X} | {}", id, file->fileName);
					}
				}
			}
		}
	}

	void Validator::LoadEnchantmentList()
	{
		ResolveExclusion();

		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		auto& enchantments = dataHandler->GetFormArray<ENIT>();

		for (auto* enchantment : enchantments) {
			if (_excludedFile.contains(enchantment->GetFile()) || _excludedEnch.contains(enchantment)) {
				DEBUG("Skipping excluded entry: {}({:X}) | {}", enchantment->GetName(), enchantment->GetFormID(), enchantment->GetFile()->fileName)
				continue;
			}

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

		_keywords.erase(RE::TESForm::LookupByID<KWDA>(0x0C27BD));

		DEBUG("Keywords preloaded"sv);
	}

	template <typename T>
	void Validator::LoadKeywordList(RE::BSTArray<T*>& a_array)
	{
		auto* factory = RE::IFormFactory::GetConcreteFormFactoryByType<KWDA>();
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
					form->keywords[index]->GetFormID() == 0x0C27BD &&
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
	auto Validator::BaseEnchantment(ENIT* a_ench)
		-> ENIT*
	{
		auto* ench = a_ench;
		while (true) {
			if (!ench->data.baseEnchantment ||
				ench == ench->data.baseEnchantment) {
				return ench;
			}
			ench = ench->data.baseEnchantment;
		}
	}

	void Validator::DumpStats()
	{
		auto raw = _stats | std::views::transform([this](auto& pair) { return fmt::format("{:4d} ({:04.1f}%)[{}]", pair.second, static_cast<float>(pair.second) / GetEnchantmentsAmount() * 100, pair.first); });
		INFO("\n{:->45}\nStat report:\nEnchantments processed: {}\nKeywords processed: {}\n{:->45}\n{}\n{:->45}", "", GetEnchantmentsAmount(), GetKeywordsAmount(), "", dku::string::join({ raw.begin(), raw.end() }, "\n"sv), "");

		if (spdlog::get_level() == spdlog::level::debug) {
			DEBUG("Form dump:"sv);

			for (auto& keyword : _keywords) {
				DEBUG("KWDA({:X})[{}]", keyword->GetFormID(), keyword->GetFormEditorID());
			}

			for (auto& enchantment : _enchantments) {
				DEBUG("ENIT({:X})[{}]", enchantment->GetFormID(), enchantment->GetFullName());
			}
		}

		INFO("Exiting");
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
