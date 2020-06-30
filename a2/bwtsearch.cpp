#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
//void decodeInput(std::map<unsigned char,std::vector<mpfr_ptr>> &in, std::string input, int count);
//std::string decodeInput(std::string &input);
void printTables(void);
//int constructTables(void);
int constructTables(std::string &code);
//char*  decodeInput(int startIdx);
char*  decodeInput(int startIdx,std::string &code);
std::ofstream outfile; 
std::ifstream infile;
std::unordered_map<char,int> cMap;
std::vector<int> rank; //occurrences
int FILELEN;
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
        int startIdx = constructTables(code);
        char* decoded = decodeInput(startIdx,code);
        outfile<< decoded;
        delete decoded;
        infile.close();
        outfile.close();
    }
    return 0;
}

// comparator function for sorting vectors
bool sortVectorLex(const std::vector<char>& v1, const std::vector<char> &v2){
    std::string s1{v1.begin(),v1.end()};
    std::string s2{v2.begin(),v2.end()};
    return s1 < s2;
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
int constructTables(std::string &code){
    int startPos;
    int N = code.length();
    std::map<char,int> count;
    for (int i = 0; i< N;++i){
        if (count.find(code.at(i)) == count.end()){
            if (code.at(i)=='\n') startPos = i;
            count.insert({code.at(i),1});
            rank.push_back(0);
        } else{
            rank.push_back(count[code.at(i)]);
            ++count[code.at(i)];            
        }
    }
    //modify countMap to the less than values
    int runningValue = 0;
    for (std::map<char,int>::iterator it = count.begin(); it != count.end(); ++it){
        cMap.insert({it->first,runningValue});
        runningValue += it->second;
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

char* decodeInput(int startIdx, std::string &code){
    char* decoded = new char[FILELEN+1];
    char curVal = '\n';
    int valIdx = startIdx;
    decoded[FILELEN] = '\0';
    for(int i = FILELEN-1; i>= 0; --i){
        decoded[i] = curVal;
        valIdx = rank.at(valIdx)+cMap[curVal];
        curVal = code.at(valIdx);
    //    std::cout<<curVal<<std::endl;
    }
//std::string s(decoded);
    return decoded;
}

char* decodeInput(int startIdx){
    char* decoded = new char[FILELEN+1];
    char curVal = '\n';
    int valIdx = startIdx;
    decoded[FILELEN] = '\0';
    infile.clear();
    for(int i = FILELEN-1; i>= 0; --i){
        decoded[i] = curVal;
        valIdx = rank.at(valIdx)+cMap[curVal];
        infile.seekg(valIdx, infile.beg);
        infile.get(curVal);
    //    std::cout<<curVal<<std::endl;
    }
//std::string s(decoded);
    return decoded;
}
/*

//dumb method
std::string decodeInput1(std::string input){
    int N = input.length();
    std::vector<std::vector<char>> matrix(N,std::vector<char>());    
    for (int i = 0; i < N; ++i){
        for(int j = 0; j<N; ++j){
            matrix.at(j).insert(matrix.at(j).begin() ,input.at(j));
        } 
        std::sort(matrix.begin(),matrix.end(),sortVectorLex);

    }
      
    for(int k =0;k < N; ++k){
        if (matrix.at(k).back() == '\n'){
            std::string s{matrix.at(k).begin(),matrix.at(k).end()};
            return s;
        }
    }
    return "";
}
*/
