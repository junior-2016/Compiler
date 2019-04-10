//
// Created by junior on 19-4-9.
//

#include "Parser.h"
#include "Scanner.h"
#include "Exception.h"
#include "StringLiteralPool.h"

namespace Compiler::Parser {
    node stmt_sequence() {
        return nullptr;
    }

    node parse() {
        using namespace Compiler::Exception;
        auto token = Scanner::getToken();
        auto t = stmt_sequence();
        if (token.tokenType != END_FILE) {
            ExceptionHandle::getHandle().add_exception(ExceptionType::SYNTAX_ERROR, "Syntax Error");
        }
        return t;
    }
}
