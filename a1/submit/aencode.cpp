#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <gmp.h>
#include <mpfr.h>


void encodeInput(std::map<unsigned char,std::vector<mpfr_ptr>> &in, std::string &input);
int main (void) {
    std::map<unsigned char,int> dict;
    std::map<unsigned char,std::vector<mpfr_ptr>> ranges;
    
    std::string line;
    std::getline(std::cin, line);

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_set_default_prec(8192);
    double n = 0;
    mpfr_t tmp,unit,lo,hi;
    mpfr_inits2(8192,tmp,unit,lo,hi,(mpfr_ptr) 0);
    mpfr_ptr ptrLo, ptrHi;

    // create dictionary of counts
    for (char& c : line){
        ++n;
        unsigned char myC = static_cast<unsigned char>(c); 
        if (dict.find(myC) == dict.end()){
            dict[c] = 1;
        } else {
            dict[c]++;
        }
    }
 
    mpfr_set_d(lo,0,rnd);
    mpfr_set_d(tmp,1,rnd);
    mpfr_div_d(unit,tmp,n,rnd);

    // construct dictionary of ranges
    for( auto & tup : dict) {
        mpfr_mul_d(tmp,unit,tup.second,rnd);
        mpfr_add(hi,lo,tmp,rnd);           // hi = lo + counts*unit
        std::vector<mpfr_ptr> vect;
        ptrLo = new mpfr_t;
        ptrHi = new mpfr_t;
        mpfr_init_set(ptrLo, lo, rnd);
        mpfr_init_set(ptrHi, hi, rnd);
        vect.push_back(ptrLo);
        vect.push_back(ptrHi);
        ranges[tup.first] = vect;
        mpfr_set(lo,hi,rnd);            // lo = hi
        std::cout << tup.first
                    << ' '  
                     << tup.second 
                      << std::endl ;
    }
    mpfr_clears(tmp, unit , lo , hi,(mpfr_ptr) 0);
    encodeInput(ranges,line);
    // clear mem
    for (auto & tup : ranges){
        delete (tup.second.at(0));
        delete (tup.second.at(1));       
        mpfr_clears(tup.second.at(0),tup.second.at(1), (mpfr_ptr)0);

    }
    return 0;
}

void encodeInput(std::map<unsigned char,std::vector<mpfr_ptr>> &in, std::string &input) {
    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t tmp, lo, hi, range;
    mpfr_inits2(8192,tmp,lo,hi,range,(mpfr_ptr)0);
    mpfr_set_d(lo,0,rnd);
    mpfr_set_d(hi,1,rnd);
    
    for (char& c : input){
        mpfr_sub(range,hi,lo,rnd); // range = hi-lo
        mpfr_mul(tmp,range,in[c].at(1),rnd);
        mpfr_add(hi,lo,tmp,rnd);  // hi = lo+symHi*range) 
        mpfr_mul(tmp,range,in[c].at(0),rnd);
        mpfr_add(lo,lo,tmp,rnd);   // lo = lo+symLo*range
    }
    
    int i = 0;
    //finding the digit place where the two numbers first differ
    mpfr_sub(tmp,hi,lo,rnd);
    while(mpfr_cmp_d(tmp,1)<0){
        mpfr_mul_d(tmp,tmp,10,rnd);
        i++;
    }
    
    // formatting for output
    // lo rounds up, hi rounds down at place after first differing digit
    std::string strTempLo = "%."+std::to_string(i+1)+"RUf";
    std::string strTempHi = "%."+std::to_string(i+1)+"RDf";
    mpfr_printf (strTempLo.c_str(), lo);
    putchar(' ');
    mpfr_printf (strTempHi.c_str(), hi);
    putchar('\n');
    mpfr_clears(tmp,lo,hi,range,(mpfr_ptr)0);
}
