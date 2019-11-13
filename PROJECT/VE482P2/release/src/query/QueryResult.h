//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_QUERYRESULT_H
#define PROJECT_QUERYRESULT_H

#include "../utils/formatter.h"
#include "../db/Table.h"

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

class QueryResult {
public:
    typedef std::unique_ptr<QueryResult> Ptr;

    virtual bool success() = 0;

    virtual bool display() = 0;

    virtual ~QueryResult() = default;

    friend std::ostream &operator<<(std::ostream &os, const QueryResult &table);

protected:
    virtual std::ostream &output(std::ostream &os) const = 0;
};

class FailedQueryResult : public QueryResult {
    const std::string data;
public:
    bool success() override { return false; }

    bool display() override { return false; }
};

class SuceededQueryResult : public QueryResult {
public:
    bool success() override { return true; }
};

class NullQueryResult : public SuceededQueryResult {
public:
    bool display() override { return false; }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "\n";
    }
};

class ErrorMsgResult : public FailedQueryResult {
    std::string msg;
public:
    bool display() override { return false; }

    ErrorMsgResult(const char *qname,
                   const std::string &msg) {
        this->msg = R"(Query "?" failed : ?)"_f % qname % msg;
    }

    ErrorMsgResult(const char *qname,
                   const std::string &table,
                   const std::string &msg) {
        this->msg =
                R"(Query "?" failed in Table "?" : ?)"_f % qname % table % msg;
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << msg << "\n";
    }
};

class SuccessMsgResult : public SuceededQueryResult {
    std::string msg;
    bool ifdisplay = false;
public:
    bool display() override { return ifdisplay; }

    explicit SuccessMsgResult() {}
    explicit SuccessMsgResult(const int number) {
        this->ifdisplay = true;
        this->msg = R"(ANSWER = ?)"_f % number;
    }

    explicit SuccessMsgResult(std::vector<int> results) {
        this->ifdisplay = true;
        std::stringstream ss;
        ss << "ANSWER = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ") ";
        this->msg = ss.str();
    }

    explicit SuccessMsgResult(const char *qname) {
        this->msg = R"(Query "?" success.)"_f % qname;
    }

    SuccessMsgResult(const char *qname, const std::string &msg) {
        this->msg = R"(Query "?" success : ?)"_f % qname % msg;
    }

    SuccessMsgResult(const char *qname,
                     const std::string &table,
                     const std::string &msg) {
        this->msg = R"(Query "?" success in Table "?" : ?)"_f
                    % qname % table % msg;
    }

    // SuccessMsgResult(int row_length, std::vector<std::string> &key_names, std::vector<int> &selections) {
    //     this->ifdisplay = true;
    //     auto it_kn = key_names.begin();
    //     int i = 0;
    //     std::stringstream ss;
    //     for (auto it = selections.begin(); it != selections.end(); it++) {
    //         if (i % row_length == 0) {
    //             ss << "( ";
    //             ss << *it_kn << " ";
    //             it_kn++;
    //         }
    //         ss << *it << " ";
    //         i++;
    //         if (i % row_length == 0){
    //             ss << ")";
    //             if(i != (int)selections.size()){
    //                 ss << "\n";
    //             }
    //         } 
    //     }
    //     this->msg = ss.str();
        
    // }

    SuccessMsgResult(std::vector<Table::KeyType> &results) {
        if(results.empty()){
            this->ifdisplay = false;
        } else{
            this->ifdisplay = true;
            std::stringstream ss;
            // std::cerr << "[DEBUG@SuccessMsgResult::SuccessMsgResult] Selction result received." << std::endl;
            for(auto str = results.begin(); str != results.end(); str++) {
                ss << static_cast<std::string>(*str);
                if(str != (results.end() - 1))
                    ss << '\n';
            }
            this->msg = ss.str();
        }
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        // std::cerr << "[DEBUG@SuccessMsgResult::output()] Final message is " << this->msg << std::endl;
        return os << msg << "\n";
    }
};

class RecordCountResult : public SuceededQueryResult {
    const int affectedRows;
public:
    bool display() override { return true; }

    explicit RecordCountResult(int count) : affectedRows(count) {}

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "Affected ? rows."_f % affectedRows << "\n";
    }
};

#endif //PROJECT_QUERYRESULT_H
