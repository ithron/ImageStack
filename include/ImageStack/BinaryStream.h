#pragma once

#include <cinttypes>
#include <istream>

namespace ImageStack {

namespace detail {
template <std::size_t N> struct ChangeEndianness {};

template <> struct ChangeEndianness<1> {
  template <class T> inline T operator()(T a) const { return a; }
};

template <> struct ChangeEndianness<2> {
  template <class T> inline T operator()(T a) const {
    T b;
    reinterpret_cast<std::uint8_t *>(&b)[0] =
        reinterpret_cast<std::uint8_t const *>(&a)[1];
    reinterpret_cast<std::uint8_t *>(&b)[1] =
        reinterpret_cast<std::uint8_t const *>(&a)[0];
    return b;
  }
};

template <> struct ChangeEndianness<4> {
  template <class T> inline T operator()(T a) const {
    T b;
    reinterpret_cast<std::uint8_t *>(&b)[0] =
        reinterpret_cast<std::uint8_t const *>(&a)[3];
    reinterpret_cast<std::uint8_t *>(&b)[1] =
        reinterpret_cast<std::uint8_t const *>(&a)[2];
    reinterpret_cast<std::uint8_t *>(&b)[2] =
        reinterpret_cast<std::uint8_t const *>(&a)[1];
    reinterpret_cast<std::uint8_t *>(&b)[3] =
        reinterpret_cast<std::uint8_t const *>(&a)[0];
    return b;
  }
};
} // namespace detail

/// @brief Changes the endianness of the given value.
/// @note Currently only Big- and Little-Endian is supported.
/// also note that currently there is no support for 64bit or larger types.
template <class T> inline T ChangeEndianness(T const &a) {
  return detail::ChangeEndianness<sizeof(T)>()(a);
}

/// Endianness enum
enum class Endianness { BigEndian, LittleEndian };

/// @brief Function to determine host byte order
/// @note Only Big- and Little-Endian is supported.
inline Endianness hostByteOrder() {
  constexpr std::uint16_t kMagicNumber = (0x3e) << 8 | 0xff;
  return reinterpret_cast<std::uint8_t const *>(&kMagicNumber)[0] == 0xff
             ? Endianness::LittleEndian
             : Endianness::BigEndian;
}

/// @brief Binary Wrapper class
///
/// Use this to wrap a value for reading from binary stream using operator>>.
/// Also compatible with std::istream_operator. E.g.
/// std::istream<BinWrapper<value_type> >.
///
/// \tparam T value type to wrap
/// \tparam ByteOrder_ Endianness of the read operation.
template <class T, Endianness ByteOrder_> class BinWrapper {
public:
  using value_type = T;
  static constexpr Endianness ByteOrder = ByteOrder_;

  constexpr BinWrapper() noexcept = default;
  constexpr BinWrapper(T val) noexcept : val_(val) {}

  friend inline std::istream &operator>>(std::istream &stream,
                                         BinWrapper &wrapper) {
    value_type tmp;
    stream.read(reinterpret_cast<typename std::remove_reference<decltype(
                    stream)>::type::char_type *>(&tmp),
                sizeof(value_type));
    wrapper = ByteOrder == hostByteOrder() ? tmp : ChangeEndianness(tmp);
    return stream;
  }

  inline BinWrapper &operator=(T val) noexcept(noexcept(val_ = val)) {
    val_ = val;
    return *this;
  }

  inline constexpr operator T() const noexcept { return val_; }

private:
  T val_;
};

} // namespace ImageStack
