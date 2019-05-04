//
// Created by junior on 19-4-7.
//

#include "Compiler.h"
#include "FileUtil.h"
#include "Exception.h"
#include "SymbolTable.h"
#include "Scanner.h"
#include "Parser.h"
#include "Analyser.h"

namespace Compiler {
    FILE *file = nullptr;

    void compile(int n, char *argv[]) {
        using namespace Compiler::Exception;
        using namespace Compiler::FileUtil;
        using namespace Compiler::Scanner;
        using namespace Compiler::Parser;
        using namespace Compiler::Analyser;
        if (n < 2) {
            fprintf(stderr, "usage: %s <filename> <filename> ... <filename>\n", argv[0]);
            exit(1);
        }
        readFromFile(n - 1, argv + 1);
        for (auto &pair:files) {
            file = pair.second;
            node root = parse();
            if (!ExceptionHandle::getHandle().hasException()) { // 词法/语法没有错误才能继续语义分析
                analyse(root);
                if (ExceptionHandle::getHandle().hasException()) { // 语义错误输出
                    std::cout << "Process File " << pair.first << " has exceptions:\n" <<
                              ExceptionHandle::getHandle();
                    return;
                } else {
                    fprintf(stdout, "Process File %s success..\n", pair.first.c_str());
                }
            } else { // 词法和语法错误输出
                std::cout << "Process File " << pair.first << " has exceptions:\n" <<
                          ExceptionHandle::getHandle();
                return;
            }
            clearAll(); // Scanner clearAll
            if (fclose(file)) {
                fprintf(stderr, "Close File %s fail.\n", pair.first.c_str());
                exit(1);
            }
        }
    }
}
