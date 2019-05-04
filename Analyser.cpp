//
// Created by junior on 19-5-2.
//

#include "Analyser.h"
#include "SymbolTable.h"
#include "Exception.h"

namespace Compiler::Analyser {
    void build_symbol_table(Parser::node n) {

    }

    void check_type(Parser::node n) {

    }

    void analyse(Parser::node n) {
        build_symbol_table(n);
        if (TRACE_ANALYSER) {
            std::cout << SymbolTable::globalTable();
        }
        check_type(n);
    }
}