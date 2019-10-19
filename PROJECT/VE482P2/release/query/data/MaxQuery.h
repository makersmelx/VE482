//
// Created by 60381 on 2019/10/19.
//

#ifndef LEMONDB_MAXTABLEQUERY_H
#define LEMONDB_MAXTABLEQUERY_H

#include "../Query.h"

class MaxQuery: public ComplexQuery{
    static constexpr const char *qname = "MAX";
    std::vector<Table::ValueType> _MAX;
public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};

#endif //LEMONDB_MAXTABLEQUERY_H
