//
// Created by junior on 19-4-7.
//
#include "Token.h"

namespace Compiler {
    // 下面两个表(关键字表和合法字符表),本来是可以直接写成静态成员的,
    // 但是那样写会报警告,可能会无法捕获异常.
    // 所以一个更好的写法是,利用一个get-function来获取表,同时get-function构造的表前面加上static,
    // 这样就避免每次调用get-function都去生成一次表.

    // 用哈希表处理关键字查询表
    std::map<std::string, TokenType> getKeyWordTable() {
        static std::map<std::string, TokenType> table{
                {"if",     IF},
                {"then",   THEN},
                {"else",   ELSE},
                {"end",    END},
                {"repeat", REPEAT},
                {"until",  UNTIL},
                {"do",     DO},
                {"while",  WHILE},
                {"read",   READ},
                {"write",  WRITE},
                {"true",   TRUE},
                {"false",  FALSE},
                {"or",     OR},
                {"and",    AND},
                {"not",    NOT},
                {"int",    INT},
                {"bool",   BOOL},
                {"float",  FLOAT},
                {"double", DOUBLE},
                {"string", STRING}
        };
        return table;
    }

    // 手动创建合法字符表,用哈希集合实现
    std::unordered_set<char> getLegalCharTable() {
        static std::unordered_set<char> table{
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
                'v', 'w', 'x', 'y', 'z',
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
                'V', 'W', 'X', 'Y', 'Z',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                '+', '-', '*', '/',
                '(', ')', '{', '}',
                '>', '<', '=', '!',
                ':', ',', '\'', ';', '.',
                '\r', '\n', '\t', ' ',
                EOF
        };
        return table;
    }

    std::string getTokenRepresentation(TokenType type, const string_ptr &ptr) {
        switch (type) {
            case IF:
                return "if";
            case THEN:
                return "then";
            case ELSE:
                return "else";
            case END:
                return "end";
            case REPEAT:
                return "repeat";
            case UNTIL:
                return "until";
            case DO:
                return "do";
            case WHILE:
                return "while";
            case READ:
                return "read";
            case WRITE:
                return "write";
            case TRUE:
                return "true";
            case FALSE:
                return "false";
            case OR:
                return "or";
            case AND:
                return "and";
            case NOT:
                return "not";
            case INT:
                return "int";
            case BOOL:
                return "bool";
            case FLOAT:
                return "float";
            case DOUBLE:
                return "double";
            case STRING:
                return "string";
            case ASSIGN:
                return ":=";
            case LT:
                return "<";
            case BT:
                return ">";
            case EQ:
                return "=";
            case LE:
                return "<=";
            case BE:
                return ">=";
            case NE:
                return "!=";
            case LPAREN:
                return "(";
            case RPAREN:
                return ")";
            case SEMI:
                return ";";
            case COMMA:
                return ",";
            case PLUS:
                return "+";
            case MINUS:
                return "-";
            case TIMES:
                return "*";
            case OVER:
                return "/";
            case END_FILE:
                return "EOF";
            case STR:
                return (ptr != nullptr) ? *ptr : "STR";
            case ID:
                return (ptr != nullptr) ? *ptr : "ID";
            case NUM:
                return (ptr != nullptr) ? *ptr : "NUM";
            case ERROR:
                return (ptr != nullptr) ? *ptr : "ERROR";
        }
        return (ptr != nullptr) ? *ptr : "";
    }

    void printToken(TokenType type, const string_ptr &ptr) {
        std::string numType;
        std::string representation = getTokenRepresentation(type, ptr);
        switch (type) {
            case IF:
            case THEN:
            case ELSE:
            case END:
            case REPEAT:
            case UNTIL:
            case DO:
            case WHILE:
            case READ:
            case WRITE:
            case TRUE:
            case FALSE:
            case OR:
            case AND:
            case NOT:
            case INT:
            case BOOL:
            case FLOAT:
            case DOUBLE:
            case STRING:
                fprintf(OUTPUT_STREAM, "reserved word: %s\n", representation.c_str());
                break;
            case ASSIGN:
            case LT:
            case BT:
            case EQ:
            case LE:
            case BE:
            case NE:
            case LPAREN:
            case RPAREN:
            case SEMI:
            case COMMA:
            case PLUS:
            case MINUS:
            case TIMES:
            case OVER:
            case END_FILE:
                fprintf(OUTPUT_STREAM, "%s\n", representation.c_str());
                break;
            case NUM:
                switch (getNumType(*ptr)) {
                    case NUM_TYPE::DECIMAL:
                        numType = "DECIMAL";
                        break;
                    case NUM_TYPE::OCT:
                        numType = "OCT";
                        break;
                    case NUM_TYPE::HEX:
                        numType = "HEX";
                        break;
                    case NUM_TYPE::FLOAT:
                        numType = "FLOAT";
                        break;
                    case NUM_TYPE::DOUBLE:
                        numType = "DOUBLE";
                        break;
                }
                fprintf(OUTPUT_STREAM, "NUMBER, val=%s, type=%s\n", representation.c_str(), numType.c_str());
                break;
            case ID:
                fprintf(OUTPUT_STREAM, "ID, name=%s\n", representation.c_str());
                break;
            case STR:
                fprintf(OUTPUT_STREAM, "STR, val=%s\n", representation.c_str());
                break;
            default:
                fprintf(OUTPUT_STREAM, "Unknown token: %s\n", representation.c_str());
                break;
        }
    }
}
