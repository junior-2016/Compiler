//
// Created by junior on 19-4-5.
//
/**
 * 对于数值来说，不需要常量池，因为数值本身就应该直接嵌入代码，包括最后生成的汇编代码，都是可以直接将数值作为立即数来计算的，
 * 甚至一个基本的优化是编译提前计算数值常量表达式，然后直接赋值，省掉运行开销，
 * 比如 int a=1+9/3，最后的汇编代码只会是int a=4的汇编，不需要运行时再计算，并且没有数字常量的需求。。
 *
 * 一般来说只有字符串常量池需要，因为字符串不定长且字符串是不可变对象，还有字符串重复利用可能性高，
 * 因此有必要做成常量池，token持有常量在池的地址即可。
 */

/**
 * 字符串常量池使用单例模式.
 * 字符串常量池在Scanner阶段就可以调用.
 */

#ifndef SCANNER_STRINGLITERALPOOL_H
#define SCANNER_STRINGLITERALPOOL_H

#include "Compiler.h"

namespace Compiler {
    struct stringLiteralHash {
        std::size_t operator()(const std::shared_ptr<std::string> &ptr) const {
            return std::hash<std::string>()(*ptr);
        }
    };

    struct stringLiteralEqual {
        bool operator()(const std::shared_ptr<std::string> &lhs, const std::shared_ptr<std::string> &rhs) const {
            return *lhs == *rhs;
        }
    };


    class StringLiteralPool {
    private:
        StringLiteralPool() = default;

        typedef std::unordered_set<std::shared_ptr<std::string>,
                stringLiteralHash, stringLiteralEqual> StringLiteralSet;

        StringLiteralSet set;

    public:
        static StringLiteralPool &getInstance() {
            static StringLiteralPool pool;
            return pool;
        }

    public:
        StringLiteralPool(StringLiteralPool const &) = delete;

        void operator=(StringLiteralPool const &) = delete;

        std::shared_ptr<std::string> getLiteralString(const std::string &string) {
            auto ptr = std::make_shared<std::string>(string);
            StringLiteralSet::iterator pos;
            if ((pos = set.find(ptr)) == set.end()) {
                return *set.insert(ptr).first;
            }
            return *pos;
        }
    };
}

#endif //SCANNER_STRINGLITERALPOOL_H
