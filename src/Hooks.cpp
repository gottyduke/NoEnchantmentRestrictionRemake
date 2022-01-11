#include "Hooks.h"
#include "Config.h"


namespace Hooks
{
#if ANNIVERSARY_EDITION

	// 1-6-323: 0x894EE0 + 0x212
	constexpr std::uint64_t FuncID = 51242;
	constexpr std::ptrdiff_t OffsetLow = 0x212;
	constexpr std::ptrdiff_t OffsetHigh = 0x243;

#else

	// 1-5-97: 0x866E20 + 0x1CD
	constexpr std::uint64_t FuncID = 50329;
	constexpr std::ptrdiff_t OffsetLow = 0x1CD;
	constexpr std::ptrdiff_t OffsetHigh = 0x1FF;

#endif

	constexpr OpCode AsmSrc[]{
		0xB8,					// mov eax,
		0x00, 0x00, 0x00, 0x00, // Imm32
	};

	// opcode
	constexpr std::ptrdiff_t ImmediateOffset = sizeof(OpCode);

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
