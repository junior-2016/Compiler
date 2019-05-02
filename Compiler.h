//
// Created by junior on 19-4-6.
//

#ifndef SCANNER_COMPILER_H
#define SCANNER_COMPILER_H

// standard library include
#include <algorithm>
#include <cinttypes>
#include <cstdint>
#include <variant>
#include <any>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

// boost include
#include <boost/format.hpp>

// user include(不要包含Util.h, Util.h需要用的时候再包含, 否则Util.h里面定义的一些类型别名可能报错)
#include "config.h"

namespace Compiler {
    extern FILE *file; // 当前处理的文件. 注意用extern强制声明,不定义.

    void compile(int n, char *argv[]);
}
#endif //SCANNER_COMPILER_H
