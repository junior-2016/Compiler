#include <utility>

#include <utility>

//
// Created by junior on 19-4-5.
//

#ifndef SCANNER_SYMBOLTABLE_H
#define SCANNER_SYMBOLTABLE_H

#include "Compiler.h"
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
            mutable std::list<int> symbol_appear_lines;       // 符号出现过的行号列表(可变)
            // struct typeinfo;                               // 类型信息
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

        void insert(const string_ptr &name, int lineNumber, uintptr_t memory_address) {
            SymbolEntry search(name);
            symbol_table_t::iterator pos;
            if ((pos = table.find(search)) == table.end()) {
                search.memory_address = memory_address;
                search.symbol_appear_lines.push_back(lineNumber);
                table.insert(search);
            } else {
                (*pos).symbol_appear_lines.push_back(lineNumber);
            }
        }

        uintptr_t lookup(const string_ptr &name) {
            symbol_table_t::iterator pos;
            if ((pos = table.find(SymbolEntry(name))) != table.end()) {
                return (*pos).memory_address;
            }
            return null_address;
        }

        friend std::ostream &operator<<(std::ostream &out, const SymbolTable &symbolTable) {
            out << "Variable_Name" << std::setw(20) << "Memory_Address"
                << std::setw(28) << "Appear_Line_Number" << "\n";
            for (auto &entry:symbolTable.table) {
                out << boost::format("%-20s 0x%08x %-12s") % *entry.symbol_name % entry.memory_address % "";
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
