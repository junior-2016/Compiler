//
// Created by junior on 19-4-6.
//

#ifndef SCANNER_COMPILER_H
#define SCANNER_COMPILER_H

// global include
#include <algorithm>
#include <cinttypes>
#include <variant>
#include <any>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "config.h"

namespace Compiler {
    extern FILE *file; // 当前处理的文件. 注意用extern强制声明,不定义.

    void compile(int n, char *argv[]);
}
#endif //SCANNER_COMPILER_H
