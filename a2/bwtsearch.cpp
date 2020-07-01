#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <vector>
#include <map>
#include <array>
#include <tuple>
//void decodeInput(std::map<unsigned char,std::vector<mpfr_ptr>> &in, std::string input, int count);
//std::string decodeInput(std::string &input);
void printTables(void);
void constructTables(std::string &code);
int bwtSearch(std::string &searchTerm);

std::ifstream infile;
std::map<char,int> cMap;
std::vector<int> rank; //occurrences

const int BUCKET_INTERVAL = 10000000; //10 mil char
int FILELEN;
std::vector<std::map<char,int>> occBucket;

int main (int argc, char* argv[]) {
    infile.open(argv[1]);

    // find file length;
    infile.seekg (0, infile.end);
    FILELEN = infile.tellg();
    infile.seekg(0,infile.beg);
    infile.clear();
    std::cout<< FILELEN <<std::endl;
    std::stringstream buffer;
    std::string line;
    if (infile.is_open()){
        buffer << infile.rdbuf();
        std::string code = buffer.str();
        constructTables(code);
        while(std::getline(std::cin, line)){
            std::cout<< bwtSearch(line) << std::endl;
        }
    
        infile.close();
    }
    
    
        return 0;
}

char getNextKey(char c){
    char prev = '\0';
    for (auto tup : cMap){
        if (prev == c) return tup.first;
        prev = tup.first;
    }
    return 0;
}

int occ(char &c, int pos){
    infile.clear();
    std::cout<<"char "<< c <<" pos "<< pos;
    int count = 0;
    int startPos = 0;
    if (pos > BUCKET_INTERVAL){
        int b = occBucket.size();
        while ( b*BUCKET_INTERVAL > pos){
            --b;
        }
        count = occBucket.at(b-1)[c];
        startPos = b*BUCKET_INTERVAL;
        pos = pos-startPos;
    }
    infile.seekg(startPos,infile.beg);
    char d;
    while(infile.get(d) && pos >= 0){
        if(d == c) ++count;
        --pos;
    }
    std::cout<< " count " <<count<<std::endl;
    infile.clear(); 
    return count;
}

int bwtSearch(std::string &searchTerm){
    std::cout << "Searching: "<< searchTerm<<std::endl;
    int i   = searchTerm.length()-1;
    char c  = searchTerm[i];
    int First = cMap[c];

    int Last  = cMap[getNextKey(c)]-1;
    if (Last <= 0) Last = FILELEN-1;
    //std::cout << First << "---FL---" << Last<<std::endl; 
    while (First <= Last &&  i >= 1){
        c = searchTerm[i-1];
        First = cMap[c] + occ(c,First-1);
        Last = cMap[c] + occ(c,Last)-1;
        //std::cout << First << "---FL---" << Last<<std::endl; 
        --i;
    }
    if (Last<First){
        return 0;
    } else {
        return Last-First+1;
    }
}



void constructTables(std::string &code){
    int N = code.length();
    std::cout<< "TOTAL BWT LENGTH " <<N << " FILELEN " << FILELEN<<std::endl;
    int bucketFill = 0;
    for (int i = 0; i< N;++i){
        if (cMap.find(code.at(i)) == cMap.end()){
            cMap.insert({code.at(i),0});
        }

        // cache count of characters before current character for larger files
        if (bucketFill >= BUCKET_INTERVAL){
            occBucket.push_back(std::map<char,int>());
            occBucket.at(occBucket.size()-1).insert({'A',cMap['A']});
            occBucket.at(occBucket.size()-1).insert({'C',cMap['C']});
            occBucket.at(occBucket.size()-1).insert({'T',cMap['T']});
            occBucket.at(occBucket.size()-1).insert({'G',cMap['G']});
            bucketFill = 0;
        }
        ++cMap[code.at(i)];            
        ++bucketFill;

    }

    //modify cMap to the less than values
    int runningValue = 0;
    int tmp = 0;
    for (std::map<char,int>::iterator it = cMap.begin(); it != cMap.end(); ++it){
        if (it->first == '\n'){
            ++runningValue;
            continue;
        }
        tmp = it->second;
        it->second = runningValue;
        runningValue += tmp;
    }
    cMap.erase('\n');
    printTables();
}
void printTables(void){
    //for (int i = 0; i< FILELEN; ++i){
    //    std::cout<< (rank.at(i)) << " ";
    //}
    std::cout<< "cMap size " << cMap.size() << " occBucket size " << occBucket.size();
    std::cout <<std::endl;
    for (auto tup : cMap){
        std::cout << tup.first << " " << tup.second << std::endl;
    }
    for (auto v : occBucket){
        for (auto tup : v){
            std::cout<< tup.first << " " << tup.second<< std::endl;
        }
    }
}
