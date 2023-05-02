//
//  Engine.h
//  ListEngine
//
//  Created by Wael Youssef on 01/05/2023.
//

#ifndef Engine_h
#define Engine_h

#include <stdio.h>
#import "StructureDeclrations.h"
#include <string.h>

DataHeader ls_dataHeader(void);
DataSystem ls_mainData(void);
DataSystem ls_archivedData(void);

bool ls_setupEngine(void * diskData);
void ls_initializeDataForTheFirstTime(void);
bool ls_freeDataD(unsigned char D);
bool ls_freeAllData(void);
unsigned int ls_loadDataFromBuffer(const void * DataBuffer, unsigned int SegmentLocation, unsigned char D);
bool ls_loadDataFromDisk(void *DataBuffer);
unsigned int ls_saveDataToBuffer(void * DataBuffer, unsigned int SegmentLocation, unsigned char D);
BufferResult ls_saveDataToDisk(void);

Date ls_getTimeStampID(void);
bool ls_dateIsEqualToDate(Date First, Date Second);
bool ls_dateIsBiggerThanDate(Date First, Date Second);
unsigned short ls_indexOfIDInList(Date ID, ChronoHeader **IDList, unsigned short IDCount);
ItemPath ls_itemIDExistsInSystem(Date ItemID, unsigned char D);
IDConfirmation ls_idExistsInList(Date ID, ChronoHeader **IDList, unsigned short IDCount);
bool ls_equalTitles(void *title1, void *title2, unsigned int Size);
TitleConfirmation ls_listWithTitleExists(void *Title, unsigned int TitleSize, unsigned char D);
bool ls_stringHasText(char *S, unsigned int SLength);
CleanS ls_cleanString(char *S, unsigned int SLength);
bool la_addListAtIndex(unsigned short ListIndex, unsigned short Color, void * Title, unsigned int TitleSize, unsigned char D);
bool ls_addItemAtIndex(unsigned short ItemIndex, unsigned short ListIndex, unsigned short Color, unsigned char Notification, ShortDate date, void * Title, unsigned int TitleSize, bool HasID, Date ID, unsigned char D);
bool ls_archiveListAtIndex(unsigned short FListIndex, bool Reverse);
bool ls_archiveItemAtIndex(unsigned short FItemIndex, unsigned short FListIndex, bool Reverse);
bool ls_deleteListAtIndex(unsigned short ListIndex, unsigned char D);
bool ls_deleteItemAtIndex(unsigned short ItemIndex, unsigned short ListIndex, unsigned char D);
bool ls_moveListFromIndex(unsigned short FromIndex, unsigned short ToIndex, unsigned char D);
bool ls_moveItemInListAtIndex(unsigned short ListIndex, unsigned short FromIndex, unsigned short ToIndex, unsigned char D);
bool ls_initializeListArrayD(unsigned char D);
bool ls_initializeItemArrayAtIndex(unsigned short ListIndex, unsigned char D);
bool ls_resizeListArrayD(unsigned char D);
bool ls_resizeItemArrayAtIndex(unsigned short ListIndex, unsigned char D);


#endif /* Engine_h */
