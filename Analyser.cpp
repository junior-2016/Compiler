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
                switch (n->stmt_or_exp) {
                    case StmtOrExp::StmtK:
                        switch (std::get<StmtKind>(n->kind)) {
                            case StmtKind::DeclarationK:

                                break;
                            case StmtKind::AssignK:
                            case StmtKind::ReadK:

                                break;
                            default:
                                break;
                        }
                        break;
                    case StmtOrExp::ExpK:
                        switch (std::get<ExpKind>(n->kind)) {
                            case ExpKind::IdK:

                                break;
                            default:
                                break;
                        }
                        break;
                }
            }
        });
    }

    void check_type(const TreeNode::ptr &n) {

    }

    void analyse(const TreeNode::ptr &n) {
        build_symbol_table(n);
        if (TRACE_ANALYSER) {
            std::cout << SymbolTable::globalTable();
        }
        check_type(n);
    }
}