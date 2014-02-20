/*
 * Copyright (C) 2014 Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include <cassert>
#include <stdint.h>
#include <string>
#include <uavcan/internal/util.hpp>
#include <uavcan/internal/transport/transfer_buffer.hpp>

namespace uavcan
{

extern "C" void bitarray_copy(const unsigned char *src_org, int src_offset, int src_len,
                                    unsigned char *dst_org, int dst_offset);

class BitStream
{
    enum { MAX_BYTES_PER_RW = 16 };

    ITransferBuffer& buf_;
    int bit_offset_;
    uint8_t byte_cache_;

    static inline unsigned int bitlenToBytelen(unsigned int bits) { return (bits + 7) / 8; }

    static inline void copyBitArray(const uint8_t* src_org, int src_offset, int src_len,
                                          uint8_t* dst_org, int dst_offset)
    {
        bitarray_copy(reinterpret_cast<const unsigned char*>(src_org), src_offset, src_len,
                      reinterpret_cast<unsigned char*>(dst_org), dst_offset);
    }

public:
    enum { MAX_BITS_PER_RW = MAX_BYTES_PER_RW * 8 };

    enum
    {
        RESULT_OUT_OF_BUFFER = 0,
        RESULT_OK            = 1
    };

    BitStream(ITransferBuffer& buf)
    : buf_(buf)
    , bit_offset_(0)
    , byte_cache_(0)
    {
        StaticAssert<sizeof(uint8_t) == 1>::check();
    }

    /**
     * Write/read calls interpret bytes as bit arrays, 8 bits per byte, where the most
     * significant bits have lower index, i.e.:
     *   Hex:     55       2d
     *   Bits:    01010101 00101101
     *   Indices: 0  ..  7 8  ..  15
     * Return values:
     *   Negative - Error
     *   Zero     - Out of buffer space
     *   Positive - OK
     */
    int write(const uint8_t* bytes, const int bitlen);
    int read(uint8_t* bytes, const int bitlen);

    std::string toString() const;
};

}