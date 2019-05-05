//
// Created by junior on 19-5-5.
//

#ifndef COMPILER_TYPESYSTEM_H
#define COMPILER_TYPESYSTEM_H

#include "Compiler.h"

namespace Compiler {

    /**
     * 内置基本类型系统
     */
    enum class Type {
        Void,
        Integer,
        Boolean,
        String,
        Float,
        Double
    };

}
#endif //COMPILER_TYPESYSTEM_H
