#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

int findInsId(InstrumentTree tree, char* instrument);
int findInsIdRec(TreeNode* root, char* instrument);
TreeNode* checkLocationInTree(TreeNode* root, char* ch, int* way);
TreeNode* BuildTheTree(FILE* f, TreeNode* root);
void checkFile(FILE* f);
void checkMemoryAllocation(void* ptr);
TreeNode* createNewTreeNode(char* str, unsigned short count, TreeNode* left, TreeNode* right);