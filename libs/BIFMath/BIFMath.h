#ifndef BIFMATH_H
#define BIFMATH_H

namespace bif {
namespace math {

class BIFMath {
public:
    static double sqrt(double x);
    static double pow(double base, double exp);
    static double abs(double x);
    static double floor(double x);
    static double ceil(double x);
    static double sin(double x);
    static double cos(double x);
    static double tan(double x);
};

} // namespace math
} // namespace bif

#endif // BIFMATH_H
