#pragma once

#include <type_traits>

namespace ImageStack {

template <class Derived> class ImageStackLoaderBase {};

template <class T>
struct IsLoader : public std::is_base_of<ImageStackLoaderBase<T>, T> {};

template <class T> constexpr bool isLoader_v = IsLoader<T>::value;

} // namespace ImageStack
