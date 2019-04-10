//
// Created by junior on 19-4-7.
//
#include "Scanner.h"
#include "StringLiteralPool.h"
#include "Exception.h"

namespace Compiler::Scanner {
    // DFA 状态
    typedef enum {
        START,
        INCOMMENT, // {comment}
        INSTR,    // 'str'
        INNUM,   // [0-9]*.[0-9]+ | [0-9]*
        INID,     // [a-z;A-z][0-9;A-Z;a-z]*
        INASSIGN, // :=
        INNE,     // !=
        INBE,     // >=
        INLE,     // <=
        DONE
    } State;

    char buffer[LINE_BUFFER_SIZE]; // 用buffer储存文件读取的每一行
    int bufSize = 0;    // 读取一行后实际的一行长度
    int lineNumber = 0; // 文件行数
    int pos = 0;        // 读取一行后,每一个字符的游标
    bool EOF_flag = false;

    int getNextChar() {
        if (pos >= bufSize) { // 读取新的一行
            lineNumber++;
            /* fgets(char* buffer,int buffer_size,FILE*) 从文件读取一行,
               这一行的数据包括换行符"\n"都会包括在缓存buffer里.*/
            if (fgets(buffer, LINE_BUFFER_SIZE - 1, Compiler::file)) {
                bufSize = (int) strlen(buffer);
                pos = 0;
                if (ECHO_SOURCE) { // 打印源码
                    if (buffer[bufSize - 1] == '\n')
                        fprintf(OUTPUT_STREAM, "%4d: %s", lineNumber, buffer);
                    else
                        fprintf(OUTPUT_STREAM, "%4d: %s\n", lineNumber, buffer);
                }
                return buffer[pos++];
            } else { // 读取一行失败,说明已经到文件尾
                if (ECHO_SOURCE)
                    fprintf(OUTPUT_STREAM, "%4d: EOF\n", lineNumber);
                EOF_flag = true;
                return EOF;    // 返回EOF字符
            }
        } else return buffer[pos++];
    }

    // 字符回退
    void undoGetNextChar() {
        if (!EOF_flag) pos--;
    }

    TokenRet getToken() {
        using namespace Compiler::Exception;
        std::string tokenString;
        TokenType currentToken = END_FILE;
        State state = START;
        bool saveTokenString;
        auto legalCharTable = getLegalCharTable();
        auto keyWordTable = getKeyWordTable();

        while (state != DONE) {
            int c = getNextChar();
            if (legalCharTable.find((char) c) == legalCharTable.end()) {
                if (state == INCOMMENT || state == INSTR) {
                    // pass,不处理
                } else {
                    std::string message = "LineNumber:" + std::to_string(lineNumber) + ",Pos:" +
                                          std::to_string(pos) + ",illegal char:" + char(c);
                    ExceptionHandle::getHandle().add_exception(ExceptionType::ILLEGAL_CHAR, message);
                    continue; // 跳过非法字符
                }
            }// 处理非法字符,直接跳过,在注释里或者字符串里的字符不管合不合法.
            saveTokenString = true;
            switch (state) {
                case START:
                    if (isdigit(c) || c == '.') {
                        state = INNUM;
                    } else if (isalpha(c)) {
                        state = INID;
                    } else if (c == ':') {
                        state = INASSIGN;
                    } else if (c == '!') {
                        state = INNE;
                    } else if (c == '>') {
                        state = INBE;
                    } else if (c == '<') {
                        state = INLE;
                    } else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                        saveTokenString = false;
                    } else if (c == '{') {
                        saveTokenString = false;
                        state = INCOMMENT;
                    } else if (c == '\'') {
                        saveTokenString = false; // str常量开头的'不记录在tokenString里
                        state = INSTR;
                    } else {
                        state = DONE;
                        switch (c) {
                            case EOF:
                                saveTokenString = false;
                                currentToken = END_FILE;
                                break;
                            case '=':
                                currentToken = EQ;
                                break;
                            case '+':
                                currentToken = PLUS;
                                break;
                            case '-':
                                currentToken = MINUS;
                                break;
                            case '*':
                                currentToken = TIMES;
                                break;
                            case '/':
                                currentToken = OVER;
                                break;
                            case '(':
                                currentToken = LPAREN;
                                break;
                            case ')':
                                currentToken = RPAREN;
                                break;
                            case ';':
                                currentToken = SEMI;
                                break;
                            case ',':
                                currentToken = COMMA;
                                break;
                            default:
                                currentToken = ERROR;
                                break;
                        }
                    }
                    break;
                case INCOMMENT:
                    saveTokenString = false;
                    if (c == '}') {
                        state = START;
                    } else if (c == EOF) {
                        // 处理不匹配的情况,在EOF时如果还没有结束注释(仍然处于INCOMMENT状态)就是不匹配
                        // (注释允许跨多行,因此可以一直判断到EOF)
                        state = DONE;
                        currentToken = END_FILE;
                        std::string message = "Comment match error on : LineNumber " + std::to_string(lineNumber);
                        ExceptionHandle::getHandle().add_exception(ExceptionType::MATCH_ERROR, message);
                    }
                    break;
                case INSTR:
                    if (c == '\'') {
                        saveTokenString = false; // 不记录str常量最后的'字符
                        state = DONE;
                        currentToken = STR;
                    } else if (c == EOF || c == '\n') {
                        // 处理不匹配情况,在一行结束后还不结束字符串就是不匹配.
                        saveTokenString = false;
                        state = DONE;
                        if (c == EOF) currentToken = END_FILE;
                        else currentToken = ERROR;
                        std::string message = "String match error on : LineNumber " + std::to_string(lineNumber);
                        ExceptionHandle::getHandle().add_exception(ExceptionType::MATCH_ERROR, message);
                    } else {
                        saveTokenString = true;
                    }
                    break;
                case INNUM:
                    if (!isdigit(c) && c != '.') {
                        undoGetNextChar();
                        saveTokenString = false;
                        state = DONE;
                        if (tokenString == "." ||
                            // 使用 std::count(begin,end,find_)可以非常容易得到一个find_在容器出现的次数..
                            std::count(tokenString.begin(), tokenString.end(), '.') > 1) {
                            currentToken = ERROR;
                        } else currentToken = NUM;
                    }
                    break;
                case INID:
                    if (!isalpha(c) && !isdigit(c)) {
                        undoGetNextChar();
                        saveTokenString = false;
                        state = DONE;
                        currentToken = ID;
                    }
                    break;
                case INASSIGN:
                    state = DONE;
                    if (c == '=') {
                        currentToken = ASSIGN;
                    } else {
                        undoGetNextChar();
                        saveTokenString = false;
                        currentToken = ERROR;
                    }
                    break;
                case INNE:
                    state = DONE;
                    if (c == '=') {
                        currentToken = NE;
                    } else {
                        undoGetNextChar();
                        saveTokenString = false;
                        currentToken = ERROR;
                    }
                    break;
                case INBE:
                    state = DONE;
                    if (c == '=') {
                        currentToken = BE;
                    } else {
                        undoGetNextChar();
                        saveTokenString = false;
                        currentToken = BT; // 只保留 > Token
                    }
                    break;
                case INLE:
                    state = DONE;
                    if (c == '=') {
                        currentToken = LE;
                    } else {
                        undoGetNextChar();
                        saveTokenString = false;
                        currentToken = LT; // 只保留 < Token
                    }
                    break;
                case DONE:
                    break;
            }
            if (saveTokenString) {
                tokenString += (char) c;
            }
            if (state == DONE) {
                if (currentToken == ID) {
                    auto key_ = keyWordTable.find(tokenString);
                    if (key_ != keyWordTable.end()) {
                        currentToken = (*key_).second;
                    }
                }
            }
        }
        if (TRACE_SCAN) {
            fprintf(OUTPUT_STREAM, "\t%d ", lineNumber);
            printToken(currentToken, tokenString);
        }

        std::shared_ptr<std::string> ptr;
        if (currentToken == STR) {
            ptr = StringLiteralPool::getInstance().getLiteralString(tokenString);
        } else if (currentToken == ID || currentToken == NUM) {
            ptr = std::make_shared<std::string>(tokenString);
        } else { // 其他类型的Token,比如关键字,特殊符号,END_FILE,ERROR等等,都不需要一个TokenString.
            ptr = nullptr;
        }
        return {lineNumber, currentToken, ptr};
    }

    void clearAll() {
        // 清空缓存,指示变量归零
        memset(buffer, '\0', LINE_BUFFER_SIZE);
        bufSize = 0;
        lineNumber = 0;
        pos = 0;
        EOF_flag = false;
    }
}
