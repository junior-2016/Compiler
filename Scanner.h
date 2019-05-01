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
        string_ptr tokenString;
    };

    TokenRet getToken();

    void clearAll();
}
#endif //SCANNER_SCANNER_H
