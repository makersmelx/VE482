//
// Created by liu on 18-10-25.
//

#include "SwapQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>

constexpr const char *SwapQuery::qname;
static unsigned int thr_num;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static Table* table;
static int affected;
static SwapQuery* qryptr; 
static unsigned int range;
static unsigned int thread_counter;
static std::vector<std::string> *operandptr;
static std::vector<QueryCondition> *conditionptr;
static Table::FieldIndex destid, srcid;
void* SwapHelper(void*){
    pthread_mutex_lock(&mutex);
    int thread_id = thread_counter;
    thread_counter++;
    pthread_mutex_unlock(&mutex);
    auto start = table->begin() + (thread_id*range);
    auto end = start;
    if(thread_id == (int)thr_num - 1) end = table->end();
    else end = start + range;
    if(conditionptr->empty()){
        for(auto item = start; item != end; item ++) {
                auto tmp = (*item)[srcid];
                (*item)[srcid] = (*item)[destid];
                (*item)[destid] = tmp;                        
        }
    }
    else{
        int aff_temp = 0;
        for(auto item = start; item != end; item ++ ){
            if(qryptr->evalCondition(*item)){
                aff_temp ++;
                auto tmp = (*item)[srcid];
                (*item)[srcid] = (*item)[destid];
                (*item)[destid] = tmp;
            }
        }

        pthread_mutex_lock(&mutex);
        affected += aff_temp;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
QueryResult::Ptr SwapQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
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
                    auto tmp = (*item)[srcid];
                    (*item)[srcid] = (*item)[destid];
                    (*item)[destid] = tmp;
                }
            }
            else{
                auto result = initCondition(*table);
                if(result.second){
                    for(auto item = table->begin(); item != table->end(); item ++ ){
                        if(this->evalCondition(*item)){
                            affected ++;
                            auto tmp = (*item)[srcid];
                            (*item)[srcid] = (*item)[destid];
                            (*item)[destid] = tmp;
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
                pthread_create(t+i, NULL, SwapHelper, &i);
            }
            SwapHelper(NULL);
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

std::string SwapQuery::toString() {
    return "QUERY = SWAP " + this->targetTable + "\"";
}
