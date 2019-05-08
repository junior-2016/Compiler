//
// Created by junior on 19-5-4.
//

#ifndef COMPILER_CODEGEN_H
#define COMPILER_CODEGEN_H

#include "Compiler.h"
#include "Parser.h"

namespace Compiler::CodeGen {
    using namespace Compiler::Parser;

    void code_generation(const TreeNode::ptr &root, const string_t &code_file_name);
}
#endif //COMPILER_CODEGEN_H
