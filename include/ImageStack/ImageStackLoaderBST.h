#pragma once

#include "BinaryStream.h"
#include "Types.h"

extern "C" {
#include <endian.h>
}

#include <algorithm>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>

namespace ImageStack {

namespace detail {

template <typename T> constexpr T ntohT(T value) noexcept {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  auto *ptr = reinterpret_cast<char *>(&value);
  std::reverse(ptr, ptr + sizeof(T));
#endif
  return value;
}

} // anonymous namespace

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
/// @brief Loader class for ImageStack for loading .bst files
///
/// Unit tests are in \ref testBSTLoader.cpp
///
/// @tparam ImageStack_ ImageStack type to load the data for
/// @tparam IsMask bool value indicating if the loader should load a mask
///(true) or an image (false).
template <class ImageStack_, bool IsMask = false> class ImageStackLoaderBST {
  enum class State { Initialized, HeaderRead };

public:
  using ImageStack = ImageStack_;

  /// @brief Create a loader object for the given filename
  /// @param filename path to the file to load
  /// @throw std::runtime_error if the file could not be opened
  explicit ImageStackLoaderBST(std::string filename) {
    auto in = std::make_unique<std::ifstream>(
        filename, std::ios_base::in | std::ios_base::binary);

    if (!*in)
      throw std::runtime_error("Failed to open file '" + filename + "'");

    istream_ = std::move(in);
  }

  /// @brief Returns the size of the image or mask to be read
  /// @note Calling this method may result in the header of the file beeing
  /// read, depending if it was read before. That is why this methid is not
  /// const or noexcept.
  /// @return 3D size of the object to be loaded, the return type is a model of
  ///  \ref MultiIndexConcept
  inline auto size() {
    if (state_ != State::HeaderRead) readHeader();

    return size_;
  }

  /// @brief Returns the resolution in mm of the file to be read
  /// @note Calling this method may result in the header of the file beeing
  /// read, depending if it was read before. That is why this methid is not
  /// const or noexcept.
  /// @return The 3D resolution of the object to be loaded in mm.
  inline auto resolution() {
    if (state_ != State::HeaderRead) readHeader();

    return resolution_;
  }

  /// @brief Read the data from the file into the given output iterator
  /// @tparam T data type to read
  /// @tparam OutIter output iterator
  /// @param out output iterator where the read data goes
  template <class T, class OutIter> void readData(OutIter out) {
    using value_type = T;
    using Iter =
        std::istream_iterator<BinWrapper<value_type, Endianness::BigEndian>>;

    if (state_ != State::HeaderRead) readHeader();

    istream_->seekg(startOfDtata_);
    std::copy(Iter(*istream_), Iter(), out);
  }

private:
  void readHeader() {
    // the header should not be read twice
    Expects(state_ == State::Initialized);

    std::size_t size;
    if (!IsMask) {
      // skip xyz information
      istream_->seekg(6 * sizeof(std::int32_t), std::ios_base::beg);
      // read size
      for (unsigned int i = 0; i < 3; ++i) {
        std::int32_t tmp;
        std::uint32_t tmpU;
        istream_->read(reinterpret_cast<char *>(&tmp), sizeof(std::int32_t));
        // tmpU = ntohl(tmp);
        tmpU = static_cast<std::uint32_t>(detail::ntohT(tmp));
        size_[static_cast<int>(i)] =
            gsl::narrow<std::size_t>(*reinterpret_cast<std::int32_t *>(&tmpU));
      }
      // skip fuc
      istream_->seekg(1 * sizeof(std::int32_t), std::ios_base::cur);
      // read resolution
      for (int i = 0; i < 3; ++i) {
        double tmp;
        static_assert(sizeof(double) == 8, "Unsupported double size.");
        istream_->read(reinterpret_cast<char *>(&tmp), sizeof(double));
        resolution_[i] = detail::ntohT(tmp);
      }

      size = static_cast<Size>(indexProduct(size_) *
                               sizeof(typename ImageStack::StorageType));
    } else {
      // Load header of mask file
      char dummy[1024];
      // skip xyz
      istream_->getline(dummy, sizeof(dummy), 0x0a);
      // read size
      istream_->getline(dummy, sizeof(dummy), 0x0a);
      {
        std::stringstream sstream(dummy);
        for (int i = 0; i < 3; ++i) {
          sstream >> size_[i];
          sstream.get();
        }
      }
      // skip measurement date
      istream_->getline(dummy, sizeof(dummy), 0x0a);
      // read resolution
      istream_->getline(dummy, sizeof(dummy), 0x0a);
      {
        std::stringstream sstream(dummy);
        for (int i = 0; i < 3; ++i) {
          sstream >> resolution_[i];
          sstream.get();
        }
      }
      size = static_cast<std::size_t>(indexProduct(size_)) *
             sizeof(typename ImageStack::StorageType);
    }
    // set start of data
    istream_->seekg(-static_cast<long>(size), std::ios_base::end);
    startOfDtata_ = istream_->tellg();
    istream_->seekg(0, std::ios_base::end);

    Ensures(istream_->tellg() - startOfDtata_ == static_cast<long>(size) &&
            "Size mismatch");

    state_ = State::HeaderRead;
  }

  Eigen::Vector3d resolution_{Eigen::Vector3d::Zero()};
  Size3 size_{Size3::Zero()};
  State state_{State::Initialized};
  std::unique_ptr<std::ifstream> istream_;
  std::istream::pos_type startOfDtata_;
};

#pragma clang diagnostic pop

} // namespace ImageStack
