//
// Created by liu on 18-10-25.
//

#include "CopyTableQuery.h"
#include "../../db/Database.h"

#include <fstream>

constexpr const char *CopyTableQuery::qname;

QueryResult::Ptr CopyTableQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        //db.printAllTable();
        auto tbl_old = db[this->targetTable];
        // somehiow get tbl_new
        // auto &&tbl_new = std::make_unique<Table>(new_table, tbl_old);
        db.registerTable(std::make_unique<Table>(new_table, tbl_old));
        //db.printAllTable();
        return make_unique<SuccessMsgResult>(qname, targetTable);
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, e.what());
    }
}

std::string CopyTableQuery::toString() {
    return "QUERY = Copy TABLE, OLD_TABLE = \"" + this->targetTable + "\", NEW_TABLE = \""
    + this->new_table + "\"";
}
