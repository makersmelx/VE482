//
// Created by liu on 18-10-25.
//

#include "DuplicateQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include "../../thread/MultiThread.h"

#include <algorithm>
#include <pthread.h>
#include <iostream>

constexpr const char *DuplicateQuery::qname;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static Table* table;
static int affected;
static ComplexQuery* qryptr; 
static unsigned int dq_thread_num;
static unsigned int range;
static unsigned int thread_counter;
static std::vector<Table::KeyType> keys;

void * DuplicateQueryHelper(void *a){
    pthread_mutex_lock(&mutex);
    int i = thread_counter;
    thread_counter++;
    pthread_mutex_unlock(&mutex);
    //std::cerr << "start a new thread " << i << " range " << "range" << std::endl;
    auto start = table->begin() + (i * range);
    auto end = start;
    if(i == (int)dq_thread_num - 1)
        end = table->end();
    else
        end += range;
    int temp = 0;
    std::vector<Table::KeyType> tempkeys;
    for(;start != end;start++){
        if(qryptr->evalCondition(*start) && table->duplicateEval((*start).key())){
            tempkeys.push_back((*start).key());
            temp++;
        }
    }
    pthread_mutex_lock(&mutex);
    keys.insert(keys.end(), tempkeys.begin(), tempkeys.end());
    //std::cerr << temp <<std::endl;
    affected += temp;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

QueryResult::Ptr DuplicateQuery::execute() {
    using namespace std;
    if (! this->operands.empty())
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        table = &db[this->targetTable];
        affected = 0;
        auto result = initCondition(*table);
        qryptr = this;
        dq_thread_num = getThreadNum();
        keys.clear();

        if(result.second){
            if(dq_thread_num <= 1 || (int)table->size() <= 20){
                auto end = table->end();
                for(auto it = table->begin();it != end;it++){
                    if(this->evalCondition(*it) && table->duplicateEval((*it).key())){
                        affected++;
                        keys.push_back((*it).key());
                    }
                }
            } else {
                range = (table->size())/(dq_thread_num);
                pthread_t *t=new pthread_t[dq_thread_num - 1];
                thread_counter = 0;
                int i;
                for(i = 0;i < (int)dq_thread_num - 1;i++){
                    pthread_create(t+i, NULL, DuplicateQueryHelper, NULL);
                }
                DuplicateQueryHelper(NULL);
                for(i = 0;i < (int)dq_thread_num - 1;i++){
                    pthread_join(*(t+i), NULL);
                    //std::cerr << affected <<std::endl;
                }
                //pthread_exit(NULL);
                delete [] t;
            }
        }
        //int i = 1;
        //std::cerr<<"keys: "<<keys.size() << " a: " << affected<<endl;
        for(auto itt = keys.begin();itt != keys.end();itt++){
            //std::cerr<<i++<<endl;
            table->duplicateByIndex((*itt));
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

std::string DuplicateQuery::toString() {
    return "QUERY = DUPLICATE " + this->targetTable + "\"";
}
