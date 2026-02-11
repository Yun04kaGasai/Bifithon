#ifndef BIFMATH_H
#define BIFMATH_H

#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>

namespace bifMath {
    // Constants
    const double PI = 3.14159265358979323846;
    const double E = 2.71828182845904523536;
    const double TAU = 6.28318530717958647692;
    
    // Basic math functions
    inline double sqrt(double x) { return std::sqrt(x); }
    inline double pow(double x, double y) { return std::pow(x, y); }
    inline double abs(double x) { return std::abs(x); }
    inline double ceil(double x) { return std::ceil(x); }
    inline double floor(double x) { return std::floor(x); }
    inline double round(double x) { return std::round(x); }
    
    // Trigonometric functions
    inline double sin(double x) { return std::sin(x); }
    inline double cos(double x) { return std::cos(x); }
    inline double tan(double x) { return std::tan(x); }
    inline double asin(double x) { return std::asin(x); }
    inline double acos(double x) { return std::acos(x); }
    inline double atan(double x) { return std::atan(x); }
    inline double atan2(double y, double x) { return std::atan2(y, x); }
    
    // Hyperbolic functions
    inline double sinh(double x) { return std::sinh(x); }
    inline double cosh(double x) { return std::cosh(x); }
    inline double tanh(double x) { return std::tanh(x); }
    
    // Exponential and logarithmic functions
    inline double exp(double x) { return std::exp(x); }
    inline double log(double x) { return std::log(x); }
    inline double log10(double x) { return std::log10(x); }
    inline double log2(double x) { return std::log2(x); }
    
    // Angular conversion
    inline double degrees(double rad) { return rad * 180.0 / PI; }
    inline double radians(double deg) { return deg * PI / 180.0; }
    
    // Special functions
    inline double gcd(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return std::abs(a);
    }
    
    inline int factorial(int n) {
        if (n < 0) return -1;
        if (n == 0 || n == 1) return 1;
        int result = 1;
        for (int i = 2; i <= n; i++) {
            result *= i;
        }
        return result;
    }
    
    inline double sum(const std::vector<double>& vec) {
        return std::accumulate(vec.begin(), vec.end(), 0.0);
    }
    
    inline double max(const std::vector<double>& vec) {
        return *std::max_element(vec.begin(), vec.end());
    }
    
    inline double min(const std::vector<double>& vec) {
        return *std::min_element(vec.begin(), vec.end());
    }
}

#endif // BIFMATH_H
