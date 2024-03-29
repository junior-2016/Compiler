//
// Created by junior on 19-5-2.
//

#include "Analyser.h"
#include "SymbolTable.h"
#include "Exception.h"

namespace Compiler::Analyser {
    uintptr_t global_address = 0; // 用于分配内存地址

    /**
     * 遍历AST.
     * pre_proc: 先序处理节点的函数
     * post_proc: 后序处理节点的函数
     */
    void traverse_parser_tree(const TreeNode::ptr &n,
                              const std::function<void(TreeNode::ptr)> &pre_proc,
                              const std::function<void(TreeNode::ptr)> &post_proc) {
        if (n != nullptr) {
            pre_proc(n);
            for (const auto &child:n->children) {
                traverse_parser_tree(child, pre_proc, post_proc);
            }
            post_proc(n);
            traverse_parser_tree(n->sibling, pre_proc, post_proc);
        }
    }

    void pre_traverse_parser_tree(const TreeNode::ptr &n,
                                  const std::function<void(TreeNode::ptr)> &pre_proc) {
        traverse_parser_tree(n, pre_proc, [](TreeNode::ptr) { return; });
    }

    void post_traverse_parser_tree(const TreeNode::ptr &n,
                                   const std::function<void(TreeNode::ptr)> &post_proc) {
        traverse_parser_tree(n, [](TreeNode::ptr) { return; }, post_proc);
    }

    void build_symbol_table(const TreeNode::ptr &n) {
        pre_traverse_parser_tree(n, [](TreeNode::ptr n) {
            if (n != nullptr) {
                Type type;
                TreeNode::ptr p;
                switch (n->stmt_or_exp) {
                    case StmtOrExp::StmtK:
                        switch (std::get<StmtKind>(n->kind)) {
                            case StmtKind::DeclarationK:
                                type = TypeSystem::getTypeFromToken(std::get<TokenType>(n->attribute));
                                p = n->children.at(0); // declaration_statement的第一个children是variable_list.
                                while (p != nullptr) {
                                    SymbolTable::globalTable().insert(
                                            std::get<string_ptr>(p->attribute), p->lineNumber, global_address++, type);
                                    p = p->sibling;
                                }
                                break;
                            case StmtKind::AssignK:
                            case StmtKind::ReadK:
                                SymbolTable::globalTable().update(std::get<string_ptr>(n->attribute), n->lineNumber);
                                break;
                            default:
                                break;
                        }
                        break;
                    case StmtOrExp::ExpK:
                        switch (std::get<ExpKind>(n->kind)) {
                            case ExpKind::IdK:
                                SymbolTable::globalTable().update(std::get<string_ptr>(n->attribute), n->lineNumber);
                                break;
                            default:
                                break;
                        }
                        break;
                }
            }
        });
    }

    void report_analysis_error(const string_t &expr_name, int lineNumber) {
        using namespace Compiler::Exception;
        ExceptionHandle::getHandle().add_exception(ExceptionType::ANALYSIS_ERROR,
                                                   expr_name + " check type error on line " +
                                                   std::to_string(lineNumber));
    }

    // check_type 时已经确保符号表没有错误,即符号不会重定义,也不会在无定义的时候被使用.
    void check_type(const TreeNode::ptr &n) {
        post_traverse_parser_tree(n, [](TreeNode::ptr n) {
            constexpr size_t first_child = 0;
            constexpr size_t second_child = 1;
            Type t1, t2, temp;
            TreeNode::ptr ptr;
            if (n != nullptr) {
                switch (n->stmt_or_exp) {
                    case StmtOrExp::StmtK:
                        switch (std::get<StmtKind>(n->kind)) {
                            case StmtKind::DeclarationK:
                                // Declaration => Type variable_list;
                                // variable_list => ID[:=expr]{,ID[:=expr]}*
                                // 检查　variable_list 中 所有 ID:=expr 的 expr 是否与 Type 匹配.
                                // 1. Type为String的, 则 type(expr)==Type::String
                                // 2. Type为 Integer|Float|Double 的, 则 type(expr)==Type::Integer|Float|Double 都可以
                                // 注意这里即使expr类型是double,ID的type是int也没有关系,可以 int ID = int_cast(double_expr);
                                // 3. Type为Boolean 则 type(expr)==Type::Boolean.
                                temp = TypeSystem::getTypeFromToken(std::get<TokenType>(n->attribute));
                                ptr = n->children.at(first_child); // variable_list_node
                                while (ptr != nullptr) {
                                    if (!ptr->children.empty() && ptr->children.at(first_child) != nullptr) {
                                        t1 = ptr->children.at(first_child)->type; // expr存在并获取其类型
                                        if (temp == Type::String || temp == Type::Boolean) {
                                            if (t1 != temp) {
                                                report_analysis_error("variable_list statement", ptr->lineNumber);
                                            }
                                        } else if (temp == Type::Integer || temp == Type::Float ||
                                                   temp == Type::Double) {
                                            if (t1 != Type::Integer && t1 != Type::Float && t1 != Type::Double) {
                                                report_analysis_error("variable_list statement", ptr->lineNumber);
                                            }
                                        }
                                    }
                                    ptr = ptr->sibling;
                                }
                                break;
                            case StmtKind::AssignK:
                                t1 = n->children.at(first_child)->type;
                                temp = SymbolTable::globalTable().getSymbolType(std::get<string_ptr>(n->attribute));
                                if (temp == Type::String || temp == Type::Boolean) {
                                    if (t1 != temp) {
                                        report_analysis_error("assign statement", n->lineNumber);
                                    }
                                } else if (temp == Type::Integer || temp == Type::Float || temp == Type::Double) {
                                    if (t1 != Type::Integer && t1 != Type::Float && t1 != Type::Double) {
                                        report_analysis_error("assign statement", n->lineNumber);
                                    }
                                }
                                break;
                            case StmtKind::IfK:
                                if (n->children.at(first_child)->type != Type::Boolean) {
                                    report_analysis_error("if statement", n->lineNumber);
                                }
                                break;
                            case StmtKind::RepeatK:
                            case StmtKind::WhileK:
                                if (n->children.at(second_child)->type != Type::Boolean) {
                                    report_analysis_error("loop statement", n->lineNumber);
                                }
                                break;
                            case StmtKind::WriteK:
                                // 不限制输出类型,除了void
                                if (n->children.at(first_child)->type == Type::Void) {
                                    report_analysis_error("write statement", n->lineNumber);
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case StmtOrExp::ExpK:
                        switch (std::get<ExpKind>(n->kind)) {
                            case ExpKind::ConstIntK :
                                n->type = Type::Integer;
                                break;
                            case ExpKind::ConstBoolK :
                                n->type = Type::Boolean;
                                break;
                            case ExpKind::ConstFloatK :
                                n->type = Type::Float;
                                break;
                            case ExpKind::ConstDoubleK:
                                n->type = Type::Double;
                                break;
                            case ExpKind::ConstStringK :
                                n->type = Type::String;
                                break;
                            case ExpKind::IdK:
                                // 从符号表获取ID的类型,如果符号表没有ID的信息(ID没有正确声明)会返回void(空类型,实际上是语义错误的标志)
                                n->type = SymbolTable::globalTable().getSymbolType(std::get<string_ptr>(n->attribute));
                                break;
                            case ExpKind::OpK :
                                switch (std::get<TokenType>(n->attribute)) {
                                    case TokenType::NOT: //单目逻辑运算 NOT
                                        if (n->children.at(first_child)->type == Type::Boolean) {
                                            n->type = Type::Boolean;
                                        } else {
                                            n->type = Type::Void; // 设置类型为空,作为类型错标志
                                            report_analysis_error("logical-not expression", n->lineNumber);
                                        }
                                        break;

                                        //双目数值运算 PLUS MINUS TIMES OVER MOD (输入两个NUM,输出一个NUM)
                                    case TokenType::PLUS:
                                    case TokenType::MINUS:
                                    case TokenType::TIMES:
                                    case TokenType::OVER:
                                    case TokenType::MOD:
                                        t1 = n->children.at(first_child)->type, t2 = n->children.at(second_child)->type;
                                        if ((t1 == Type::Integer || t1 == Type::Float || t1 == Type::Double)
                                            &&
                                            (t2 == Type::Integer || t2 == Type::Float || t2 == Type::Double)) {
                                            /* type cast (往大的类型cast)
                                             *         int       float     double
                                             * int      I          F          D
                                             * float    F          F          D
                                             * double   D          D          D
                                             */
                                            if (t1 == Type::Double || t2 == Type::Double) {
                                                n->type = Type::Double;
                                            } else if (t1 == Type::Float || t2 == Type::Float) {
                                                n->type = Type::Float;
                                            } else { // t1 and t2 are both int
                                                n->type = Type::Integer;
                                            }
                                        } else {
                                            n->type = Type::Void;
                                            report_analysis_error("arithmetic expression", n->lineNumber);
                                        }
                                        break;

                                        // 双目逻辑运算 AND OR (输入两个bool,输出一个bool)
                                    case TokenType::AND:
                                    case TokenType::OR:
                                        t1 = n->children.at(first_child)->type, t2 = n->children.at(second_child)->type;
                                        if ((t1 == Type::Boolean) && (t2 == Type::Boolean)) {
                                            n->type = Type::Boolean;
                                        } else {
                                            n->type = Type::Void;
                                            report_analysis_error("logical-and-or expression", n->lineNumber);
                                        }
                                        break;

                                        // 双目比较运算 LT BT LE BE EQ NE (输入两个NUM,输出一个bool)
                                    case TokenType::LT:
                                    case TokenType::LE:
                                    case TokenType::BT:
                                    case TokenType::BE:
                                    case TokenType::EQ:
                                    case TokenType::NE:
                                        t1 = n->children.at(first_child)->type, t2 = n->children.at(second_child)->type;
                                        if ((t1 == Type::Integer || t1 == Type::Float || t1 == Type::Double)
                                            &&
                                            (t2 == Type::Integer || t2 == Type::Float || t2 == Type::Double)) {
                                            n->type = Type::Boolean;
                                        } else {
                                            n->type = Type::Void;
                                            report_analysis_error("comparison expression", n->lineNumber);
                                        }
                                        break;
                                    default:
                                        break;
                                }
                                break;
                        }
                        break;
                }
            }
        });
    }

    void analyse(const TreeNode::ptr &n) {
        build_symbol_table(n);
        if (TRACE_ANALYSER) {
            std::cout << SymbolTable::globalTable();
        }
        if (!Exception::ExceptionHandle::getHandle().hasException()) {
            check_type(n); // 如果建立符号表没有错误,才允许执行语义类型检查,否则就是浪费时间
        }
    }
}