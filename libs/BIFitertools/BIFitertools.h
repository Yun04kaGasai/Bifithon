#ifndef BIFITERTOOLS_H
#define BIFITERTOOLS_H

#include <string>
#include <vector>

namespace bif {
namespace itertools {

class BIFitertools {
public:
    static std::vector<int> range(int stop);
    static std::vector<int> range(int start, int stop, int step);
    static std::vector<std::string> repeat(const std::string& value, int times);
    static std::vector<int> count(int start, int step, int count);
    static std::vector<int> cycle(const std::vector<int>& items, int times);
    static std::vector<int> chain(const std::vector<int>& first, const std::vector<int>& second);
};

} // namespace itertools
} // namespace bif

#endif // BIFITERTOOLS_H
