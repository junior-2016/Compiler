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

    node declaration_statement();

    node do_while_statement();

    node repeat_until_statement();

    node read_statement();

    node write_statement();

    /* expression */
    node expression();

    node logical_or_expression();

    node logical_and_expression();

    node equality_expression();

    node relational_expression();

    node arithmetic_additive_expression();

    node arithmetic_multiplicative_expression();

    node factor_expression();

    Scanner::TokenRet token;

    inline void match(TokenType target) {
        using namespace Compiler::Exception;
        if (token.tokenType == target) token = Scanner::getToken();
        else {
            std::string message = "match() unexpected token ["
                                  + getTokenRepresentation(token.tokenType, token.tokenString)
                                  + "], expected token ["
                                  + getTokenRepresentation(target, nullptr)
                                  + "] on line:"
                                  + std::to_string(Scanner::lineNumber);
            ExceptionHandle::getHandle().add_exception(ExceptionType::SYNTAX_ERROR, message);
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
        if (n != nullptr) n->children.push_back(expression());
        match(TokenType::THEN);
        if (n != nullptr) n->children.push_back(statement_sequence());
        if (token.tokenType == TokenType::ELSE) { // 这里必须写成 if(token_type is ELSE){ match (ELSE); ...},代表一种可选的推导分支
            match(TokenType::ELSE);
            if (n != nullptr) n->children.push_back(statement_sequence());
        }
        match(TokenType::END);
        return n;
    }

    /**
     * declaration_statement => Type ID := expression (对变量声明并且完成定义,不允许无初始化定义的声明)
     */
    node declaration_statement() {
        node n = newStatementNode(StmtKind::DeclarationK);
        if (n != nullptr) {
            n->attribute = token.tokenType;//这里要求当前token已经判定为INT/DOUBLE/FLOAT/BOOL/STRING
        }
        match(token.tokenType); // 必定命中
        if (token.tokenType == TokenType::ID) {
            n->children.push_back(assign_statement());
        }
        return n;
    }

    /**
     * assign_statement => ID := expression (赋值语句,对变量ID赋值之前必须先存在声明)
     */
    node assign_statement() {
        node n = newStatementNode(StmtKind::AssignK);
        if (n != nullptr && token.tokenType == TokenType::ID) {
            n->attribute = token.tokenString;
        }
        match(TokenType::ID);
        match(TokenType::ASSIGN);
        if (n != nullptr) n->children.push_back(expression());
        return n;
    }

    /**
     * do_while_statement => do statement_sequence while expression
     */
    node do_while_statement() {
        node n = newStatementNode(StmtKind::WhileK);
        match(TokenType::DO);
        if (n != nullptr) n->children.push_back(statement_sequence());
        match(TokenType::WHILE);
        if (n != nullptr) n->children.push_back(expression());
        return n;
    }

    /**
     * repeat_until_statement => repeat statement_sequence until expression
     */
    node repeat_until_statement() {
        node n = newStatementNode(StmtKind::RepeatK);
        match(TokenType::REPEAT);
        if (n != nullptr) n->children.push_back(statement_sequence());
        match(TokenType::UNTIL);
        if (n != nullptr) n->children.push_back(expression());
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
        if (n != nullptr) n->children.push_back(expression());
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
    node expression_template(const std::function<node()> &nextExpression, std::initializer_list<TokenType> tokenList) {
        node n = nextExpression();
        while (std::find(tokenList.begin(), tokenList.end(), token.tokenType) != tokenList.end()) {
            node p = newExpressionNode(ExpKind::OpK);
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

    /**
     * expression => logical_or_expression
     */
    node expression() {
        return logical_or_expression();
    }

    /**
     * logical_or_expression => logical_and_expression { or logical_and_expression }*

     */
    node logical_or_expression() {
        return expression_template(logical_and_expression, {TokenType::OR});
    }

    /**
     * logical_and_expression => equality_expression { and equality_expression }*
     */
    node logical_and_expression() {
        return expression_template(equality_expression, {TokenType::AND});
    }

    /**
     * equality_expression => relational_expression { = | !=  relational_expression}*
     */
    node equality_expression() {
        return expression_template(relational_expression, {TokenType::EQ, TokenType::NE});
    }

    /**
     * relational_expression => additive_expression { > | < | >= | <=  additive_expression } *
     */
    node relational_expression() {
        return expression_template(arithmetic_additive_expression,
                                   {TokenType::LT, TokenType::LE, TokenType::BT, TokenType::BE});
    }

    /**
     * additive_expression => multiplicative_expression { +|- multiplicative_expression }*
     */
    node arithmetic_additive_expression() {
        return expression_template(arithmetic_multiplicative_expression, {TokenType::PLUS, TokenType::MINUS});
    }

    /**
     *  multiplicative_expression => factor_expression { * | / | % factor_expression }*
     */
    node arithmetic_multiplicative_expression() {
        return expression_template(factor_expression, {TokenType::TIMES, TokenType::OVER, TokenType::MOD});
    }

    /**
     * factor_expression => not factor_expression | ID | NUM | STR | BOOL | ( expression )
     * 注意到: STR 不能参与到 arithmetic 或者 logical 运算中,只能在 assignment/declaration/write expression 这些出现.
     */
    node factor_expression() {
        node n = nullptr;
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
                            if (n != nullptr) n->attribute = std::stoi(*token.tokenString);// 16/10/8 进制字符串转换为整数
                            break;
                        case NUM_TYPE::FLOAT:
                            n = newExpressionNode(ExpKind::ConstFloatK);
                            if (n != nullptr) n->attribute = std::stof(*token.tokenString);
                            break;
                        case NUM_TYPE::DOUBLE:
                            n = newExpressionNode(ExpKind::ConstDoubleK);
                            if (n != nullptr) n->attribute = std::stod(*token.tokenString);
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
                using namespace Compiler::Exception;
                std::string message = "arithmetic_factor() unexpected token ["
                                      + getTokenRepresentation(token.tokenType, token.tokenString)
                                      + "] on line:" + std::to_string(Scanner::lineNumber);
                ExceptionHandle::getHandle().add_exception(ExceptionType::SYNTAX_ERROR, message);
                // token = Scanner::getToken(); // TODO: 不清楚这里是否需要前进一个Token
                break;
        }
        return n;
    }

    /**
     * version_1:
     * statement => if_else_stat | repeat_until_stat | do_while_stat | assign_stat | read_stat | write_stat
     *
     * version_2:
     * statement => if_else_stat | repeat_until_stat; | do_while_stat; | assign_stat; | read_stat; | write_stat;
     */
    node statement() {
        using namespace Compiler::Exception;
        node n = nullptr;
        switch (token.tokenType) {
            case TokenType::IF:
                n = if_else_statement();
                break;
            case TokenType::REPEAT:
                n = repeat_until_statement();
                match(TokenType::SEMI);
                break;
            case TokenType::DO:
                n = do_while_statement();
                match(TokenType::SEMI);
                break;
            case TokenType::ID:
                n = assign_statement();
                match(TokenType::SEMI);
                break;
            case TokenType::READ:
                n = read_statement();
                match(TokenType::SEMI);
                break;
            case TokenType::WRITE:
                n = write_statement();
                match(TokenType::SEMI);
                break;
            case TokenType::INT:
            case TokenType::FLOAT:
            case TokenType::DOUBLE:
            case TokenType::BOOL:
            case TokenType::STRING:
                n = declaration_statement();
                match(TokenType::SEMI);
                break;
            default:
                using namespace Compiler::Exception;
                std::string message = "statement() unexpected token ["
                                      + getTokenRepresentation(token.tokenType, token.tokenString)
                                      + "] on line:" + std::to_string(Scanner::lineNumber);
                ExceptionHandle::getHandle().add_exception(ExceptionType::SYNTAX_ERROR, message);
                // token = Scanner::getToken(); // TODO: 不清楚这里是否需要前进一个Token
                break;
        }
        return n;
    }

    /**
     * version_1:
     * statement_sequence -> statement { ; statement }*
     * 解析时
     *                statement_sequence
     *                       |
     *        stat ; stat ; stat; stat; ... ; stat (statement_sequence最后一个stat不需要分号)
     *
     * version_2 (为了让文法更接近C语言文法,第二个版本设计为: 除了if_else_statement,每一个statement后面都必须加一个分号.
     * 实现这一点需要将分号的match放在statement()函数里处理):
     * statement_sequence => { statement }+
     * statement => if_else_statement | other_statement ;
     *
     * 注意statement_sequence会被用在
     * program => statement_sequence [END_FILE]
     * if_else_statement => if expr then statement_sequence [else] statement_sequence [end]
     * do_while_statement => do statement_sequence [while] expr
     * repeat_until_statement => repeat statement_sequence [until] expr
     * 上面所有 statement_sequence 结束后紧接着出现的token有: END_FILE , else , end , while , until 几种类型,
     * 因此 statement_sequence()函数 在实现时要以这几种token作为退出循环的条件.
     */
    node statement_sequence() {
        node n = statement();
        node p = n;
        while (token.tokenType != TokenType::END_FILE &&
               token.tokenType != TokenType::ELSE &&
               token.tokenType != TokenType::END &&
               token.tokenType != TokenType::UNTIL &&
               token.tokenType != TokenType::WHILE) {
            // match(TokenType::SEMI); // version_1
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
     * program -> statement_sequence [END_FILE]
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
        node n = std::make_shared<TreeNode>();
        n->lineNumber = Scanner::lineNumber;
        n->stmt_or_exp = StmtOrExp::StmtK;
        n->kind = stmtKind;
        return n;
    }

    node newExpressionNode(ExpKind expKind) {
        node n = std::make_shared<TreeNode>();
        n->lineNumber = Scanner::lineNumber;
        n->stmt_or_exp = StmtOrExp::ExpK;
        n->kind = expKind;
        n->expType = ExpType::Void;
        return n;
    }

    void printTree(node n, int tab_count) {
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
                        fprintf(OUTPUT_STREAM, "Assign to : %s\n", std::get<string_ptr>(n->attribute)->c_str());
                        break;
                    case StmtKind::DeclarationK:
                        fprintf(OUTPUT_STREAM, "Declaration type : %s\n",
                                getTokenRepresentation(std::get<TokenType>(n->attribute), nullptr).c_str());
                        break;
                    case StmtKind::ReadK:
                        fprintf(OUTPUT_STREAM, "Read : %s\n", std::get<string_ptr>(n->attribute)->c_str());
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
                        fprintf(OUTPUT_STREAM, "Op: %s\n",
                                getTokenRepresentation(std::get<TokenType>(n->attribute), nullptr).c_str());
                        break;
                    case ExpKind::ConstIntK:
                        fprintf(OUTPUT_STREAM, "ConstInt: %d\n", std::get<int>(n->attribute));
                        break;
                    case ExpKind::ConstFloatK:
                        fprintf(OUTPUT_STREAM, "ConstFloat: %f\n", std::get<float>(n->attribute));
                        break;
                    case ExpKind::ConstDoubleK:
                        fprintf(OUTPUT_STREAM, "ConstDouble: %f\n", std::get<double>(n->attribute));
                        break;
                    case ExpKind::ConstBoolK:
                        switch (std::get<BOOL>(n->attribute)) {
                            case BOOL::TRUE:
                                fprintf(OUTPUT_STREAM, "ConstBool: true\n");
                                break;
                            case BOOL::FALSE:
                                fprintf(OUTPUT_STREAM, "ConstBool: false\n");
                                break;
                        }
                        break;
                    case ExpKind::ConstStringK:
                        fprintf(OUTPUT_STREAM, "ConstString: %s\n", std::get<string_ptr>(n->attribute)->c_str());
                        break;
                    case ExpKind::IdK:
                        fprintf(OUTPUT_STREAM, "Id: %s\n", std::get<string_ptr>(n->attribute)->c_str());
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
