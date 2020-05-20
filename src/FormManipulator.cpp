#include "FormManipulator.h"
#include "Validator.h"


namespace FormManipulator
{
	void TryReplaceForm()
	{
		Validator validator;
		validator->PreloadEnchantmentList();
		validator->PreloadKeywordList();

		auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSListForm>();
		auto list = factory->Create();

		list->forms.resize(validator->GetKeywordsAmount());
		std::size_t iter = 0;
		for (auto keyword : validator->GetLoadedKeywords()) {
			list->forms[iter] = keyword;
			++iter;
		}

		for (auto ench : validator->GetLoadedEnchantments()) {
			ench->data.wornRestrictions = list;
		}

		_MESSAGE("Finished enchantment form replacing");
	}
}
