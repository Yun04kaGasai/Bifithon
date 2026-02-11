#ifndef BIFTOOLS_H
#define BIFTOOLS_H

#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <iterator>

namespace biftools {
    // Range generator (like Python's range)
    class Range {
    private:
        int start_, stop_, step_;
    public:
        Range(int stop) : start_(0), stop_(stop), step_(1) {}
        Range(int start, int stop) : start_(start), stop_(stop), step_(1) {}
        Range(int start, int stop, int step) : start_(start), stop_(stop), step_(step) {}
        
        class iterator {
        private:
            int current_;
            int step_;
        public:
            iterator(int val, int step) : current_(val), step_(step) {}
            int operator*() const { return current_; }
            iterator& operator++() { current_ += step_; return *this; }
            bool operator!=(const iterator& other) const { return current_ != other.current_; }
        };
        
        iterator begin() const { return iterator(start_, step_); }
        iterator end() const { return iterator(stop_, step_); }
    };
    
    // Count (infinite counter)
    template<typename T = int>
    class Count {
    private:
        T start_, step_;
        int max_count_;
    public:
        Count(T start = 0, T step = 1, int max_count = 1000000) 
            : start_(start), step_(step), max_count_(max_count) {}
        
        class iterator {
        private:
            T current_;
            T step_;
            int count_;
            int max_count_;
        public:
            iterator(T val, T step, int count, int max_count) 
                : current_(val), step_(step), count_(count), max_count_(max_count) {}
            T operator*() const { return current_; }
            iterator& operator++() { 
                current_ += step_; 
                count_++;
                return *this; 
            }
            bool operator!=(const iterator& other) const { 
                return count_ < max_count_; 
            }
        };
        
        iterator begin() const { return iterator(start_, step_, 0, max_count_); }
        iterator end() const { return iterator(start_, step_, max_count_, max_count_); }
    };
    
    // Cycle through elements
    template<typename Container>
    class Cycle {
    private:
        Container container_;
        int max_iterations_;
    public:
        Cycle(const Container& c, int max_iter = 1000) 
            : container_(c), max_iterations_(max_iter) {}
        
        class iterator {
        private:
            typename Container::const_iterator current_;
            typename Container::const_iterator begin_;
            typename Container::const_iterator end_;
            int iteration_;
            int max_iterations_;
        public:
            iterator(typename Container::const_iterator current,
                    typename Container::const_iterator begin,
                    typename Container::const_iterator end,
                    int iter, int max_iter)
                : current_(current), begin_(begin), end_(end), 
                  iteration_(iter), max_iterations_(max_iter) {}
            
            auto operator*() const { return *current_; }
            iterator& operator++() {
                ++current_;
                if (current_ == end_) {
                    current_ = begin_;
                    iteration_++;
                }
                return *this;
            }
            bool operator!=(const iterator& other) const {
                return iteration_ < max_iterations_;
            }
        };
        
        iterator begin() const { 
            return iterator(container_.begin(), container_.begin(), 
                          container_.end(), 0, max_iterations_); 
        }
        iterator end() const { 
            return iterator(container_.end(), container_.begin(), 
                          container_.end(), max_iterations_, max_iterations_); 
        }
    };
    
    // Accumulate (cumulative sum)
    template<typename T>
    std::vector<T> accumulate(const std::vector<T>& vec) {
        std::vector<T> result;
        T sum = 0;
        for (const auto& val : vec) {
            sum += val;
            result.push_back(sum);
        }
        return result;
    }
    
    // Chain multiple containers
    template<typename Container>
    std::vector<typename Container::value_type> chain(
        const std::vector<Container>& containers) {
        std::vector<typename Container::value_type> result;
        for (const auto& container : containers) {
            result.insert(result.end(), container.begin(), container.end());
        }
        return result;
    }
    
    // Combinations
    template<typename T>
    std::vector<std::vector<T>> combinations(const std::vector<T>& vec, int r) {
        std::vector<std::vector<T>> result;
        std::vector<bool> selector(vec.size());
        std::fill(selector.begin(), selector.begin() + r, true);
        
        do {
            std::vector<T> combo;
            for (size_t i = 0; i < vec.size(); i++) {
                if (selector[i]) {
                    combo.push_back(vec[i]);
                }
            }
            result.push_back(combo);
        } while (std::prev_permutation(selector.begin(), selector.end()));
        
        return result;
    }
    
    // Permutations
    template<typename T>
    std::vector<std::vector<T>> permutations(std::vector<T> vec, int r = -1) {
        if (r == -1) r = vec.size();
        std::vector<std::vector<T>> result;
        
        if (r > vec.size()) return result;
        
        std::vector<T> subset(vec.begin(), vec.begin() + r);
        std::sort(subset.begin(), subset.end());
        
        do {
            result.push_back(subset);
        } while (std::next_permutation(subset.begin(), subset.end()));
        
        return result;
    }
    
    // Filter
    template<typename T>
    std::vector<T> filter(const std::vector<T>& vec, 
                         std::function<bool(const T&)> pred) {
        std::vector<T> result;
        std::copy_if(vec.begin(), vec.end(), std::back_inserter(result), pred);
        return result;
    }
    
    // Map
    template<typename T, typename U>
    std::vector<U> map(const std::vector<T>& vec, 
                      std::function<U(const T&)> func) {
        std::vector<U> result;
        std::transform(vec.begin(), vec.end(), std::back_inserter(result), func);
        return result;
    }
    
    // Zip
    template<typename T, typename U>
    std::vector<std::pair<T, U>> zip(const std::vector<T>& vec1, 
                                     const std::vector<U>& vec2) {
        std::vector<std::pair<T, U>> result;
        size_t min_size = std::min(vec1.size(), vec2.size());
        for (size_t i = 0; i < min_size; i++) {
            result.push_back(std::make_pair(vec1[i], vec2[i]));
        }
        return result;
    }
}

#endif // BIFTOOLS_H
