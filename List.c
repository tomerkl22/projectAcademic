#include "Header.h"

int findInsId(InstrumentTree tree, char* instrument) {
	// find the index 
	TreeNode* root = tree.root;
	int id = findInsIdRec(root, instrument);
}

void tomer(int num) {

}

int findInsIdRec(TreeNode* root, char* instrument) {
	// find the index of the instrument recorsive
	int indRight, indLeft;
	
	if (root == NULL)
		return NOT_FOUND;

	if (strcmp(root->instrument, instrument) == 0)
		return root->insId;
	else {
		indRight = findInsIdRec(root->right, instrument);
		indLeft = findInsIdRec(root->left, instrument);
	}	
	return MAX(indRight, indLeft);
}

TreeNode* checkLocationInTree(TreeNode* root, char* ch, int* way) {
	// check where (Node address) the instrument should be in the tree
	TreeNode* right, * left;

	if (root == NULL) { // root <-> head
		*way = NOT_FOUND;
		return root;
	}

	if (strcmp(ch, root->instrument) > 0) { // need to go right
		if (root->right == NULL) {
			*way = RIGHT;
			return root;
		}
		else
			right = checkLocationInTree(root->right, ch, way);
		return right;
	}

	else { // going left
		if (root->left == NULL) {
			*way = LEFT;
			return root;
		}
		else
			left = checkLocationInTree(root->left, ch, way);
		return left;
	}
	
	// check this with debugger
	/*
	if (right != NULL)
		return right;
	else
		return left;
	*/
}

TreeNode* BuildTheTree(FILE* f, TreeNode* root){
	// build the tree from the file
	char str[150];
	TreeNode* newNode;
	unsigned short count = 0;
	int way; // to know what way the node should be
	int flag;
	
	flag = fscanf(f, "%s\n", &str);

	while (str != NULL && flag != NOT_FOUND) { // go over the file until there is no more instruments

		newNode = checkLocationInTree(root, str, &way);
		TreeNode* insNode = createNewTreeNode(str, count, NULL, NULL); // instrument node
		if (way == LEFT)
			newNode->left = insNode;
		else if (way == RIGHT)
			newNode->right = insNode;
		else {
			root = insNode;
		}
		count++;
		flag = fscanf(f, "%s\n", &str);
	}	
	
	fclose(f); // close file

	return root;
}

TreeNode* createNewTreeNode(char* str, unsigned short count, TreeNode* left, TreeNode* right)
{	//create new tree node

	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));

	checkMemoryAllocation(node);
	strcpy(node->instrument,str);
	node->left = left;
	node->right = right;
	node->insId = count;
	return node;
}

void checkFile(FILE* f) {
	// check the file
	if (f == NULL)
		printf("File %s, does not exist\n", f);
}

void checkMemoryAllocation(void* ptr)
{	// check the allocation
	if (ptr == NULL)
	{
		printf("Memory allocation failed!\n");
		exit(-1);
	}
}
