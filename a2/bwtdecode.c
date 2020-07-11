#include <stdio.h>
#include <stdlib.h>

void printTables(void);

int constructTablesL(void);
int constructTablesS(char* code);
int occ(char c, int pos);

void  decodeInputL(int startIdx);
void  decodeInputS(int startIdx,char* code);

void initializeGlobal(void);
void cleanupGlobal(void);

int FILELEN;
int bufferStart;
int totalBuckets;
const int BUFFERSIZE = 1000000;
const int CHARTYPES = 5; 

FILE* outfile; 
FILE* infile;

int* cMap;
int* rank; //occurrences
int** occBucket;
char* buffer;

int main (int argc, char* argv[])
{
    infile = fopen(argv[1],"r");
    outfile = fopen(argv[2],"w"); 
    initializeGlobal();
    // find file length;
    if (infile != NULL && outfile != NULL){
        if(FILELEN < BUFFERSIZE){
            bufferStart = 0; 
            fseek(infile,bufferStart,SEEK_SET);
            fread(buffer, BUFFERSIZE,1,infile);
            int startIdx = constructTablesS(buffer);
            decodeInputS(startIdx,buffer);
        } else {
            int startIdx = constructTablesL();
            decodeInputL(startIdx);
        }
        fclose(infile);
        fclose(outfile);
    }
    cleanupGlobal();
    return 0;
}
void cleanupGlobal(){
    if (FILELEN < BUFFERSIZE)
        free(rank);
    for (int i =0 ; i< totalBuckets; ++i){
         free(occBucket[i]);
    }
    free(occBucket);
    free(cMap);
    free(buffer);
}
void initializeGlobal(){
    fseek(infile, 0, SEEK_END);
    FILELEN = ftell(infile);
    fseek(infile,0,SEEK_SET);
    int b = 0;
    buffer = malloc(BUFFERSIZE * sizeof(char));
    //bleb
    while(b*BUFFERSIZE < FILELEN){
        b++;
    }
    totalBuckets = b;
    if(FILELEN < BUFFERSIZE){
        rank = malloc(sizeof(int)*FILELEN);
    }
    occBucket = malloc(sizeof(int*)*b);
    for (int i = 0; i<b; ++i){
        occBucket[i] = malloc(sizeof(int)*4);
    }
    cMap = malloc(sizeof(int)*5);

}
// comparator function for sorting vectors
/*bool sortVectorLex(const std::vector<char>& v1, const std::vector<char> &v2){
    std::string s1{v1.begin(),v1.end()};
    std::string s2{v2.begin(),v2.end()};
    return s1 < s2;
}*/
//Memory efficient version
int getCharCode(char c){
    switch(c) {
        case '\n':
            return 0;
        case 'A':
            return 1;
        case 'C':
            return 2;
        case 'G':
            return 3;
        case 'T':
            return 4;
        default:
            return 5;
   }
}

int constructTablesL(void){
    // find file length;
    int startPos;
    int iterEnd = BUFFERSIZE;
    int b =0;
    for (int i = 0; i*BUFFERSIZE< FILELEN;++i){
        bufferStart = i*BUFFERSIZE; 
        fseek(infile,bufferStart,SEEK_SET);
        fread(buffer, BUFFERSIZE,1,infile);
        if (FILELEN-bufferStart < BUFFERSIZE){
            iterEnd = FILELEN-bufferStart;
        }

        for (int j =0; j<iterEnd; ++j){
            if (buffer[j] == '\n'){
                startPos = bufferStart+j;
            }
            ++cMap[getCharCode(buffer[j])];            
        }
        
        if (FILELEN > BUFFERSIZE){
            occBucket[b][0] = cMap[1];
            occBucket[b][1] = cMap[2];
            occBucket[b][2] = cMap[3];
            occBucket[b][3] = cMap[4];
            b++;
        }

    }

    //modify cMap to the less than values
    int runningValue = 0;
    int tmp = 0;
    for (int i = 0; i < CHARTYPES; ++i){
        if (i == 0){
            cMap[0] = 0;
            ++runningValue;
            continue;
        }
        tmp = cMap[i];
        cMap[i] = runningValue;
        runningValue += tmp;
    }
    return startPos;
}


int constructTablesS(char* code){
    int startPos;
    int N = FILELEN;
    for (int i = 0; i< N;++i){
        if (code[i]=='\n') startPos = i;
        rank[i] = cMap[getCharCode(code[i])];
        ++cMap[getCharCode(code[i])];     
    }
    //modify cMap to the less than values
    int runningValue = 0;
    int tmp = 0;
    for (int i = 0; i < CHARTYPES; ++i){
        if (i == 0){ // newline char
            cMap[0] = 0;
            ++runningValue;
            continue;
        }
        tmp = cMap[i];
        cMap[i] = runningValue;
        runningValue += tmp;
    }

    //printTables();
    return startPos;
}
void printTables(void){
    /*for (int i = 0; i< FILELEN; ++i){
        std::cout<< (aank.at(i)) << " ";
    }*/
    /*std::cout <<std::endl;
    std::cout<< "\\n " << cMap[0] << std::endl;
    std::cout<< "A " << cMap[1] << std::endl;
    std::cout<< "C " << cMap[2] << std::endl;
    std::cout<< "G " << cMap[3] << std::endl;
    std::cout<< "T " << cMap[4] << std::endl;
*/
}

int occ(char c, int pos){
    int count = 0;
    //std::cout<< c<< " POS "<<pos ;
    if (pos < bufferStart || pos >= bufferStart+BUFFERSIZE ){
        int b = totalBuckets;
        while ( b*BUFFERSIZE > pos){
            --b;
        }
        count = occBucket[b-1][getCharCode(c)];
        bufferStart = b*BUFFERSIZE;
        fseek(infile,bufferStart,SEEK_SET);
        fread(buffer, BUFFERSIZE,1,infile);
    }
    pos = pos-bufferStart;
    for(int i =0; i<pos; ++i){
        if(buffer[i] == c){
            
            ++count;
        }
    }
    //std::cout<< " COUNT "<<count<<std::endl;
    return count;
}

void decodeInputS(int startIdx, char* code){
    char curVal = '\n';
    int valIdx = startIdx;
    for(int i = FILELEN-1; i>= 0; --i){
        fseek(outfile,i,SEEK_SET);
        fputc(curVal,outfile);
        valIdx = rank[valIdx]+cMap[getCharCode(curVal)];
        curVal = code[valIdx];
    //    std::cout<<curVal<<std::endl;
    }
//std::string s(decoded);
}

void seekBuffer(int pos){
    int b = totalBuckets;
    while ( b*BUFFERSIZE > pos){
        --b;
    }
    bufferStart = b*BUFFERSIZE;
    fseek(infile,bufferStart,SEEK_SET);
    fread(buffer, BUFFERSIZE,1, infile);
}


void decodeInputL(int startIdx){
    char curVal = '\n';
    int valIdx = startIdx;
    for(int i = FILELEN-1; i>= 0; --i){
        fseek(outfile,i,SEEK_SET);
        fputc(curVal,outfile);
        valIdx = occ(curVal,valIdx)+cMap[getCharCode(curVal)];
        if (valIdx < bufferStart || valIdx >= bufferStart + BUFFERSIZE){
            seekBuffer(valIdx);
        }
        curVal = buffer[valIdx-bufferStart];
    }
    //std::cout<< valIdx<<" idx - val " <<curVal<<std::endl;
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
