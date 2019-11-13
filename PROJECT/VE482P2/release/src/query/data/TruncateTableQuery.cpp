//
// Created by liu on 18-10-25.
//

#include "TruncateTableQuery.h"
#include "../../db/Database.h"

#include <fstream>
#include <iostream>

constexpr const char *TruncateTableQuery::qname;

QueryResult::Ptr TruncateTableQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        auto &tbl = db[this->targetTable];
        tbl.clear();
        //std::cout << "-------------------" << (int)tbl.size() << std::endl;
        if(!tbl.empty()) {
            exit(-1);
        }
        return make_unique<SuccessMsgResult>(qname, targetTable);
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, e.what());
    }
}

std::string TruncateTableQuery::toString() {
    return "QUERY = TRUNCATE TABLE, TABLE = \"" + this->targetTable + "\"";
}
