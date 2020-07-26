#include "helper.h"

int main (int argc, char* argv[])
{
    indes = open(argv[1],O_RDONLY);
    infile = fopen(argv[1],"r");
    outfile = fopen(argv[2],"w");
    if(infile == NULL){
        printf("Can't open file\n");
        exit(0); 
    }
    initializeGlobal();
    
    int startIdx = constructTablesL();
    decodeInputL(startIdx);
    
    fclose(infile);
    close(indes);
    fclose(outfile);
    cleanupGlobal();
    return 0;
}

void cleanupGlobal(){
    for (int i =0 ; i< totalBuckets; ++i){
        free(occBucket[i]);
    }
    free(occBucket);
    free(bitArray);
    free(cMap);
}

void initializeGlobal(){
    setvbuf (infile , NULL , _IOFBF , 0 );
    fseek(infile, 0, SEEK_END);
    FILELEN = ftell(infile);
    fseek(infile,0,SEEK_SET);
    if (FILELEN<BUFFERSIZE){
        if (FILELEN<BLOCKSIZE){
            BLOCKSIZE = FILELEN;
        }
        BUFFERSIZE = FILELEN;
    }
    buffer = malloc(BUFFERSIZE * sizeof(char));
    bitArray = (int*)calloc((1+ (FILELEN/16)),sizeof(u_int32_t));
    totalBuckets = FILELEN/BUCKETINTERVAL;
    occBucket = malloc(sizeof(int*)*totalBuckets);
    for (int i = 0; i<totalBuckets; ++i){
        occBucket[i] = malloc(sizeof(int)*4);
    }
    cMap = malloc(sizeof(int)*5);
    //printf("Length of file: %d\n",FILELEN);
    //printf("Size of Buffer : %d\n",BUFFERSIZE);
    //printf("Buckets created: %d \n",totalBuckets);
}

void setChar(int idx, char c){
    int idx1 = 2*idx;
    int idx2 = idx1+1;
    switch(c){
        case '\n':
        case 'A': // A & newline is 00
            setZero(bitArray,idx1);           
            setZero(bitArray,idx2);            
            return;
        case 'C': // C 01
            setZero(bitArray,idx1);            
            setOne(bitArray,idx2);
            return;
        case 'G': // G 10
            setOne(bitArray,idx1);            
            setZero(bitArray,idx2);           
            return; 
        case 'T': // T 11
            setOne(bitArray,idx1);            
            setOne(bitArray,idx2);            
            return;
        default:
            return;
    }

}

char getChar(int idx){
    int idx1 = idx*2;
    int idx2 = idx1+1;
    if ( isOne(bitArray,idx1)){
        if ( isOne(bitArray,idx2)) return 'T';//11
        //10
        return 'G';
    } else{
        if (isOne(bitArray,idx2)) return 'C'; //01
        //00
        if (idx == startPos){
            return '\n';
        }
        return 'A';
    }
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
void printTables(void){ // print cMap and occBuckets
    for (int i = 0; i<totalBuckets; ++i){
        printf("Bucket %d \n",i);
        printf("A %d C %d G %d T %d \n",
                occBucket[i][0],
                occBucket[i][1],
                occBucket[i][2],
                occBucket[i][3]);
    }

    printf("TOTAL CMAP COUNT: nl %d A %d C %d G %d T %d \n",
            cMap[0],cMap[1],cMap[2],cMap[3],cMap[4]);

}


// constructs cMap and Occ table, 
// returns position of '\n' character
int constructTablesL(void){
    // find file length;
    int charCode;
    bufferStart = 0;
    pread(indes, buffer, BUFFERSIZE, bufferStart);
    for (int i = 0; i < totalBuckets+1; ++i){
        // if buffer not enough to read to end
        if (bufferStart+BUFFERSIZE < (i+1) * BUCKETINTERVAL){
            bufferStart = i*BUCKETINTERVAL;
            pread(indes, buffer, BUFFERSIZE, bufferStart);
        }
        int bucketStart = i*BUCKETINTERVAL-bufferStart;
        int bucketEnd  = bucketStart+BUCKETINTERVAL;
        if (i == totalBuckets)
            bucketEnd   = FILELEN-bufferStart;
        for(int j=bucketStart; j<bucketEnd; ++j){
            charCode = getCharCode(buffer[j]);
            if (buffer[j] == '\n') startPos = j+bufferStart;
            setChar(j+bufferStart,buffer[j]);
            ++cMap[charCode];
        }

        if (i == totalBuckets)
            break;
        occBucket[i][0] = cMap[1];
        occBucket[i][1] = cMap[2];
        occBucket[i][2] = cMap[3];
        occBucket[i][3] = cMap[4];
    } 
    // modify cMap
    int tmp = 0;
    int runningCount = 0;
    for(int i = 0; i<CHARTYPES; ++i){
        tmp = cMap[i];
        cMap[i] = runningCount;
        runningCount+=tmp;
    }
    
    free(buffer);
    return startPos;
}

int occ(char c, int pos, int code){
    if (c == '\n')
        return 0;
    int count = 0;
    int b = pos/BUCKETINTERVAL;
    int start = b*BUCKETINTERVAL;
    int end = pos;
    for(int i =start ;i < end; ++i){
        if(getChar(i) == c){
            ++count;
        }
    }
    if (b == 0)
        return count;
    return count + occBucket[b-1][code-1];

}

void seekBuffer(int b,int pos){
    bufferStart = b*BUCKETINTERVAL;
    if (bufferStart < 0)
        bufferStart = 0;
    if(bufferStart + BLOCKSIZE >FILELEN){
        bufferStart = FILELEN-BLOCKSIZE;
    }
    pread(indes, buffer, BLOCKSIZE, bufferStart);
}


void decodeInputL(int startIdx){

    //setvbuf (outfile , NULL , _IOFBF , 0 );
    char curVal = '\n';
    int valIdx = startIdx;
    char decoded[BUFFERSIZE];
    int dIdx = BUFFERSIZE; //decoded index
    int remChar = FILELEN;
    int code = -1; 
    for(int i = FILELEN-1; i>= 0; --i){
        --dIdx;
        decoded[dIdx] = curVal;
        if (dIdx == 0) {
            //printf("%d %d %d %d\n",i, FILELEN-i, BUFFERSIZE, remChar);
            fseek(outfile,i,SEEK_SET); 
            fwrite(decoded,1,BUFFERSIZE,outfile);
            remChar -= BUFFERSIZE;
            dIdx = BUFFERSIZE;
        }
        switch(curVal) {
            case '\n': code = 0; break;
            case 'A':  code = 1; break;
            case 'C':  code = 2; break;
            case 'G':  code = 3; break;
            case 'T':  code = 4; break;
            default:   code = -1;
        }
       
        int occVal = occ(curVal,valIdx,code);
        valIdx = occVal+cMap[code];
        //if(i == FILELEN-50) return;
        curVal = getChar(valIdx);
        //curVal = buffer[valIdx-bufferStart];
    }
    if (remChar > 0){
        //printf("remaining char %d\n",remChar);
        fseek(outfile,0,SEEK_SET); 
        char *remDecode = malloc(sizeof(char)*remChar);
        strncpy(remDecode,decoded+(BUFFERSIZE-remChar),remChar);
        fputs(remDecode,outfile);
        free(remDecode);
    }
}

