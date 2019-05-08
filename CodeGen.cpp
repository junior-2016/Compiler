//
// Created by junior on 19-5-4.
//
#include "CodeGen.h"
#include "Code.h"

namespace Compiler::CodeGen {
    FILE *code_file = nullptr;


    void cGenExpr(const TreeNode::ptr &node) {

    }

    void cGenStmt(const TreeNode::ptr &node) {

    }

    void cGen(const TreeNode::ptr &node) {
        if (node != nullptr) {
            switch (node->stmt_or_exp) {
                case StmtOrExp::StmtK:
                    cGenStmt(node);
                    break;
                case StmtOrExp::ExpK:
                    cGenExpr(node);
                    break;
            }
            cGen(node->sibling);
        }
    }

    void code_generation(const TreeNode::ptr &root, const string_t &code_file_name) {
        code_file = fopen(code_file_name.c_str(), "w");
        if (code_file == nullptr) {
            fprintf(stderr, "can't open file %s\n", code_file_name.c_str());
            return;
        }
        cGen(root);
        fclose(code_file);
    }
}