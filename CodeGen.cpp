//
// Created by junior on 19-5-4.
//
#include "CodeGen.h"
#include "Code.h"

namespace Compiler::CodeGen {
    FILE *code_file = nullptr;


    void cGenExpr(const TreeNode::ptr &node) {

    }

    /**
     * 对于 declaration 语句 Type variable_list, 其中 variable_list => ID [:=expr]{,ID [:=expr]}*
     * 如果存在 ID := expr,则按照expr给ID分配初始值;
     * 如果只有一个 ID,则按照Declaration声明的Type给ID赋默认初始值:
     * 对于 Integer/Float/Double => 默认值为0; 对于 String => 默认值为 ""; 对于 Boolean => 默认值为false.
     */
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