#include "Header.h"

void main(int argc, char* argv[])
{
	FILE* f;
	InstrumentTree tr;
	tr.root = NULL;
	int treeSize; // the size of the tree
	int numOfmusicians;

	// question 1
	f = fopen(argv[1], "r");
	checkFile(f);
	tr.root = BuildTheTree(f, tr.root, &treeSize);
	fclose(f);

	// question 3
	FILE* musiciansFile = fopen(argv[2], "r");
	checkFile(musiciansFile);
	Musician** MusicianGroup;
	numOfmusicians = buildMusicianGroupArr(musiciansFile, &MusicianGroup, tr);
	fclose(musiciansFile);

	// question 4
	IMix* MusiciansCollection = (IMix*)malloc(sizeof(IMix) * treeSize);
	buildMusiciansCollection(MusiciansCollection, MusicianGroup, numOfmusicians, treeSize);

	// question 5
	getConcertSortAndPrintDetails(MusiciansCollection, tr, treeSize);

	//free all allocated memory
	freeTree(tr);
	freeMusicianGroupArr(MusicianGroup, numOfmusicians);
	freeMusiciansCollection(MusiciansCollection, treeSize);

}