/*
 * Copyright (c) 2020, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Format.h>
#include <AK/Forward.h>
#include <AK/Platform.h>

#if defined(AK_OS_MACOS)
#    include <libkern/OSByteOrder.h>
#    include <machine/endian.h>

#    define htobe16(x) OSSwapHostToBigInt16(x)
#    define htole16(x) OSSwapHostToLittleInt16(x)
#    define be16toh(x) OSSwapBigToHostInt16(x)
#    define le16toh(x) OSSwapLittleToHostInt16(x)

#    define htobe32(x) OSSwapHostToBigInt32(x)
#    define htole32(x) OSSwapHostToLittleInt32(x)
#    define be32toh(x) OSSwapBigToHostInt32(x)
#    define le32toh(x) OSSwapLittleToHostInt32(x)

#    define htobe64(x) OSSwapHostToBigInt64(x)
#    define htole64(x) OSSwapHostToLittleInt64(x)
#    define be64toh(x) OSSwapBigToHostInt64(x)
#    define le64toh(x) OSSwapLittleToHostInt64(x)

#    define __BIG_ENDIAN BIG_ENDIAN
#    define __LITTLE_ENDIAN LITTLE_ENDIAN
#    define __BYTE_ORDER BYTE_ORDER
#endif

namespace AK {

template<typename T>
ALWAYS_INLINE constexpr T convert_between_host_and_little_endian(T value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return value;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    if constexpr (sizeof(T) == 8)
        return static_cast<T>(__builtin_bswap64(static_cast<u64>(value)));
    if constexpr (sizeof(T) == 4)
        return static_cast<T>(__builtin_bswap32(static_cast<u32>(value)));
    if constexpr (sizeof(T) == 2)
        return static_cast<T>(__builtin_bswap16(static_cast<u16>(value)));
    if constexpr (sizeof(T) == 1)
        return value;
#endif
}

template<typename T>
ALWAYS_INLINE constexpr T convert_between_host_and_big_endian(T value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    if constexpr (sizeof(T) == 8)
        return static_cast<T>(__builtin_bswap64(static_cast<u64>(value)));
    if constexpr (sizeof(T) == 4)
        return static_cast<T>(__builtin_bswap32(static_cast<u32>(value)));
    if constexpr (sizeof(T) == 2)
        return static_cast<T>(__builtin_bswap16(static_cast<u16>(value)));
    if constexpr (sizeof(T) == 1)
        return value;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return value;
#endif
}

template<typename T>
ALWAYS_INLINE T convert_between_host_and_network_endian(T value)
{
    return convert_between_host_and_big_endian(value);
}

template<typename T>
class [[gnu::packed]] LittleEndian {
public:
    constexpr LittleEndian() = default;

    constexpr LittleEndian(T value)
        : m_value(convert_between_host_and_little_endian(value))
    {
    }

    constexpr operator T() const { return convert_between_host_and_little_endian(m_value); }

private:
    T m_value { 0 };
};

template<typename T>
class [[gnu::packed]] BigEndian {
public:
    constexpr BigEndian() = default;

    constexpr BigEndian(T value)
        : m_value(convert_between_host_and_big_endian(value))
    {
    }

    constexpr operator T() const { return convert_between_host_and_big_endian(m_value); }

private:
    T m_value { 0 };
};

template<typename T>
using NetworkOrdered = BigEndian<T>;

template<typename T>
requires(HasFormatter<T>) struct Formatter<LittleEndian<T>> : Formatter<T> {
};

template<typename T>
requires(HasFormatter<T>) struct Formatter<BigEndian<T>> : Formatter<T> {
};

template<typename T>
struct Traits<LittleEndian<T>> : public GenericTraits<LittleEndian<T>> {
    static constexpr bool is_trivially_serializable() { return Traits<T>::is_trivially_serializable(); }
};

template<typename T>
struct Traits<BigEndian<T>> : public GenericTraits<BigEndian<T>> {
    static constexpr bool is_trivially_serializable() { return Traits<T>::is_trivially_serializable(); }
};

constexpr u16 bitswap(u16 v)
{
    v = ((v >> 1) & 0x5555) | ((v & 0x5555) << 1);    // even & odd bits
    v = ((v >> 2) & 0x3333) | ((v & 0x3333) << 2);    // pairs
    v = ((v >> 4) & 0x0F0F) | ((v & 0x0F0F) << 4);    // nibbles
    return ((v >> 8) & 0x00FF) | ((v & 0x00FF) << 8); // bytes
}

}

#if USING_AK_GLOBALLY
using AK::BigEndian;
using AK::bitswap;
using AK::LittleEndian;
using AK::NetworkOrdered;
#endif
