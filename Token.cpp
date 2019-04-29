//
// Created by junior on 19-4-7.
//
#include "Token.h"
#include "Util.h"

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
                {"elseif", ELSEIF},
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

    void printToken(TokenType type, std::string &tokenString) {
        std::string numType;
        switch (type) {
            case IF:
            case THEN:
            case ELSE:
            case ELSEIF:
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
                fprintf(OUTPUT_STREAM, "reserved word: %s\n", tokenString.c_str());
                break;
            case ASSIGN:
                fprintf(OUTPUT_STREAM, ":=\n");
                break;
            case LT:
                fprintf(OUTPUT_STREAM, "<\n");
                break;
            case BT:
                fprintf(OUTPUT_STREAM, ">\n");
                break;
            case EQ:
                fprintf(OUTPUT_STREAM, "=\n");
                break;
            case LE:
                fprintf(OUTPUT_STREAM, "<=\n");
                break;
            case BE:
                fprintf(OUTPUT_STREAM, ">=\n");
                break;
            case NE:
                fprintf(OUTPUT_STREAM, "!=\n");
                break;
            case LPAREN:
                fprintf(OUTPUT_STREAM, "(\n");
                break;
            case RPAREN:
                fprintf(OUTPUT_STREAM, ")\n");
                break;
            case SEMI:
                fprintf(OUTPUT_STREAM, ";\n");
                break;
            case COMMA:
                fprintf(OUTPUT_STREAM, ",\n");
                break;
            case PLUS:
                fprintf(OUTPUT_STREAM, "+\n");
                break;
            case MINUS:
                fprintf(OUTPUT_STREAM, "-\n");
                break;
            case TIMES:
                fprintf(OUTPUT_STREAM, "*\n");
                break;
            case OVER:
                fprintf(OUTPUT_STREAM, "/\n");
                break;
            case END_FILE:
                fprintf(OUTPUT_STREAM, "EOF\n");
                break;
            case NUM:
                /*
                 * 虽然我们不将十进制,八进制,十六进制和浮点数分开,但是打印Token的时候可以显式输出这些信息.
                 */
                switch (Compiler::getNumType(tokenString)) {
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
                fprintf(OUTPUT_STREAM, "NUMBER, val=%s, type=%s\n", tokenString.c_str(), numType.c_str());
                break;
            case ID:
                fprintf(OUTPUT_STREAM, "ID, name=%s\n", tokenString.c_str());
                break;
            case STR:
                fprintf(OUTPUT_STREAM, "STR, val=%s\n", tokenString.c_str());
                break;
            default:
                fprintf(OUTPUT_STREAM, "Unknown token: %s\n", tokenString.c_str());
                break;
        }
    }
}
