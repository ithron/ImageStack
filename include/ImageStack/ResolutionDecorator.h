#ifndef RESOLUTION_DECORATOR_HH
#define RESOLUTION_DECORATOR_HH

#include "ImageStack.hh"

#include <Base/Math/Vector3.hh>

namespace SITools {

class ResolutionDecorator  {
public:
  BIAS::Vector3<double> resolution;

protected:
  ResolutionDecorator() : resolution(0, 0, 0) {}
  template <class Loader> ResolutionDecorator(Loader &&loader, LoaderTag) {
    resolution = loader.Resolution();
  }
};

} // namespace SITools

#endif // RESOLUTION_DECORATOR_HH
