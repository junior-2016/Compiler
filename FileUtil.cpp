//
// Created by junior on 19-4-7.
//

#include "FileUtil.h"

namespace Compiler::FileUtil {
    std::vector<std::pair<string_t, FILE *>> files;

    void readFromFile(int fileCount, char *fileName[]) {
        for (int i = 0; i < fileCount; i++) {
            FILE *file = fopen(fileName[i], "r");
            if (file == nullptr) {
                fprintf(stderr, "File %s not found!\n", fileName[i]);
                exit(1);
            }
            files.emplace_back(string_t(fileName[i]), file);
        }
    }

}