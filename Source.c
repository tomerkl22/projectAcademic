#include "Header.h"

void main(int argc, char* argv[])
{
	FILE* f;
	InstrumentTree tr;
	tr.root = NULL;

	f = fopen(argv[1], "r");
	checkFile(f);
	tr.root = BuildTheTree(f, tr.root);

	FILE* musiciansFile = fopen(argv[2], "r");
	checkFile(musiciansFile);
	Musician** MusicianGroup;
	buildMusicianGroupArr(musiciansFile, &MusicianGroup, tr);
}
	