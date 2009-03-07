#include <cstdio>
#include <cstdlib>
#include "File.h"
#include "Ripper.h"
#include "PatternMatcher.h"
#include "BMPRipper.h"
#include "RIFFRipper.h"
#include "MIDIRipper.h"
#include "ICORipper.h"
#include "MODRipper.h"
#include "IFFRipper.h"
#include "VOCRipper.h"
#include "GIFRipper.h"
#include "JPEGRipper.h"
#include "RTFRipper.h"
#include "PDFRipper.h"
#include "PVRRipper.h"
#include "DDSRipper.h"
#include "MPEGRipper.h"

#define MAX_HEADERS 1024
#define MAX_RIPPERS 256

struct Header {
	HeaderStruct header;
	Ripper *ripper;
};

Ripper **rippers;
int numHeaders = 0;
int numRippers = 0;
unsigned long totalCalls = 0;
unsigned long failedCalls = 0;
unsigned long starttime;
unsigned long stoptime;
File *f;
PatternMatcher *pm = new PatternMatcher();

void AddHeaders(Ripper *ripper)
{
	const HeaderStruct *hd = ripper->getHeaders();

	while (hd->length != 0)
	{
    pm->AddPattern(hd, ripper);
    hd++;
	}
}

void SaveFile(FoundStruct *fstruct)
{
	static const char fNameTemplate[] = "RIPPED_%08d%s.%s";
	static unsigned long fileNumber = 0;
	FILE *out;

	char fNameBuf[512];
	sprintf(fNameBuf, fNameTemplate, fileNumber++, fstruct->criterium == CRIT_STRONG ? "" : "WEAK" ,fstruct->extension);

	out = fopen(fNameBuf, "wb");
	fwrite(fstruct->startoffset, 1, fstruct->length, out);
	fclose(out);
}

void PrintStatistics()
{
	unsigned long secs = (stoptime - starttime) / 1000;
	fprintf(stderr, "%d MB scanned in %ld seconds, %ld of %ld checks successful (%5.1f%%)\n(%d patterns and %d ripper modules loaded)\n",
		f->getSize() / (1024*1024),
		secs,
		totalCalls - failedCalls,
		totalCalls,
		100.0 * (totalCalls - failedCalls)/totalCalls,
		numHeaders,
		numRippers);
}

void ShowRippers()
{
	fprintf(stdout, "Installed ripper modules:\n");
	for (Ripper **rip = rippers; *rip != 0; rip++)
	{
		fprintf(stdout, "  %s\n", (*rip)->getName());
	}
	_exit(0);
}

int main(int argc, char *argv[])
{
	unsigned long fileSize;
	unsigned char *fileBase;
	unsigned long pos;
	unsigned long percent = -1;
	unsigned long newPercent = 0;
	Ripper **rip;
	FoundStruct fstruct;

	rippers = (Ripper **)malloc(MAX_RIPPERS * sizeof(Ripper *));

	// TODO: add new modules here
	rippers[numRippers++] = new BMPRipper();
	rippers[numRippers++] = new RIFFRipper();
	rippers[numRippers++] = new MIDIRipper();
	rippers[numRippers++] = new ICORipper();
	rippers[numRippers++] = new MODRipper();
	rippers[numRippers++] = new IFFRipper();
	rippers[numRippers++] = new VOCRipper();
	rippers[numRippers++] = new GIFRipper();
	rippers[numRippers++] = new JPEGRipper();
	rippers[numRippers++] = new RTFRipper();
	rippers[numRippers++] = new PDFRipper();
	rippers[numRippers++] = new PVRRipper();
	rippers[numRippers++] = new DDSRipper();
	rippers[numRippers++] = new MPEGRipper();
	// TODO: add new modules here

	rippers[numRippers] = 0; // terminate list

	fprintf(stderr, "Ripper v1.0 (C) 2005 by Darkstar <darkstar@drueing.de>\n\n");
	if (argc != 2)
	{
		fprintf(stderr, "Usage: ripper <file_to_rip>\n");
		fprintf(stderr, "       ripper -l\n");
		fprintf(stderr, "       ripper -list           -- list all installed rip modules\n");
		_exit(1);
	}
	if ((stricmp(argv[1], "-list") == 0) || (stricmp(argv[1], "-l") == 0))
	{
		ShowRippers();
	}
	f = new File(argv[1]);
	fileBase = (unsigned char *)f->Open();
	if (!fileBase)
	{
    fprintf(stderr, "Error opening file %s: %s\n", argv[1], f->getError());
		_exit(2);
	}
	fileSize = f->getSize();

  // new: initialize Aho-Corasick search algorithm
	rip = rippers;
	while (*rip)
	{
		AddHeaders(*rip);
		(*rip)->setupFile(fileBase, fileSize);
		rip++;
	}
  pm->FinalizeMatcher(); // generate fail-functions

  pm->BeginSearch(); // initialize search state

	starttime = GetTickCount();

	// main ripping loop
	for (pos = 0; pos < fileSize; pos++)
	{
		// show progress
		newPercent = (unsigned long)(100.0 * (double)pos / (double)fileSize);
		if (newPercent != percent)
		{
			percent = newPercent;
			fprintf(stderr, "%3d%% done...\r", percent);
		}

    // Aho-Corasick
    PatternMatcher::ACNode *q = 0;
    if (pm->NextByte(*(unsigned char *)(fileBase + pos), &q))
    {
			totalCalls++;
			// possible match found
      if (q->ripper->checkLocation(fileBase + pos - q->header->length + 1, q->header, &fstruct))
			{
				// found it! save...
				fprintf(stderr, "found %s at 0x%08x!\n", fstruct.extension, fstruct.startoffset - fileBase);
				SaveFile(&fstruct);
				// TODO: check if this makes sense, maybe remove it or make it selectable by command line
				pos = (unsigned long)(fstruct.startoffset - fileBase) + fstruct.length - 1;
			}
			else
			{
				failedCalls++;
			}
    }
	}
	stoptime = GetTickCount();

	fprintf(stderr, "100.0%% complete. ALL DONE.\n");
	PrintStatistics();

	return 0;
}
