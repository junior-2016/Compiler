//
// Created by junior on 19-4-9.
//

#include "Parser.h"
#include "Scanner.h"
#include "Exception.h"
#include "StringLiteralPool.h"

namespace Compiler::Parser {
    Scanner::TokenRet token;

    inline void syntax_error_report() {
        using namespace Compiler::Exception;
        ExceptionHandle::getHandle().add_exception(
                ExceptionType::SYNTAX_ERROR, std::string("Not match token ")
                                             + *token.tokenString
                                             + std::string(" on line:")
                                             + std::to_string(token.lineNumber));
    }

    inline auto match(TokenType target) {
        if (token.tokenType == target) token = Scanner::getToken();
        else {
            syntax_error_report();
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

    node assign_statement() {
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
        using namespace Compiler::Exception;
        node n = nullptr;
        switch (token.tokenType) {
            case TokenType::IF:
                n = if_else_statement();
                break;
            case TokenType::REPEAT:
                n = repeat_until_statement();
                break;
            case TokenType::ID:
                n = assign_statement();
                break;
            case TokenType::READ:
                n = read_statement();
                break;
            case TokenType::WRITE:
                n = write_statement();
                break;
            default:
                syntax_error_report();
                token = Scanner::getToken();
                break;
        }
        return n;
    }

    /**
     * statement_sequence -> statement { ; statement }*
     * 解析时
     *          statement_sequence
     *                  |
     *        stat ; stat ; stat; stat; ...
     */
    node statement_sequence() {
        node n = statement();
        node p = n;
        while (token.tokenType == SEMI) {
            match(TokenType::SEMI);
            node q = statement();
            if (q != nullptr) {
                if (n == nullptr) {
                    n = q;
                    p = q;
                } else {
                    p->sibling = q;
                    p = q;
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
        auto t = statement_sequence();
        if (token.tokenType != END_FILE) {
            ExceptionHandle::getHandle().add_exception(
                    ExceptionType::SYNTAX_ERROR, "Parser don't reach END_FILE finally");
        }
        return t;
    }
}
