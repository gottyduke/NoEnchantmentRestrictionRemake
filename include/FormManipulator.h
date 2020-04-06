#pragma once

#include "Validator.h"


namespace FormManipulator
{

	inline void TryReplaceForm()
	{
		auto validator = Validator::GetSingleton();
		validator->PreloadEnchantmentList();
		validator->PreloadKeywordList();

		auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSListForm>();
		auto list = factory->Create();

		list->forms.resize(validator->GetKeywordsAmount());
		size_t it = 0;
		for (auto keyword : validator->GetLoadedKeywords()) {
			list->forms[it] = keyword;
			++it;
		}

		for (auto ench : validator->GetLoadedEnchantments()) {
			ench->data.wornRestrictions = list;
		}

		_MESSAGE("Finished enchantment form replacing");
	}


	inline void TryReformData()
	{
		_MESSAGE("Finished enchantment data reforming");
	}
}
