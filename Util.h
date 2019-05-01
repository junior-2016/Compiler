//
// Created by junior on 19-4-17.
//

#ifndef COMPILER_UTIL_H
#define COMPILER_UTIL_H

#include "Compiler.h"

namespace Compiler {
    /**
     * 判断数值(NUM型)TokenString的具体类型(10进制/16进制/8进制/浮点),
     * 主要有两个用途:
     * 一是用于打印NUM Token的信息;
     * 二是在后面语法分析的时候要真正cast数据的时候判断用.
     */
    enum class NUM_TYPE {
        DECIMAL,
        OCT,
        HEX,
        FLOAT,
        DOUBLE
    };

    NUM_TYPE getNumType(const std::string &tokenString);

    /**
     * Compiler 常用的类型别名
     */
    typedef std::shared_ptr<std::string> string_ptr;

    inline string_ptr make_string_ptr(const std::string &str) {
        return std::make_shared<std::string>(str);
    }
}
#endif //COMPILER_UTIL_H
