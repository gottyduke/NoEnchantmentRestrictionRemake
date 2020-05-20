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

	// Collect valid enchantments
	void PreloadEnchantmentList();
	void PreloadKeywordList();

	template <typename T>
	void PreloadKeywordList(RE::BSTArray<T*>& a_array);

	[[nodiscard]] std::size_t GetKeywordsAmount() const noexcept { return _keywords.size(); }
	[[nodiscard]] std::size_t GetEnchantmentsAmount() const noexcept { return _enchantments.size(); }

	std::unordered_set<Keyw>& GetLoadedKeywords() noexcept { return _keywords; }
	std::unordered_set<Ench>& GetLoadedEnchantments() noexcept { return _enchantments; };

	Validator() = default;
	~Validator() { this->DumpStats(); }

	Validator(const Validator&) = delete;
	Validator(Validator&&) = delete;

	Validator& operator=(const Validator&) = delete;
	Validator& operator=(Validator&&) = delete;

	Validator* operator->() { return this; }
private:
	Ench NestedValidate(Ench a_ench);

	void TryFillStats(Ench a_ench);
	void DumpStats();

	std::unordered_set<Ench> _enchantments;
	std::unordered_set<Keyw> _keywords;

	std::unordered_map<char*, int> _stats;
};
