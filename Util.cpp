//
// Created by junior on 19-4-17.
//
#include "Util.h"
#include <cassert>

namespace Compiler {
    NUM_TYPE getNumType(const std::string &tokenString) {
        // assert(tokenString.size() > 0);
        if (tokenString.find('.') != std::string::npos) {
            return NUM_TYPE::FLOAT;
        }
        if (tokenString[0] == '0') {
            if (tokenString.size() > 1 && (tokenString[1] == 'x' || tokenString[1] == 'X')) {
                return NUM_TYPE::HEX;
            }
            return NUM_TYPE::OCT;
        }
        return NUM_TYPE::DECIMAL;
    }
}
