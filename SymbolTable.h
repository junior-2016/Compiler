//
// Created by junior on 19-4-5.
//

#ifndef SCANNER_SYMBOLTABLE_H
#define SCANNER_SYMBOLTABLE_H

#include "Compiler.h"
#include "Exception.h"
#include "TypeSystem.h"
#include "Util.h"

namespace Compiler {
    /**
     * 单例模式,全局符号表
     */
    class SymbolTable {
    private:
        /**
          * 1. 储存一个变量的内存地址用什么类型? 参考:
          * https://stackoverflow.com/questions/13235280/how-to-store-a-memory-address-in-an-integer
          * 2. 哈希集set<entry>用entry的symbol_name作为key来插入和查找, 但是找到一个存在的entry时,有时候需要更新它的symbol_appear_lines.
          * 如果使用set.find(entry)就无法实现这种更新,因为set.find(entry)返回的是const iterator,无法修改内部成员.
          * C++设计的思路是不能破坏哈希集合内部的元素,因为哈希集的元素可能参与到hash()值的计算中,为了安全干脆一刀切.
          * 要解决这一点,有三种方法:
          * 1. 查找到一个元素后,将这个元素拷贝一份,修改这个拷贝,然后将原来的元素删除,再将拷贝插入哈希集合,这种开销太大;
          * 2. 将原来的哈希集合拆解变成哈希表,即 map<key,value>,然后将symbol_appear_lines,memory_address这些存在value里,这是比较好的重构思路.
          * 3. 在原来SymbolEntry结构体的 symbol_appear_lines　前加上 mutable(可变) 关键字, C++允许对const对象的mutable成员进行修改.
          * 这个方法可以不破坏原来的代码,而且明确了Entry的哪些元素可以修改
          * (这里只给symbol_appear_lines加上mutable,内存地址我认为第一次分配后就不需要改变了).
          * 参考: https://stackoverflow.com/questions/18704129/unordered-set-non-const-iterator
          */
        struct SymbolEntry {
            explicit SymbolEntry(string_ptr name) : symbol_name(std::move(name)) {}

            string_ptr symbol_name = nullptr;                 // 符号名称
            uintptr_t memory_address = 0;                     // 内存地址
            mutable std::list<int> symbol_appear_lines;       // 符号出现过的行号列表(加上mutable表示可变)
            Type type = Type::Void;                           // 类型信息
        };

        struct SymbolEntryHash {
            std::size_t operator()(const SymbolEntry &entry) const {
                return std::hash<string_t>()(*entry.symbol_name);
            }
        };

        struct SymbolEqual {
            bool operator()(const SymbolEntry &a, const SymbolEntry &b) const {
                return *(a.symbol_name) == *(b.symbol_name);
            }
        };

    public:
        static const uintptr_t null_address = std::numeric_limits<uintptr_t>::max();

    private:
        SymbolTable() = default;

        typedef std::unordered_set<SymbolEntry, SymbolEntryHash, SymbolEqual> symbol_table_t;
        symbol_table_t table;

    public:
        static SymbolTable &globalTable() {
            static SymbolTable symbolTable;
            return symbolTable;
        }

        SymbolTable(SymbolTable const &) = delete;

        void operator=(SymbolTable const &) = delete;

        /**
         * 遍历AST时,如果遇到其他使用symbol的statement或者expr,更新它的lineNumber.
         * 如果更新的时候发现symbol还没有插入符号表,则报符号未声明错误.
         * 比如下面的:
         * x := 5 (第一次出现x时并没有声明)
         * 会报未声明错误.
         */
        void update(const string_ptr &name, int lineNumber) {
            SymbolEntry search(name);
            symbol_table_t::iterator pos;
            if ((pos = table.find(search)) != table.end()) {
                (*pos).symbol_appear_lines.push_back(lineNumber);
            } else {
                using namespace Compiler::Exception;
                string_t message = "Symbol " + *name + " not declaration on line " + std::to_string(lineNumber);
                ExceptionHandle::getHandle().add_exception(ExceptionType::ANALYSIS_ERROR, message);
            }
        }

        /**
         * 遍历AST 遇到declaration_statement时调用.
         * 如果同一个symbol两次调用insert,将报重复定义错误.
         * 比如:
         * int a := 1;
         * double a := 1.2;
         * 会报重复定义错误
         */
        void insert(const string_ptr &name, int lineNumber, uintptr_t memory_address, Type type) {
            SymbolEntry search(name);
            if (table.find(search) == table.end()) {
                search.memory_address = memory_address;
                search.symbol_appear_lines.push_back(lineNumber);
                search.type = type;
                table.insert(search);
            } else {
                using namespace Compiler::Exception;
                string_t message = "Symbol " + *name + " declaration more than once on line "
                                   + std::to_string(lineNumber);
                ExceptionHandle::getHandle().add_exception(ExceptionType::ANALYSIS_ERROR, message);
            }
        }

        /**
         * 注意插入符号表的symbol都是非空类型的. 因为当前语言没有void关键字,不允许声明一个void类型的ID.
         * 如果返回空类型就说明查找的symbol不存在.
         */
        Type getSymbolType(const string_ptr &name) {
            symbol_table_t::iterator pos;
            if ((pos = table.find(SymbolEntry(name))) != table.end()) {
                return (*pos).type; // 必然返回非空类型
            }
            return Type::Void;
        }

        uintptr_t getSymbolAddress(const string_ptr &name) {
            symbol_table_t::iterator pos;
            if ((pos = table.find(SymbolEntry(name))) != table.end()) {
                return (*pos).memory_address;
            }
            return null_address;
        }

        friend std::ostream &operator<<(std::ostream &out, const SymbolTable &symbolTable) {
            out << "Variable_Name" << std::setw(20)
                << "Memory_Address" << std::setw(20)
                << "Data_Type" << std::setw(28)
                << "Appear_Line_Number" << "\n";
            for (auto &entry:symbolTable.table) {
                out << boost::format("%-20s 0x%08x %-12s %-20s")
                       % *entry.symbol_name % entry.memory_address % ""
                       % TypeSystem::getTypeRepresentation(entry.type);
                for (auto &line:entry.symbol_appear_lines) {
                    out << boost::format("%-8d") % line;
                }
                out << "\n";
            }
            return out;
        }
    };
}

#endif //SCANNER_SYMBOLTABLE_H
