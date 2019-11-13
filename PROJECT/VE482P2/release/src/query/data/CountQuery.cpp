//
// Created by liu on 18-10-25.
//

#include "CountQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include "../../thread/MultiThread.h"

#include <algorithm>
#include <pthread.h>
#include <iostream>

constexpr const char *CountQuery::qname;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static Table* table;
static int affected;
static ComplexQuery* qryptr; 
static unsigned int cq_thread_num;
static unsigned int range;
static unsigned int thread_counter;

void * CountQueryHelper(void *a) {
    pthread_mutex_lock(&mutex);
    int i = thread_counter;
    thread_counter++;
    pthread_mutex_unlock(&mutex);
    //std::cerr << "start a new thread " << i << " range " << "range" << std::endl;
    auto start = table->begin() + (i * range);
    auto end = start;
    if(i == (int)cq_thread_num - 1)
        end = table->end();
    else
        end += range;
    int temp = 0;
    for(;start != end;start++){
        if(qryptr->evalCondition(*start)){
            temp++;
        }
    }
    pthread_mutex_lock(&mutex);
    affected += temp;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

QueryResult::Ptr CountQuery::execute() {
    using namespace std;
    if (! this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        table = &(db[this->targetTable]);
        affected = 0;
        auto result = this->initCondition(*table);
        qryptr = this;
        cq_thread_num = getThreadNum();

        if(this->condition.empty()){
            affected = (int) table->size();
            return std::make_unique<SuccessMsgResult>(affected);
        }

        if(result.second){
            if(cq_thread_num <= 1 || (int)table->size() <= 20){
                auto end = table->end();
                for(auto it = table->begin();it != end;it++){
                    if(qryptr->evalCondition(*it)){
                        affected++;
                    }
                }
            } else {
                range = (table->size())/(cq_thread_num);
                pthread_t *t=new pthread_t[cq_thread_num - 1];
                thread_counter = 0;
                int i;
                for(i = 0;i < (int)cq_thread_num - 1;i++){
                    pthread_create(t+i, NULL, CountQueryHelper, &i);
                }
                CountQueryHelper(NULL);
                for(i = 0;i < (int)cq_thread_num - 1;i++){
                    pthread_join(*(t+i), NULL);
                    //std::cerr << affected <<std::endl;
                }
                //pthread_exit(NULL);
                delete [] t;
            }
            
        }
        return std::make_unique<SuccessMsgResult>(affected);
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

std::string CountQuery::toString() {
    return "QUERY = COUNT " + this->targetTable + "\"";
}
