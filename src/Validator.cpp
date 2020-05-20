//#define DEBUG_DUMP

#include "Validator.h"


// Collect valid enchantments
void Validator::PreloadEnchantmentList()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	auto& enchantments = dataHandler->GetFormArray<RE::EnchantmentItem>();
	for (auto& ench : enchantments) {
		auto base = NestedValidate(ench);
		if (base) {
			if (!_enchantments.count(base)) {
				_enchantments.insert(base);
				TryFillStats(base);
#ifdef _DEBUG
				// EnchantmentItem Dump
				_DMESSAGE("ENIT(%x)[%s]", base->GetFormID(), base->GetFullName());
#endif
			}
		}
	}
}


void Validator::PreloadKeywordList()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	auto armorList = dataHandler->GetFormArray<RE::TESObjectARMO>();
	auto weaponList = dataHandler->GetFormArray<RE::TESObjectWEAP>();

	PreloadKeywordList<RE::TESObjectARMO>(armorList);
	PreloadKeywordList<RE::TESObjectWEAP>(weaponList);

	_keywords.erase(RE::TESForm::LookupByID<RE::BGSKeyword>(0x000C27BD));
}


template <typename T>
void Validator::PreloadKeywordList(RE::BSTArray<T*>& a_array)
{
	auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSKeyword>();
	const auto emptyForm = factory->Create();

	for (auto form : a_array) {
		const auto size = form->GetNumKeywords();

		if (size == 0) {
			continue;
		}

		for (auto iter = 0; iter < size; ++iter) {
			if (form->GetKeywordAt(iter) == std::nullopt) {
				continue;
			}

			if (*Settings::disenchantEverything &&
				form->GetKeywordAt(iter).value() == RE::TESForm::LookupByID<RE::BGSKeyword>(0x000C27BD)) {
				form->keywords[iter] = emptyForm;
				--form->numKeywords;
				continue;
			}

			_keywords.insert(form->GetKeywordAt(iter).value());
		}
	}
}


// nested validator to find the very base form
auto Validator::NestedValidate(const Ench a_ench)
-> Ench
{
	return (a_ench->data.baseEnchantment && a_ench != a_ench->data.baseEnchantment)
		       ? NestedValidate(a_ench->data.baseEnchantment)
		       : a_ench;
}


void Validator::TryFillStats(const Ench a_ench)
{
	auto file = a_ench->GetFile()->fileName;
	if (_stats.find(file) == _stats.end()) {
		_stats.insert({file, 1});
	} else {
		_stats.at(file) += 1;
	}
}


void Validator::DumpStats()
{
#ifdef _DEBUG
	for (auto it : _keywords) {
		_MESSAGE("KWDA(%x)[%s]", it->GetFormID(), it->GetFormEditorID());
	}
#endif

	const auto total = _enchantments.size();
	_MESSAGE("Stat report:");
	_MESSAGE("Processed enchantments: %d", total);

	for (auto it = _stats.begin(); it != _stats.end(); ++it) {
		_MESSAGE("%-3d (%05.2f%%)[%s]", it->second, static_cast<float>(it->second) / total * 100, it->first);
	}

	_MESSAGE("Registered %d keywords", GetKeywordsAmount());
	_MESSAGE("Validator finished preloading");
}
