//
// Created by junior on 19-4-9.
//

#include "Parser.h"
#include "Scanner.h"
#include "Exception.h"
#include "StringLiteralPool.h"

namespace Compiler::Parser {

    /**
     * if-else statement:
     * C语言的文法是:
     *  statement = ... | selection-statement
     *  selection-statement = ... | if ( expression ) statement | if ( expression ) statement else statement
     *  这个是有歧义的,对于 if (expr) if (expr) else stat 会有:
     *  if (expr) { if (expr) else stat } 和
     *  if (expr) { if (expr) } else stat 两种树.
     *  C语言的解决方案是只选择第一颗树,即选择靠近else的if进行解析;
     *  另外一种方案是引入更多的变量将它变成非歧义的文法,比如JAVA语言(参考Java的EBNF).
     */
    node if_statement() {
        return nullptr;
    }

    node stmt_sequence() {
        return nullptr;
    }

    node parse() {
        using namespace Compiler::Exception;
        auto token = Scanner::getToken();
        auto t = stmt_sequence();
        if (token.tokenType != END_FILE) {
            ExceptionHandle::getHandle().add_exception(ExceptionType::SYNTAX_ERROR, "Syntax Error");
        }
        return t;
    }
}
