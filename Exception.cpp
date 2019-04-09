//
// Created by junior on 19-4-7.
//

#include "Exception.h"

namespace Compiler::Exception {
    void ExceptionHandle::add_exception(ExceptionType type, const std::string &message) {
        errors.push_back(ExceptionEntry{message, type});
    }

    std::map<ExceptionType, std::string> getExceptionTypeStrings() {
        static auto map = std::map<ExceptionType, std::string>
                {{ExceptionType::ID_REPEAT,    "ID_REPEAT"},
                 {ExceptionType::MATCH_ERROR,  "MATCH_ERROR"},
                 {ExceptionType::ILLEGAL_CHAR, "ILLEGAL_CHAR"},
                 {ExceptionType::SYNTAX_ERROR, "SYNTAX_ERROR"}};
        return map;
    }

    std::string printExceptionType(ExceptionType type) {
        auto map = getExceptionTypeStrings();
        std::map<ExceptionType, std::string>::iterator pos;
        if ((pos = map.find(type)) == map.end()) {
            return "UNKNOWN_EXCEPTION";
        } else {
            return (*pos).second;
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
