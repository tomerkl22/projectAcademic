#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct musicianPriceInstrument
{
	unsigned short insId; // מספר מזהה של כלי נגינה
	float price; // מחיר שדורש הנגן עבור נגינה בכלי זה
} MusicianPriceInstrument;

typedef struct listNode
{
	MusicianPriceInstrument* mpi;
	struct listNode* next;
}ListNode;

typedef struct mpiList
{
	ListNode* head;
	ListNode* tail;
} MPIList;

typedef struct musician
{
	char** name; // מערך של השמות המרכיבים את שמו המלא
	int nameParts;
	MPIList instruments; // MusicianPriceInstrument כלי הנגינה ברשימה מקושרת של
} Musician;

/* struct for the musician collection to know the musician array size all the time. */
typedef struct musiciansCollectionOfMusicians {
	Musician** arr;
	int logSize; // actual size
	int psySize; // memory size
} IMix ;

typedef struct treeNode {
	char instrument[150]; // name 
	unsigned short insId; // id for every instrument
	struct treeNode* left;
	struct treeNode* right;
} TreeNode;

typedef struct tree
{
	TreeNode* root;
}InstrumentTree;

#define NOT_FOUND -1
#define MAX(x,y)(((x)>(y))? (x):(y))
#define LEFT 0 
#define RIGHT 1
#define MIN_NAME_PARTS 2
#define MIN_PARTS_IN_LINE 4
#define MAX_LINE_LEN 151

int findInsId(InstrumentTree tree, char* instrument);
int findInsIdRec(TreeNode* root, char* instrument);
TreeNode* checkLocationInTree(TreeNode* root, char* ch, int* way);
TreeNode* BuildTheTree(FILE* f, TreeNode* root, int* counter);
TreeNode* createNewTreeNode(char* str, unsigned short count, TreeNode* left, TreeNode* right);

int buildMusicianGroupArr(FILE* musiciansFile, Musician*** musicianArr, InstrumentTree tree);
int countNofMusicians(FILE* musiciansFile);
char** buildArrOfSubStrings(FILE* musiciansFile, int* nofSubStrings);
int getMusicianName(char** subStringsArr, Musician** curMusician, unsigned short* firstInsId, InstrumentTree tree);
void buildMpiList(char** arr, int nofInstruments, MPIList* lst, unsigned short firstInsId, InstrumentTree tree);
void freeArr(char** arr, int size);

void insertDataToStartList(MPIList* lst, MusicianPriceInstrument* mpi);
ListNode* createNewListNode(MusicianPriceInstrument* mpi, ListNode* next);
void insertNodeToStartList(MPIList* lst, ListNode* newHead);
void makeEmptyList(MPIList* lst);
bool isEmptyList(MPIList lst);
void insertDataToEndList(MPIList* lst, MusicianPriceInstrument* mpi);
void insertNodeToEndList(MPIList* lst, ListNode* newTail);
void freeList(MPIList* lst);
void freeListRec(ListNode* head);

void checkMemoryAllocation(void* ptr);
void checkFile(FILE* f);

void buildMusiciansCollection(IMix* MusiciansCollection, Musician** MusicianGroup, int musicianNum, int treeSize);
void updateMusicianCollectionByInstrumentList(IMix* MusiciansCollectionInstru, Musician* musicianTmp, ListNode* head);
int checkSize(IMix* tmp);
void buildMemoryCollection(IMix* MusiciansCollection, int treeSize);
void printMusicianCollection(IMix* MusiciansCollection, int treeSize);