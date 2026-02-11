#pragma once 
#include <cstdint>

namespace elf {

	enum class PHType : std::uint32_t {
		Null       = 0x00000000,
		Load       = 0x00000001,
		Dynamic    = 0x00000002,
		Interp     = 0x00000003,
		Note       = 0x00000004,
		Shlib      = 0x00000005,
		PHdr       = 0x00000006,
		TLS        = 0x00000007,
		LoOS       = 0x60000000,
		HiOS       = 0x6FFFFFFF,
		LoProc     = 0x70000000,
		HiProc     = 0x7FFFFFFF
	};

	enum class PHFlags : uint32_t {
		X = 0x1,
		W = 0x2,
		R = 0x4
	};

	struct ElfProgramHeader {
		std::uint32_t p_type;
		std::uint32_t p_flags;	// Different offset for 32/64-bit structures (0x18 vs 0x04)
		std::uint64_t p_offset;	// uint32_t for 32-bit structures
		std::uint64_t p_vaddr;	// uint32_t for 32-bit structures
		std::uint64_t p_paddr;	// uint32_t for 32-bit structures
		std::uint64_t p_filesz;	// uint32_t for 32-bit structures
		std::uint64_t p_memsz;	// uint32_t for 32-bit structures
		std::uint64_t p_align;	// uint32_t for 32-bit structures
	};

	inline std::string PHtype_to_string(std::uint32_t type) {
		switch (static_cast<PHType>(type)) {
		case PHType::Null:
			return "NULL";
		case PHType::Load:
			return "LOAD";
		case PHType::Dynamic:
			return "DYNAMIC";
		case PHType::Interp:
			return "INTERP";
		case PHType::Note:
			return "NOTE";
		case PHType::Shlib:
			return "SHLIB";
		case PHType::PHdr:
			return "PHDR";
		case PHType::TLS:
			return "TLS";
		case PHType::LoOS:
			return "LOOS";
		case PHType::HiOS:
			return "HIOS";
		case PHType::LoProc:
			return "LOPROC";
		case PHType::HiProc:
			return "HIPROC";
		default:
			return "Unknown";
		}
	}

	inline std::string PHflags_to_string(std::uint32_t flags) {
		std::string result = "";
		if (flags & static_cast<std::uint32_t>(PHFlags::R))
			result += "R";
		else
			result += " ";
		if (flags & static_cast<std::uint32_t>(PHFlags::W))
			result += "W";
		else
			result += " ";
		if (flags & static_cast<std::uint32_t>(PHFlags::X))
			result += "E";
		else
			result += " ";
		
		return result;
	}
}