#include "BIFitertools.h"

namespace bif {
namespace itertools {

std::vector<int> BIFitertools::range(int stop) {
    return range(0, stop, 1);
}

std::vector<int> BIFitertools::range(int start, int stop, int step) {
    std::vector<int> out;
    if (step == 0) {
        return out;
    }

    if (step > 0) {
        for (int i = start; i < stop; i += step) {
            out.push_back(i);
        }
    } else {
        for (int i = start; i > stop; i += step) {
            out.push_back(i);
        }
    }

    return out;
}

std::vector<std::string> BIFitertools::repeat(const std::string& value, int times) {
    std::vector<std::string> out;
    if (times <= 0) {
        return out;
    }

    out.reserve(static_cast<size_t>(times));
    for (int i = 0; i < times; ++i) {
        out.push_back(value);
    }

    return out;
}

std::vector<int> BIFitertools::count(int start, int step, int count) {
    std::vector<int> out;
    if (count <= 0) {
        return out;
    }

    out.reserve(static_cast<size_t>(count));
    int value = start;
    for (int i = 0; i < count; ++i) {
        out.push_back(value);
        value += step;
    }

    return out;
}

std::vector<int> BIFitertools::cycle(const std::vector<int>& items, int times) {
    std::vector<int> out;
    if (times <= 0 || items.empty()) {
        return out;
    }

    out.reserve(static_cast<size_t>(items.size() * times));
    for (int i = 0; i < times; ++i) {
        out.insert(out.end(), items.begin(), items.end());
    }

    return out;
}

std::vector<int> BIFitertools::chain(const std::vector<int>& first, const std::vector<int>& second) {
    std::vector<int> out;
    out.reserve(first.size() + second.size());
    out.insert(out.end(), first.begin(), first.end());
    out.insert(out.end(), second.begin(), second.end());
    return out;
}

} // namespace itertools
} // namespace bif
