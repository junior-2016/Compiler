//
// Created by junior on 19-5-5.
//

#ifndef COMPILER_TYPESYSTEM_H
#define COMPILER_TYPESYSTEM_H

#include "Compiler.h"
#include "Token.h"

namespace Compiler {

    /**
     * 内置基本类型系统
     */
    enum class Type {
        Void, // 空类型.当前语言里没有void关键字,所以不存在void类型的ID或者表达式,这里引入void类型是为了在语义检查错误时设置一个错误类型标志.
        Integer,
        Boolean,
        String,
        Float,
        Double
    };

    struct TypeSystem {
        static string_t getTypeRepresentation(Type type) {
            switch (type) {
                case Type::Integer:
                    return "Integer";
                case Type::Boolean:
                    return "Boolean";
                case Type::String:
                    return "String";
                case Type::Float:
                    return "Float";
                case Type::Double:
                    return "Double";
                default:
                    return "Void";
            }
        }

        static Type getTypeFromToken(TokenType token) {
            switch (token) {
                case TokenType::INT:
                    return Type::Integer;
                case TokenType::DOUBLE:
                    return Type::Double;
                case TokenType::FLOAT:
                    return Type::Float;
                case TokenType::STRING:
                    return Type::String;
                case TokenType::BOOL:
                    return Type::Boolean;
                default:
                    return Type::Void;
            }
        }
    };
}
#endif //COMPILER_TYPESYSTEM_H
