//
// Created by liu on 18-10-25.
//

#include "AddQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"
#include <pthread.h>
#include <semaphore.h>
#include <algorithm>
#include "../../thread/MultiThread.h"

constexpr const char *AddQuery::qname;
static unsigned int thr_num;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static Table* table;
static int affected;
static ComplexQuery* qryptr; 
static unsigned int range;
static unsigned int thread_counter;
static std::vector<std::string> *operandptr;
static std::vector<QueryCondition> *conditionptr;
static int size;
static Table::FieldIndex destid; 
void* Helper(void*){
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
            auto temp = (*item)[table->getFieldIndex((*operandptr)[0])];
            for(int i = 1; i < size; i++){
                auto id =  table->getFieldIndex((*operandptr)[i]);
                temp += (*item)[id];
            }                        
            (*item)[destid] = temp;
        }
    }
    else{
        int aff_temp = 0;
        for(auto item = start; item != end; item ++ ){
            if(qryptr->evalCondition(*item)){
                aff_temp ++;
                auto temp = (*item)[table->getFieldIndex((*operandptr)[0])];
                for(int i = 1; i < size; i++){
                    auto id =  table->getFieldIndex((*operandptr)[i]);
                    temp += (*item)[id];
                }
                (*item)[destid] = temp;
            }
        }
        pthread_mutex_lock(&mutex);
        affected += aff_temp;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
QueryResult::Ptr AddQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        affected = 0;
        size = this->operands.size() - 1;
        if(size == 0) {
            throw IllFormedQuery("not enough operand");
        }
        table = &db[this->targetTable];
        destid = table->getFieldIndex(*(this->operands.end()-1));
        operandptr = &this->operands;
        thr_num = getThreadNum();
        conditionptr = &this->condition;
        if(this->condition.empty()) affected = table->size();
        auto result = this->initCondition(*table);
        if(!result.second) return std::make_unique<RecordCountResult>(0);
        if(thr_num < 2 || table->size() < 20){
            if(this->condition.empty()){
                for(auto item = table->begin(); item != table->end(); item ++ ) {
                    auto temp = (*item)[table->getFieldIndex(this->operands[0])];
                    for(int i = 1; i < size; i++){
                        auto id =  table->getFieldIndex(this->operands[i]);
                        temp += (*item)[id];
                    }                        
                    (*item)[destid] = temp;
                }
            }
            else{
                auto result = initCondition(*table);
                if(result.second){
                    for(auto item = table->begin(); item != table->end(); item ++ ){
                        if(this->evalCondition(*item)){
                            affected ++;
                            auto temp = (*item)[table->getFieldIndex(this->operands[0])];
                            for(int i = 1; i < size; i++){
                                auto id =  table->getFieldIndex(this->operands[i]);
                                temp += (*item)[id];
                            }
                            (*item)[destid] = temp;
                        }
                    }
                }
            }
        }
        else{
            qryptr = this;
            range = (table->size())/thr_num;
            pthread_t *t=new pthread_t[thr_num - 1];
            thread_counter = 0;
            for(int i = 0;i < (int)thr_num - 1; i++){
                pthread_create(t+i, NULL, Helper, &i);
            }
            Helper(NULL);
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

std::string AddQuery::toString() {
    return "QUERY = ADD " + this->targetTable + "\"";
}
