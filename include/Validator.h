#pragma once

#include "Settings.h"

#include "RE/Skyrim.h"

#include <unordered_set> // unordered_set
#include <unordered_map> // unordered_map


class Validator
{
public:
	using Ench = RE::EnchantmentItem*;
	using Keyw = RE::BGSKeyword*;

	static Validator* GetSingleton();

	// Collect valid enchantments
	void PreloadEnchantmentList();
	void PreloadKeywordList();
	template <typename T>
	void PreloadKeywordList(RE::BSTArray<T*> a_array);

	[[nodiscard]] size_t GetKeywordsAmount() const;
	[[nodiscard]] size_t GetEnchantmentsAmount() const;

	void DumpStats();

	std::unordered_set<Ench>& GetLoadedEnchantments();
	std::unordered_set<Keyw>& GetLoadedKeywords();

	Validator(const Validator&) = delete;
	Validator(Validator&&) = delete;

	Validator& operator=(const Validator&) = delete;
	Validator& operator=(Validator&&) = delete;

protected:

	Validator() = default;
	~Validator() = default;


	static Ench NestedValidate(Ench a_ench);

	void TryFillStats(Ench a_ench);

private:
	std::unordered_set<Ench> _enchantments;
	std::unordered_set<Keyw> _keywords;

	std::unordered_map<char*, int> _stats;
};
