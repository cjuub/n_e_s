#include "core/ppu_factory.h"

#include <gtest/gtest.h>

using namespace n_e_s::core;

namespace n_e_s::core {

bool operator==(const PpuRegisters &a, const PpuRegisters &b) {
    return a.ctrl == b.ctrl && a.mask == b.mask && a.status == b.status &&
           a.oamaddr == b.oamaddr && a.oamdata == b.oamdata &&
           a.scroll == b.scroll && a.data == b.data;
}

} // namespace n_e_s::core

namespace {

class PpuTest : public ::testing::Test {
public:
    PpuTest()
            : registers(new PpuRegisters),
              ppu(PpuFactory::create(std::unique_ptr<PpuRegisters>(registers))),
              expected() {}

    PpuRegisters *const registers;
    std::unique_ptr<IPpu> ppu;

    PpuRegisters expected;
};

TEST_F(PpuTest, read_status_register) {
    registers->status = 0x25;

    const uint8_t status = ppu->read_byte(0x2002);

    EXPECT_EQ(status, status);
}

TEST_F(PpuTest, clear_status_when_reading_status) {
    registers->status = 0xFF;
    expected.status = 0x7F;

    ppu->read_byte(0x2002);

    EXPECT_EQ(expected, *registers);
}

} // namespace
