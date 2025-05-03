#pragma once
#include <iostream>

inline void println_error(const std::string &message) {
    std::cout << "\x1B[31m" << "[FEHLER]" << "\033[0m " << message << "\"\n";
}

template<typename Iterable>
inline void print_list(const std::string &name, Iterable list) {
    std::cout << name << " = [ ";
    for (auto element : list) {
        std::cout << element << ", ";
    }
    std::cout << " ]\n";
}