#include "BIFMath.h"

#include <cmath>

namespace bif {
namespace math {

double BIFMath::sqrt(double x) {
    return std::sqrt(x);
}

double BIFMath::pow(double base, double exp) {
    return std::pow(base, exp);
}

double BIFMath::abs(double x) {
    return std::fabs(x);
}

double BIFMath::floor(double x) {
    return std::floor(x);
}

double BIFMath::ceil(double x) {
    return std::ceil(x);
}

double BIFMath::sin(double x) {
    return std::sin(x);
}

double BIFMath::cos(double x) {
    return std::cos(x);
}

double BIFMath::tan(double x) {
    return std::tan(x);
}

} // namespace math
} // namespace bif
