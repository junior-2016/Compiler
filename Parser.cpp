//
// Created by junior on 19-4-9.
//

#include "Parser.h"
#include "Scanner.h"
#include "Exception.h"
#include "StringLiteralPool.h"

namespace Compiler::Parser {
    /* parse tree node */
    node newStatementNode(StmtKind stmtKind);

    node newExpressionNode(ExpKind expKind);

    /* statement  */
    node statement_sequence();

    node statement();

    node if_else_statement();

    node assign_statement();

    node do_while_statement();

    node repeat_until_statement();

    node read_statement();

    node write_statement();

    node function_statement();

    /* expression */
    node expression();

    node arithmetic_expression();

    Scanner::TokenRet token;

    inline void syntax_error_report() {
        using namespace Compiler::Exception;
        ExceptionHandle::getHandle().add_exception(
                ExceptionType::SYNTAX_ERROR,
                std::string("Not match token ")
                + *token.tokenString
                + std::string(" on line:")
                + std::to_string(Scanner::lineNumber));
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
     *  statement => ... | selection-statement
     *  selection-statement => ... | if ( expression ) statement | if ( expression ) statement else statement
     *  这个是有歧义的,对于 if (expr) if (expr) else stat 会有:
     *  if (expr) { if (expr) else stat } 和
     *  if (expr) { if (expr) } else stat 两种树.
     *  C语言的解决方案是只选择第一颗树,即选择靠近else的if进行解析;
     *  另外一种方案是引入更多的变量将它变成非歧义的文法,比如JAVA语言(参考Java的EBNF).
     *
     *  这里使用的是C的方案,即:
     *  statement_sequence => statement {; statement }*
     *  statement => if_else_statement | .. | ..
     *  if_else_statement => if expression then statement_sequence end
     *                   | if expression then statement_sequence else statement_sequence end
     *  解析如下语句:
     *           if expr then
     *               if expr then
     *                   stat_sequence
     *               else
     *                   stat_sequence
     *               end
     *           end
     *  过程: Start -> ... -> if [expr] then [stat_sequence] end ->
     *                ... -> if [expr] then [ if [expr] then [stat_sequence] else [stat_sequence] end ] end
     */
    node if_else_statement() {
        node n = newStatementNode(StmtKind::IfK);
        match(TokenType::IF);
        if (n != nullptr) n->childs.push_back(expression());
        match(TokenType::THEN);
        if (n != nullptr) n->childs.push_back(statement_sequence());
        if (token.tokenType == TokenType::ELSE) { // 这里必须写成 if(token_type is ELSE){ match (ELSE); ...},代表一种可选的推导分支
            match(TokenType::ELSE);
            if (n != nullptr) n->childs.push_back(statement_sequence());
        }
        match(TokenType::END);
        return n;
    }

    /**
     * assign_statement => ID := expression
     */
    node assign_statement() {
        node n = newStatementNode(StmtKind::AssignK);
        if (n != nullptr && token.tokenType == TokenType::ID) {
            n->attribute = token.tokenString;
        }
        match(TokenType::ID);
        match(TokenType::ASSIGN);
        if (n != nullptr) n->childs.push_back(expression());
        return n;
    }

    /**
     * do_while_statement => do statement_sequence while expression
     */
    node do_while_statement() {
        node n = newStatementNode(StmtKind::WhileK);
        match(TokenType::DO);
        if (n != nullptr) n->childs.push_back(statement_sequence());
        match(TokenType::WHILE);
        if (n != nullptr) n->childs.push_back(expression());
        return n;
    }

    /**
     * repeat_until_statement => repeat statement_sequence until expression
     */
    node repeat_until_statement() {
        node n = newStatementNode(StmtKind::RepeatK);
        match(TokenType::REPEAT);
        if (n != nullptr) n->childs.push_back(statement_sequence());
        match(TokenType::UNTIL);
        if (n != nullptr) n->childs.push_back(expression());
        return n;
    }

    /**
     * read_statement => read ID
     */
    node read_statement() {
        node n = newStatementNode(StmtKind::ReadK);
        match(TokenType::READ);
        if (n != nullptr && token.tokenType == TokenType::ID) {
            n->attribute = token.tokenString;
        }
        match(TokenType::ID);
        return n;
    }

    /**
     * write_statement => write expression
     */
    node write_statement() {
        node n = newStatementNode(StmtKind::WriteK);
        match(TokenType::WRITE);
        if (n != nullptr) n->childs.push_back(expression());
        return n;
    }

    // TODO: 引入function的文法
    node function_statement() {
        node n = new TreeNode;

        return n;
    }

    node expression() {
        node n = arithmetic_expression();
        if (token.tokenType == TokenType::LT
            || token.tokenType == TokenType::BT
            || token.tokenType == TokenType::EQ
            || token.tokenType == TokenType::NE
            || token.tokenType == TokenType::LE
            || token.tokenType == TokenType::BE) {
            node p = newExpressionNode(ExpKind::OpK);
            if (p != nullptr) {
                p->childs.push_back(n);
                p->attribute = token.tokenType;
                n = p;
            }
            match(token.tokenType); // 这里的match必成功
            if (n != nullptr) {
                n->childs.push_back(arithmetic_expression());
            }
        }
        return n;
    }

    node arithmetic_expression() {
        node n = new TreeNode;
        return n;
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
            case TokenType::DO:
                n = do_while_statement();
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
        auto root = statement_sequence();
        if (token.tokenType != END_FILE) {
            ExceptionHandle::getHandle().add_exception(
                    ExceptionType::SYNTAX_ERROR, "Parser don't reach END_FILE finally");
        }
        return root;
    }

    node newStatementNode(StmtKind stmtKind) {
        node n = new TreeNode;
        n->lineNumber = Scanner::lineNumber;
        n->nodeKind = NodeKind::StmtK;
        n->kind = stmtKind;
        return n;
    }

    node newExpressionNode(ExpKind expKind) {
        node n = new TreeNode;
        n->lineNumber = Scanner::lineNumber;
        n->nodeKind = NodeKind::ExpK;
        n->kind = expKind;
        n->expType = ExpType::Void;
        return n;
    }
}
