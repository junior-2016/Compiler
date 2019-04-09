//
// Created by junior on 19-4-5.
//

#ifndef SCANNER_EXCEPTION_H
#define SCANNER_EXCEPTION_H

#include "Compiler.h"

namespace Compiler::Exception {
    typedef enum {
        ID_REPEAT,
        // 标识符重复定义,这个要鉴定有一定难度,需要考虑上下文.

        ILLEGAL_CHAR,
        // 非法字符,比如 $ 就是不属于当前编程语言的合法字符,要处理这个需要预先有一个合法字符表(用hash-set实现)

        MATCH_ERROR
        // 匹配错误,比如字符串'...匹配错误,或者注释{...匹配错误
    } ExceptionType;

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
