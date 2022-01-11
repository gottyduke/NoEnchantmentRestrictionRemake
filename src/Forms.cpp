#include "Forms.h"
#include "Config.h"


namespace Forms
{
	void Validator::LoadEnchantmentList()
	{
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		auto& enchantments = dataHandler->GetFormArray<RE::EnchantmentItem>();

		for (auto& enchantment : enchantments) {
			if (auto* base = NestedValidate(enchantment);
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
			const auto count = form->GetNumKeywords();
			if (!count) {
				continue;
			}

			for (std::uint32_t index = 0; index < count; ++index) {
				if (!form->GetKeywordAt(index).has_value()) {
					continue;
				}

				// DE option
				if (*Config::EnableDE && form->keywords[index]->GetFormID() == 0x000C27BD) {
					form->keywords[index] = emptyForm;
					--form->numKeywords;
					continue;
				}

				_keywords.insert(form->keywords[index]);
			}
		}
	}


	// nested validator to find the very base form
	auto Validator::NestedValidate(const ENIT a_ench)
		-> ENIT
	{
		return (a_ench->data.baseEnchantment && a_ench != a_ench->data.baseEnchantment)
			? NestedValidate(a_ench->data.baseEnchantment)
			: a_ench;
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
