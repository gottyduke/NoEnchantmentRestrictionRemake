#pragma once


namespace Forms
{
	class Validator
	{
	public:
		using ENIT = RE::EnchantmentItem*;
		using KWDA = RE::BGSKeyword*;

		Validator() = default;
		~Validator() { this->DumpStats(); }


		void LoadEnchantmentList();
		void LoadKeywordList();
		template <typename T>
		void LoadKeywordList(RE::BSTArray<T*>& a_array);

		auto& GetLoadedEnchantments() noexcept { return _enchantments; };
		auto& GetLoadedKeywords() noexcept { return _keywords; }

		[[nodiscard]] auto GetEnchantmentsAmount() const noexcept { return static_cast<std::uint32_t>(_enchantments.size()); }
		[[nodiscard]] auto GetKeywordsAmount() const noexcept { return static_cast<std::uint32_t>(_keywords.size()); }
	private:
		ENIT BaseEnchantment(const ENIT a_ench);
		void DumpStats();


		std::unordered_set<ENIT>				_enchantments;
		std::unordered_set<KWDA>				_keywords;
		std::unordered_map<char*, unsigned int>	_stats; // file, count
	};


	void PatchAll();
} //namespace Forms
