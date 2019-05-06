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
        Void,
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
