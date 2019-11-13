//
// Created by liu on 18-10-21.
//

#include "query/QueryParser.h"
#include "query/QueryBuilders.h"

#include "thread/MultiThread.h"

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <string>
struct {
    std::string listen;
    long threads = 0;
} parsedArgs;

void parseArgs(int argc, char *argv[]) {
    const option longOpts[] = {
            {"listen",  required_argument, nullptr, 'l'},
            {"threads", required_argument, nullptr, 't'},
            {nullptr,   no_argument,       nullptr, 0}
    };
    const char *shortOpts = "l:t:";
    int opt, longIndex;
    while ((opt = getopt_long(argc, argv, shortOpts, longOpts, &longIndex)) != -1) {
        if (opt == 'l') {
            parsedArgs.listen = optarg;
        } else if (opt == 't') {
            parsedArgs.threads = std::strtol(optarg, nullptr, 10);
        } else {
            std::cerr << "lemondb: warning: unknown argument " << longOpts[longIndex].name << std::endl;
        }
    }

}

std::string extractQueryString(std::istream &is) {
    std::string buf;
    do {
        int ch = is.get();
        if (ch == '\n') continue;
        if (ch == ';') return buf;
        if (ch == EOF) {
            throw std::ios_base::failure("End of input");
        }
        buf.push_back((char) ch);
    } while (true);
}

int main(int argc, char *argv[]) {
    // Assume only C++ style I/O is used in lemondb
    // Do not use printf/fprintf in <cstdio> with this line
    std::ios_base::sync_with_stdio(false);

    parseArgs(argc, argv);

    std::fstream fin;
    if (!parsedArgs.listen.empty()) {
        fin.open(parsedArgs.listen);
        if (!fin.is_open()) {
            std::cerr << "lemondb: error: " << parsedArgs.listen << ": no such file or directory" << std::endl;
            exit(-1);
        }
    }
    std::istream is(fin.rdbuf());

 #ifdef NDEBUG
    // In production mode, listen argument must be defined
    if (parsedArgs.listen.empty()) {
        std::cerr << "lemondb: error: --listen argument not found, not allowed in production mode" << std::endl;
        exit(-1);
    }
#else
    // In debug mode, use stdin as input if no listen file is found
    if (parsedArgs.listen.empty()) {
        std::cerr << "lemondb: warning: --listen argument not found, use stdin instead in debug mode" << std::endl;
        is.rdbuf(std::cin.rdbuf());
    }
#endif

    if (parsedArgs.threads < 0) {
        std::cerr << "lemondb: error: threads num can not be negative value " << parsedArgs.threads << std::endl;
        exit(-1);
    } else if (parsedArgs.threads == 0) {
        // @TODO Auto detect the thread num
        std::cerr << "lemondb: info: auto detect thread num" << std::endl;
        autoDetectThreadNum();
    } else {
        setThreadNum(parsedArgs.threads);
        std::cerr << "lemondb: info: running in " << parsedArgs.threads << " threads" << std::endl;
    }

    std::cerr << getThreadNum() << " threads" << std::endl;
    //exit(0);



    QueryParser p;

    p.registerQueryBuilder(std::make_unique<QueryBuilder(Debug)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageTable)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Complex)>());

    size_t counter = 0;


    std::vector<std::istream*>isvec; 
    std::vector<std::fstream*>fsvec;
    isvec.push_back(&is);
    while (!isvec.empty()) {
        try {
            // A very standard REPL
            // REPL: Read-Evaluate-Print-Loop
            
            std::string queryStr = extractQueryString(*isvec.back());
            std::cerr << counter << " " << queryStr << std::endl;
            if(queryStr.compare(0, 6, "LISTEN") == 0){
                std::stringstream sss;
                sss.str(queryStr);
                for (int i = 0; i < 3; i++){
                    sss >> queryStr;
                }  
                std::fstream* newfin = new std::fstream;
                newfin->open(queryStr);  

                if (!newfin->is_open()){
                    std::cerr << "Error: could not open " << queryStr <<  std::endl;
                    exit(-1);                        
                }
                std::istream* pt = new std::istream(newfin->rdbuf());
                // std::cerr << pt << std::endl;
                isvec.push_back(pt);
                std::string filename;
                int it;
                for(it = queryStr.length()-1; it > -1; it--){
                    if(queryStr[it] =='/') break;
                } 
                filename.assign(queryStr, it+1, queryStr.length()-it);
                std::cout << ++counter << "\nANSWER = ( listening from " << filename << ")\n"; 
                continue;
            }

            Query::Ptr query = p.parseQuery(queryStr);

            QueryResult::Ptr result = query->execute();

            std::cout << ++counter << "\n";
            
            if (result->success()) {
                if (result->display()) {
                    std::cout <<*result;
                    //std::cout.flush();
                } else {
                }
            } else {
                std::cout.flush();
                std::cerr << "QUERY FAILED:\n\t" << *result;
            }
        }  catch (const std::ios_base::failure& e) {
            if(isvec.size() == 1)
                break;
            else{
                if(isvec.size() > 1)
                    delete isvec.back();
                if(fsvec.size()> 0)
                // std::cerr << isvec[i-1] << std::endl;
                    delete fsvec.back();
                // std::cerr << isvec.size();
                isvec.pop_back();
                continue;
            }
        } catch (const std::exception& e) {
            std::cout.flush();
            std::cerr << e.what() << std::endl;
        }
    }
    std::cout.flush();
    pthread_exit(NULL);
    return 0;
}