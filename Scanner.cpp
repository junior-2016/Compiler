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
        INID,     // [a-z;A-z][0-9;A-Z;a-z]*
        INASSIGN, // :=
        INNE,     // !=
        INBE,     // >=
        INLE,     // <=
        /*
         * 下面的 DECIMAL,OCT,HEX,FLOAT 都会被解析为 NUM（数值常量型）Token.
         * 而不会分成四种Token来处理,原因很简单,在没有语法语义分析之前尚不能确定它们所指变量的实际类型,
         * 比如 int a = 1.23; 实际a的类型为int,只有后面语法语义分析的时候才能正确解析:
         * 在语法分析的时候,解析常量 1.23 的属性为 double ,然后储存在语法树节点里;
         * 在语义分析里,根据a的类型为int,选择将常量1.23转换为int型,再赋值给a,最后a的值是1.
         * 因此,数值常量都当做NUM型,都用字符串储存数值才是最佳选择,用字符串储存不会丢失任何精度信息,
         * 在后面语法分析设置语法树节点属性值(int/double/float)的时候也比较容易cast.
         *
         * TODO: 现在还有个负数常量没有解决,我之前考虑将所有的整型常量(16/10/8进制)都看成没有符号的(但是在语法树节点属性里依旧设置为int属性).
         *   实际处理负号(-)或者正号(+)是通过解析为运算符解决,但是运算符只能处理 NUM (+/- NUM)* 的情况, 如果是单独一个 (+/-)NUM
         *   就不行了. 这里考虑的解决方案是: 给正负运算规则加上 (+/-)NUM 的规则,在语义分析里,这个规则的语义是: 0 (+/-) NUM.
         */
        IN_DECIMAL,  // 十进制
        IN_OCT,     // 八进制
        IN_HEX,     // 十六进制
        IN_FLOAT,   // 浮点数值常量,包括双精度浮点常量和单精度浮点常量(最后带一个f或者F)
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
                    if (c == '.') {
                        state = IN_FLOAT;
                    } else if (c == '0') {
                        state = IN_OCT;
                    } else if (c >= '1' && c <= '9') {
                        state = IN_DECIMAL;
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
                case IN_DECIMAL:
                    if (!isdigit(c)) {
                        if (c == '.') state = IN_FLOAT;
                        else {
                            undoGetNextChar();
                            saveTokenString = false;
                            state = DONE;
                            currentToken = NUM;
                        }
                    }
                    break;
                case IN_FLOAT:
                    // 注意其他状态已经处理了第一次小数点的出现,所以这里只需要处理[小数部分]即可,一旦发现非数字就结束.
                    // 当然这样做在处理像 0.2555.23555.356.33 的时候会切成: 0.2555 .23555 .356 .33 四个NUM,
                    // 看上去好像是很不合理的,但是在词法分析这一轮我们不关心语法的问题,只要把词切出来即可,
                    // 后面语法分析很容易就可以分析出　NUM NUM NUM NUM 这样的句子是非法的.
                    // 另外这里有一些特殊情况需要处理:
                    // 比如.021或者12.这些值是允许的,可以认为是 0.021 以及 12.0, 但如果只有单独一个小数点,则是非法的Token
                    if (!isdigit(c)) {
                        if (tokenString == ".") {
                            undoGetNextChar();
                            saveTokenString = false;
                            currentToken = ERROR; // 只出现一个小数点,Token非法.
                        } else {
                            if (c != 'f' && c != 'F') {
                                // 如果是末尾带一个f或者F,不需要undoGetNextChar,同时需要把f/F附加到tokenString
                                undoGetNextChar();
                                saveTokenString = false;
                            }
                            currentToken = NUM;
                        }
                        state = DONE;
                    }
                    break;
                case IN_OCT:
                    if (!(c >= '0' && c <= '7')) {
                        if ((c == 'x' || c == 'X') && tokenString == "0") {
                            state = IN_HEX;  // 0x或者0X
                        } else if (c == '.') {
                            state = IN_FLOAT; // [0-7]+. 浮点数值
                        } else {
                            // 这里不需要对c=='8'或者c=='9'或者其他情况做错误分析处理,直接丢给下一个Token即可,
                            // 反正后面语法分析的时候很容易就可以发现这些错误了,
                            // 比如 int a := 0147999,切成 INT ID := NUM(0147) NUM(999),根据语法完全可以判断出错误.
                            undoGetNextChar();
                            saveTokenString = false;
                            state = DONE;
                            currentToken = NUM;
                            // 如果输入串只是一个"0",虽然是十进制,但Token类型依旧是NUM型,所以不用额外处理.
                        }
                    }
                    break;
                case IN_HEX:
                    if (!(isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                        if (tokenString == "0x" || tokenString == "0X") {
                            // 只有一个0x或者0X,是无法构成数值的,token为ERROR.
                            currentToken = ERROR;
                        } else {
                            currentToken = NUM;
                        }
                        undoGetNextChar();
                        saveTokenString = false;
                        state = DONE;
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
        return {currentToken, ptr};
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
