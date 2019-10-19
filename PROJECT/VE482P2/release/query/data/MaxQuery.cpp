//
// Created by 60381 on 2019/10/19.
//

#include "MaxQuery.h"
#include "../../db/Database.h"

#include <iostream>
#include <cstdlib>
constexpr const char *MaxQuery::qname;

QueryResult::Ptr MaxQuery::execute() {
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
                        Table::ValueType max = INT32_MIN;
                        for(auto itr = table.begin();itr!=table.end();itr++)
                        {
                            if(this->evalCondition(*itr))
                            {
                                Table::ValueType &tmp = itr->get(i);
                                if(tmp > max)
                                {
                                    max = tmp;
                                }
                            }

                        }
                        _MAX.push_back(max);
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

std::string MaxQuery::toString() {
    std::string res = "Answer = ( ";
    for(const auto  &val : _MAX)
    {
        res += (to_string(val) + " ");
    }
    res += ")";
    return res;
}
