//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_SWAPQUERY_H
#define PROJECT_SWAPQUERY_H

#include "../Query.h"

class SwapQuery : public ComplexQuery {
    static constexpr const char *qname = "SWAP";
public:
    using ComplexQuery::ComplexQuery;

    QueryResult::Ptr execute() override;

    std::string toString() override;
};


#endif //PROJECT_SWAPQUERY_H
