#pragma once
#include <cstdint>

namespace elf {

	enum class SHFlags : std::uint64_t {
		Write =					1 << 0,
		Alloc =					1 << 1,
		ExecInstr = 			1 << 2,
		Merge = 				1 << 4,
		Strings = 				1 << 5,
		Info_Link = 			1 << 6,
		Link_Order = 			1 << 7,
		Os_NonConforming =		1 << 8,
		Group = 				1 << 9,
		TLS = 					1 << 10,
		Compressed = 			1 << 11,
		MaskOS = 				0x0FF00000,
		MaskProt = 				0xF0000000,
		Ordered = 				1 << 26,
		Exclude = 				1 << 27
	};

	enum class SHType : std::uint32_t {
		Null              = 0x0,
		Progbits          = 0x1,
		Symtab            = 0x2,
		Strtab            = 0x3,
		Rela              = 0x4,
		Hash              = 0x5,
		Dynamic           = 0x6,
		Note              = 0x7,
		Nobits            = 0x8,
		Rel               = 0x9,
		Shlib             = 0x0A,
		Dynsym            = 0x0B,
		Init_array        = 0x0E,
		Fini_array        = 0x0F,
		Preinit_array     = 0x10,
		Group             = 0x11,
		Symtab_shndx      = 0x12,
		Num               = 0x13,
		Loos              = 0x60000000
	};

	struct ElfSectionHeader {
		std::uint32_t	sh_name;
		std::uint32_t	sh_type;
		std::uint64_t	sh_flags;
		std::uint64_t	sh_addr;
		std::uint64_t	sh_offset;
		std::uint64_t	sh_size;
		std::uint32_t	sh_link;
		std::uint32_t	sh_info;
		std::uint64_t	sh_addralign;
		std::uint64_t	sh_entsize;
	};


	inline const char* SHType_to_cstring(std::uint32_t type) {
		switch (static_cast<SHType>(type)) {
		case SHType::Null:
			return "NULL";
		case SHType::Progbits:
			return "PROGBITS";
		case SHType::Symtab:
			return "SYMTAB";
		case SHType::Strtab:
			return "STRTAB";
		case SHType::Rela:
			return "RELA";
		case SHType::Hash:
			return "HASH";
		case SHType::Dynamic:
			return "DYNAMIC";
		case SHType::Note:
			return "NOTE";
		case SHType::Nobits:
			return "NOBITS";
		case SHType::Rel:
			return "REL";
		case SHType::Shlib:
			return "SHLIB";
		case SHType::Dynsym:
			return "DYNSYM";
		case SHType::Init_array:
			return "INIT_ARRAY";
		case SHType::Fini_array:
			return "FINI_ARRAY";
		case SHType::Preinit_array:
			return "PREINIT_ARRAY";
		case SHType::Group:
			return "GROUP";
		case SHType::Symtab_shndx:
			return "SYMTAB_SHNDX";
		default:
			if (type >= 0x60000000)
				return "OS-spec";
			else
				return "Unknown type";
		}
	}

	inline std::string SHFlags_to_cstring(std::uint64_t flags) {
		std::string result = "";
		if (flags & static_cast<std::uint64_t>(SHFlags::Write))
			result += "W";
		if (flags & static_cast<std::uint64_t>(SHFlags::Alloc))
			result += "A";
		if (flags & static_cast<std::uint64_t>(SHFlags::ExecInstr))
			result += "X";
		if (flags & static_cast<std::uint64_t>(SHFlags::Merge))
			result += "M";
		if (flags & static_cast<std::uint64_t>(SHFlags::Strings))
			result += "S";
		if (flags & static_cast<std::uint64_t>(SHFlags::Info_Link))
			result += "I";
		if (flags & static_cast<std::uint64_t>(SHFlags::Link_Order))
			result += "L";
		if (flags & static_cast<std::uint64_t>(SHFlags::Os_NonConforming))
			result += "O";
		if (flags & static_cast<std::uint64_t>(SHFlags::Group))
			result += "G";
		if (flags & static_cast<std::uint64_t>(SHFlags::TLS))
			result += "T";
		if (flags & static_cast<std::uint64_t>(SHFlags::Compressed))
			result += "C";
		if (flags & static_cast<std::uint64_t>(SHFlags::Ordered))
			result += "R";
		if (flags & static_cast<std::uint64_t>(SHFlags::Exclude))
			result += "E";
		return result;
	}
}