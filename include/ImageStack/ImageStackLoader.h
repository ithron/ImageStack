#pragma once

#include <type_traits>

namespace ImageStack {

template <class Derived> class ImageStackLoaderBase {};

template <class T>
struct IsLoader : public std::is_base_of<ImageStackLoaderBase<std::decay_t<T>>,
                                         std::decay_t<T>> {};

template <class T> constexpr bool isLoader_v = IsLoader<T>::value;

} // namespace ImageStack
