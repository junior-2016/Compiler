//
// Created by junior on 19-4-9.
//

#ifndef SCANNER_PARSER_H
#define SCANNER_PARSER_H

#include "Compiler.h"
#include "Token.h"
/**
 * 基于 LL(1) 文法的手写递归下降语法分析器. LL(1)文法也就是 backtracking-free 文法(无需递归后回溯搜索)
 * LL(1) 文法(left to right scan source & left most derivation & lookahead 1 token)的基本要求:
 * 1. 消除语法的二义性,即不能拥有两个最左推导或者两个最右推导(要求语法树的唯一性);
 * 2. 消除左递归以及间接的左递归;
 * 3. 消除语法的左因子,比如　S -> aB | aC | aD ,需要消除左因子a,变成 S->aE ; E->B|C|D. 这样就可以避免回溯.
 * 4. 一个确定性LL(1)文法的最终结果是,对于 V->P1|P2|..., 其中任意两个产生式 V->Pi 和 V->Pj 的 First+ 集的交集为空.
 * 除了上面三个要求,还有优先级顺序要求,优先级按照常见顺序(比如先计算乘法再计算加法等等),
 * 先推导出低优先级的产生式,最后再推导高优先级的产生式.
 */
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
