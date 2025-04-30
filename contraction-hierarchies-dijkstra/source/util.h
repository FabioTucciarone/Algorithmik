#pragma once
#include <iostream>

inline void println_error(const std::string message) {
    std::cout << "\x1B[31m" << "[FEHLER]" << "\033[0m " << message << "\"\n";
}
