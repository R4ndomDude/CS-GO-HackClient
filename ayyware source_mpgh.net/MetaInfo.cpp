/*
Syn's AyyWare Framework
*/

#include "MetaInfo.h"
#include "Utilities.h"

void PrintMetaHeader()
{
	printf("                                  Ayy");
	Utilities::SetConsoleColor(FOREGROUND_INTENSE_GREEN);
	printf("Ware\n");
	Utilities::SetConsoleColor(FOREGROUND_WHITE);
	Utilities::Log("Build %s", __DATE__);
	Utilities::Log("Setting Up AyyWare for %s", AYYWARE_META_GAME);
}