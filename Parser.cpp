//
// Created by junior on 19-4-9.
//

#include "Parser.h"
#include "Scanner.h"
#include "Exception.h"
#include "StringLiteralPool.h"

namespace Compiler::Parser {
    /* global token */
    Scanner::TokenRet token;

    /* parse tree node */
    TreeNode::ptr newStatementNode(StmtKind stmtKind);

    TreeNode::ptr newExpressionNode(ExpKind expKind);

    /* statement  */
    TreeNode::ptr statement_sequence();

    TreeNode::ptr statement();

    TreeNode::ptr if_else_statement();

    TreeNode::ptr assign_statement();

    TreeNode::ptr variable_list_statement();

    TreeNode::ptr declaration_statement();

    TreeNode::ptr do_while_statement();

    TreeNode::ptr repeat_until_statement();

    TreeNode::ptr read_statement();

    TreeNode::ptr write_statement();

    /* expression */
    TreeNode::ptr expression();

    TreeNode::ptr logical_or_expression();

    TreeNode::ptr logical_and_expression();

    TreeNode::ptr equality_expression();

    TreeNode::ptr relational_expression();

    TreeNode::ptr arithmetic_additive_expression();

    TreeNode::ptr arithmetic_multiplicative_expression();

    TreeNode::ptr factor_expression();

    /**
     * 提交语法错误
     * @param func_string 函数名字符串
     * @param expected_token_string 期待的token字符串
     */
    inline void report_syntax_error(const std::string &func_string, const std::string &expected_token_string) {
        using namespace Compiler::Exception;
        std::string message = func_string
                              + " unexpected token ["
                              + getTokenRepresentation(token.tokenType, token.tokenString)
                              + "], expected token ["
                              + expected_token_string
                              + "] on line:" + std::to_string(Scanner::lineNumber);
        ExceptionHandle::getHandle().add_exception(ExceptionType::SYNTAX_ERROR, message);
    }

    inline void match(TokenType target) {
        if (token.tokenType == target) token = Scanner::getToken();
        else {
            report_syntax_error("match()", getTokenRepresentation(target, nullptr));
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
    TreeNode::ptr if_else_statement() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::IF:
                n = newStatementNode(StmtKind::IfK);
                if (n != nullptr) {
                    match(TokenType::IF);
                    n->children.push_back(expression());
                    match(TokenType::THEN);
                    n->children.push_back(statement_sequence());
                    if (token.tokenType == TokenType::ELSE) {
                        match(TokenType::ELSE);
                        n->children.push_back(statement_sequence());
                    }
                    match(TokenType::END);
                }
                break;
            default:
                report_syntax_error("if_else_statement()", getTokenRepresentation(TokenType::IF, nullptr));
                break;
        }
        return n;
    }

    /**
     * variable_list_statement => ID [:= expression](可选) { , ID [:= expression]}*
     */
    TreeNode::ptr variable_list_statement() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::ID:
                n = newStatementNode(StmtKind::VariableListK);
                if (n != nullptr) {
                    n->attribute = token.tokenString;
                    match(TokenType::ID);
                    if (token.tokenType == TokenType::ASSIGN) { // 可选分支
                        match(TokenType::ASSIGN);
                        n->children.push_back(expression());
                    }
                    if (token.tokenType == TokenType::COMMA) {
                        match(TokenType::COMMA);
                        n->sibling = variable_list_statement();
                    }
                }
                break;
            default:
                report_syntax_error("variable_list_statement()", getTokenRepresentation(TokenType::ID, nullptr));
        }
        return n;
    }

    /**
     * declaration_statement => Type variable_list_statement
     * 对变量进行声明
     */
    TreeNode::ptr declaration_statement() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::INT:
            case TokenType::FLOAT:
            case TokenType::DOUBLE:
            case TokenType::BOOL:
            case TokenType::STRING:
                n = newStatementNode(StmtKind::DeclarationK);
                if (n != nullptr) {
                    n->attribute = token.tokenType;
                    match(token.tokenType);
                    n->children.push_back(variable_list_statement());
                }
                break;
            default:
                report_syntax_error("declaration_statement()", "......");
                break;
        }
        return n;
    }

    /**
     * assign_statement => ID := expression (赋值语句. 注意对变量ID的任何赋值之前必须先存在声明)
     */
    TreeNode::ptr assign_statement() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::ID:
                n = newStatementNode(StmtKind::AssignK);
                if (n != nullptr) {
                    n->attribute = token.tokenString;
                    match(TokenType::ID);
                    match(TokenType::ASSIGN);
                    n->children.push_back(expression());
                }
                break;
            default:
                report_syntax_error("assign_statement()", getTokenRepresentation(TokenType::ID, nullptr));
                break;
        }
        return n;
    }

    /**
     * do_while_statement => do statement_sequence while expression
     */
    TreeNode::ptr do_while_statement() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::DO:
                n = newStatementNode(StmtKind::WhileK);
                if (n != nullptr) {
                    match(TokenType::DO);
                    n->children.push_back(statement_sequence());
                    match(TokenType::WHILE);
                    n->children.push_back(expression());
                }
                break;
            default:
                report_syntax_error("do_while_statement()", getTokenRepresentation(TokenType::DO, nullptr));
                break;
        }
        return n;
    }

    /**
     * repeat_until_statement => repeat statement_sequence until expression
     */
    TreeNode::ptr repeat_until_statement() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::REPEAT:
                n = newStatementNode(StmtKind::RepeatK);
                if (n != nullptr) {
                    match(TokenType::REPEAT);
                    n->children.push_back(statement_sequence());
                    match(TokenType::UNTIL);
                    n->children.push_back(expression());
                }
                break;
            default:
                report_syntax_error("repeat_until_statement()", getTokenRepresentation(TokenType::REPEAT, nullptr));
                break;
        }
        return n;
    }

    /**
     * read_statement => read ID
     */
    TreeNode::ptr read_statement() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::READ:
                n = newStatementNode(StmtKind::ReadK);
                if (n != nullptr) {
                    match(TokenType::READ);
                    if (token.tokenType == TokenType::ID) { // 没有语法错误的情况下,设置正确的属性
                        n->attribute = token.tokenString;
                    } // 如果存在语法错误,n->attribute没有被正确设置,则n->attribute.index()默认为0,即空属性.
                    match(TokenType::ID); // 如果没有语法错误match成功,否则match失败.
                }
                break;
            default:
                report_syntax_error("read_statement()", getTokenRepresentation(TokenType::READ, nullptr));
                break;
        }
        return n;
    }

    /**
     * write_statement => write expression
     */
    TreeNode::ptr write_statement() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::WRITE:
                n = newStatementNode(StmtKind::WriteK);
                if (n != nullptr) {
                    match(TokenType::WRITE);
                    n->children.push_back(expression());
                }
                break;
            default:
                report_syntax_error("write_statement()", getTokenRepresentation(TokenType::WRITE, nullptr));
                break;
        }
        return n;
    }

    /**
     * 为了简化代码量,写了个通用的expression parser模板,适应如下文法:
     *  Expression => next_expression { [tokenList] next_expression }*
     *  解析后的parser tree 举例如下:
     * eg: 其中 op is in [tokenList]
     *                 op => return
     *           op         next_expr (后执行)
     * next_expr    next_expr (先执行)
     */
    TreeNode::ptr expression_template(const std::function<TreeNode::ptr()> &nextExpression,
                                      std::initializer_list<TokenType> tokenList) {
        auto n = nextExpression();
        while (std::find(tokenList.begin(), tokenList.end(), token.tokenType) != tokenList.end()) {
            auto p = newExpressionNode(ExpKind::OpK);
            if (p != nullptr) {
                p->attribute = token.tokenType;
                p->children.push_back(n);
                n = p;
                match(token.tokenType);
                n->children.push_back(nextExpression());
            }
        }
        return n;
    }


    // expression => logical_or_expression
    TreeNode::ptr expression() { return logical_or_expression(); }

    // logical_or_expression => logical_and_expression { or logical_and_expression }*
    TreeNode::ptr logical_or_expression() {
        return expression_template(logical_and_expression, {TokenType::OR});
    }

    //logical_and_expression => equality_expression { and equality_expression }*
    TreeNode::ptr logical_and_expression() {
        return expression_template(equality_expression, {TokenType::AND});
    }

    // equality_expression => relational_expression { = | !=  relational_expression}*
    TreeNode::ptr equality_expression() {
        return expression_template(relational_expression, {TokenType::EQ, TokenType::NE});
    }

    // relational_expression => additive_expression { > | < | >= | <=  additive_expression } *
    TreeNode::ptr relational_expression() {
        return expression_template(arithmetic_additive_expression,
                                   {TokenType::LT, TokenType::LE, TokenType::BT, TokenType::BE});
    }

    // additive_expression => multiplicative_expression { +|- multiplicative_expression }*
    TreeNode::ptr arithmetic_additive_expression() {
        return expression_template
                (arithmetic_multiplicative_expression, {TokenType::PLUS, TokenType::MINUS});
    }

    //multiplicative_expression => factor_expression { * | / | % factor_expression }*
    TreeNode::ptr arithmetic_multiplicative_expression() {
        return expression_template(factor_expression, {TokenType::TIMES, TokenType::OVER, TokenType::MOD});
    }

    /**
     * factor_expression => not factor_expression | ID | NUM | STR | BOOL | ( expression )
     * 注意到: STR 不能参与到 arithmetic 或者 logical 运算中,只能在 assignment/declaration/write expression 这些出现.
     */
    TreeNode::ptr factor_expression() {
        TreeNode::ptr n = nullptr;
        switch (token.tokenType) {
            case TokenType::NOT:
                n = newExpressionNode(ExpKind::OpK);
                if (n != nullptr) {
                    n->attribute = token.tokenType;
                    match(TokenType::NOT);
                    n->children.push_back(factor_expression());
                }
                break;
            case TokenType::NUM:
                try {
                    switch (getNumType(*token.tokenString)) {
                        case NUM_TYPE::DECIMAL:
                        case NUM_TYPE::OCT:
                        case NUM_TYPE::HEX:
                            n = newExpressionNode(ExpKind::ConstIntK);
                            if (n != nullptr)
                                n->attribute = (int_t) (std::stoi(*token.tokenString));// 16/10/8 进制字符串转换为整数
                            break;
                        case NUM_TYPE::FLOAT:
                            n = newExpressionNode(ExpKind::ConstFloatK);
                            if (n != nullptr) n->attribute = (float_t) (std::stof(*token.tokenString));
                            break;
                        case NUM_TYPE::DOUBLE:
                            n = newExpressionNode(ExpKind::ConstDoubleK);
                            if (n != nullptr) n->attribute = (double_t) (std::stod(*token.tokenString));
                            break;
                    }
                } catch (std::invalid_argument &e) {
                    std::cerr << "arithmetic_factor() convert number "
                              << *token.tokenString << " error:" << e.what() << "\n";
                }
                match(TokenType::NUM);
                break;
            case TokenType::TRUE:
            case TokenType::FALSE:
                n = newExpressionNode(ExpKind::ConstBoolK);
                if (n != nullptr) {
                    n->attribute = (token.tokenType == TokenType::TRUE) ? BOOL::TRUE : BOOL::FALSE;
                }
                match(token.tokenType);
                break;
            case TokenType::STR:
                n = newExpressionNode(ExpKind::ConstStringK);
                if (n != nullptr) {
                    n->attribute = token.tokenString;
                }
                match(TokenType::STR);
                break;
            case TokenType::ID:
                n = newExpressionNode(ExpKind::IdK);
                if (n != nullptr) {
                    n->attribute = token.tokenString;
                }
                match(TokenType::ID);
                break;
            case TokenType::LPAREN:
                match(TokenType::LPAREN);
                n = expression();
                match(TokenType::RPAREN);
                break;
            default:
                report_syntax_error("factor_expression()", "......");
                // token = Scanner::getToken(); // 这里应该不需要前进一个token.
                break;
        }
        return n;
    }

    /**
     * statement => if_else_stat | repeat_until_stat | do_while_stat | assign_stat | read_stat | write_stat | declaration_stat
     */
    TreeNode::ptr statement() {
        using namespace Compiler::Exception;
        TreeNode::ptr n = nullptr;
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
            case TokenType::INT:
            case TokenType::FLOAT:
            case TokenType::DOUBLE:
            case TokenType::BOOL:
            case TokenType::STRING:
                n = declaration_statement();
                break;
            default:
                report_syntax_error("statement()", "......");
                /**
                 　* 注意下面这一步很重要,如果在statement()语句一开始没有匹配到任何一个token,必须往下前进一个token.
                   * 如果token不改变,statement()结束回到statement_sequence(), statement_sequence()又进一步调用statement(),
                   * 然后又是一轮不匹配,就会陷入 statement() ---不匹配---> statement_sequence ---调用---> statement() ---不匹配---> ...
                   * 的死循环. 当测试源文件开头是一个运算比较符号比如 ">" 的时候这种情况就出现了.
                 　*/
                token = Scanner::getToken();
                break;
        }
        return n;
    }

    /**
     * statement_sequence -> statement { ; statement }*
     * 解析时
     *                statement_sequence
     *                       |
     *        stat ; stat ; stat; stat; ... ; stat (statement_sequence最后一个stat不需要分号)
     */
    TreeNode::ptr statement_sequence() {
        auto n = statement();
        auto p = n;
        while (token.tokenType != TokenType::END_FILE &&
               token.tokenType != TokenType::ELSE &&
               token.tokenType != TokenType::END &&
               token.tokenType != TokenType::UNTIL &&
               token.tokenType != TokenType::WHILE) {
            match(TokenType::SEMI);
            auto q = statement();
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
     * program -> statement_sequence [END_FILE]
     */
    TreeNode::ptr parse() {
        using namespace Compiler::Exception;
        token = Scanner::getToken();
        auto root = statement_sequence();
        if (token.tokenType != END_FILE) {
            ExceptionHandle::getHandle().add_exception(
                    ExceptionType::SYNTAX_ERROR, "Parser don't reach END_FILE finally");
        }
        if (TRACE_PARSER) {
            printTree(root);
        }
        return root;
    }

    TreeNode::ptr newStatementNode(StmtKind stmtKind) {
        auto n = std::make_shared<TreeNode>();
        n->lineNumber = Scanner::lineNumber;
        n->stmt_or_exp = StmtOrExp::StmtK;
        n->kind = stmtKind;
        return n;
    }

    TreeNode::ptr newExpressionNode(ExpKind expKind) {
        auto n = std::make_shared<TreeNode>();
        n->lineNumber = Scanner::lineNumber;
        n->stmt_or_exp = StmtOrExp::ExpK;
        n->kind = expKind;
        return n;
    }

    // 注意printTree的第一个参数不要用node&n,否则调用printTree(root)后root也被修改了.
    void printTree(TreeNode::ptr n, int tab_count) {
        while (n != nullptr) {
            for (int i = 0; i < tab_count; i++) {
                fprintf(OUTPUT_STREAM, "\t");
            }
            if (n->stmt_or_exp == StmtOrExp::StmtK) {
                switch (std::get<StmtKind>(n->kind)) {
                    case StmtKind::IfK:
                        fprintf(OUTPUT_STREAM, "If\n");
                        break;
                    case StmtKind::RepeatK:
                        fprintf(OUTPUT_STREAM, "Repeat\n");
                        break;
                    case StmtKind::AssignK:
                        fprintf(OUTPUT_STREAM, "Assign to ID : %s\n", get_attribute_string(n).c_str());
                        break;
                    case StmtKind::DeclarationK:
                        fprintf(OUTPUT_STREAM, "Declaration Type : %s\n", get_attribute_string(n).c_str());
                        break;
                    case StmtKind::VariableListK:
                        fprintf(OUTPUT_STREAM, "ID : %s\n", get_attribute_string(n).c_str());
                        break;
                    case StmtKind::ReadK:
                        fprintf(OUTPUT_STREAM, "Read : %s\n", get_attribute_string(n).c_str());
                        break;
                    case StmtKind::WriteK:
                        fprintf(OUTPUT_STREAM, "Write\n");
                        break;
                    case StmtKind::WhileK:
                        fprintf(OUTPUT_STREAM, "Do\n");
                        break;
                }
            } else if (n->stmt_or_exp == StmtOrExp::ExpK) {
                switch (std::get<ExpKind>(n->kind)) {
                    case ExpKind::OpK:
                        fprintf(OUTPUT_STREAM, "Op: %s\n", get_attribute_string(n).c_str());
                        break;
                    case ExpKind::ConstIntK:
                        fprintf(OUTPUT_STREAM, "ConstInt: %s\n", get_attribute_string(n).c_str());
                        break;
                    case ExpKind::ConstFloatK:
                        fprintf(OUTPUT_STREAM, "ConstFloat: %s\n", get_attribute_string(n).c_str());
                        break;
                    case ExpKind::ConstDoubleK:
                        fprintf(OUTPUT_STREAM, "ConstDouble: %s\n", get_attribute_string(n).c_str());
                        break;
                    case ExpKind::ConstBoolK:
                        fprintf(OUTPUT_STREAM, "ConstBool: %s\n", get_attribute_string(n).c_str());
                        break;
                    case ExpKind::ConstStringK:
                        fprintf(OUTPUT_STREAM, "ConstString: %s\n", get_attribute_string(n).c_str());
                        break;
                    case ExpKind::IdK:
                        fprintf(OUTPUT_STREAM, "Id: %s\n", get_attribute_string(n).c_str());
                        break;
                }
            } else {
                fprintf(OUTPUT_STREAM, "Unknown kind of tree node.\n");
            }
            for (auto &child:n->children) {
                printTree(child, tab_count + 1);
            }
            n = n->sibling;
        }
    }
}
