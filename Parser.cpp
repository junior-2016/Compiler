//
// Created by junior on 19-4-9.
//

#include "Parser.h"
#include "Scanner.h"
#include "Exception.h"
#include "StringLiteralPool.h"

namespace Compiler::Parser {
    Scanner::TokenRet token;

    inline auto match(TokenType target) {
        using namespace Compiler::Exception;
        if (token.tokenType == target) token = Scanner::getToken();
        else {
            ExceptionHandle::getHandle().add_exception(
                    ExceptionType::SYNTAX_ERROR, "Not match token on line:" + std::to_string(token.lineNumber));
        }
    }

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
    node if_else_statement() {
        return nullptr;
    }

    node do_while_statement() {
        return nullptr;
    }

    node repeat_until_statement() {
        return nullptr;
    }

    node read_statement() {
        return nullptr;
    }

    node write_statement() {
        return nullptr;
    }

    node function_statement() {
        return nullptr;
    }

    node expression() {
        return nullptr;
    }

    node statement() {
        return nullptr;
    }

    /**
     * stmt_sequence -> statement { ; statement }
     */
    node stmt_sequence() {
        node n = statement();
        node p = n;
        while (token.tokenType == TokenType::SEMI) {
            match(TokenType::SEMI);
            node t = statement();
            if (n == nullptr) {
                n = t;
                p = t;
            } else {
                if (t != nullptr) {
                    p->sibling = t;
                    p = t;
                }
            }
        }
        return n;
    }

    /**
     * program -> stmt_sequence
     */
    node parse() {
        using namespace Compiler::Exception;
        token = Scanner::getToken();
        auto t = stmt_sequence();
        if (token.tokenType != END_FILE) {
            ExceptionHandle::getHandle().add_exception(
                    ExceptionType::SYNTAX_ERROR, "Parser don't reach END_FILE finally");
        }
        return t;
    }
}
