//
// Created by junior on 19-4-4.
//

#ifndef SCANNER_SCANNER_H
#define SCANNER_SCANNER_H

#include "Compiler.h"
#include "Token.h"

namespace Compiler::Scanner {

    extern int lineNumber;

    struct TokenRet {
        TokenType tokenType;
        std::shared_ptr<std::string> tokenString;
    };

    TokenRet getToken();

    void clearAll();
}
#endif //SCANNER_SCANNER_H
