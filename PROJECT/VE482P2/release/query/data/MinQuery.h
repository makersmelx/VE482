//
// Created by 60381 on 2019/10/19.
//

#ifndef LEMONDB_MINQUERY_H
#define LEMONDB_MINQUERY_H

#include "../Query.h"

class MinQuery: public ComplexQuery{
    static constexpr const char *qname = "MIN";
    std::vector<Table::ValueType> _MIN;
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};



#endif //LEMONDB_MINQUERY_H
