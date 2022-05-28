#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NOT_FOUND -1
#define MAX(x,y)(((x)>(y))? (x):(y))
#define LEFT 0 
#define RIGHT 1
#define MIN_NAME_PARTS 2
#define MIN_PARTS_IN_LINE 4
#define NOF_MINUTES_IN_HOUR 60
#define MAX_LINE_LEN 500

typedef struct musicianPriceInstrument
{
	unsigned short insId; // מספר מזהה של כלי נגינה
	float price; // מחיר שדורש הנגן עבור נגינה בכלי זה
} MusicianPriceInstrument;

typedef struct mpiListNode
{
	MusicianPriceInstrument* mpi;
	struct mpiListNode* next;
}MPIListNode;

typedef struct mpiList
{
	MPIListNode* head;
	MPIListNode* tail;
} MPIList;

typedef struct treeNode
{
	char instrument[MAX_LINE_LEN]; // name 
	unsigned short insId; // id for every instrument
	struct treeNode* left;
	struct treeNode* right;
} TreeNode;

typedef struct tree
{
	TreeNode* root;
}InstrumentTree;

typedef struct musician
{
	char** name; // מערך של השמות המרכיבים את שמו המלא
	int nameParts;
	MPIList instruments; // MusicianPriceInstrument כלי הנגינה ברשימה מקושרת של
	bool freeToPlay;  //Paramater to check if current musician is free to play in the concert (true). 
					  //false - if he is already playing another instrument in the concert
} Musician;

typedef struct musicianPtrByIns
{
	Musician* musicianPtr;
	float priceForIns;  //price of current musician for current instrument
} MusicianPtrByIns;

/* struct for the musician collection to know the musician array size all the time. */
typedef struct musiciansCollectionOfMusicians {
	MusicianPtrByIns** arr;
	int logSize; // actual size
	int psySize; // memory size
} IMix;

typedef struct date
{
	int day, month, year;
	float hour;
} Date;

typedef struct concertInstrument
{
	int num; // כמה מאותו כלי
	int inst; // מספר מזהה של כלי נגינה
	char importance; // האם לכלי נגינה זה יש תקציב עבור נגנים יקרים 0/1
	char name[MAX_LINE_LEN];
	MusicianPtrByIns** musicianArrIns;  //for each instrument needed in the concert, allocate an array of pointers to musicians that will play this instrument
} ConcertInstrument;

typedef struct conInsListNode
{
	ConcertInstrument* concertInstPtr;
	struct conInsListNode* next;
} ConInsListNode;

typedef struct ciList
{
	ConInsListNode* head;
	ConInsListNode* tail;
} CIList;

typedef struct concert
{
	Date date_of_concert; // תאריך קיום ההופעה
	char* name; // שם ההופעה
	CIList instruments; // ConcertInstrument כלי הנגינה ברשימה מקושרת של
	float totalConcertPrice;
} Concert;

// question 1 functions
TreeNode* BuildTheTree(FILE* f, TreeNode* root, int* counter);
TreeNode* createNewTreeNode(char* str, unsigned short count, TreeNode* left, TreeNode* right);

// question 2 functions
int findInsId(InstrumentTree tree, char* instrument);
int findInsIdRec(TreeNode* root, char* instrument);
TreeNode* checkLocationInTree(TreeNode* root, char* ch, int* way);

// question 3 functions
int buildMusicianGroupArr(FILE* musiciansFile, Musician*** musicianArr, InstrumentTree tree);
int countNofMusicians(FILE* musiciansFile);
char** buildArrOfSubStrings(FILE* musiciansFile, int* nofSubStrings);
int getMusicianName(char** subStringsArr, Musician** curMusician, unsigned short* firstInsId, InstrumentTree tree);
void buildMpiList(char** arr, int nofInstruments, MPIList* lst, unsigned short firstInsId, InstrumentTree tree);
void freeInsAndPricesStringsInSubStringsArr(char** arr, int size, int firstIndToDeleteFrom);

void makeEmptyMpiList(MPIList* lst);
bool isEmptyMpiList(MPIList lst);
void insertDataToEndMpiList(MPIList* lst, MusicianPriceInstrument* mpi);
MPIListNode* createNewMpiListNode(MusicianPriceInstrument* mpi, MPIListNode* next);
void insertNodeToEndMpiList(MPIList* lst, MPIListNode* newTail);

// question 4 functions
void buildMusiciansCollection(IMix* MusiciansCollection, Musician** MusicianGroup, int musicianNum, int treeSize);
void updateMusicianCollectionByInstrumentList(IMix* MusiciansCollectionInstru, Musician* musicianTmp, MPIListNode* head);
int checkSize(IMix* tmp);
void buildMemoryCollection(IMix* MusiciansCollection, int treeSize);

// question 5 functions
void getConcertSortAndPrintDetails(IMix* musiciansCollection, InstrumentTree tr, int treeSize);
bool getConcertDetails(Concert* curConcert, InstrumentTree tr);
void makeAllMusiciansFreeToPlay(IMix* musiciansCollection, int sizeTree);
void sortMusiciansCollection(IMix* musiciansCollection, Concert* curConcert);
void mergeSort(MusicianPtrByIns** arr, int size, char importance);
void mergeFromExpensiveToCheap(MusicianPtrByIns** arr1, int size1, MusicianPtrByIns** arr2, int size2, MusicianPtrByIns** tmpArr);
void mergeFromCheapToExpensive(MusicianPtrByIns** arr1, int size1, MusicianPtrByIns** arr2, int size2, MusicianPtrByIns** tmpArr);
bool getMusiciansForShow(IMix* musiciansCollection, Concert* curConcert);
bool getMusiciansForCurIns(MusicianPtrByIns** musiciansArrConcert, IMix* musiciansInsArr, int nofInsNeeded, float* totalPrice);
void printShowDetails(Concert* curConcert);

void makeEmptyCIList(CIList* lst);
bool isEmptyCIList(CIList lst);
void insertDataToEndCIList(CIList* lst, ConcertInstrument* ciPtr);
ConInsListNode* createNewCIListNode(ConcertInstrument* ciPtr, ConInsListNode* next);
void insertNodeToEndCIList(CIList* lst, ConInsListNode* newTail);


//management functions
void freeTree(InstrumentTree tr);
void freeTreeRec(TreeNode* root);
void freeMpiList(MPIList* lst);
void freeMpiListRec(MPIListNode* head);
void freeMusicianGroupArr(Musician** MusicianGroup, int numOfmusicians);
void freeMusiciansCollection(IMix* MusiciansCollection, int size);
void freeCIList(CIList* lst);
void freeCIListRec(ConInsListNode* head);
void checkMemoryAllocation(void* ptr);
void checkFile(FILE* f);
