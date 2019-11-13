//
// Created by 吴佳遥 on 2019-10-19.
//

#ifndef LEMONDB_SUMQUERY_H
#define LEMONDB_SUMQUERY_H

#include "../Query.h"


class SumQuery : public ComplexQuery{
    static constexpr const char *qname = "SUM";

public:
    std::vector<Table::ValueType> _SUM;
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
};


#endif //LEMONDB_SUMQUERY_H
