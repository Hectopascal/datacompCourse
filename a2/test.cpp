#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>


int main (void)
{
    std::ofstream myfile;
    std::string root = "tests/";
    std::uniform_int_distribution<int> charGen(32,127); 
    std::uniform_int_distribution<int> extGen(32,255); 
    std::uniform_int_distribution<int> lenGen(1,1024);
    for ( int i =0; i<100; ++i){
        std::mt19937 rng(i); //Standard mersenne_twister_engine seeded with rd()
        int messageLen = lenGen(rng); 
        myfile.open (root+std::to_string(i)+".txt");
        for (int j=0; j < messageLen; ++j){
            unsigned char c = charGen(rng);
            myfile << c ;
        }
        myfile.close(); 
    }
    for ( int i =0; i<20; ++i){
        std::mt19937 rng(i); //Standard mersenne_twister_engine seeded with rd()
        int messageLen = lenGen(rng); 
        myfile.open (root+std::to_string(i)+"ext.txt");
        for (int j=0; j < messageLen; ++j){
            unsigned char c = extGen(rng);
            myfile << c ;
        }
        myfile.close(); 
    }

    int messageLen = 1024; 
    for ( int i =0; i<20; ++i){
        std::mt19937 rng(i); //Standard mersenne_twister_engine seeded with rd()
        myfile.open (root+std::to_string(i)+"max.txt");
        for (int j=0; j < messageLen; ++j){
            unsigned char c = charGen(rng);
            myfile << c ;
        }
        myfile.close(); 
    }



    return 0;
}
