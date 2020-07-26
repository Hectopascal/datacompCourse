#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define setOne(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define setZero(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )            
#define isOne(A,k)    ( A[(k/32)] & (1 << (k%32)) )


int FILELEN;
int bufferStart;
int bucketBuffer;
int totalBuckets;
const int BUCKETINTERVAL = 100;
const int CHARTYPES      = 5; 
int BUFFERSIZE           = 819200;
int BLOCKSIZE            = 100;


FILE* outfile; 
FILE* infile;
int indes;


int startPos;
int* cMap;
int* bitArray;
int** occBucket;
char* buffer;


int occ(char c, int pos,int code);
int constructTablesL(void);
int constructTablesS(char* code);
void  decodeInputL(int startIdx);
void  decodeInputS(int startIdx,char* code);
void seekBucket(int b);
void initializeGlobal(void);
void cleanupGlobal(void);
int getCharCode(char c);


