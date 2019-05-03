//
// Created by junior on 19-4-4.
//

#ifndef SCANNER_TOKEN_H
#define SCANNER_TOKEN_H

#include "Compiler.h"
#include "Util.h"

namespace Compiler {
    typedef enum {
        /* 记录文件结束和错误信息 */
                END_FILE, ERROR,

        /* 关键字 */
                IF, THEN, ELSE,             // IF 语句
        REPEAT, UNTIL, DO, WHILE,           // 循环语句
        END,                                // 控制流终止
        READ, WRITE,                        // IO


        TRUE, FALSE, OR, AND, NOT,
        // 逻辑布尔运算(逻辑操作主要是在if和循环的condition条件上用到)
        // eg: if(x>0 and y>0); if (not(x>0)); if (x>0 or y>0);
        // 注意到还有位操作的与/或/非/异或,这些就只能在特殊符号上支持了.
        // 暂定:与&,或|,非!,异或^.

        INT, BOOL, FLOAT, DOUBLE, STRING,
        // 基本类型,注意这里的STRING代表字符串类型,用string关键字表示,与后面的STR字符串常量Token不一样
        // 基本类型赋值规则(后面语义检查用到):
        // int: 范围从-2^31到2^31-1, 允许8/10/16进制赋值;
        // bool: 仅true,false两个取值
        // float: 32位单精度浮点数
        // double: 64位双精度浮点数
        // string: 字符串,通过两个单引号''标出,不提供char类型,可以通过声明单个字符的字符串来充当char.

        /* 标识符　*/
                ID,

        /* 数值和字符串常量,数值常量分浮点数和整数 */
                NUM, STR,

        /* 特殊符号 */
                ASSIGN,  // :=
        EQ, LT, BT, LE, BE, NE,  // = < > <= >= !=
        PLUS, MINUS, TIMES, OVER, // + - * /
        LPAREN, RPAREN,  // ( )
        SEMI, // 分号
        COMMA, // 逗号
        // SINGLE_QUOTES , 单引号作为字符串的匹配符号,应该不需要作为一个Token, 跟 { } 注释符号不作为Token是一个道理.
        // POINT  // 小数点.(用于提取浮点数以及后面支持对象对成员的访问)
    } TokenType;

    std::map<std::string, TokenType> getKeyWordTable();

    std::unordered_set<char> getLegalCharTable();

    std::string getTokenRepresentation(TokenType type, const string_ptr &ptr);

    void printToken(TokenType type, const string_ptr &ptr);
}
#endif //SCANNER_TOKEN_H
