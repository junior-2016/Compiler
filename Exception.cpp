//
// Created by junior on 19-4-7.
//

#include "Exception.h"

namespace Compiler::Exception {
    void ExceptionHandle::add_exception(ExceptionType type, const std::string &message) {
        errors.push_back(ExceptionEntry{message, type});
    }

    std::string printExceptionType(ExceptionType type) {
        switch (type) {
            case ID_REPEAT:
                return "ID_REPEAT";
            case MATCH_ERROR:
                return "MATCH_ERROR";
            case ILLEGAL_CHAR:
                return "ILLEGAL_CHAR";
            default:
                return "Unknown_Exception";
        }
    }

    std::ostream &operator<<(std::ostream &out, const ExceptionHandle &handle) {
        out << "\t ExceptionType \t ExceptionMessage \n";
        for (auto &exception:handle.errors) {
            out << "\t " << printExceptionType(exception.type) << " \t " << exception.message << " \n";
        }
        return out;
    }

    bool ExceptionHandle::hasException() const {
        return !errors.empty();
    }

    ExceptionHandle &ExceptionHandle::getHandle() {
        static ExceptionHandle handle;
        return handle;
    }
}
