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
int constructTables(std::string &code);


std::ofstream outfile; 
std::ifstream infile;
std::map<char,int> cMap;
std::vector<int> rank; //occurrences

const int BUCKET_INTERVAL = 10000000; //10 mil char
int FILELEN;
std::vector<std::map<char,int>> occBucket;

int main (int argc, char* argv[])
{
    infile.open(argv[1]);
    outfile.open(argv[2]); 

    // find file length;
    infile.seekg (0, infile.end);
    FILELEN = infile.tellg();
    infile.seekg(0,infile.beg);
    infile.clear();
    std::cout<< FILELEN <<std::endl;
    std::stringstream buffer;
    if (infile.is_open() && outfile.is_open()){
        buffer << infile.rdbuf();
        std::string code = buffer.str();
        infile.close();
        outfile.close();
    }
    return 0;
}

int constructTables(void){
    int startPos;
    char c;
    int i = 0;
    std::map<char,int> count;
    infile.clear();
    infile.seekg(0, infile.beg);
    while (infile.get(c)){
        if (count.find(c) == count.end()){
            if (c=='\n') startPos = i;
            count.insert({c,1});
            rank.push_back(0);
        } else{
            rank.push_back(count[c]);
            ++count[c];            
        }
        ++i;
    }
    //modify countMap to the less than values
    int runningValue = 0;
    for (std::map<char,int>::iterator it = count.begin(); it != count.end(); ++it){
        cMap.insert({it->first,runningValue});
        runningValue += it->second;
    }
    return startPos;
}
// TODO 
char getNextKey(char c){
    return 'i';
}

int occ(char &c, int pos){
    infile.clear();
    int b = occBucket.size();
    while ( b*BUCKET_INTERVAL > pos){
        b--;
    }

    int count = 0;
    if (b>=0) count = occBucket.at(b-1)[c];

    infile.seekg(b*BUCKET_INTERVAL,infile.beg);
    char d;

    while(infile.get(d)){
        if(d == c) ++count;
    } 
    return count;
}

std::pair<int,int> backwardSearch(std::string searchTerm){
    int i   = searchTerm.length()-1;
    char c  = searchTerm[i];
    int First = cMap[c];
    int Last  = cMap[getNextKey(c)]-1 ;
    while (First < Last &&  i >= 1){
        c = searchTerm[i-2];
        First = cMap[c] + occ(c,First-1);
        Last = cMap[c] + occ(c,Last);
        --i;
    }
    if (Last<First){
        return std::make_pair(0,0);
    } 
    return std::make_pair(First,Last);
}



int constructTables(std::string &code){
    int startPos;
    int N = code.length();
    int bucketFill = 0;
    for (int i = 0; i< N;++i){
        if (cMap.find(code.at(i)) == cMap.end()){
            if (code.at(i)=='\n') startPos = i;
            cMap.insert({code.at(i),0});
        }

        // bucket count of characters before current character
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
        if (it->first == '\n') continue;
        tmp = it->second;
        it->second = runningValue;
        runningValue += tmp;
    }
    return startPos;
}
void printTables(void){
    for (int i = 0; i< FILELEN; ++i){
        std::cout<< (rank.at(i)) << " ";
    }
    std::cout <<std::endl;
    for (auto tup : cMap){
        std::cout << tup.first << " " << tup.second << std::endl;
    }
}
