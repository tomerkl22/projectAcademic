#include "Header.h"

void main(int argc, char* argv[]) {

	FILE* f;
	InstrumentTree tr;
	tr.root = NULL;

	f = fopen(argv[1], "r");
	checkFile(f);
	tr.root = BuildTheTree(f ,tr.root);

	printf("");
}