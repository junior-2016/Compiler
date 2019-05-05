//
// Created by junior on 19-5-2.
//

#ifndef COMPILER_ANALYSER_H
#define COMPILER_ANALYSER_H

#include "Compiler.h"
#include "Parser.h"
#include "Util.h"

namespace Compiler::Analyser {
    using namespace Compiler::Parser;

    void analyse(const TreeNode::ptr &n);
}
#endif //COMPILER_ANALYSER_H
