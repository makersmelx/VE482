//
// Created by 60381 on 2019/10/19.
//

#include "MinQuery.h"
#include "../../db/Database.h"

#include <iostream>
#include <cstdlib>
constexpr const char *MinQuery::qname;

QueryResult::Ptr MinQuery::execute() {
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
                        Table::ValueType min = INT32_MAX;
                        for(auto itr = table.begin();itr!=table.end();itr++)
                        {
                            if(this->evalCondition(*itr))
                            {
                                Table::ValueType &tmp = itr->get(i);
                                if(tmp < min)
                                {
                                    min = tmp;
                                }
                            }

                        }
                        _MIN.push_back(min);
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

std::string MinQuery::toString() {
    std::string res = "Answer = ( ";
    for(const auto  &val : _MIN)
    {
        res += (to_string(val) + " ");
    }
    res += ")";
    return res;
}
