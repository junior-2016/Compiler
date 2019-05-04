//
// Created by junior on 19-4-5.
//

#ifndef SCANNER_EXCEPTION_H
#define SCANNER_EXCEPTION_H

#include "Compiler.h"

namespace Compiler::Exception {
    enum class ExceptionType {
        LEXICAL_ERROR,
        // 词法错误 => 包括非法字符/字符串匹配错误/注释匹配错误

        SYNTAX_ERROR,
        // 语法错误 => 在parser.cpp里面收集各种语法错误

        ANALYSIS_ERROR
        // 分析错误,在Analyser模块处理, 包括:
        // 1. ID_REPEAT_DECLARATION,标识符重复定义.比如 int a:=1; int a:=3; double a:=1.23; 后面两个都要检测错误.
        // 2. ID_USE_BEFORE_DECLARATION,在定义标识符之前就使用这个标识符.比如 if x > 3 then ..., 但是x还没有定义.
        // (当遍历AST第一次遇到变量x的declaration语句时,符号表里面必须没有x的记录,否则报错;
        //  当其他语句比如if-else/while等遇到变量x时,它必须已经存在于符号表里面了,否则也要报错).
        // 3. TYPE_CHECK_ERROR,类型检查错误,这里主要是:
        // a. assign 时左右类型不匹配(如果是int,double,float之类的可以考虑隐式转换,但是string,bool就不允许)
        // b. 表达式计算类型不匹配,比如 not NUM, 或者 string > NUM,都是不允许的.
        // 第一个 not 后面的表达式必须是 BOOL类型; 第二个 string 与 NUM 之间不允许比较.等等...
    };

    struct ExceptionEntry {
        std::string message;
        ExceptionType type;
    };

    // 使用单例模式
    class ExceptionHandle {
    private:
        std::vector<ExceptionEntry> errors;

        ExceptionHandle() = default;

    public:
        static ExceptionHandle &getHandle();

    public:
        ExceptionHandle(ExceptionHandle const &) = delete;

        void operator=(ExceptionHandle const &) = delete;

        void add_exception(ExceptionType type, const std::string &message);

        friend std::ostream &operator<<(std::ostream &out, const ExceptionHandle &handle);

        bool hasException() const;
    };
}
#endif //SCANNER_EXCEPTION_H
