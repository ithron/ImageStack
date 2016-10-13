#ifndef SIFILETOOLS_IMAGESTACKLOADERBST_HH
#define SIFILETOOLS_IMAGESTACKLOADERBST_HH

extern "C" {
// #include <arpa/inet.h>
#include <endian.h>
}

#include <Base/Debug/Error.hh>
#include <Base/Math/Vector3.hh>
#include <Debug/Dvector.hh>
#include <MetaUtils/AlgorithmWrapper.hh>
#include <MetaUtils/MakeUnique.hh>
#include <Misc/BinaryStream.hh>

#include <string>
#include <sstream>
#include <fstream>
#include <memory>
#include <cassert>
#include <iterator>
#include <algorithm>

namespace {

template <typename T> constexpr T ntohT(T value, char *ptr = nullptr) noexcept {
  return
#if __BYTE_ORDER == __LITTLE_ENDIAN
      ptr = reinterpret_cast<char *>(&value),
      std::reverse(ptr, ptr + sizeof(T)),
#endif
      value;
}

} // anonymous namespace

namespace SITools {

template <class ImageStack_, bool IsMask = false> class ImageStackLoaderBST {
  enum class State { Uninitialized, Initialized, HeaderRead };

public:
  typedef ImageStack_ ImageStack;

  ImageStackLoaderBST(std::string filename) : state_(State::Uninitialized) {
    std::ifstream in(filename);

    if (!in.is_open()) {
      BIASERR("Failted to open file '" << filename << "'");
      throw std::runtime_error("Failed to open file '" + filename + "'");
    }

    in.close();

    istream_ = std::make_unique<std::ifstream>(
        filename, std::ios_base::in | std::ios_base::binary);
    state_ = State::Initialized;
  }

  inline bool IsReady() const { return state_ != State::Uninitialized; }

  inline operator bool() const { return IsReady(); }

  BIAS::Vector3<unsigned int> Dimension() {
    if (!IsReady()) {
      BIASERR("Load not Initialized");
      return {0, 0, 0};
    }

    if (state_ == State::Initialized) { ReadHeader_(); }

    assert(std::all_of(dimension_.begin(), dimension_.end(),
                       [](std::int32_t i) { return i >= 0; }));

    return {static_cast<unsigned int>(dimension_[0]),
            static_cast<unsigned int>(dimension_[1]),
            static_cast<unsigned int>(dimension_[2])};
  }

  BIAS::Vector3<double> Resolution() {
    if (!IsReady()) {
      BIASERR("Load not Initialized");
      return {0, 0, 0};
    }

    if (state_ == State::Initialized) { ReadHeader_(); }

    return resolution_;
  }

  template <class T, class OutIter> void ReadData(OutIter out) {
    typedef T value_type;
    typedef std::istream_iterator<BinWrapper<value_type, Endianness::BigEndian>>
        Iter;

    assert(state_ == State::HeaderRead && "Must read header first");

    istream_->seekg(startOfDtata_);
    std::copy(Iter(*istream_), Iter(), out);
  }

private:
  void ReadHeader_() {
    assert(state_ == State::Initialized && "Uninitalized");
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
        tmpU = static_cast<std::uint32_t>(ntohT(tmp));
        dimension_[static_cast<int>(i)] =
            *reinterpret_cast<std::int32_t *>(&tmpU);
      }
      // skip fuc
      istream_->seekg(1 * sizeof(std::int32_t), std::ios_base::cur);
      // read resolution
      for (int i = 0; i < 3; ++i) {
        double tmp;
        static_assert(sizeof(double) == 8, "Unsupported double size.");
        istream_->read(reinterpret_cast<char *>(&tmp), sizeof(double));
        resolution_[i] = ntohT(tmp);
      }

      size = static_cast<std::size_t>(MIP::MU::product(dimension_)) *
             sizeof(typename ImageStack::StorageType);
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
          sstream >> dimension_[i];
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
      size = static_cast<std::size_t>(MIP::MU::product(dimension_)) *
             sizeof(typename ImageStack::StorageType);
    }
    // set start of data
    istream_->seekg(-static_cast<long>(size), std::ios_base::end);
    startOfDtata_ = istream_->tellg();
#ifdef BIAS_DEBUG
    istream_->seekg(0, std::ios_base::end);
    if (istream_->tellg() - startOfDtata_ != static_cast<long>(size)) {
      std::cout << "Size mismatch: " << size << " vs. "
                << istream_->tellg() - startOfDtata_ << std::endl;
    }
    assert(istream_->tellg() - startOfDtata_ == static_cast<long>(size) &&
           "Size mismatch");
#endif
    state_ = State::HeaderRead;
  }

  State state_;
  BIAS::Vector3<std::int32_t> dimension_;
  BIAS::Vector3<double> resolution_;
  std::unique_ptr<std::ifstream> istream_;
  std::istream::pos_type startOfDtata_;
};

} // namespace SITools

#endif // SIFILETOOLS_IMAGESTACKLOADERBST_HH
