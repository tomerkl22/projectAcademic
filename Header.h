#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct treeNode
{
    unsigned char data;
    struct treeNode* left;
    struct treeNode* right;
}    TreeNode;

typedef struct tree
{
    TreeNode* root;
}Tree;
