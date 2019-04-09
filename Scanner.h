//
// Created by junior on 19-4-4.
//

#ifndef SCANNER_SCANNER_H
#define SCANNER_SCANNER_H

#include "Compiler.h"
#include "Token.h"

namespace Compiler::Scanner {

    typedef std::pair<TokenType, std::shared_ptr<std::string>> TokenRet;

    TokenRet getToken();

    void clearAll();
}
#endif //SCANNER_SCANNER_H
