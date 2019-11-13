//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_UPDATEQUERY_H
#define PROJECT_UPDATEQUERY_H

#include "../Query.h"

class UpdateQuery : public ComplexQuery {
    static constexpr const char *qname = "UPDATE";

public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;
};


#endif //PROJECT_UPDATEQUERY_H
