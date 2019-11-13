//
// Created by 60381 on 2019/10/19.
//

#include "MinQuery.h"
#include "../../db/Database.h"

#include <cstdlib>
#include <iostream>
constexpr const char *MinQuery::qname;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int threadNum;
static unsigned int range;
static unsigned int threadCounter;
static MinQuery *ptr;
static Table* table;
static bool exists;
static int opSize;
static std::vector<std::string> *operandptr;
void* MinQueryHelper(void* a)
{
    pthread_mutex_lock(&mutex);
    unsigned int curCount = threadCounter;
    threadCounter++;
    auto it = table->begin() + int(curCount*range);
    auto end = (curCount == threadNum - 1)? table->end() : it+int(range);
    for(;it != end;it++)
    {
        if(ptr->evalCondition(*it))
        {
            exists = true;
            for (int i = 0; i < opSize; i++)
            {
                Table::ValueType tmp = (*it)[table->getFieldIndex((*operandptr)[i])];
                if(tmp < ptr->_MIN[i])
                {
                    ptr->_MIN[i] = tmp;
                }
            }
        }
    }
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

QueryResult::Ptr MinQuery::execute()
{
    using namespace std;
    exists = false;
    Database &db = Database::getInstance();
    try
    {
        table = &db[this->targetTable];
        auto result = initCondition(*table);
        operandptr = &this->operands;
        opSize = int(this->operands.size());
        if (result.second)
        {
            for (int i = 0; i < opSize; i++)
            {
                _MIN.push_back(INT32_MAX);
            }
            if(threadNum <= 1 || table->size() < 20)
            {
                for (auto it = table->begin(); it != table->end(); it++)
                {
                    if (this->evalCondition(*it))
                    {
                        exists  = true;
                        for (int i = 0; i < opSize; i++)
                        {
                            Table::ValueType tmp = (*it)[table->getFieldIndex(this->operands[i])];
                            if(tmp < _MIN[i])
                            {
                                _MIN[i] = tmp;
                            }
                        }
                    }
                }
            }
            else
            {
                range = table->size() / threadNum;
                auto t = new pthread_t[threadNum - 1];
                threadCounter = 0;
                unsigned int i = 0;
                for(i = 0; i < threadNum - 1;i++)
                {
                    pthread_create(t+i,NULL,MinQueryHelper,&i);
                }
                MinQueryHelper(NULL);
                for(i = 0 ; i < threadNum - 1; i ++)
                {
                    pthread_join(*(t+i),NULL);
                }
                delete[] t;
            }
        }
        if(!exists)
            return make_unique<SuccessMsgResult>();
        else
            return make_unique<SuccessMsgResult>(_MIN);
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

std::string MinQuery::toString()
{
    std::string res = "Answer = ( ";
    for (const auto &val : _MIN)
    {
        res += (to_string(val) + " ");
    }
    res += ")";
    return res;
}
