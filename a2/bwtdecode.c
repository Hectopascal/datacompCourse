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
        BUFFERSIZE = FILELEN;
    }
    BUCKETINTERVAL = FILELEN/100000;
    if(BUCKETINTERVAL < 32)
        BUCKETINTERVAL = 32;

    buffer = malloc(BUFFERSIZE * sizeof(char));
    bitArray = (int*)calloc((1+ (FILELEN/16)),sizeof(int));
    totalBuckets = FILELEN/BUCKETINTERVAL;
    occBucket = malloc(sizeof(int*)*totalBuckets);
    for (int i = 0; i<totalBuckets; ++i){
        occBucket[i] = malloc(sizeof(int)*4);
    }
    cMap = malloc(sizeof(int)*5);
}

void setChar(int idx, char c){
    switch(c){
        case '\n':
        case 'A': // A & newline is 00
            setZero(bitArray,(2*idx));           
            setZero(bitArray,(2*idx+1));            
            return;
        case 'C': // C 01
            setZero(bitArray,(2*idx) );            
            setOne(bitArray,(2*idx+1) );
            return;
        case 'G': // G 10
            setOne(bitArray,(2*idx));            
            setZero(bitArray,(2*idx+1));           
            return; 
        case 'T': // T 11
            setOne(bitArray,(2*idx));            
            setOne(bitArray,(2*idx+1));            
            return;
        default:
            return;
    }

}

char getChar(int idx){
    if ( isOne(bitArray,(idx*2))){
        if ( isOne(bitArray,(2*idx+1))) return 'T';//11
        //10
        return 'G';
    } else{
        if (isOne(bitArray, (2*idx+1))) return 'C'; //01
        //00
        if (idx == startPos){
            return '\n';
        }
        return 'A';
    }
}


// constructs cMap and Occ table, 
// returns position of '\n' character
int constructTablesL(void){
    // find file length;
    int charCode;
    bufferStart = 0;
    pread(indes, buffer, BUFFERSIZE, bufferStart);
    for (int i = 0; i < totalBuckets; ++i){
        // if buffer not enough to read to end
        if (bufferStart+BUFFERSIZE < (i+1) * BUCKETINTERVAL){
            bufferStart = i*BUCKETINTERVAL;
            pread(indes, buffer, BUFFERSIZE, bufferStart);
        }
        int bucketStart = i*BUCKETINTERVAL-bufferStart;
        int bucketEnd  = bucketStart+BUCKETINTERVAL;
        for(int j=bucketStart; j<bucketEnd; ++j){
            charCode = -1;
            switch(buffer[j]) {
                case '\n': charCode = 0; break;
                case 'A': charCode = 1; break;
                case 'C': charCode = 2; break;
                case 'G': charCode = 3; break;
                case 'T': charCode = 4; break;
                default: charCode = -1;
            }

            if (buffer[j] == '\n') startPos = j+bufferStart;
            setChar(j+bufferStart,buffer[j]);
            ++cMap[charCode];
        }
        occBucket[i][0] = cMap[1];
        occBucket[i][1] = cMap[2];
        occBucket[i][2] = cMap[3];
        occBucket[i][3] = cMap[4];
    }
    
    int bucketEnd   = FILELEN-bufferStart;
    int bucketStart = totalBuckets*BUCKETINTERVAL-bufferStart;
    for(int j=bucketStart; j<bucketEnd; ++j){
        charCode = -1;
        switch(buffer[j]) {
            case '\n': charCode = 0; break;
            case 'A': charCode = 1; break;
            case 'C': charCode = 2; break;
            case 'G': charCode = 3; break;
            case 'T': charCode = 4; break;
            default: charCode = -1;
        }

        if (buffer[j] == '\n') startPos = j+bufferStart;
        setChar(j+bufferStart,buffer[j]);
        ++cMap[charCode];
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

void decodeInputL(int startIdx){
    setvbuf (outfile , NULL , _IOFBF , 0 );
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
            //pwrite(outdes,decoded,BUFFERSIZE,i);
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
        char *remDecode = malloc(sizeof(char)*remChar);
        strncpy(remDecode,decoded+(BUFFERSIZE-remChar),remChar);
        fseek(outfile,0,SEEK_SET); 
        fputs(remDecode,outfile);
        //pwrite(outdes,remDecode,remChar,0);
        free(remDecode);
    }
}

