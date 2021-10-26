#include "Validator.h"


void Validator::PreloadEnchantmentList()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	auto& enchantments = dataHandler->GetFormArray<RE::EnchantmentItem>();
	for (auto& enchantment : enchantments) {
		auto base = NestedValidate(enchantment);
		if (base && !base->fullName.empty() && !_enchantments.count(base)) {
			_enchantments.insert(base);

			auto file = base->GetFile()->fileName;
			if (!_stats.try_emplace(file, 1).second) {
				++_stats.at(file);
			}
		}
	}

	DEBUG("Enchantments preloaded"sv);
}


void Validator::PreloadKeywordList()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	auto& armorList = dataHandler->GetFormArray<RE::TESObjectARMO>();
	auto& weaponList = dataHandler->GetFormArray<RE::TESObjectWEAP>();

	PreloadKeywordList<RE::TESObjectARMO>(armorList);
	PreloadKeywordList<RE::TESObjectWEAP>(weaponList);

	_keywords.erase(RE::TESForm::LookupByID<RE::BGSKeyword>(0x000C27BD));

	DEBUG("Keywords preloaded"sv);
}


template <typename T>
void Validator::PreloadKeywordList(RE::BSTArray<T*>& a_array)
{
	auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSKeyword>();
	const auto emptyForm = factory->Create();

	for (auto form : a_array) {
		const auto count = form->GetNumKeywords();
		if (count == 0) {
			continue;
		}

		for (auto index = 0; index < count; ++index) {
			if (form->GetKeywordAt(index) == std::nullopt) {
				continue;
			}

			// DE option
			if (form->GetKeywordAt(index).value() == RE::TESForm::LookupByID<RE::BGSKeyword>(0x000C27BD)) {
				form->keywords[index] = emptyForm;
				--form->numKeywords;
				continue;
			}

			_keywords.insert(form->GetKeywordAt(index).value());
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

	for (auto&& [ file, count ] : _stats) {
		INFO("{:3d} ({:04.1f}%)[{}]", count, static_cast<float>(count) / GetEnchantmentsAmount() * 100, file);
	}

	DEBUG("Form dump:"sv);

	for (auto& keyword : _keywords) {
		DEBUG("KWDA({:#x})[{}]", keyword->GetFormID(), keyword->GetFormEditorID());
	}

	for (auto& enchantment : _enchantments) {
		DEBUG("ENIT({:#x})[{}]", enchantment->GetFormID(), enchantment->GetFullName());
	}
}
