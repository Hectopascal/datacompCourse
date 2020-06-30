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
//void decodeInput(std::map<unsigned char,std::vector<mpfr_ptr>> &in, std::string input, int count);
std::string decodeInput(std::string &input);

std::ofstream outfile; 
std::ifstream infile;


int main (int argc, char* argv[])
{
    infile.open(argv[1]);
    outfile.open(argv[2]); 
    //std::stringstream buffer;
    if (infile.is_open() && outfile.is_open()){
        int N = input.length();
        std::map<char,int> count;
        std::vector<int> rank;
        int startPos;
        char c;
        int i = 0;
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
        int tmp = 0;
        for (std::map<char,int>::iterator it = count.begin(); it != count.end(); ++it){
            tmp = it->second;
            it->second = runningValue;
            runningValue += tmp;
        }
       //buffer << infile.rdbuf();
        std::string encoded = buffer.str();
        std::string decoded = decodeInput(count,rank,rank.size());
        outfile<< decoded;
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
std::string decodeInput(std::map count, std::array rank, int N, int startIdx, char startVal){
/*
std::string decodeInput(std::string &input){ 
    int N = input.length();
    std::map<char,int> count;
    std::vector<int> rank(N);
    int startPos;
    for (int i = 0; i<N ; ++i){
        if (count.find(input.at(i)) == count.end()){
            if(input.at(i) == '\n') startPos = i;
            count.insert({input.at(i),1});
            rank.at(i) = 0;
        } else{
            rank.at(i) = count[input.at(i)];
            ++count[input.at(i)];            
        }
    }

    //modify countMap to the less than values
    int runningValue = 0;
    int tmp = 0;
    for (std::map<char,int>::iterator it = count.begin(); it != count.end(); ++it){
        tmp = it->second;
        it->second = runningValue;
        runningValue += tmp;
    }*/
    /*for (int i = 0; i< N; ++i){
        std::cout<< (rank.at(i)) << " ";
    }
    std::cout <<std::endl;
    for (std::map<char,int>::iterator it = count.begin(); it != count.end(); ++it){
        std::cout << it->first << " " << it->second << std::endl;
    }*/
    char* decoded = new char[N+1];
    char curVal = startVal;
    int valIdx = startIdx;
    decoded[N] = '\0';
    for(int i = N-1; i>= 0; --i){
        decoded[i] = curVal;
        valIdx = rank[valIdx]+count[curVal];
        curVal = input.at(valIdx);
    }
    std::string s(decoded);
    delete decoded;
    return s;
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
