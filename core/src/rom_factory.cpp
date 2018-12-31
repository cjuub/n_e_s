#include "core/rom_factory.h"

#include "rom/nrom.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

static_assert(sizeof(n_e_s::core::INesHeader) == 16);

namespace {

std::ifstream::pos_type filesize(const std::string &filename) {
    std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);
    if (!file) {
        return 0;
    }

    return file.tellg();
}

} // namespace

namespace n_e_s::core {

IRom *RomFactory::fromFile(const std::string &filepath) {
    std::vector<uint8_t> bytes(filesize(filepath));

    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        assert(false);
        exit(1); // Unable to open file.
    }

    if (bytes.size() < 16) {
        assert(false);
        exit(1); // File isn't big enough to contain a header.
    }

    if (!file.read(reinterpret_cast<char *>(&bytes[0]), bytes.size())) {
        assert(false);
        exit(1); // Unable to get file bytes.
    }

    INesHeader h;
    if (!std::equal(bytes.begin(), bytes.begin() + sizeof(h.nes), h.nes)) {
        assert(false);
        exit(1); // No valid iNes header.
    }

    // This is fine because the header is exactly 16 bytes with no padding.
    memcpy(&h, &bytes[0], sizeof(h));
    if (h.prg_ram_size == 0) {
        h.prg_ram_size = 1; // For compatibility reasons, 0 ram means 1 ram.
    }

    // Not null terminated, so we'll get some trash, but that's fine.
    printf("NES: %s\n", h.nes);
    printf("prg_rom: %u (%u)\n", h.prg_rom_size, h.prg_rom_size * 16 * 1024);
    printf("chr_rom: %u (%u)\n", h.chr_rom_size, h.chr_rom_size * 8 * 1024);
    printf("flags_6: %u\n", h.flags_6);
    printf("flags_7: %u\n", h.flags_7);
    printf("prg_ram_size: %u (%u)\n",
            h.prg_ram_size,
            h.prg_ram_size * 8 * 1024);
    printf("flags_9: %u\n", h.flags_9);
    printf("flags_10: %u\n", h.flags_10);

    uint8_t mapper = (h.flags_6 & 0xF0) >> 4;
    mapper |= h.flags_7 & 0xF0;
    printf("mapper: %u\n", mapper);

    const uint32_t expected_rom_size = sizeof(INesHeader) +
                                       h.prg_rom_size * 16 * 1024 +
                                       h.chr_rom_size * 8 * 1024;
    if (bytes.size() != expected_rom_size) {
        assert(false);
        exit(1);
    }

    std::vector<uint8_t> prg_rom(bytes.begin() + 16,
            bytes.begin() + 16 + h.prg_rom_size * 16 * 1024);
    printf("prg rom size: %zu\n", prg_rom.size());

    std::vector<uint8_t> chr_rom(
            bytes.begin() + 16 + h.prg_rom_size * 16 * 1024,
            bytes.begin() + 16 + h.prg_rom_size * 16 * 1024 +
                    h.chr_rom_size * 8 * 1024);
    printf("chr rom size: %zu\n", chr_rom.size());

    if (mapper == 0) {
        return new Nrom(h, prg_rom, chr_rom);
    }

    std::stringstream err;
    err << "Unsupported mapper: " << +mapper;
    throw std::logic_error(err.str());
}

} // namespace n_e_s::core
