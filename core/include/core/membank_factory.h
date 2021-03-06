#pragma once

#include "core/imembank.h"
#include "core/ippu.h"

namespace n_e_s::core {

class MemBankFactory {
public:
    static MemBankList create_nes_mem_banks(IPpu *ppu);

    static MemBankList create_nes_ppu_mem_banks();
};

} // namespace n_e_s::core
