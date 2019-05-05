//
// Created by junior on 19-4-9.
//

#ifndef SCANNER_PARSER_H
#define SCANNER_PARSER_H

#include "Compiler.h"
#include "Token.h"
#include "Util.h"
#include "TypeSystem.h"

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
    enum class StmtOrExp {
        StmtK, ExpK
    };

    /* statement type 分类 */
    enum class StmtKind {
        IfK, RepeatK, AssignK, ReadK, WriteK, WhileK, DeclarationK
    };

    /* expression type 分类 */
    enum class ExpKind {
        OpK, ConstIntK, ConstFloatK, ConstDoubleK, ConstStringK, ConstBoolK, IdK
    };

    struct TreeNode {
    public:
        using ptr = std::shared_ptr<struct TreeNode>;
    public:
        std::vector<ptr> children;
        ptr sibling = nullptr;
        int lineNumber = 0;

        // select stmt_or_exp => select kind
        StmtOrExp stmt_or_exp;
        std::variant<StmtKind, ExpKind> kind;

        // 对于expression来说 => 后面遍历AST的时候才能得到表达式的类型;
        // 对于statement来说  => 只有Declaration-statement需要持有类型信息.
        Type type = Type::Void; // 默认空类型

    public:
        std::variant<
                /* null_t: variant.index()为0的占位空属性.
                 * 当语法错误发生在属性值时, 将导致attribute没有被设置任何确定的值.
                 * 此时attribute会默认其值为index=0的空属性.
                 * (实际写代码时,不需要显式地给attribute赋一个空属性,只需要在std::get<>的时候判断index()是不是0就行了)
                 */
                null_t,
                TokenType, /* 运算逻辑符号Token(operator)或者声明类型的Token(data type) */
                int_t,       /* 解析整型常量(10/16/8进制)的字符串为int类型的值,储存在节点属性值里 */
                float_t,     /* 解析单精度浮点常量(f/F结尾)的字符串为float类型的值,储存在节点属性值里 */
                double_t,    /* 解析双精度浮点常量的字符串为double类型的值,储存在节点属性值里 */
                bool_t,      /* 解析bool类型的值,只有true,false两种取值 */
                string_ptr   /* ID型Token的名称 或者 字符串常量 => 通过ExpKind区分 */
        > attribute; // 节点属性
    };

    /**
     * 判断一个节点的属性值是否为空(即没有被正确设置)
     */
    inline bool is_attribute_null(const TreeNode::ptr &n) {
        return n->attribute.index() == 0;
    }

    inline std::string get_attribute_string(const TreeNode::ptr &n) {
        if (is_attribute_null(n)) return "null_attribute";
        try {
            switch (n->attribute.index()) {
                case 1:
                    return getTokenRepresentation(std::get<TokenType>(n->attribute), nullptr);
                case 2:
                    return std::to_string(std::get<int_t>(n->attribute));
                case 3:
                    return std::to_string(std::get<float_t>(n->attribute));
                case 4:
                    return std::to_string(std::get<double_t>(n->attribute));
                case 5:
                    switch (std::get<bool_t>(n->attribute)) {
                        case BOOL::TRUE:
                            return "true";
                        case BOOL::FALSE:
                            return "false";
                        default:
                            return "";
                    }
                case 6:
                    return *std::get<string_ptr>(n->attribute);
            }
        } catch (std::bad_variant_access &error) {
            std::cerr << "get_attribute_string() exception: " << error.what();
        }
        return "";
    }

    TreeNode::ptr parse();

    void printTree(TreeNode::ptr n, int tab_count = 0);
}
#endif //SCANNER_PARSER_H
