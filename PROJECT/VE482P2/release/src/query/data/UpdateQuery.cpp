//
// Created by liu on 18-10-25.
//

#include "UpdateQuery.h"
#include "../../db/Database.h"

constexpr const char *UpdateQuery::qname;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static Table* table;
static UpdateQuery* qryptr; 
static unsigned int uq_thread_num;
static unsigned int range;
static unsigned int thread_counter;
static Table::SizeType counter;

static Table::ValueType fieldValue;// = (operands[0]=="KEY")? 0 :std::stoi(operands[1]);
static Table::FieldIndex fieldId;
static Table::KeyType keyValue;

void * UpdateQueryHelper(void *a) {

    pthread_mutex_lock(&mutex);
    int i = thread_counter;
    thread_counter++;
    pthread_mutex_unlock(&mutex);

    auto start = table->begin() + (i * range);
    auto end = start;

    if(i == (int)uq_thread_num - 1)
        end = table->end();
    else
        end += range;

    Table::SizeType temp_counter = 0;

    // do the real job
    for(; start != end; start++){
        if (qryptr->evalCondition(*start)) {
            if (keyValue.empty()) {
                (*start)[fieldId] = fieldValue;
            } else {
                start->setKey(keyValue);
            }
            ++temp_counter;
        }
    }

    pthread_mutex_lock(&mutex);
    counter += temp_counter;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

QueryResult::Ptr UpdateQuery::execute() {
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    counter = 0;
    try {
    	// initialize table, field (or key) and new value.
        table = &db[this->targetTable];
        if (this->operands[0] == "KEY") {
            keyValue = this->operands[1];
        } else {
            fieldId = table->getFieldIndex(this->operands[0]);
            fieldValue = (Table::ValueType) strtol(this->operands[1].c_str(), nullptr, 10);
        }
        // initialize multithread
        auto result = initCondition(*table);
        qryptr = this;
        uq_thread_num = getThreadNum();
        // execute
        
        if (result.second) {
        	if(uq_thread_num <= 1 || (int)table->size() <= 20) {
        		auto end = table->end();
        		for (auto it = table->begin(); it != end; ++it) {
	                if (this->evalCondition(*it)) {
	                    if (keyValue.empty()) {
	                        (*it)[fieldId] = fieldValue;
	                    } else {
	                        it->setKey(keyValue);
	                    }
	                    ++counter;
	                }
	            }
        	} else {
        		// multithread
                range = (table->size())/(uq_thread_num);
                pthread_t *t=new pthread_t[uq_thread_num - 1];
                thread_counter = 0;
                int i;
                for(i = 0; i < (int)uq_thread_num - 1; i++){
                    pthread_create(t+i, NULL, UpdateQueryHelper, &i);
                }
                UpdateQueryHelper(NULL);
                for(i = 0; i < (int)uq_thread_num - 1; i++){
                    pthread_join(*(t+i), NULL);
                    //std::cerr << affected <<std::endl;
                }
                //pthread_exit(NULL);
                delete [] t;
        	}

            
        }
        return make_unique<RecordCountResult>(counter);
    } catch (const TableNameNotFound &e) {
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

std::string UpdateQuery::toString() {
    return "QUERY = UPDATE " + this->targetTable + "\"";
}
