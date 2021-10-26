#pragma once


class Validator
{
public:
	using ENIT = RE::EnchantmentItem*;
	using KWDA = RE::BGSKeyword*;

	void PreloadEnchantmentList();
	void PreloadKeywordList();

	template <typename T>
	void PreloadKeywordList(RE::BSTArray<T*>& a_array);

	[[nodiscard]] std::size_t GetEnchantmentsAmount() const noexcept { return _enchantments.size(); }
	[[nodiscard]] std::size_t GetKeywordsAmount() const noexcept { return _keywords.size(); }

	std::unordered_set<ENIT>& GetLoadedEnchantments() noexcept { return _enchantments; };
	std::unordered_set<KWDA>& GetLoadedKeywords() noexcept { return _keywords; }

	Validator() = default;
	~Validator() { this->DumpStats(); }

	Validator(const Validator&) = delete;
	Validator(Validator&&) = delete;

	Validator& operator=(const Validator&) = delete;
	Validator& operator=(Validator&&) = delete;

	Validator* operator->() { return this; }
private:
	ENIT NestedValidate(ENIT a_ench);

	void DumpStats();

	std::unordered_set<ENIT>				_enchantments;
	std::unordered_set<KWDA>				_keywords;
	std::unordered_map<char*, unsigned int>	_stats; // file, count
};
