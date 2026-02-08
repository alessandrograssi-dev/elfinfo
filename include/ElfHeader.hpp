#pragma once
#include <cstdint>
#include <string>

namespace elf {

    enum class ElfClass : uint8_t {
        Elf32 = 1,
        Elf64 = 2
    };

    enum class ElfEndianness : uint8_t {
        LittleEndian    = 1,
        BigEndian       = 2
    };

    enum class ElfOSABI : uint8_t           {
        SystemV                     = 0x00,
        HP_UX                       = 0x01,
        NetBSD                      = 0x02,
        Linux                       = 0x03,
        GNU_Hurd                    = 0x04,
        Solaris                     = 0x06,
        AIX                         = 0x07,
        IRIX                        = 0x08,
        FreeBSD                     = 0x09,
        Tru64                       = 0x0A,
        Novell_Modesto              = 0x0B,
        OpenBSD                     = 0x0C,
        OpenVMS                     = 0x0D,
        NonStop_Kernel              = 0x0E,
        AROS                        = 0x0F,
        FenixOS                     = 0x10,
        Nuxi_CloudABI               = 0x11,
        Stratus_Technology_OpenVOS  = 0x12
    };
    
    struct ElfHeader             {
        bool            valid_magic;
        ElfClass        e_class;
        ElfEndianness   e_endianness;
        ElfOSABI        e_osabi;
        uint8_t         e_abiversion;
        uint16_t        e_type;
        uint16_t        e_machine;
        uint32_t        e_version;
        uint64_t        e_entry;
        uint64_t        e_phoff;
        uint64_t        e_shoff;
        uint32_t        e_flags;
        uint16_t        e_ehsize;
        uint16_t        e_phentsize;
        uint16_t        e_phnum;
        uint16_t        e_shentsize;
        uint16_t        e_shnum;
        uint16_t        e_shstrndx;
    };

    inline const char* os_abi_names(ElfOSABI osabi) {
        switch (osabi)
        {
        case ElfOSABI::SystemV:
            return "UNIX - System V";
        case ElfOSABI::HP_UX:
            return "HP-UX";
        case ElfOSABI::NetBSD:
            return "NetBSD";
        case ElfOSABI::Linux:
            return "Linux";
        case ElfOSABI::GNU_Hurd:
            return "GNU Hurd";
        case ElfOSABI::Solaris:
            return "Solaris";
        case ElfOSABI::AIX:
            return "AIX (Monterey)";
        case ElfOSABI::IRIX:
            return "IRIX";
        case ElfOSABI::FreeBSD:
            return "FreeBSD";
        case ElfOSABI::Tru64:
            return "Tru64";
        case ElfOSABI::Novell_Modesto:
            return "Novell Modesto";
        case ElfOSABI::OpenBSD:
            return "OpenBSD";
        case ElfOSABI::OpenVMS:
            return "OpenVMS";
        case ElfOSABI::NonStop_Kernel:
            return "NonStop Kernel";
        case ElfOSABI::AROS:
            return "AROS";
        case ElfOSABI::FenixOS:
            return "FenixOS";
        case ElfOSABI::Nuxi_CloudABI:
            return "Nuxi CloudABI";
        case ElfOSABI::Stratus_Technology_OpenVOS:
            return "Stratus Technologies OpenVOS";
        }
        return "";
    }

    inline const char* type_name(uint16_t index) {
        switch (index)
        {
        case 0x00:
            return "NONE (Unknown type)";
        case 0x01:
            return "REL (Relocatable file)";
        case 0x02:
            return "EXEC (Executable file)";
        case 0x03:
            return "DYN (Shared object)";
        case 0x04:
            return "CORE (Core file)";
        default:
            if (index >= 0xFE00 && index <= 0xFEFF)
                return "LOOS/HIOS (Operating system specific)";
            else if (index >= 0xFF00)
                return "LOPROC/HIPROC (Processor specific)";
            else
                return "Unknown";
        }
    }

    inline const char* machine_name(uint16_t index) {
        switch (index)
        {
        case 0x00:
            return "No specific instruction set";
        case 0x01:
            return "AT&T WE 32100";
        case 0x02:
            return "SPARC";
        case 0x03:
            return "x86";
        case 0x04:
            return "Motorola 68000 (M68k)";
        case 0x05:
            return "Motorola 88000 (M88k)";
        case 0x06:
            return "Intel MCU";
        case 0x07:
            return "Intel 80860";
        case 0x08:
            return "MIPS";
        case 0x09:
            return "IBM System/370";
        case 0x0A:
            return "MIPS RS3000 Little-endian";
        case 0x0F:
            return "Hewlett-Packard PA-RISC";
        case 0x13:
            return "Intel 80960";
        case 0x14:
            return "PowerPC";
        case 0x15:
            return "PowerPC (64-bit)";
        case 0x16:
            return "S390, including S390x";
        case 0x17:
            return "IBM SPU/SPC";
        case 0x24:
            return "NEC V800";
        case 0x25:
            return "Fujitsu FR20";
        case 0x26:
            return "TRW RH-32";
        case 0x27:
            return "Motorola RCE";
        case 0x28:
            return "Arm (up to Armv7/AArch32)";
        case 0x29:
            return "Digital Alpha";
        case 0x2A:
            return "SuperH";
        case 0x2B:
            return "SPARC Version 9";
        case 0x2C:
            return "Siemens TriCore embedded processor";
        case 0x2D:
            return "Argonaut RISC Core";
        case 0x2E:
            return "Hitachi H8/300";
        case 0x2F:
            return "Hitachi H8/300H";
        case 0x30:
            return "Hitachi H8S";
        case 0x31:
            return "Hitachi H8/500";
        case 0x32:
            return "IA-64";
        case 0x33:
            return "Stanford MIPS-X";
        case 0x34:
            return "Motorola ColdFire";
        case 0x35:
            return "Motorola M68HC12";
        case 0x36:
            return "Fujitsu MMA Multimedia Accelerator";
        case 0x37:
            return "Siemens PCP";
        case 0x38:
            return "Sony nCPU embedded RISC processor";
        case 0x39:
            return "Denso NDR1 microprocessor";
        case 0x3A:
            return "Motorola Star*Core processor";
        case 0x3B:
            return "Toyota ME16 processor";
        case 0x3C:
            return "STMicroelectronics ST100 processor";
        case 0x3D:
            return "Advanced Logic Corp. TinyJ embedded processor family";
        case 0x3E:
            return "AMD x86-64";
        case 0x3F:
            return "Sony DSP Processor";
        case 0x40:
            return "Digital Equipment Corp. PDP-10";
        case 0x41:
            return "Digital Equipment Corp. PDP-11";
        case 0x42:
            return "Siemens FX66 microcontroller";
        case 0x43:
            return "STMicroelectronics ST9+ 8/16-bit microcontroller";
        case 0x44:
            return "STMicroelectronics ST7 8-bit microcontroller";
        case 0x45:
            return "Motorola MC68HC16 Microcontroller";
        case 0x46:
            return "Motorola MC68HC11 Microcontroller";
        case 0x47:
            return "Motorola MC68HC08 Microcontroller";
        case 0x48:
            return "Motorola MC68HC05 Microcontroller";
        case 0x49:
            return "Silicon Graphics SVx";
        case 0x4A:
            return "STMicroelectronics ST19 8-bit microcontroller";
        case 0x4B:
            return "Digital VAX";
        case 0x4C:
            return "Axis Communications 32-bit embedded processor";
        case 0x4D:
            return "Infineon Technologies 32-bit embedded processor";
        case 0x4E:
            return "Element 14 64-bit DSP Processor";
        case 0x4F:
            return "LSI Logic 16-bit DSP Processor";
        case 0x8C:
            return "TMS320C6000 Family";
        case 0xAF:
            return "MCST Elbrus e2k";
        case 0xB7:
            return "Arm 64-bits (Armv8/AArch64)";
        case 0xDC:
            return "Zilog Z80";
        case 0xF3:
            return "RISC-V";
        case 0xF7:
            return "Berkeley Packet Filter";
        case 0x101:
            return "WDC 65C816";
        case 0x102:
            return "LoongArch";
        default:
            return "Unknown";
        }
    }
}