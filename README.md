# ELFInfo --- A Native ELF Binary Inspector in C++

[![C++](https://img.shields.io/badge/lang-C++17-blue.svg)](https://isocpp.org/)\
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A lightweight ELF file inspector written in modern C++, inspired by
tools like `readelf` and `objdump`.

This project parses and displays key parts of the ELF (Executable and
Linkable Format), including:

-   ELF header
-   Section headers
-   Program headers
-   Section ↔ Segment mapping

It supports both 32‑bit and 64‑bit ELF formats and handles endianness
correctly.

------------------------------------------------------------------------

## 🧠 Motivation

ELF (Executable and Linkable Format) is the standard binary format on
Unix‑like systems (Linux, BSD, etc.).

This project is a pedagogical reimplementation in **pure C++** to:

✔ Understand low‑level binary layout\
✔ Learn endianness, ABI differences, and file parsing\
✔ Build your own inspection tools\
✔ Demonstrate systems programming skills

------------------------------------------------------------------------

## 🛠 Features

  Feature                      Status
  ---------------------------- --------------
  ELF Header Parsing           ✅
  Section Header Table         ✅
  Program Header Table         ✅
  Section ↔ Segment Mapping    ✅
  CLI Interface                ✅
  Supports 32 & 64‑bit         ✅
  Endianness Awareness         ✅
  Dynamic Sections / Symbols   🚧 (Planned)

------------------------------------------------------------------------

## 📦 Build

Requires a C++17‑compatible compiler:

``` bash
git clone https://github.com/alessandrograssi-dev/elfinfo.git
cd elfinfo
mkdir build && cd build
cmake ..
make -j$(nproc)
```

------------------------------------------------------------------------

## 📌 Usage

``` bash
./elfinfo <filename> [options]
```

Example:

``` bash
./elfinfo /bin/ls --headers --section-headers --program-headers
```

------------------------------------------------------------------------

## 🧩 How It Works

1.  Reads the ELF header to determine format (32/64 bit + endianness)
2.  Parses section headers
3.  Parses program headers
4.  Maps sections to segments based on offset ranges
5.  Prints formatted tables similar to `readelf`

------------------------------------------------------------------------

## 🚀 Why This Is Interesting

This project demonstrates:

✔ Low‑level file I/O\
✔ Safe binary parsing in C++\
✔ Template-based typed reads\
✔ Endianness handling\
✔ Understanding of executable internals

Ideal for systems programming, compiler tooling, or infrastructure
roles.

------------------------------------------------------------------------

## 📜 License

MIT License. 