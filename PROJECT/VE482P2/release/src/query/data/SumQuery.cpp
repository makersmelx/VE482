
#include "SumQuery.h"
#include "../../db/Database.h"

#include <cstdlib>
#include <iostream>
constexpr const char *SumQuery::qname;
//multi-thread
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int threadNum;
static unsigned int range;
static unsigned int threadCounter;
//Query pointer
static SumQuery *ptr;
//Table pointer
static Table* table;
static int opSize;
static std::vector<std::string> *operandptr;
void* SumQueryHelper(void*)
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
            for (int i = 0; i < opSize; i++)
            {
                ptr->_SUM[i] += (*it)[table->getFieldIndex((*operandptr)[i])];
            }
        }
    }
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

QueryResult::Ptr SumQuery::execute()
{
    using namespace std;
    Database &db = Database::getInstance();
    try
    {
        table = &db[this->targetTable];
        auto result = initCondition(*table);
        operandptr = &this->operands;
        opSize = int(this->operands.size());
        for (int i = 0; i < opSize; i++)
        {
            _SUM.push_back(0);
        }
        if (result.second)
        {
            if(threadNum <= 1 || table->size() < 20)
            {
                for (auto it = table->begin(); it != table->end(); it++)
                {
                    if (this->evalCondition(*it))
                    {
                        for (int i = 0; i < opSize; i++)
                        {
                            _SUM[i] += (*it)[table->getFieldIndex(this->operands[i])];
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
                    pthread_create(t+i,NULL,SumQueryHelper,&i);
                }
                SumQueryHelper(NULL);
                for(i = 0 ; i < threadNum - 1; i ++)
                {
                    pthread_join(*(t+i),NULL);
                }
                delete[] t;
            }
        }
        return make_unique<SuccessMsgResult>(_SUM);
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

std::string SumQuery::toString()
{
    std::string res = "Answer = ( ";
    for (const auto &val : _SUM)
    {
        res += (to_string(val) + " ");
    }
    res += ")";
    return res;
}
