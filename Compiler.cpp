//
// Created by junior on 19-4-7.
//

#include "Compiler.h"
#include "FileUtil.h"
#include "Exception.h"
#include "Scanner.h"

namespace Compiler {
    FILE *file = nullptr;

    void compile(int n, char *argv[]) {
        using namespace Compiler::Exception;
        using namespace Compiler::FileUtil;
        using namespace Compiler::Scanner;
        if (n < 2) {
            fprintf(stderr, "usage: %s <filename> <filename> ... <filename>\n", argv[0]);
            exit(1);
        }
        readFromFile(n - 1, argv + 1);
        for (auto &pair:files) {
            file = pair.second;
            while (getToken().first != END_FILE);
            if (ExceptionHandle::getHandle().hasException()) {
                std::cerr << "Process File " << pair.first << " has exceptions:\n" <<
                          ExceptionHandle::getHandle();
                return;
            } else {
                fprintf(stdout, "Process File %s success..\n", pair.first.c_str());
            }
            clearAll(); // Scanner clearAll
            if (fclose(file)) {
                fprintf(stderr, "Close File %s fail.\n", pair.first.c_str());
                exit(1);
            }
        }
    }
}
