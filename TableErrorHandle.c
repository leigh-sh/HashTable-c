#include <stdio.h>
#include "TableErrorHandle.h"

void ReportError(TableErrors tableError)
{
	switch (tableError) {
		case MEM_OUT:
			fprintf(stderr, "ERROR: Out Of Memory\n");
			return;
		default:
			fprintf(stderr, "ERROR: General error.\n");
			return;
	}
}
