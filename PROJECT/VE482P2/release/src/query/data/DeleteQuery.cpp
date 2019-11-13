//
// Created by liu on 18-10-25.
//

#include "DeleteQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>
#include <iostream>

constexpr const char *DeleteQuery::qname;

QueryResult::Ptr DeleteQuery::execute() {
    using namespace std;
    if (! this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        Table &table = db[this->targetTable];

        int affected = 0;
        if(this->condition.empty()){
            affected = (int) table.clear();
            return std::make_unique<RecordCountResult>(affected);
        }

        auto result = initCondition(table);
        int fwd = 0;
        std::vector<Table::KeyType> keys;

        if(result.second){
            for(auto it = table.begin();it != table.end();it++){
                if(this->evalCondition(*it)){
                    affected++;
                    fwd++;
                    keys.push_back((*it).key());
                    table.prepareToDelete((*it).key());
                } else{
                    table.fwdByIndex((*it).key(), fwd);
                }
            }
        }
        //std::cerr<<"start deleting"<<std::endl;



        if(keys.empty())
            return std::make_unique<RecordCountResult>(0);

        table.startDelete();

        auto ittt = keys.end();
        ittt--;
        for(auto itt = ittt;itt != keys.begin();itt--){
            
            table.deleteByIndex(*itt);
        }
        table.deleteByIndex(*(keys.begin()));
        
        return std::make_unique<RecordCountResult>(affected);
    }
    catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
    } catch (const invalid_argument &e) {
        // Cannot convert operand to string
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unknown error '?'"_f % e.what());
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unkonwn error '?'."_f % e.what());
    }
}

std::string DeleteQuery::toString() {
    return "QUERY = DELETE " + this->targetTable + "\"";
}
