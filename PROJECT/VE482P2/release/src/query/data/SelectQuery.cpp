//
// Created by liu on 18-10-25.
//

#include "SelectQuery.h"
#include "../../db/Database.h"
#include "../QueryResult.h"

#include <algorithm>
#include <iostream>

constexpr const char *SelectQuery::qname;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static Table* table;
static ComplexQuery* qryptr; 
static unsigned int sq_thread_num;
static unsigned int range;
static unsigned int thread_counter;
static std::vector<Table::KeyType> keys;
static std::vector<std::string> *fields;

void * SelectQueryHelper(void *a) {

    pthread_mutex_lock(&mutex);
    int i = thread_counter;
    thread_counter++;
    pthread_mutex_unlock(&mutex);

    auto start = table->begin() + (i * range);
    auto end = start;

    if(i == (int)sq_thread_num - 1)
        end = table->end();
    else
        end += range;

    std::vector<Table::KeyType> temp_keys;

    for(; start != end; start++){
        if(qryptr->evalCondition(*start)){
            temp_keys.push_back(start->key());
        }
    }

    pthread_mutex_lock(&mutex);
    keys.insert(keys.end(), temp_keys.begin(), temp_keys.end());
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void * SelectQueryOutputHelper(void *a) {

    pthread_mutex_lock(&mutex);
    int i = thread_counter;
    thread_counter++;
    pthread_mutex_unlock(&mutex);

    auto start = keys.begin() + (i * range);
    auto end = start;

    if(i == (int)sq_thread_num - 1)
        end = keys.end();
    else
        end += range;

    // std::vector<Table::KeyType> temp_keys;
    std::string temp;

    for(auto it = start; it != end; it++){
        for (auto fld = fields->begin(); fld != fields->end(); fld++) {
            if (fld == fields->begin()) {
                if ((*fld) == "KEY") {
                    temp = *it;
                    it->insert(0,"( ");
                    it->append(" ");
                } else {
                    exit(-1);
                }
                continue;
            }
            it->append(std::to_string((*table)[temp]->get(*fld)));
            it->append(" ");
        }
        it->append(")");
    }

    // pthread_mutex_lock(&mutex);
    // keys.insert(keys.end(), temp_keys.begin(), temp_keys.end());
    // pthread_mutex_unlock(&mutex);

    return NULL;
}

QueryResult::Ptr SelectQuery::execute() {

    using namespace std;

    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();

    // int row_length = (this->operands).size();
    // this -> key_names.clear();
    // this -> selections.clear();
    keys.clear();
    fields = &(this->operands);
    try {

        table = &db[this->targetTable];
        auto result = initCondition(*table);
        qryptr = this;
        sq_thread_num = getThreadNum();
        

        // store all keys that satisfy condition to keys
        if(result.second){

            if(sq_thread_num <= 1 || (int)table->size() <= 20){
                // single thread, or table is small.
                auto end = table->end();
                for(auto it = table->begin(); it != end; it++){
                    if(this->evalCondition(*it)){
                        keys.push_back((*it).key());
                    }
                }
            } else {
                // multithread
                range = (table->size())/(sq_thread_num);
                pthread_t *t=new pthread_t[sq_thread_num - 1];
                thread_counter = 0;
                int i;
                for(i = 0; i < (int)sq_thread_num - 1; i++){
                    pthread_create(t+i, NULL, SelectQueryHelper, &i);
                }
                SelectQueryHelper(NULL);
                for(i = 0; i < (int)sq_thread_num - 1; i++){
                    pthread_join(*(t+i), NULL);
                    //std::cerr << affected <<std::endl;
                }
                //pthread_exit(NULL);
                delete [] t;
            }
            
        }

        // std::cerr << "[DEBUG@SelectQuery] keys collected" << std::endl;


        // sort on keys by key
        std::sort(keys.begin(), keys.end());

        // std::cerr << "[DEBUG@SelectQuery] keys sorted\n" << std::endl;
        // for (auto it : keys) {
        //     std::cerr << it << " ";
        // }
        // std::cerr << std::endl;

        std::string temp;

        if(sq_thread_num <= 1 || (int)(keys.size()) <= 20) {
            for(auto it = keys.begin(); it != keys.end(); it++){
                // std::cerr << "[DEBUG@SelectQuery::execute()] Processing key: " << *it << std::endl;
                for (auto fld = fields->begin(); fld != fields->end(); fld++) {
                    if (fld == fields->begin()) {
                        if ((*fld) == "KEY") {
                            temp = *it;
                            it->insert(0,"( ");
                            it->append(" ");
                            // std::cerr << "[DEBUG@SelectQuery::execute()] KEY: " << *it << std::endl;
                            
                        } else {
                            return make_unique<ErrorMsgResult>(qname, this->targetTable, "First operand is not \"KEY\".");
                        }
                        continue;
                    }
                    // std::cerr << "[DEBUG@SelectQuery::execute()] fld = " << *fld << std::endl;
                    // std::cerr << static_cast <int> ((*table)[temp]->get(*fld)) << " ";
                    it->append(std::to_string((*table)[temp]->get(*fld)));
                    it->append(" ");
                    // std::cerr << "[DEBUG@SelectQuery::execute()] string modified to " << *it << std::endl;
                }
                it->append(")");
            }
        } else {
            range = (keys.size())/(sq_thread_num);
            pthread_t *t=new pthread_t[sq_thread_num - 1];
            thread_counter = 0;
            int i;
            for(i = 0; i < (int)sq_thread_num - 1; i++){
                pthread_create(t+i, NULL, SelectQueryOutputHelper, &i);
            }
            SelectQueryOutputHelper(NULL);
            for(i = 0; i < (int)sq_thread_num - 1; i++){
                pthread_join(*(t+i), NULL);
            }
            //pthread_exit(NULL);
            delete [] t;
        }

        // show data items
        
        // std::cerr << "[DEBUG@SelectQuery] out of the loop\n";

        return std::make_unique<SuccessMsgResult>(keys);
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

std::string SelectQuery::toString() {
    return "QUERY = SELECT " + this->targetTable + "\"";
}
