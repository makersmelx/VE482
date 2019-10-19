//
// Created by 吴佳遥 on 2019-10-19.
//

#include "SumQuery.h"
#include "../../db/Database.h"

#include <iostream>
#include <cstdlib>
constexpr const char *SumQuery::qname;

QueryResult::Ptr SumQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        if(result.second)
        {
            for(const auto &str : this->operands)
            {
                for(unsigned long i = 0;i < table.field().size();i++)
                {
                    if(table.field()[i] == str)
                    {
                        Table::ValueType sum = 0;
                        for(auto itr = table.begin();itr!=table.end();itr++)
                        {
                            if(this->evalCondition(*itr))
                            {
                                Table::ValueType &tmp = itr->get(i);
                                sum += tmp;
                            }

                        }
                        _SUM.push_back(sum);
                        break;
                    }
                }
            }
            cout << this->toString() << endl;
        }
        return make_unique<SuccessMsgResult>(qname, this->targetTable);
    }
    catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "No such table."s);

    }
}

std::string SumQuery::toString() {
    std::string res = "Answer = ( ";
    for(const auto  &val : _SUM)
    {
        res += (to_string(val) + " ");
    }
    res += ")";
    return res;
}
