//
// Created by junior on 19-4-4.
//

#ifndef SCANNER_TOKEN_H
#define SCANNER_TOKEN_H

#include "Compiler.h"

namespace Compiler {
    typedef enum {
        /* 记录文件结束和错误信息 */
                END_FILE, ERROR,

        /* 关键字 */
                IF, THEN, ELSE, ELSEIF, END,          // IF 语句
                REPEAT, UNTIL, DO, WHILE,             // 循环语句
                READ, WRITE,                         // IO

                TRUE, FALSE, OR, AND, NOT,
                // 逻辑布尔运算(逻辑操作主要是在if和循环的condition条件上用到)
                // eg: if(x>0 and y>0); if (not(x>0)); if (x>0 or y>0);
                // 注意到还有位操作的与/或/非/异或,这些就只能在特殊符号上支持了.
                // 暂定:与&,或|,非!,异或^.

                INT, BOOL, FLOAT, DOUBLE, STRING,
                // 基本类型,注意这里的STRING代表字符串类型,用string关键字表示,与后面的STR字符串常量Token不一样


        /* 标识符　*/
                ID,

        /* 数值和字符串常量 */
        /* 数值常量除了支持整数常量还需要支持浮点数常量,注意不需要考虑正负符号,因为正负符号应该考虑在运算符里面 */
        /* 如果要支持浮点数常量,在自动机上需要注意小数点.的处理 */
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

    void printToken(TokenType type, std::string &tokenString);
}
#endif //SCANNER_TOKEN_H
