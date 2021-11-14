#include "FormManipulator.h"
#include "Validator.h"


namespace FormManipulator
{
	void ReplaceForm()
	{
		auto validator = std::make_unique<Validator>();

		validator->PreloadEnchantmentList();
		validator->PreloadKeywordList();

		auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSListForm>();
		auto list = factory->Create();

		list->forms.resize(static_cast<std::uint32_t>(validator->GetKeywordsAmount()));
		for (auto& keyword : validator->GetLoadedKeywords()) {
			list->forms.push_back(std::move(keyword));
		}

		for (auto& ench : validator->GetLoadedEnchantments()) {
			ench->data.wornRestrictions = std::move(list);
		}

		INFO("Forms replaced"sv);
	}
}
