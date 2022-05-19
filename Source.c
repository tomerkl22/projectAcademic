#include "Header.h"

void main(int argc, char* argv[])
{
	FILE* f;
	InstrumentTree tr;
	tr.root = NULL;
	int treeSize;

	// question 1
	f = fopen(argv[1], "r");
	checkFile(f);
	tr.root = BuildTheTree(f, tr.root, &treeSize);

	// question 3
	FILE* musiciansFile = fopen(argv[2], "r");
	checkFile(musiciansFile);
	Musician** MusicianGroup;
	buildMusicianGroupArr(musiciansFile, &MusicianGroup, tr);
	
	// question 4
	Musician** MusiciansCollection = (Musician**)malloc(sizeof(Musician*) * treeSize); 
	buildMuiciansCollection(MusiciansCollection, treeSize);
}
	