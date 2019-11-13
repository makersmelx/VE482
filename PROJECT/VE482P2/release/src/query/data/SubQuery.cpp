//
// Created by liu on 18-10-25.
//

#include "SubQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>

constexpr const char *SubQuery::qname;
static unsigned int thr_num;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static Table* table;
static int affected;
static SubQuery* qryptr; 
static unsigned int range;
static unsigned int thread_counter;
static std::vector<std::string> *operandptr;
static std::vector<QueryCondition> *conditionptr;
static int size;
static Table::FieldIndex destid, srcid;
void* SubHelper(void*){
    pthread_mutex_lock(&mutex);
    int thread_id = thread_counter;
    thread_counter++;
    pthread_mutex_unlock(&mutex);
    auto start = table->begin() + (thread_id*range);
    auto end = start;
    if(thread_id == (int)thr_num - 1) end = table->end();
    else end = start + range;
    if(conditionptr->empty()){
        for(auto item = start; item != end; item ++ ) {
            int ans = (*item)[srcid];
            for(int i = 0; i < size; i++){
                auto id =  table->getFieldIndex((*operandptr)[i+1]);
                ans -= (*item)[id];
            }  
            (*item)[destid] = ans;                      
        }
    }
    else{
        int aff_temp = 0;

            for(auto item = start; item != end; item ++ ){
                if(qryptr->evalCondition(*item)){
                    aff_temp ++;
                    int ans = (*item)[srcid];
                    for(int i = 0; i < size; i++){
                        auto id =  table->getFieldIndex((*operandptr)[i+1]);
                        ans -= (*item)[id];
                    }
                    (*item)[destid] = ans;                      
                }
            }
        pthread_mutex_lock(&mutex);
        affected += aff_temp;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
QueryResult::Ptr SubQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        size = this->operands.size() - 2;
        if(size < 0) {
            throw IllFormedQuery("Operand must more than 1");
        }
        affected = 0;
        table = &db[this->targetTable];
        srcid =  table->getFieldIndex(this->operands[0]);       
        auto deststr = this->operands.end()-1;
        destid = table->getFieldIndex(*deststr);
        operandptr = &this->operands;
        conditionptr = &this->condition;
        thr_num = getThreadNum();
        thread_counter = 0;
        auto result = this->initCondition(*table);
        if(!result.second) return std::make_unique<RecordCountResult>(0);
        if(this->condition.empty()) affected = table->size();
        if(thr_num < 2 || table->size() < 20){
            if(this->condition.empty()){
                for(auto item = table->begin(); item != table->end(); item ++ ) {
                    int ans = (*item)[srcid];
                    for(int i = 0; i < size; i++){
                        auto id =  table->getFieldIndex((*operandptr)[i+1]);
                        ans -= (*item)[id];
                    }
                    (*item)[destid] = ans;                       
                }
            }
            else{
                auto result = initCondition(*table);
                if(result.second){
                    for(auto item = table->begin(); item != table->end(); item ++ ){
                        if(this->evalCondition(*item)){
                            affected ++;
                            int ans = (*item)[srcid];
                            for(int i = 0; i < size; i++){
                                auto id =  table->getFieldIndex((*operandptr)[i+1]);
                                ans -= (*item)[id];
                            }
                            (*item)[destid] = ans;
                        }
                    }
                }
            }
        }
        else{
            qryptr = this;
            range = (table->size())/thr_num;

            pthread_t *t=new pthread_t[thr_num - 1];
            for(int i = 0;i < (int)thr_num - 1; i++){
                pthread_create(t+i, NULL, SubHelper, &i);
            }
            SubHelper(NULL);
            for(int i = 0; i < (int)thr_num - 1; i++){
                pthread_join(*(t+i), NULL);
            }       
            delete [] t;
        }
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

std::string SubQuery::toString() {
    return "QUERY = Sub " + this->targetTable + "\"";
}
