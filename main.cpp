#include <iostream>

#include "libs/BIFMath/BIFMath.h"

int main() {
	using bif::math::BIFMath;

	std::cout << "sqrt(9) = " << BIFMath::sqrt(9.0) << "\n";
	std::cout << "pow(2, 8) = " << BIFMath::pow(2.0, 8.0) << "\n";
	std::cout << "abs(-3.5) = " << BIFMath::abs(-3.5) << "\n";
	std::cout << "floor(3.9) = " << BIFMath::floor(3.9) << "\n";
	std::cout << "ceil(3.1) = " << BIFMath::ceil(3.1) << "\n";
	std::cout << "sin(0) = " << BIFMath::sin(0.0) << "\n";
	std::cout << "cos(0) = " << BIFMath::cos(0.0) << "\n";
	std::cout << "tan(0) = " << BIFMath::tan(0.0) << "\n";

	return 0;
}
