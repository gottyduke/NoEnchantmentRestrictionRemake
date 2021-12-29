#include "Hooks.h"
#include "Config.h"


namespace Hooks
{
#if ANNIVERSARY_EDITION

	// 1-6-323: 0x894EE0 + 0x212
	constexpr std::uint64_t FuncID = 51242;
	constexpr std::uintptr_t OffsetLow = 0x212;
	constexpr std::uintptr_t OffsetHigh = 0x243;

	constexpr OpCode AsmSrc[]{
		0xB8,					// mov eax,
		0x00, 0x00, 0x00, 0x00, // Imm32
	};

	// opcode
	constexpr std::uintptr_t ImmediateOffset = sizeof(OpCode);

#else

	constexpr std::uint64_t FuncID = 50565;
	constexpr std::uintptr_t OffsetLow = 0x24;
	constexpr std::uintptr_t OffsetHigh = 0x2A;

	constexpr DKUtil::Hook::Patch Prolog = {
		"\x31\xED\xC7\x86\x98\x01\x00\x00\x00\x00\x00\x00",
		12,
	};

	constexpr DKUtil::Hook::Patch Epilog = {
		"\x48\x89\x69\xE0",
		4
	};

	// placeholding bytes | epilog | JmpRel
	constexpr std::uintptr_t ImmediateOffset = sizeof(DKUtil::Hook::Imm32) + Epilog.Size + sizeof(DKUtil::Hook::detail::JmpRel);

#endif


	HookHandle _Hook_UES;


	void Install()
	{
		static std::once_flag HookInit;
		std::call_once(HookInit, [&]()
			{
				constexpr Patch AsmPatch = {
					std::addressof(AsmSrc),
					sizeof(AsmSrc)
				};

				_Hook_UES = DKUtil::Hook::AddASMPatch<OffsetLow, OffsetHigh>(DKUtil::Hook::IDToAbs(FuncID), &AsmPatch);
			}
		);

		_Hook_UES->Enable();

		DKUtil::Hook::WriteImm(_Hook_UES->TramPtr + ImmediateOffset, static_cast<Imm32>(*Config::UEMax));

		INFO("Hooks installed"sv);
	}


	void Uninstall()
	{
		_Hook_UES->Disable();

		INFO("Hooks uninstalled"sv);
	}
}
