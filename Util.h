//
// Created by junior on 19-4-17.
//

#ifndef COMPILER_UTIL_H
#define COMPILER_UTIL_H

#include "Compiler.h"
#include "Token.h"

namespace Compiler {
    enum class NUM_TYPE {
        DECIMAL,
        OCT,
        HEX,
        FLOAT,
        DOUBLE
    };

    /**
     * 判断数值(NUM型)TokenString的具体类型(10进制/16进制/8进制/浮点),
     * 主要有两个用途:
     * 一是用于打印NUM Token的信息;
     * 二是在后面语义分析的时候要真正cast数据的时候判断用.
     * 比如后面语义分析得到某个NUM型token的实际类型为 double,
     * 就可以这样cast:
     *   double real_value(){
     *       switch(getNumType(num_token_string)){
     *        case NUM::HEX :
     *            return hex_string_to_double(num_token_string);
     *        case NUM::OCT :
     *            .....
     *       }
     *   }
     *   double hex_string_to_double(const std::string& str){
     *       double d = 0.0;
     *       try{
     *          *reinterpret_cast<unsigned long long*>(&d) = std::stoull(str, nullptr, 16);
     *       } catch(...){}
     *       return d;
     *   }
     */
    NUM_TYPE getNumType(const std::string &tokenString);
}
#endif //COMPILER_UTIL_H
