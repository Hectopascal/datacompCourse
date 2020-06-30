#include <stdio.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <map>

#include <gmp.h>
#include <mpfr.h>


void decodeInput(std::map<unsigned char,std::vector<mpfr_ptr>> &in, std::string input, int count);

int main (void)
{
    std::map<unsigned char,int> dict; // unordered map
    std::map<unsigned char,std::vector<mpfr_ptr>> ranges; // unordered map

    std::string line;
    std::string code;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_set_default_prec(8192); 
    mpfr_t lo, hi, unit, tmp;
    mpfr_inits2(8192,tmp,unit,lo,hi,(mpfr_ptr) 0);
    double n = 0;
    // construct dict of occurences
    while(std::getline(std::cin, line)) { 
        if (line[1] != ' '){ // code
            std::istringstream iss (line);
            std::getline(iss,code,' ');
        } else {
            int count = std::atoi(line.substr(2).c_str());
            n+=count;
            dict[static_cast<unsigned char>(line[0])] = count;
        }
    }
    
    mpfr_set_d(lo,0,rnd);
    mpfr_set_d(unit,1,rnd);
    mpfr_set_d(tmp,n,rnd);
    mpfr_div(unit,unit,tmp,rnd);
    for( auto const& tup : dict) {
        mpfr_mul_d(tmp,unit,tup.second,rnd);
        mpfr_add(hi,lo,tmp,rnd);           // hi = lo + counts*unit
        std::vector<mpfr_ptr> vect;
        mpfr_ptr ptrLo, ptrHi;
        ptrLo = new mpfr_t;
        ptrHi = new mpfr_t;
        mpfr_init_set(ptrLo, lo, rnd);
        mpfr_init_set(ptrHi, hi, rnd);
        vect.push_back(ptrLo);
        vect.push_back(ptrHi);
        ranges[tup.first] = vect;

        mpfr_set(lo,hi,rnd);            // lo = hi
       /* std::cout << tup.first
                    << ' '  
                     << tup.second
                      << std::endl ;*/
    }
    mpfr_clears(tmp, unit , lo , hi,(mpfr_ptr) 0);
    decodeInput(ranges,code,n);

    for (auto & tup : ranges){
        delete (tup.second.at(0));
        delete (tup.second.at(1));       
        mpfr_clears(tup.second.at(0),tup.second.at(1), (mpfr_ptr)0);

    }

    return 0;
}

void decodeInput(std::map<unsigned char,std::vector<mpfr_ptr>> &in, std::string input, int count){
    mpfr_t tmp,symLo,symHi, code,range;
    mpfr_inits2(8192,tmp,code,symLo,symHi,range,(mpfr_ptr)0);
    mpfr_rnd_t rnd = MPFR_RNDN;

    mpfr_set_str(code,input.c_str(),10,rnd);
    for(int i = 0; i<count; ++i){
        
        for (auto const& entry : in){
            mpfr_set(symLo,entry.second.at(0),rnd);
            mpfr_set(symHi,entry.second.at(1),rnd);
            if (mpfr_cmp(code,symLo)>=0 && mpfr_cmp(code,symHi) < 0){
                std::cout << entry.first; 
                break;
            }
        }
        
        // range = symHi - symLo
        mpfr_sub(range,symHi, symLo,rnd);
        // code = (code-symLo)/range  
        mpfr_sub(code,code,symLo,rnd);
        mpfr_div(code,code,range,rnd);
        
    }
     
    mpfr_clears(tmp, symLo, symHi, code,range,(mpfr_ptr)0);

}
