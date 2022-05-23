#include "Header.h"

void buildMusiciansCollection(IMix* MusiciansCollection, Musician** MusicianGroup, int musicianNum, int treeSize) {
	// Build the musicians collection that point from the instrument to the musician

	buildMemoryCollection(MusiciansCollection, treeSize);

	for (int i = 0; i < musicianNum; i++) {
		updateMusicianCollectionByInstrumentList(MusiciansCollection, MusicianGroup[i], MusicianGroup[i]->instruments.head);
	}

	// realloc the memory of the final sizes arrays
	for (int i = 0; i < musicianNum; i++) {
		int size = MusiciansCollection[i].logSize;
		int pizS = MusiciansCollection[i].psySize;
		for (int j = size; j < pizS; j++) {
			// free the not relavent malloc ! 
			free(MusiciansCollection[i].arr[j]->musicianPtr);
			free(MusiciansCollection[i].arr[j]);
		}
		MusiciansCollection[i].arr = (MusicianPtrByIns**)realloc(MusiciansCollection[i].arr, sizeof(MusicianPtrByIns*) * size);
	}

}

void buildMemoryCollection(IMix* MusiciansCollection, int treeSize) {
	// malloc the array
	for (int i = 0; i < treeSize; i++) {
		MusiciansCollection[i].arr = (MusicianPtrByIns**)malloc(sizeof(MusicianPtrByIns*));
		MusiciansCollection[i].arr[0] = (MusicianPtrByIns*)malloc(sizeof(MusicianPtrByIns)); // at least 1 musician
		MusiciansCollection[i].arr[0]->musicianPtr = (Musician*)malloc(sizeof(Musician));
		MusiciansCollection[i].logSize = 0;
		MusiciansCollection[i].psySize = 1;
	}
}

void updateMusicianCollectionByInstrumentList(IMix* MusiciansCollectionInstru, Musician* musicianTmp, ListNode* head)
{ // Go through the list of the instrument and update the musician collection

	ListNode* curr = head;
	int index, nextInx;
	float curPrice;

	while (curr != NULL) {
		index = curr->mpi->insId; // get the instrument id that the ***musician*** is playing with
		curPrice = curr->mpi->price;
		nextInx = checkSize(&(MusiciansCollectionInstru[index])); // check the size of the ***musician*** array inside the instrument array
		MusiciansCollectionInstru[index].arr[nextInx]->musicianPtr = musicianTmp;
		MusiciansCollectionInstru[index].arr[nextInx]->priceForIns = curPrice;
		MusiciansCollectionInstru[index].logSize++;
		curr = curr->next;
	}

}

int checkSize(IMix* tmp) {
	// check the size of array. if needed realloc it
	if (tmp->logSize == tmp->psySize) {
		tmp->psySize *= 2;
		tmp->arr = (MusicianPtrByIns**) realloc(tmp->arr, sizeof(MusicianPtrByIns*) * tmp->psySize);
		int logs = tmp->logSize, pizS = tmp->psySize;
		for (int i = logs; i < pizS; i++) {
			tmp->arr[i] = (MusicianPtrByIns*)malloc(sizeof(MusicianPtrByIns));
			tmp->arr[i]->musicianPtr = (Musician*)malloc(sizeof(Musician));
		}
	}
	return tmp->logSize;
}

TreeNode* BuildTheTree(FILE* f, TreeNode* root, int* counter) {
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

	*counter = count;
	return root;
}

TreeNode* createNewTreeNode(char* str, unsigned short count, TreeNode* left, TreeNode* right)
{	//create new tree node

	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));

	checkMemoryAllocation(node);
	strcpy(node->instrument, str);
	node->left = left;
	node->right = right;
	node->insId = count;
	return node;
}

int findInsId(InstrumentTree tree, char* instrument) {
	// find the index 
	TreeNode* root = tree.root;
	int id = findInsIdRec(root, instrument);
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
}

int buildMusicianGroupArr(FILE* musiciansFile, Musician*** musicianArr, InstrumentTree tree)
{
	int i, nofMusicians = countNofMusicians(musiciansFile);
	Musician** musicianGroup = (Musician**)malloc(nofMusicians * sizeof(Musician*));
	checkMemoryAllocation(musicianGroup);

	for (i = 0; i < nofMusicians; i++)
	{
		Musician* curMus = (Musician*)malloc(sizeof(Musician));
		checkMemoryAllocation(curMus);
		int nofSubStrings, firstInsIndInArr;
		unsigned short firstInsId;

		char** subStringsArr = buildArrOfSubStrings(musiciansFile, &nofSubStrings);

		firstInsIndInArr = getMusicianName(subStringsArr, &curMus, &firstInsId, tree);
		int nameSize = curMus->nameParts;

		MPIList lst;
		makeEmptyList(&lst);
		int nofInstruments = (nofSubStrings - firstInsIndInArr) / 2;
		buildMpiList(subStringsArr + firstInsIndInArr, nofInstruments, &lst, firstInsId, tree);
		curMus->instruments = lst;
		musicianGroup[i] = curMus;
		freeArr(subStringsArr, nofSubStrings);
	}
	*musicianArr = musicianGroup;

	return nofMusicians;
}

int countNofMusicians(FILE* musiciansFile)
{
	int num = 0;
	char* c;
	char str[MAX_LINE_LEN];
	c = fgets(str, MAX_LINE_LEN, musiciansFile);
	while (c != NULL)
	{
		num++;
		c = fgets(str, MAX_LINE_LEN, musiciansFile);
	}
	rewind(musiciansFile);
	return num;
}

char** buildArrOfSubStrings(FILE* musiciansFile, int* nofSubStrings)
{
	int curLen = 0, logSize = 0, phySize = MIN_PARTS_IN_LINE;
	char* token, line[MAX_LINE_LEN], delims[] = " ,.;:?!-\t'()[]{}<>~_";
	char** arr = (char**)malloc(sizeof(char*) * phySize);

	fgets(line, MAX_LINE_LEN, musiciansFile);
	token = strtok(line, delims);

	while (token != NULL)
	{
		if (logSize == phySize)
		{
			phySize *= 2;
			arr = (char**)realloc(arr, phySize * sizeof(char*));
			checkMemoryAllocation(arr);
		}
		curLen = strlen(token);
		arr[logSize] = (char*)malloc((curLen + 1) * sizeof(char));
		strcpy(arr[logSize], token);
		logSize++;
		token = strtok(NULL, delims);
	}
	if (logSize != phySize) arr = (char**)realloc(arr, logSize * sizeof(char*));
	*nofSubStrings = logSize;
	return arr;
}

int getMusicianName(char** subStringsArr, Musician** curMusician, unsigned short* firstInsId, InstrumentTree tree)
{
	int curLen = 0, logSize = 0, phySize = MIN_NAME_PARTS, insId = EOF;  //isIns = EOF if not instrument, else if instrument = contains insId
	char** name = (char**)malloc(phySize * sizeof(char*));
	checkMemoryAllocation(name);

	while (insId == EOF)
	{
		if (logSize >= 2) insId = findInsId(tree, subStringsArr[logSize]);
		if (insId == EOF)
		{
			if (logSize == phySize)
			{
				phySize *= 2;
				name = (char**)realloc(name, phySize * sizeof(char*));
				checkMemoryAllocation(name);
			}
			curLen = subStringsArr[logSize];
			name[logSize] = (char*)malloc((curLen + 1) * sizeof(char));
			strcpy(name[logSize], subStringsArr[logSize]);
			logSize++;
		}
	}
	if (logSize != phySize) name = (char**)realloc(name, logSize * sizeof(char*));
	checkMemoryAllocation(name);
	(*curMusician)->name = name;
	(*curMusician)->nameParts = logSize;
	*firstInsId = insId;
	return logSize;
}

void buildMpiList(char** arr, int nofInstruments, MPIList* lst, unsigned short firstInsId, InstrumentTree tree)
{
	MPIList curLst;
	makeEmptyList(&curLst);
	int i, j = 0, curPrice;

	for (i = 0; i < nofInstruments; i++)
	{
		MusicianPriceInstrument* mpi = (MusicianPriceInstrument*)malloc(sizeof(MusicianPriceInstrument));
		if (i == 0) mpi->insId = firstInsId;
		else mpi->insId = (unsigned short)findInsId(tree, arr[j]);
		j++;
		curPrice = atoi(arr[j]);
		j++;
		mpi->price = (float)curPrice;
		insertDataToEndList(&curLst, mpi);
	}
	*lst = curLst;
}

void freeArr(char** arr, int size)
{
	int i;
	for (i = 0; i < size; i++)
		free(arr[i]);

	free(arr);
}


void makeEmptyList(MPIList* lst)
{
	lst->head = lst->tail = NULL;
}

bool isEmptyList(MPIList lst)
{
	return (lst.head == NULL);
}

void insertDataToStartList(MPIList* lst, MusicianPriceInstrument* mpi)
{
	ListNode* newHead;

	newHead = createNewListNode(mpi, NULL);
	insertNodeToStartList(lst, newHead);
}

ListNode* createNewListNode(MusicianPriceInstrument* mpi, ListNode* next)
{
	ListNode* node;
	node = (ListNode*)malloc(sizeof(ListNode));
	checkMemoryAllocation(node);

	node->mpi = mpi;
	node->next = next;

	return node;
}

void insertNodeToStartList(MPIList* lst, ListNode* newHead)
{
	if (isEmptyList(*lst))
		lst->head = lst->tail = newHead;
	else
	{
		newHead->next = lst->head;
		lst->head = newHead;
	}
}

void insertDataToEndList(MPIList* lst, MusicianPriceInstrument* mpi)
{
	ListNode* newTail;
	newTail = createNewListNode(mpi, NULL);
	insertNodeToEndList(lst, newTail);
}

void insertNodeToEndList(MPIList* lst, ListNode* newTail)
{
	if (isEmptyList(*lst))
		lst->head = lst->tail = newTail;
	else
	{
		lst->tail->next = newTail;
		lst->tail = newTail;
	}
}

void freeList(MPIList* lst)
{
	freeListRec((*lst).head);
}

void freeListRec(ListNode* head)
{
	if (head == NULL)
		return;
	else
	{
		freeListRec(head->next);
		free(head->mpi);
		free(head);
	}
}


void checkMemoryAllocation(void* ptr)
{
	if (ptr == NULL)
	{
		printf("Memory Allocation failed.");
		exit(1);
	}
}

void checkFile(FILE* f)
{
	if (f == NULL)
	{
		printf("Opening file failed!\n");
		exit(-1);
	}
}

void printMusicianCollection(IMix* MusiciansCollection, int treeSize) {
	// print the musician collection - just for SELF checking
	for (int i = 0; i < treeSize; i++) {
		printf("%d - %d %d ", i, MusiciansCollection[i].logSize, MusiciansCollection[i].psySize);
		for (int j = 0; j < MusiciansCollection[i].logSize; j++)
			printf("%s, price: %f | ", MusiciansCollection[i].arr[j]->musicianPtr->name[0], MusiciansCollection[i].arr[j]->priceForIns);
		printf("\n");
	}
}
