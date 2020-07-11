#include <stdio.h>
#include <stdlib.h>

int occ(char c, int pos);

int constructTablesL(void);
int constructTablesS(char* code);

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
    
    if(FILELEN < BUFFERSIZE+1){
        bufferStart = 0; 
        fseek(infile,bufferStart,SEEK_SET);
        fread(buffer, FILELEN,1,infile);
        int startIdx = constructTablesS(buffer);
        decodeInputS(startIdx,buffer);
    } else {
        int startIdx = constructTablesL();
        decodeInputL(startIdx);
    }
    fclose(infile);
    fclose(outfile);
    cleanupGlobal();
    return 0;
}

void cleanupGlobal(){
    if (FILELEN < BUFFERSIZE+1){
        free(rank);
    }else{
        for (int i =0 ; i< totalBuckets; ++i){
            free(occBucket[i]);
        }
        free(occBucket);
    }
    free(cMap);
    free(buffer);
}

void initializeGlobal(){
    fseek(infile, 0, SEEK_END);
    FILELEN = ftell(infile);
    fseek(infile,0,SEEK_SET);
    //bleb
    if(FILELEN < BUFFERSIZE+1){
        buffer = malloc(FILELEN * sizeof(char));
        rank = malloc(sizeof(int)*FILELEN);
    }else {
        buffer = malloc(BUFFERSIZE * sizeof(char));
        int b = 0;
        while(b*BUFFERSIZE < FILELEN){
            b++;
        }

        totalBuckets = b;
        occBucket = malloc(sizeof(int*)*b);
        for (int i = 0; i<b; ++i){
            occBucket[i] = malloc(sizeof(int)*4);
        }
    }
    cMap = malloc(sizeof(int)*5);

}



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
    printf("hello\n");
    // find file length;
    int startPos;
    int iterEnd = BUFFERSIZE;
    int b =0;
    for (int i = 0; i*BUFFERSIZE< FILELEN;++i){ //for each block of buffer
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
        
        if (FILELEN > BUFFERSIZE){ //update buckets
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
    for (int i = 0; i< FILELEN;++i){
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

    return startPos;
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
    char decoded[FILELEN];
    for(int i = FILELEN-1; i>= 0; --i){
        //fseek(outfile,i,SEEK_SET);
        //putc(curVal,outfile);
        decoded[i] = curVal;
        valIdx = rank[valIdx]+cMap[getCharCode(curVal)];
        curVal = code[valIdx];
    }
    fseek(outfile,0,SEEK_SET);
    fputs(decoded,outfile);
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
}

