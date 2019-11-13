//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_COPYTABLEQUERY_H
#define PROJECT_COPYTABLEQUERY_H


#include "../Query.h"

class CopyTableQuery : public Query {
    static constexpr const char *qname = "LOAD";
    const std::string new_table;
public:

    explicit CopyTableQuery(std::string old_table, std::string new_table) :
            Query(std::move(old_table)), new_table(std::move(new_table)) {}

    QueryResult::Ptr execute() override;

    std::string toString() override;

};


#endif //PROJECT_LOADTABLEQUERY_H
