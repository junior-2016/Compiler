//
// Created by junior on 19-4-7.
//

#ifndef SCANNER_FILEUTIL_H
#define SCANNER_FILEUTIL_H

#include "Compiler.h"
#include "Util.h"

namespace Compiler::FileUtil {
    // 这里强制用 extern 声明但是不对其定义(即不进行任何初始化).如果你直接写 std::vector<FILE*> files,
    // 那么它实际上已经构造初始化了.同样的,即使是int型,如果你写 int i,其实也是定义,并不是声明,
    // 必须写成 extern int i;才算对.

    // .h里只能是声明,定义全部放在 .cpp 里.
    // 如果你不这样弄,就会出现重复定义的编译错误.
    // 解释见博客 :
    // https://blog.csdn.net/u014357799/article/details/79121340.
    // https://blog.csdn.net/supervictim/article/details/50458259.

    extern std::vector<std::pair<string_t, FILE *>> files;

    void readFromFile(int fileCount, char *fileName[]);
}
#endif //SCANNER_FILEUTIL_H
