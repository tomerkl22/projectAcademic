#include "finalProjectHeader.h"

// question 1 functions

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

	checkMemoryAllocation(node);
	strcpy(node->instrument, str);
	node->left = left;
	node->right = right;
	node->insId = count;
	return node;
}


// question 2 functions

int findInsId(InstrumentTree tree, char* instrument) {
	// find the index 
	TreeNode* root = tree.root;
	int id = findInsIdRec(root, instrument);
	return id;
}

int findInsIdRec(TreeNode* root, char* instrument) {
	// find the index of the instrument recursive
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


// question 3 functions

int buildMusicianGroupArr(FILE* musiciansFile, Musician*** musicianArr, InstrumentTree tree)
{  //Building an aray of musicians from file. For each musicin saving his name, and a list of the instruments he's playing
	int i, nofMusicians = countNofMusicians(musiciansFile);
	Musician** musicianGroup = (Musician**)malloc(nofMusicians * sizeof(Musician*));
	checkMemoryAllocation(musicianGroup);

	for (i = 0; i < nofMusicians; i++)
	{
		Musician* curMus = (Musician*) malloc(sizeof(Musician));
		checkMemoryAllocation(curMus);
		int nofSubStrings, firstInsIndInArr;
		unsigned short firstInsId;

		char** subStringsArr = buildArrOfSubStrings(musiciansFile, &nofSubStrings);
		
		firstInsIndInArr = getMusicianName(subStringsArr, &curMus, &firstInsId ,tree);
		int nameSize = curMus->nameParts;

		MPIList lst;
		makeEmptyMpiList(&lst);
		int nofInstruments = (nofSubStrings - firstInsIndInArr) / 2;
		buildMpiList(subStringsArr + firstInsIndInArr, nofInstruments, &lst, firstInsId, tree);
		//building a list of instruments the musician is playing
		curMus->instruments = lst;
		curMus->freeToPlay = true;
		musicianGroup[i] = curMus;
		freeInsAndPricesStringsInSubStringsArr(subStringsArr, nofSubStrings, firstInsIndInArr);
	}
	*musicianArr = musicianGroup;
	return nofMusicians;
}

int countNofMusicians(FILE* musiciansFile)
{  //Function to count number of instruments in file, in order to malloc the right amount.
   //At the end - rewind to the start of the file
	int num = 0;
	char* c;
	char str[MAX_LINE_LEN];
	c = fgets(str, MAX_LINE_LEN,musiciansFile);
	while (c != NULL)
	{
		num++;
		c = fgets(str, MAX_LINE_LEN, musiciansFile);
	}
	rewind(musiciansFile);
	return num;
}

char** buildArrOfSubStrings(FILE* musiciansFile, int* nofSubStrings)
{	//Building an array of strings from specific line from the file.
	//each string is one part of the line
	int curLen = 0, logSize = 0, phySize = MIN_PARTS_IN_LINE;
	char * token, line[MAX_LINE_LEN], delims[] = " ,.;:?!-\t'()[]{}<>~_";
	char** arr = (char**)malloc(sizeof(char*) * phySize);
	checkMemoryAllocation(arr);

	fgets(line, MAX_LINE_LEN, musiciansFile);
	token = strtok(line, delims);
	
	while (token != NULL && token[0] != '\n')
	{
			if (logSize == phySize)
			{
				phySize *= 2;
				arr = (char**)realloc(arr, phySize * sizeof(char*));
				checkMemoryAllocation(arr);
			}
			curLen = strlen(token);
			arr[logSize] = (char*)malloc((curLen + 1) * sizeof(char));
			checkMemoryAllocation(arr[logSize]);
			strcpy(arr[logSize], token);
			logSize++;
			token = strtok(NULL, delims);
	}
	if (logSize != phySize) arr = (char**)realloc(arr, logSize * sizeof(char*));
	checkMemoryAllocation(arr);
	*nofSubStrings = logSize;
	return arr;
}

int getMusicianName(char** subStringsArr, Musician** curMusician, unsigned short* firstInsId, InstrumentTree tree)
{	//Saving the name of the musician from subStrings array, in current musician. Also saving number of name parts for later printing.
	//no extra malloc - each pointer from the name will point to to the relevant string in subStringsArr
	int curLen = 0, logSize = 0, phySize = MIN_NAME_PARTS, insId = EOF;  //isIns = EOF if not an instrument, else if instrument = contains insId
	char** name = (char**)malloc(phySize * sizeof(char*));
	checkMemoryAllocation(name);

	while (insId == EOF)  //if got an id - end loop of name.
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
			name[logSize] = subStringsArr[logSize];
			logSize++;
		}
	}
	if (logSize != phySize) name = (char**)realloc(name, logSize * sizeof(char*));
	checkMemoryAllocation(name);
	(*curMusician)->name = name;
	(*curMusician)->nameParts = logSize;
	*firstInsId = insId; // return the first instrument id as an output parameter
	return logSize;
}

void buildMpiList(char** arr, int nofInstruments, MPIList* lst, unsigned short firstInsId, InstrumentTree tree)
{	//building a list of pointers to instruments for current musician
	MPIList curLst;
	makeEmptyMpiList(&curLst);
	int i, j = 0, curPrice;
	
	for (i = 0; i < nofInstruments; i++)
	{
		MusicianPriceInstrument* mpi = (MusicianPriceInstrument*)malloc(sizeof(MusicianPriceInstrument));
		checkMemoryAllocation(mpi);
		if (i == 0) mpi->insId = firstInsId;
			else mpi->insId = (unsigned short)findInsId(tree, arr[j]);
		j++;
		curPrice = atoi(arr[j]);
		j++;
		mpi->price = (float)curPrice;
		insertDataToEndMpiList(&curLst, mpi);
	}
	*lst = curLst;
}

void freeInsAndPricesStringsInSubStringsArr(char** arr, int size, int firstIndToDeleteFrom)
{	//free unnecessary strings in SubStringsArr (not include name parts) and free arr allocated memory
	int i;
	for (i = firstIndToDeleteFrom; i < size; i++)
		free(arr[i]);

	free(arr);
}


void makeEmptyMpiList(MPIList* lst)
{  //creating an empty list of MPI (MusicianPriceInstrument) for musician
	lst->head = lst->tail = NULL;
}

bool isEmptyMpiList(MPIList lst)
{  //checking if MPI list is empty
	return (lst.head == NULL);
}

void insertDataToEndMpiList(MPIList* lst, MusicianPriceInstrument* mpi)
{  //inserting pointer to MusicianPriceInstrument to end of MPI list
	MPIListNode* newTail;
	newTail = createNewMpiListNode(mpi, NULL);
	insertNodeToEndMpiList(lst, newTail);
}

MPIListNode* createNewMpiListNode(MusicianPriceInstrument* mpi, MPIListNode* next)
{	//creating and returning a new list node for MPI list, with pointer to MusicianPriceInstrument and pointer to next node
	MPIListNode* node;
	node = (MPIListNode*)malloc(sizeof(MPIListNode));
	checkMemoryAllocation(node);

	node->mpi = mpi;
	node->next = next;

	return node;
}

void insertNodeToEndMpiList(MPIList* lst, MPIListNode* newTail)
{  //inserting newTail to the end of the list
	if (isEmptyMpiList(*lst))
		lst->head = lst->tail = newTail;
	else
	{
		lst->tail->next = newTail;
		lst->tail = newTail;
	}
}


// question 4 functions

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
													//free(MusiciansCollection[i].arr[j]->musicianPtr);  ************not needed - double malloc*********
			free(MusiciansCollection[i].arr[j]);
		}
		MusiciansCollection[i].arr = (MusicianPtrByIns**)realloc(MusiciansCollection[i].arr, sizeof(MusicianPtrByIns*) * size);
		checkMemoryAllocation(MusiciansCollection[i].arr);
	}

}

void buildMemoryCollection(IMix* MusiciansCollection, int treeSize) {
	// malloc the array
	for (int i = 0; i < treeSize; i++) {
		MusiciansCollection[i].arr = (MusicianPtrByIns**)malloc(sizeof(MusicianPtrByIns*));
		checkMemoryAllocation(MusiciansCollection[i].arr);
		MusiciansCollection[i].arr[0] = (MusicianPtrByIns*)malloc(sizeof(MusicianPtrByIns)); // at least 1 musician
		checkMemoryAllocation(MusiciansCollection[i].arr[0]);
		//MusiciansCollection[i].arr[0]->musicianPtr = (Musician*)malloc(sizeof(Musician));   ************not needed - double malloc, just need to asign the previous address allocated befor****
		MusiciansCollection[i].logSize = 0;
		MusiciansCollection[i].psySize = 1;
	}
}

void updateMusicianCollectionByInstrumentList(IMix* MusiciansCollectionInstru, Musician* musicianTmp, MPIListNode* head)
{ // Go through the list of the instrument and update the musician collection

	MPIListNode* curr = head;
	int index, nextInx;
	float curPrice;

	while (curr != NULL) {
		index = curr->mpi->insId; // get the instrument id that the musician is playing with
		curPrice = curr->mpi->price;
		nextInx = checkSize(&(MusiciansCollectionInstru[index])); // check the size of the musician array inside the instrument array
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
		tmp->arr = (MusicianPtrByIns**)realloc(tmp->arr, sizeof(MusicianPtrByIns*) * tmp->psySize);
		checkMemoryAllocation(tmp->arr);
		int logs = tmp->logSize, pizS = tmp->psySize;
		for (int i = logs; i < pizS; i++) {
			tmp->arr[i] = (MusicianPtrByIns*)malloc(sizeof(MusicianPtrByIns));
			checkMemoryAllocation(tmp->arr[i]);
			//tmp->arr[i]->musicianPtr = (Musician*)malloc(sizeof(Musician));   ************not needed - double malloc********
		}
	}
	return tmp->logSize;
}


// question 5 functions

void getConcertSortAndPrintDetails(IMix* musiciansCollection, InstrumentTree tr,  int sizeTree)
{	//Function to manage all concert related functions - getting the input of a concert,
	//Sorting relevent musiciansCollection arrays, getting all available musicians for the show,
	//If concert can happen or not - printing relevant message.
	//The function will end after there are no more concerts

	Concert* curConcert = (Concert*)malloc(sizeof(Concert));
	checkMemoryAllocation(curConcert);
	bool gotAllMusForShow = true, gotMoreConcerts = getConcertDetails(curConcert, tr);

	while (gotMoreConcerts)
	{
		makeAllMusiciansFreeToPlay(musiciansCollection, sizeTree); // change all musicians freeToPlay to true
		sortMusiciansCollection(musiciansCollection, curConcert);

		curConcert->totalConcertPrice = 0;
		gotAllMusForShow = getMusiciansForShow(musiciansCollection, curConcert);
		 
		if (!gotAllMusForShow) printf("Could not find musicians for the concert %s\n", curConcert->name);
		else printShowDetails(curConcert);

		free(curConcert->name);

		gotMoreConcerts = getConcertDetails(curConcert, tr);  //Get next Concert details
	}
	free(curConcert);
}

bool getConcertDetails(Concert* curConcert, InstrumentTree tr)
{	//getting all details for the concert from user input - name, date, hour, and a list of instruments needed, including num and importance of each one
	int hourNum; float minutes;
	char c, * token, tempStr[MAX_LINE_LEN], delims[] = " :";
	CIList concertInstList; makeEmptyCIList(&concertInstList);

	gets(tempStr);
	token = strtok(tempStr, delims);
	if (token == NULL) return false;  //New line sign for no more concerts
	else {
		curConcert->name = (char*)malloc((strlen(token) + 1) * sizeof(char));  //allocating memory for concert name
		checkMemoryAllocation(curConcert->name);
		strcpy(curConcert->name, token);
		curConcert->date_of_concert.day = atoi(token = strtok(NULL, delims));
		curConcert->date_of_concert.month = atoi(token = strtok(NULL, delims));
		curConcert->date_of_concert.year = atoi(token = strtok(NULL, delims));
		hourNum = atoi(token = strtok(NULL, delims));
		minutes = (float) atoi(token = strtok(NULL, delims)) /(float) NOF_MINUTES_IN_HOUR;
		curConcert->date_of_concert.hour = hourNum + minutes; //calculating hour and minutes for one float num, and assigning the result to the hour field in the concert
		
		token = strtok(NULL, delims);
		while (token != NULL && token[0] != '\n') {
			ConcertInstrument* curConcertInstPtr = (ConcertInstrument*) malloc(sizeof(ConcertInstrument));
			checkMemoryAllocation(curConcertInstPtr);
			strcpy(curConcertInstPtr->name, token);   //save name of current instrument for later printing, if needed
			curConcertInstPtr->inst = findInsId(tr, token);
			curConcertInstPtr->num = atoi(token = strtok(NULL, delims));
			token = strtok(NULL, delims); curConcertInstPtr->importance = token[0];
			curConcertInstPtr->musicianArrIns = NULL;  //defined as NULL in order to free the list correctly, if there are not enough musicians for the show, some musicianArrIns arrays won't ba allocated
			insertDataToEndCIList(&concertInstList, curConcertInstPtr);
			token = strtok(NULL, delims);
		}
		curConcert->instruments = concertInstList;
		return true;
	}
}

void makeAllMusiciansFreeToPlay(IMix* musiciansCollection, int sizeTree) {
	//after finishing with one concert, and before getting musicians for the next concert,
	//changing all musicians freeToPlay parameter to true

	for (int i = 0; i < sizeTree; i++) {

		int arraySize = musiciansCollection[i].logSize;
		for (int j = 0; j < arraySize; j++) {
			musiciansCollection[i].arr[j]->musicianPtr->freeToPlay = true;
		}
	}
}

void sortMusiciansCollection(IMix* musiciansCollection, Concert* curConcert)
{	//Sorting the array musiciansCollection for current concert.
	//For each instrument, sorting the relevant array of pointers to musicians, using merge sort algorithm.
	//The sorting is from the cheapest to most expensive or vice versa, according to wanted importance for the show.

	int curInsId;
	char curImportance;
	ConInsListNode* curConInsListNode = curConcert->instruments.head;
	
	while (curConInsListNode != NULL)
	{
		curInsId = curConInsListNode->concertInstPtr->inst;  		
		curImportance = curConInsListNode->concertInstPtr->importance;
		mergeSort(musiciansCollection[curInsId].arr, musiciansCollection[curInsId].logSize, curImportance);
		curConInsListNode = curConInsListNode->next;
	}
} 

void mergeSort(MusicianPtrByIns** arr, int size, char importance)
{	//recursive function to sort current musiciansCollection array of pointers, of wanted insID.
	//The sorting is from the cheapest to most expensive or vice versa, according to wanted importance for the show.

	int i;
	MusicianPtrByIns** tmpArr = NULL;

	if (size <= 1) return;
	mergeSort(arr, size / 2, importance);
	mergeSort(arr + size / 2, size - size / 2, importance);

	tmpArr = (MusicianPtrByIns**)malloc(size * sizeof(MusicianPtrByIns*));
	checkMemoryAllocation(tmpArr);
	if (importance == '1') 
		mergeFromExpensiveToCheap(arr, size / 2, arr + size / 2, size - size / 2, tmpArr);
	else
		mergeFromCheapToExpensive(arr, size / 2, arr + size / 2, size - size / 2, tmpArr);

	for (i = 0; i < size; i++)
		arr[i] = tmpArr[i];

	free(tmpArr);

}

void mergeFromExpensiveToCheap(MusicianPtrByIns** arr1, int size1, MusicianPtrByIns** arr2, int size2, MusicianPtrByIns** tmpArr)
{	//merging 2 sorted arrays of musiciansCollection array of pointers, of wanted insID, to tmpArr.
	//each array is sorted from most expensive to cheapest, the algorithm is merging them in a sorted manner
	int ind1 = 0, ind2 = 0, newInd = 0;

	while (ind1 < size1 && ind2 < size2)
	{
		if (arr1[ind1]->priceForIns > arr2[ind2]->priceForIns) {
			tmpArr[newInd] = arr1[ind1];
			ind1++;
		}
		else {
			tmpArr[newInd] = arr2[ind2];
			ind2++;
		}
		newInd++;
	}

	while (ind1 < size1) {
		tmpArr[newInd] = arr1[ind1];
		ind1++;
		newInd++;
	}

	while (ind2 < size2) {
		tmpArr[newInd] = arr2[ind2];
		ind2++;
		newInd++;
	}
}

void mergeFromCheapToExpensive(MusicianPtrByIns** arr1, int size1, MusicianPtrByIns** arr2, int size2, MusicianPtrByIns** tmpArr)
{	//merging 2 sorted arrays of musiciansCollection array of pointers, of wanted insID, to tmpArr.
	//each array is sorted from cheapest to most expensive, the algorithm is merging them in a sorted manner
	int ind1 = 0, ind2 = 0, newInd = 0;

	while (ind1 < size1 && ind2 < size2)
	{
		if (arr1[ind1]->priceForIns < arr2[ind2]->priceForIns) {
			tmpArr[newInd] = arr1[ind1];
			ind1++;
		}
		else {
			tmpArr[newInd] = arr2[ind2];
			ind2++;
		}
		newInd++;
	}

	while (ind1 < size1) {
		tmpArr[newInd] = arr1[ind1];
		ind1++;
		newInd++;
	}

	while (ind2 < size2) {
		tmpArr[newInd] = arr2[ind2];
		ind2++;
		newInd++;
	}
}

bool getMusiciansForShow(IMix* musiciansCollection, Concert* curConcert)
{   //For each instrument building an array of pointers to Musicians to play in the show.
	//Calculating total price and updating in current concert
	int curInsIdNeeded, curNofInsNeeded;
	float totalPrice = 0;
	bool noMissingMusician = true;
	ConInsListNode * curWantedInsNode = curConcert->instruments.head;
	
	while (curWantedInsNode != NULL && noMissingMusician == true)
	{
		curInsIdNeeded = curWantedInsNode->concertInstPtr->inst; 
		curNofInsNeeded = curWantedInsNode->concertInstPtr->num;
		curWantedInsNode->concertInstPtr->musicianArrIns = (MusicianPtrByIns**)malloc(curNofInsNeeded * sizeof(MusicianPtrByIns*));
		checkMemoryAllocation(curWantedInsNode->concertInstPtr->musicianArrIns);
				noMissingMusician = getMusiciansForCurIns(curWantedInsNode->concertInstPtr->musicianArrIns,
			&(musiciansCollection[curInsIdNeeded]), curNofInsNeeded, &totalPrice);
		curWantedInsNode = curWantedInsNode->next;
	}
	if (!noMissingMusician) {     	//There are not enough musicians for the show, free CIList - will free only allocated musicianArrIns
		freeCIList(&(curConcert->instruments));
		return false;
	}

	curConcert->totalConcertPrice = totalPrice;
	return true;
}

bool getMusiciansForCurIns(MusicianPtrByIns** musiciansArrConcert, IMix* musiciansInsArr, int nofInsNeeded, float* totalPrice)
{	//Going through the array of MusicianCollection of current instrument, assuming it's sorted by importance,
	//and adding to the array of musicians for concert
	int i = 0, countNofInsGotten = 0, insArrLogSize = musiciansInsArr->logSize;
	float insTotalPrice = 0;

	while (i < insArrLogSize && countNofInsGotten < nofInsNeeded)
	{
		if (musiciansInsArr->arr[i]->musicianPtr->freeToPlay)  //checking if this musician is available
		{
			musiciansArrConcert[countNofInsGotten] = musiciansInsArr->arr[i];
			musiciansArrConcert[countNofInsGotten]->musicianPtr->freeToPlay = false;  //changing musician availablility
			insTotalPrice += musiciansArrConcert[countNofInsGotten]->priceForIns;
			countNofInsGotten++;
		}
		i++;
	}
	(*totalPrice) += insTotalPrice;
	if (countNofInsGotten == nofInsNeeded) return true;  //true - becase there are no missing musicians for this show
	else return false;

}

void printShowDetails(Concert* curConcert)
{	//function will be called if concert can happen - then print all details of the show: concert name, date, hour, 
	//list of musicians names, current instrument played and price, and total concert price.
	int i, hour, minutes, curNameParts, insNeeded;
	hour = (int)curConcert->date_of_concert.hour;
	minutes = (int)((curConcert->date_of_concert.hour - hour) * NOF_MINUTES_IN_HOUR);  //calculating the minutes

	printf("%s %02d %02d %d ", curConcert->name, curConcert->date_of_concert.day,
		curConcert->date_of_concert.month, curConcert->date_of_concert.year);
	printf("%02d:%02d: ", hour, minutes);

	ConInsListNode* tempNode = curConcert->instruments.head;
	while (tempNode != NULL)
	{
		MusicianPtrByIns** musiciansArrConcert = tempNode->concertInstPtr->musicianArrIns;
		insNeeded = tempNode->concertInstPtr->num;
		for (i = 0; i < insNeeded; i++)
		{
			curNameParts = musiciansArrConcert[i]->musicianPtr->nameParts;  //printing all name parts of current musician
			for (int j = 0; j < curNameParts; j++) printf("%s ", musiciansArrConcert[i]->musicianPtr->name[j]);
			if (tempNode == curConcert->instruments.tail && i == insNeeded - 1)
				printf("- %s (%.0f). ", tempNode->concertInstPtr->name, musiciansArrConcert[i]->priceForIns); //last musician printed - end print with dot
			else printf("- %s (%.0f), ", tempNode->concertInstPtr->name, musiciansArrConcert[i]->priceForIns);
		}
		tempNode = tempNode->next;
	}
	printf("Total cost: %.0f.\n", curConcert->totalConcertPrice);
	
	//Done with this concert, free unnecessary memory
	freeCIList(&(curConcert->instruments));
}

void makeEmptyCIList(CIList* lst)
{  //creating an empty list of CI (ConcertInstrument) for current concert

	lst->head = lst->tail = NULL;
}

bool isEmptyCIList(CIList lst)
{  //checking if CIList is empty

	return (lst.head == NULL);
}

void insertDataToEndCIList(CIList* lst, ConcertInstrument* ciPtr)
{  //inserting pointer to ConcertInstrument to end of CI list

	ConInsListNode* newTail;
	newTail = createNewCIListNode(ciPtr, NULL);
	insertNodeToEndCIList(lst, newTail);
}

ConInsListNode* createNewCIListNode (ConcertInstrument* ciPtr, ConInsListNode* next)
{ //creating and returning a new list node for CI list, with pointer to ConcertInstrument and pointer to next node

	ConInsListNode* node;
	node = (ConInsListNode*)malloc(sizeof(ConInsListNode));
	checkMemoryAllocation(node);

	node->concertInstPtr = ciPtr;
	node->next = next;

	return node;
}

void insertNodeToEndCIList(CIList* lst, ConInsListNode* newTail)
{  //inserting newTail to the end of the list

	if (isEmptyCIList(*lst))
		lst->head = lst->tail = newTail;
	else
	{
		lst->tail->next = newTail;
		lst->tail = newTail;
	}
}


//management functions

void freeTree(InstrumentTree tr)
{
	freeTreeRec(tr.root);
}

void freeTreeRec(TreeNode* root)  //recursive function that frees all sub trees and the root
{
	if (root == NULL) return;
	freeTreeRec(root->left);
	freeTreeRec(root->right);
	free(root);
}

void freeMusicianGroupArr(Musician** MusicianGroup, int numOfmusicians)
{	//function to free all pointers allocted in MusicianGroup array, including pointers to all name parts,
	//"char** name" - array of strings of name, and pointer to musician
	
	int i, j, curNameParts;

	for (i = 0; i < numOfmusicians; i++)
	{
		freeMpiList(&(MusicianGroup[i]->instruments));
		curNameParts = MusicianGroup[i]->nameParts;
		for (j = 0; j < curNameParts; j++) free(MusicianGroup[i]->name[j]);
		free(MusicianGroup[i]->name);
		free(MusicianGroup[i]);
	}

	free(MusicianGroup);

}

void freeMpiList(MPIList* lst)
{
	freeMpiListRec((*lst).head);
}

void freeMpiListRec(MPIListNode* head)
{	//recursive function that frees all MPI list nodes
	if (head == NULL)
		return;
	else
	{
		freeMpiListRec(head->next);
		free(head->mpi);  //free the pointer to MusicianPriceInstrument
		free(head);
	}
}

void freeMusiciansCollection(IMix* MusiciansCollection, int size)
{	//function to free all pointers allocted in MusiciansCollection array

	int i, j, curLogSize;
	for (i = 0; i < size; i++)
	{
		curLogSize = MusiciansCollection[i].logSize;
		for (j = 0; j < curLogSize; j++)
			free(MusiciansCollection[i].arr[j]);
		free(MusiciansCollection[i].arr);
	}
	free(MusiciansCollection);
}

void freeCIList(CIList* lst)
{
	freeCIListRec((*lst).head);
}

void freeCIListRec(ConInsListNode* head)
{  //recursive function that frees all MPI list nodes

	if (head == NULL)
		return;
	else
	{
		freeCIListRec(head->next);

		if (head->concertInstPtr->musicianArrIns != NULL)	//before getting musicians for the show defined all musicianArrIns as NULL,
			free(head->concertInstPtr->musicianArrIns);     //in order to free only musicianArrIns that were alloctedted,
															//and prevnting error trying to free not allocated memory
		free(head->concertInstPtr);
		free(head);
	}
}

void checkMemoryAllocation(void* ptr)
{	//checking if the memory alloction succeeded or failed. if failed - exit the program
	if (ptr == NULL)
	{
		printf("Memory Allocation failed.");
		exit(1);
	}
}

void checkFile(FILE* f)
{	//checking if the file opening succeeded or failed. if failed - exit the program

	if (f == NULL)
	{
		printf("Opening file failed!\n");
		exit(-1);
	}
}
