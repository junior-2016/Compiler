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

    // 为了限制bool类型取值,建立一个BOOL枚举,取代C语言的bool类型
    enum class BOOL {
        TRUE = true, FALSE = false
    };

    typedef struct empty_struct {
    } null_t; // 空类型
    typedef BOOL bool_t;
    typedef int32_t int_t;
    typedef float float_t;
    typedef double double_t;
    typedef char char_t;
    typedef std::string string_t;
    typedef std::shared_ptr<string_t> string_ptr;

    inline string_ptr make_string_ptr(const string_t &str) {
        return std::make_shared<string_t>(str);
    }

    NUM_TYPE getNumType(const string_t &tokenString);
}
#endif //COMPILER_UTIL_H
