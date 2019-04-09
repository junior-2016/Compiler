//
// Created by junior on 19-4-5.
//

#ifndef SCANNER_SYMBOLTABLE_H
#define SCANNER_SYMBOLTABLE_H

#include "Token.h"

// 符号结构体
// 注意:符号Symbol就是ID(标识符)类型的Token,所以这里不需要单独储存其Token类型
struct Symbol {
    std::shared_ptr<std::string> name; // 标识符名称.

    /**
     TODO:
     add other attribute information :
     such as Constants, variables, functions, parameters, types, fields, etc
    */
    struct Attribute {
        int line_no;
        int line_loc;
    } attribute;
};

// 符号表实现用哈希表.
// 注意符号表只管理与标识符(ID型Token)有关的信息.
// 符号表需要在语法分析(parse)结束,得到AST后才能建立.
/**
 TODO:
 符号表在插入符号时如果发现符号(即标识符)名称一样,有几步需要处理:
 首先是判断此时符号在什么作用域,是在执行哪种行为(声明定义还是使用,注意利用后面的语法分析我们可以强制声明后必须定义),
 然后才能根据这些信息,判断可能是: 符号在同一个作用域重复定义了,需要提交异常(ID_REPEAT 异常);
 符号在不同的作用域里重新定义了,需要添加新的符号信息或者更新原来符号的属性信息.
 符号在同一作用域使用了,需要更新符号的属性信息等等...
*/
const size_t TableSize = 1021; // 必须用素数作为桶的大小.
class SymbolTable {
private:
    size_t count = 0;
    Symbol table[TableSize];
public:
    SymbolTable() {

    }
};

// 符号表需要同时被词法/语法/语义分析使用,所以用全局变量
SymbolTable table;

#endif //SCANNER_SYMBOLTABLE_H
