//
// Created by junior on 19-4-9.
//

#ifndef SCANNER_PARSER_H
#define SCANNER_PARSER_H

#include "Compiler.h"
#include "Token.h"

namespace Compiler::Parser {
    /* 语法分析树节点类型 */
    enum class NodeKind {
        StmtK, ExpK
    };

    /* 语句类型 */
    enum class StmtKind {
        IfK, RepeatK, AssignK, ReadK, WriteK, WhileK
    };

    /* 表达式类型 */
    enum class ExpKind {
        OpK, ConstK, IdK
    };

    /* 类型检查 */
    enum class ExpType {
        Void, Integer, Boolean
    };

    typedef struct TreeNode *node;
    struct TreeNode {
        std::vector<node> childs;
        node sibling;
        int lineNumber;
        // 选择 nodeking => 选择 kind
        NodeKind nodeKind;
        union {
            StmtKind stmtKind;
            ExpKind expKind;
        } kind;
        union {
            TokenType op; // 符号型Token(运算,逻辑符号等)
            int i_val;    // int型
            float f_val;  // float型
            double d_val; // double型
            std::shared_ptr<std::string> name; // ID型Token
        } attribute; // 节点属性
        ExpType type; // for type checking
    };

    node parse();
}
#endif //SCANNER_PARSER_H
