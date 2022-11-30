#pragma once

namespace Forms
{
	class Validator
	{
	public:
		using ENIT = RE::EnchantmentItem;
		using KWDA = RE::BGSKeyword;

		Validator() = default;
		~Validator() { this->DumpStats(); }

		void ResolveExclusion();
		void LoadEnchantmentList();
		void LoadKeywordList();
		template <typename T>
		void LoadKeywordList(RE::BSTArray<T*>& a_array);

		[[nodiscard]] constexpr auto& GetLoadedEnchantments() const noexcept { return _enchantments; };
		[[nodiscard]] constexpr auto& GetLoadedKeywords() const noexcept { return _keywords; }

		[[nodiscard]] auto GetEnchantmentsAmount() const noexcept { return static_cast<std::uint32_t>(_enchantments.size()); }
		[[nodiscard]] auto GetKeywordsAmount() const noexcept { return static_cast<std::uint32_t>(_keywords.size()); }

	private:
		ENIT* BaseEnchantment(ENIT* a_ench);
		void DumpStats();

		std::unordered_set<ENIT*> _enchantments;
		std::unordered_set<KWDA*> _keywords;
		std::unordered_set<ENIT*> _excludedEnch;
		std::unordered_set<const RE::TESFile*> _excludedFile;
		std::unordered_map<std::string_view, std::uint32_t> _stats;  // file, count
	};

	void PatchAll();
}  //namespace Forms
