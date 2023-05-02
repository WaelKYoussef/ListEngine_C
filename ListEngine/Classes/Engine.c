//
//  Engine.c
//  ListEngine
//
//  Created by Wael Youssef on 01/05/2023.
//

#include "Engine.h"
#include <sys/time.h>

#pragma mark - Loading, Saving & Freeing memory.

InfoTitleList ImpFileList;
InfoTitleList DefaultsInfoList;
StaticSizes SSizes;
DataHeader DataH;
DataSystem Data[2];
//NSString *FileDirectoryPath;
InterfaceValuesHeader IValH;
InterfaceValues IVal;
//NSFileManager *FileMan;
InfoTitleList *ITL;
InfoOptions21 IOpt;
ColorValues CVal;

DataHeader ls_dataHeader(void) {
    return DataH;
}

DataSystem ls_mainData(void) {
    return Data[0];
}

DataSystem ls_archivedData(void) {
    return Data[1];
}

bool ls_setupEngine(void * diskData) {
    SSizes.ChronoHeaderSize = sizeof(ChronoHeader);
    SSizes.DataHeaderSize = sizeof(DataHeader);
    SSizes.InterfaceHeaderSize = sizeof(InterfaceValuesHeader);
    SSizes.ListHeaderSize = sizeof(ListHeader);
    SSizes.PointerSize = sizeof(void *);
    SSizes.ItemHeaderSize = sizeof(ItemHeader);
    SSizes.ColorThemesSize = sizeof(ColorValues);
    SSizes.InterfaceValuesSize = sizeof(InterfaceValues);
    SSizes.InfoOptionsSize = sizeof(InfoOptions21);
    SSizes.InfoTitleSize = sizeof(InfoTitle);
    SSizes.InfoTitleListSize = sizeof(InfoTitleList);
    ImpFileList.Titles = malloc(0);
    
    if (diskData == NULL)
        ls_initializeDataForTheFirstTime();
    else
        ls_loadDataFromDisk(diskData);
    
    if (DataH.NumberOfLists[0] == 0) {
        la_addListAtIndex(0, 5, "To Do", strlen("To Do") + 1, 0);
    }

    ITL = NULL;
    DefaultsInfoList.Titles = NULL;

    return true;
}

void ls_initializeDataForTheFirstTime(void) {
    DataH.DataSize = SSizes.DataHeaderSize;
    DataH.NumberOfLists[0] = 0;
    Data[0].ListHeaders = malloc(SSizes.PointerSize);
    Data[0].ItemHeaders = malloc(SSizes.PointerSize);
    Data[0].ItemIDs = malloc(SSizes.PointerSize);
    DataH.NumberOfLists[1] = 0;
    Data[1].ListHeaders = malloc(SSizes.PointerSize);
    Data[1].ItemHeaders = malloc(SSizes.PointerSize);
    Data[1].ItemIDs = malloc(SSizes.PointerSize);
}

bool ls_freeDataD(unsigned char D) {
    unsigned short Increment = 0;
    unsigned short ListCount = DataH.NumberOfLists[D];

    for (Increment = 0; Increment < ListCount; Increment++) {
        ls_deleteListAtIndex(0, D);
    }

    if (Data[D].ListHeaders) {
        free(Data[D].ListHeaders);
    }

    if (Data[D].ItemHeaders) {
        free(Data[D].ItemHeaders);
    }

    if (Data[D].ItemIDs) {
        free(Data[D].ItemIDs);
    }

    return true;
}

bool ls_freeAllData(void) {
    return ls_freeDataD(0) && ls_freeDataD(1);
}

unsigned int ls_loadDataFromBuffer(const void * DataBuffer, unsigned int SegmentLocation, unsigned char D) {
    ls_initializeListArrayD(D);
    unsigned int SegmentSize = 0;
    unsigned short ListCount = DataH.NumberOfLists[D], ItemCount = 0;
    unsigned short ListIncrement = 0, ItemIncrement = 0;

    for (ListIncrement = 0; ListIncrement < ListCount; ListIncrement++) {
        Data[D].ListHeaders[ListIncrement] = malloc(SSizes.ListHeaderSize);
        memcpy(Data[D].ListHeaders[ListIncrement], &DataBuffer[SegmentLocation], SSizes.ListHeaderSize);
        SegmentLocation += SSizes.ListHeaderSize;

        SegmentSize = Data[D].ListHeaders[ListIncrement][0].TitleSize;
        Data[D].ListHeaders[ListIncrement][0].Title = malloc(SegmentSize);
        memcpy(Data[D].ListHeaders[ListIncrement][0].Title,
               &DataBuffer[SegmentLocation], SegmentSize);
        SegmentLocation += SegmentSize;

        ItemCount = Data[D].ListHeaders[ListIncrement][0].NumberOfItems;
        ls_initializeItemArrayAtIndex(ListIncrement, D);

        for (ItemIncrement = 0; ItemIncrement < ItemCount; ItemIncrement++) {
            Data[D].ItemHeaders[ListIncrement][ItemIncrement] = malloc(SSizes.ItemHeaderSize);
            memcpy(Data[D].ItemHeaders[ListIncrement][ItemIncrement],
                   &DataBuffer[SegmentLocation], SSizes.ItemHeaderSize);
            SegmentLocation += SSizes.ItemHeaderSize;

            Data[D].ItemIDs[ListIncrement][ItemIncrement] = malloc(SSizes.ChronoHeaderSize);
            memcpy(Data[D].ItemIDs[ListIncrement][ItemIncrement],
                   &DataBuffer[SegmentLocation], SSizes.ChronoHeaderSize);
            SegmentLocation += SSizes.ChronoHeaderSize;

            SegmentSize = Data[D].ItemHeaders[ListIncrement][ItemIncrement][0].TitleSize;
            Data[D].ItemHeaders[ListIncrement][ItemIncrement][0].Title = malloc(SegmentSize);
            memcpy(Data[D].ItemHeaders[ListIncrement][ItemIncrement][0].Title,
                   &DataBuffer[SegmentLocation], SegmentSize);
            SegmentLocation += SegmentSize;
        }
    }

    return SegmentLocation;
}

bool ls_loadDataFromDisk(void *DataBuffer) {
    unsigned int SegmentLocation = 0;

    memcpy(&DataH, &DataBuffer[SegmentLocation], SSizes.DataHeaderSize);
    SegmentLocation += SSizes.DataHeaderSize;

    SegmentLocation = ls_loadDataFromBuffer(DataBuffer, SegmentLocation, 0);
    ls_loadDataFromBuffer(DataBuffer, SegmentLocation, 1);

    return true;
}


unsigned int ls_saveDataToBuffer(void * DataBuffer, unsigned int SegmentLocation, unsigned char D) {
    unsigned int SegmentSize = 0;
    unsigned short ListCount = DataH.NumberOfLists[D], ItemCount = 0;
    unsigned short ListIncrement = 0, ItemIncrement = 0;

    for (ListIncrement = 0; ListIncrement < ListCount; ListIncrement++) {
        memcpy(&DataBuffer[SegmentLocation], Data[D].ListHeaders[ListIncrement], SSizes.ListHeaderSize);
        SegmentLocation += SSizes.ListHeaderSize;

        SegmentSize = Data[D].ListHeaders[ListIncrement][0].TitleSize;
        memcpy(&DataBuffer[SegmentLocation],
               Data[D].ListHeaders[ListIncrement][0].Title, SegmentSize);
        SegmentLocation += SegmentSize;

        ItemCount = Data[D].ListHeaders[ListIncrement][0].NumberOfItems;

        for (ItemIncrement = 0; ItemIncrement < ItemCount; ItemIncrement++) {
            memcpy(&DataBuffer[SegmentLocation],
                   Data[D].ItemHeaders[ListIncrement][ItemIncrement], SSizes.ItemHeaderSize);
            SegmentLocation += SSizes.ItemHeaderSize;

            memcpy(&DataBuffer[SegmentLocation],
                   Data[D].ItemIDs[ListIncrement][ItemIncrement], SSizes.ChronoHeaderSize);
            SegmentLocation += SSizes.ChronoHeaderSize;

            SegmentSize = Data[D].ItemHeaders[ListIncrement][ItemIncrement][0].TitleSize;
            memcpy(&DataBuffer[SegmentLocation],
                   Data[D].ItemHeaders[ListIncrement][ItemIncrement][0].Title, SegmentSize);
            SegmentLocation += SegmentSize;
        }
    }

    return SegmentLocation;
}

BufferResult ls_saveDataToDisk(void) {
    unsigned int SegmentLocation = 0;
    void *DataBuffer = malloc(DataH.DataSize);

    memcpy(&DataBuffer[SegmentLocation], &DataH, SSizes.DataHeaderSize);
    SegmentLocation += SSizes.DataHeaderSize;

    SegmentLocation = ls_saveDataToBuffer(DataBuffer, SegmentLocation, 0);
    ls_saveDataToBuffer(DataBuffer, SegmentLocation, 1);
    
    BufferResult result;
    result.pointer = DataBuffer;
    result.size = DataH.DataSize;
    
    return result;
}

#pragma mark - Date/ID Functions.
Date ls_getTimeStampID(void) {
    struct timeval RawSeconds; gettimeofday(&RawSeconds, NULL);
    time_t RawTime; time(&RawTime);
    struct tm ProcessedTime; ProcessedTime = *localtime(&RawTime);
    Date ID = {
        ProcessedTime.tm_year + 1900, ProcessedTime.tm_mon + 1,
        ProcessedTime.tm_mday,        ProcessedTime.tm_hour,   ProcessedTime.tm_min,
        ProcessedTime.tm_sec,         RawSeconds.tv_usec
    };

    return ID;
}

bool ls_dateIsEqualToDate(Date First, Date Second) {
    if (First.Year == Second.Year) {
        if (First.Month == Second.Month) {
            if (First.Day == Second.Day) {
                if (First.Hour == Second.Hour) {
                    if (First.Minute == Second.Minute) {
                        if (First.Second == Second.Second) {
                            if (First.NSecond == Second.NSecond) {
                                return true;
                            } else {
                                return false;
                            }
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool ls_dateIsBiggerThanDate(Date First, Date Second) {
    if (First.Year == Second.Year) {
        if (First.Month == Second.Month) {
            if (First.Day == Second.Day) {
                if (First.Hour == Second.Hour) {
                    if (First.Minute == Second.Minute) {
                        if (First.Second == Second.Second) {
                            if (First.NSecond == Second.NSecond) {
                                return false;
                            } else {
                                return First.NSecond > Second.NSecond;
                            }
                        } else {
                            return First.Second > Second.Second;
                        }
                    } else {
                        return First.Minute > Second.Minute;
                    }
                } else {
                    return First.Hour > Second.Hour;
                }
            } else {
                return First.Day > Second.Day;
            }
        } else {
            return First.Month > Second.Month;
        }
    } else {
        return First.Year > Second.Year;
    }
}

unsigned short ls_indexOfIDInList(Date ID, ChronoHeader **IDList, unsigned short IDCount) {
    if (IDCount > 0) {
        unsigned short RangeStart = 0, RangeEnd = IDCount - 1, Index = 0;

        while (RangeStart < RangeEnd) {
            Index = ((RangeEnd - RangeStart) / 2) + RangeStart;

            if (ls_dateIsBiggerThanDate(ID, IDList[Index][0].ID)) {
                RangeStart = Index + 1;
            } else {
                if (Index > 0) {
                    RangeEnd = Index - 1;
                } else {
                    RangeEnd = 0;
                }
            }
        }
        Index = RangeStart;

        if (ls_dateIsBiggerThanDate(ID, IDList[Index][0].ID)) {
            return Index + 1;
        } else {
            return Index;
        }
    } else {
        return 0;
    }
}

ItemPath ls_itemIDExistsInSystem(Date ItemID, unsigned char D) {
    IDConfirmation IDConfirm = {
        false, 0, 0
    }; ListHeader *List;
    unsigned char F = 0, T = 1; switch (D) {
        case 0:{
            T = 0; break;
        }

        case 1:{
            F = 1; break;
        }

        default:break;
    }

    for (; F <= T; F++) {
        for (int i = 0; i < DataH.NumberOfLists[F]; i++) {
            List = Data[F].ListHeaders[i];
            IDConfirm = ls_idExistsInList(ItemID, Data[F].ItemIDs[i], List[0].NumberOfItems);

            if (IDConfirm.Exists) {
                ItemPath Path = {
                    true, F, i, IDConfirm.Index
                }; return Path;
            }
        }
    }

    ItemPath Path = {
        false, 0, 0, 0
    };
    return Path;
}

IDConfirmation ls_idExistsInList(Date ID, ChronoHeader **IDList, unsigned short IDCount) {
    if (IDCount > 0) {
        unsigned short Index = ls_indexOfIDInList(ID, IDList, IDCount);

        if (Index >= IDCount) {
            IDConfirmation Confirmation = {
                false, 0, 0
            }; return Confirmation;
        } else {
            IDConfirmation Confirmation = {
                false, Index, IDList[Index][0].Index
            };

            if (ls_dateIsEqualToDate(ID, IDList[Index][0].ID)) {
                Confirmation.Exists = true;
            }

            return Confirmation;
        }
    } else {
        IDConfirmation Confirmation = {
            false, 0, 0
        }; return Confirmation;
    }
}

bool ls_equalTitles(void *title1, void *title2, unsigned int Size) {
    char Title1[Size]; memcpy(Title1, title1, Size);
    char Title2[Size]; memcpy(Title2, title2, Size);

    for (int i = 0; i < Size; i++) {
        if (Title1[i] >= 65 && Title1[i] <= 90) {
            Title1[i] += 32;
        }

        if (Title2[i] >= 65 && Title2[i] <= 90) {
            Title2[i] += 32;
        }
    }

    if (!memcmp(Title1, Title2, Size)) {
        return true;
    } else {
        return false;
    }
}

#pragma mark - List Title Checking.
TitleConfirmation ls_listWithTitleExists(void *Title, unsigned int TitleSize, unsigned char D) {
    unsigned char F = 0, T = 1; switch (D) {
        case 0:{
            T = 0; break;
        }

        case 1:{
            F = 1; break;
        }

        default:break;
    }
    void *ListTitle;

    for (; F <= T; F++) {
        for (int i = 0; i < DataH.NumberOfLists[F]; i++) {
            if (Data[F].ListHeaders[i][0].TitleSize == TitleSize) {
                ListTitle = Data[F].ListHeaders[i][0].Title;

                if (ls_equalTitles(Title, ListTitle, TitleSize)) {
                    TitleConfirmation TConfirm = {
                        true, F, i
                    };
                    return TConfirm;
                }
            }
        }
    }

    TitleConfirmation TConfirm = {
        false, 0, 0
    };
    return TConfirm;
}

bool ls_stringHasText(char *S, unsigned int SLength) {
    if (SLength < 2) {
        return false;
    }

    for (unsigned int i = 0; i < SLength; i++) {
        if (S[i] != ' ' && S[i] != '\n' && S[i] != '\0') {
            return true;
        }
    }

    return false;
}

CleanS ls_cleanString(char *S, unsigned int SLength) {
    unsigned int F = 0, T = 0;
    CleanS Ret = {
        NULL, 0, false, true
    };

    for (unsigned int i = 0; i < SLength; i++) {
        if (S[i] != ' ' && S[i] != '\n') {
            if (S[i] != '@') {
                F = i; break;
            } else {
                Ret.IsList = true;
            }
        }
    }

    for (unsigned int i = SLength - 1; i >= F; i--) {
        if (S[i] != ' ' && S[i] != '\n') {
            T = i; break;
        }
    }

    Ret.SLength = T - F + 2;

    if (Ret.SLength < 2) {
        Ret.HasText = false;
    }

    Ret.S = malloc(Ret.SLength);
    memcpy(Ret.S, &S[F], Ret.SLength - 1);
    Ret.S[Ret.SLength - 1] = '\0';
    return Ret;
}

#pragma mark - Adding Lists/ItemSSizes.
bool la_addListAtIndex(unsigned short ListIndex, unsigned short Color, void * Title, unsigned int TitleSize, unsigned char D) {
    unsigned short ListCount = DataH.NumberOfLists[D];

    if (ListIndex > ListCount) {
        return false;
    }

    DataH.DataSize += (TitleSize + SSizes.ListHeaderSize);
    DataH.NumberOfLists[D]++;

    if (ListCount % 10 == 0) {
        ls_resizeListArrayD(D);
    }

    ListHeader **Lists = Data[D].ListHeaders;
    ListHeader NewListHeader = {
        0, Color, TitleSize, 0
    };
    NewListHeader.Title = malloc(TitleSize);
    memcpy(NewListHeader.Title, Title, TitleSize);
    unsigned int SegmentSize = SSizes.PointerSize * (ListCount - ListIndex);
    memmove(&Lists[ListIndex + 1], &Lists[ListIndex], SegmentSize);
    Lists[ListIndex] = malloc(SSizes.ListHeaderSize);
    memcpy(Lists[ListIndex], &NewListHeader, SSizes.ListHeaderSize);

    ItemHeader ***Items = Data[D].ItemHeaders;
    memmove(&Items[ListIndex + 1], &Items[ListIndex], SegmentSize);
    Items[ListIndex] = malloc(SSizes.PointerSize * 10);

    ChronoHeader ***ItemIDs = Data[D].ItemIDs;
    memmove(&ItemIDs[ListIndex + 1], &ItemIDs[ListIndex], SegmentSize);
    ItemIDs[ListIndex] = malloc(SSizes.PointerSize * 10);

    return true;
}

bool ls_addItemAtIndex(unsigned short ItemIndex, unsigned short ListIndex, unsigned short Color, unsigned char Notification, ShortDate date, void * Title, unsigned int TitleSize, bool HasID, Date ID, unsigned char D) {
    if (ListIndex >= DataH.NumberOfLists[D]) {
        return false;
    }

    ListHeader *List = Data[D].ListHeaders[ListIndex];
    unsigned short ItemsCount = List[0].NumberOfItems;

    if (ItemIndex > ItemsCount) {
        return false;
    }

    DataH.DataSize += (TitleSize + SSizes.ItemHeaderSize + SSizes.ChronoHeaderSize);
    List[0].NumberOfItems++;

    if (ItemsCount % 10 == 0) {
        ls_resizeItemArrayAtIndex(ListIndex, D);
    }

    ItemHeader **ItemList = Data[D].ItemHeaders[ListIndex];
    Date NewID = ls_getTimeStampID();
    ItemHeader NewItem = {
        NewID, NewID, date, Color, Notification, false, false, TitleSize, 0
    };

    if (HasID) {
        NewItem.ID = ID;
    }

    NewItem.Title = malloc(TitleSize);
    memcpy(NewItem.Title, Title, TitleSize);
    unsigned int SegmentSize = SSizes.PointerSize * (ItemsCount - ItemIndex);
    memmove(&ItemList[ItemIndex + 1], &ItemList[ItemIndex], SegmentSize);
    ItemList[ItemIndex] = malloc(SSizes.ItemHeaderSize);
    memcpy(ItemList[ItemIndex], &NewItem, SSizes.ItemHeaderSize);

    ChronoHeader **IDList = Data[D].ItemIDs[ListIndex];

    for (unsigned short ItemIncrement = 0; ItemIncrement < ItemsCount; ItemIncrement++) {
        if (IDList[ItemIncrement][0].Index >= ItemIndex) {
            IDList[ItemIncrement][0].Index++;
        }
    }

    ChronoHeader NewIDHeader = {
        NewItem.ID, ItemIndex
    };
    ItemIndex = ls_indexOfIDInList(NewItem.ID, IDList, ItemsCount);
    SegmentSize = SSizes.PointerSize * (ItemsCount - ItemIndex);
    memmove(&IDList[ItemIndex + 1], &IDList[ItemIndex], SegmentSize);
    IDList[ItemIndex] = malloc(SSizes.ChronoHeaderSize);
    memcpy(IDList[ItemIndex], &NewIDHeader, SSizes.ChronoHeaderSize);

    return true;
}

#pragma mark - Un/Archive Lists/ItemSSizes.
bool ls_archiveListAtIndex(unsigned short FListIndex, bool Reverse) {
    unsigned char F = 0, T = 1;

    if (Reverse) {
        F = 1; T = 0;
    }

    if (FListIndex >= DataH.NumberOfLists[F]) {
        return false;
    }

    ListHeader *FList = Data[F].ListHeaders[FListIndex];
    TitleConfirmation TConfirm = ls_listWithTitleExists(FList[0].Title, FList[0].TitleSize, T);
    unsigned short TListIndex = TConfirm.ListIndex;

    if (!TConfirm.Exists) {
        la_addListAtIndex(0, FList[0].Color, FList[0].Title, FList[0].TitleSize, T);
        TListIndex = 0;
    }

    ListHeader *TList = Data[T].ListHeaders[TListIndex];

    unsigned short FItemsCount = FList[0].NumberOfItems;
    unsigned short TItemsCount = TList[0].NumberOfItems;
    TList[0].NumberOfItems += FItemsCount;
    ls_resizeItemArrayAtIndex(TListIndex, T);

    ItemHeader **FItems = Data[F].ItemHeaders[FListIndex];
    ItemHeader **TItems = Data[T].ItemHeaders[TListIndex];
    unsigned short TItemIndex = 0;
    unsigned int SegmentSize = SSizes.PointerSize * (TItemsCount - TItemIndex);
    memmove(&TItems[TItemIndex + FItemsCount], &TItems[TItemIndex], SegmentSize);
    SegmentSize = SSizes.PointerSize * FItemsCount;
    memcpy(&TItems[TItemIndex], &FItems[0], SegmentSize);

    ChronoHeader **FItemsIDs = Data[F].ItemIDs[FListIndex];
    ChronoHeader **TItemsIDs = Data[T].ItemIDs[TListIndex];
    unsigned short ItemIncrement = 0;

    for (ItemIncrement = 0; ItemIncrement < TItemsCount; ItemIncrement++) {
        TItemsIDs[ItemIncrement][0].Index += FItemsCount;
    }

    unsigned short FIDIndex, TIDIndex; ItemHeader *Item;

    for (ItemIncrement = 0; ItemIncrement < FItemsCount; ItemIncrement++) {
        Item = FItems[ItemIncrement];
        FIDIndex = ls_indexOfIDInList(Item[0].ID, FItemsIDs, FItemsCount);
        TIDIndex = ls_indexOfIDInList(Item[0].ID, TItemsIDs, TItemsCount);
        SegmentSize = SSizes.PointerSize * (TItemsCount - TIDIndex);
        memmove(&TItemsIDs[TIDIndex + 1], &TItemsIDs[TIDIndex], SegmentSize);
        TItemsIDs[TIDIndex] = FItemsIDs[FIDIndex]; TItemsCount++;
    }

    FList[0].NumberOfItems = 0;
    ls_deleteListAtIndex(FListIndex, F);

    return true;
}

bool ls_archiveItemAtIndex(unsigned short FItemIndex, unsigned short FListIndex, bool Reverse) {
    unsigned char F = 0, T = 1;

    if (Reverse) {
        F = 1; T = 0;
    }

    if (FListIndex >= DataH.NumberOfLists[F]) {
        return false;
    }

    ListHeader *FList = Data[F].ListHeaders[FListIndex];

    if (FItemIndex >= FList[0].NumberOfItems) {
        return false;
    }

    ItemHeader *FItem = Data[F].ItemHeaders[FListIndex][FItemIndex];

    TitleConfirmation TConfirm = ls_listWithTitleExists(FList[0].Title, FList[0].TitleSize, T);
    unsigned short TListIndex = TConfirm.ListIndex;

    if (!TConfirm.Exists) {
        la_addListAtIndex(0, FList[0].Color, FList[0].Title, FList[0].TitleSize, T);
        TListIndex = 0;
    }

    ChronoHeader **FItemIDs = Data[F].ItemIDs[FListIndex];
    ChronoHeader **TItemIDs = Data[T].ItemIDs[TListIndex];
    ListHeader *TLists = Data[T].ListHeaders[TListIndex];
    unsigned short FIDIndex, TIDIndex;
    FIDIndex = ls_indexOfIDInList(FItem[0].ID, FItemIDs, FList[0].NumberOfItems);
    TIDIndex = ls_indexOfIDInList(FItem[0].ID, TItemIDs, TLists[0].NumberOfItems);

    unsigned short ItemsCount = TLists[0].NumberOfItems;
    TLists[0].NumberOfItems++;

    if (ItemsCount % 10 == 0) {
        ls_resizeItemArrayAtIndex(TListIndex, T);
    }

    ItemHeader **FItems = Data[F].ItemHeaders[FListIndex];
    ItemHeader **TItems = Data[T].ItemHeaders[TListIndex];
    FItemIDs = Data[F].ItemIDs[FListIndex];
    TItemIDs = Data[T].ItemIDs[TListIndex];
    unsigned short TItemIndex = 0, ItemsIncrement;

    for (ItemsIncrement = 0; ItemsIncrement < ItemsCount; ItemsIncrement++) {
        if (TItemIDs[ItemsIncrement][0].Index >= TItemIndex) {
            TItemIDs[ItemsIncrement][0].Index++;
        }
    }

    unsigned int SegmentSize = SSizes.PointerSize * (ItemsCount - TItemIndex);
    memmove(&TItems[TItemIndex + 1], &TItems[TItemIndex], SegmentSize);
    TItems[TItemIndex] = FItems[FItemIndex];
    SegmentSize = SSizes.PointerSize * (ItemsCount - TIDIndex);
    memmove(&TItemIDs[TIDIndex + 1], &TItemIDs[TIDIndex], SegmentSize);
    TItemIDs[TIDIndex] = FItemIDs[FIDIndex];
    TItemIDs[TIDIndex][0].Index = TItemIndex;

    ListHeader *FLists = Data[F].ListHeaders[FListIndex];
    FLists[0].NumberOfItems--;
    ItemsCount = FLists[0].NumberOfItems;
    SegmentSize = SSizes.PointerSize * (ItemsCount - FItemIndex);
    memmove(&FItems[FItemIndex], &FItems[FItemIndex + 1], SegmentSize);
    SegmentSize = SSizes.PointerSize * (ItemsCount - FIDIndex);
    memmove(&FItemIDs[FIDIndex], &FItemIDs[FIDIndex + 1], SegmentSize);

    if (ItemsCount % 10 == 0) {
        ls_resizeItemArrayAtIndex(FListIndex, F);
    }

    for (ItemsIncrement = 0; ItemsIncrement < ItemsCount; ItemsIncrement++) {
        if (FItemIDs[ItemsIncrement][0].Index > FItemIndex) {
            FItemIDs[ItemsIncrement][0].Index--;
        }
    }

    return true;
}

#pragma mark - Deleting Lists/ItemSSizes.
bool ls_deleteListAtIndex(unsigned short ListIndex, unsigned char D) {
    if (ListIndex >= DataH.NumberOfLists[D]) {
        return false;
    }

    ListHeader **Lists = Data[D].ListHeaders;

    ItemHeader ***Items = Data[D].ItemHeaders;
    ChronoHeader ***ItemIDs = Data[D].ItemIDs;
    unsigned short ItemsCount = Lists[ListIndex][0].NumberOfItems;
    unsigned short ListIncrement = 0;

    for (ListIncrement = 0; ListIncrement < ItemsCount; ListIncrement++) {
        DataH.DataSize -= (Items[ListIndex][ListIncrement][0].TitleSize);
        free(Items[ListIndex][ListIncrement][0].Title);
        free(Items[ListIndex][ListIncrement]);
        free(ItemIDs[ListIndex][ListIncrement]);
    }

    DataH.DataSize -= ((SSizes.ItemHeaderSize + SSizes.ChronoHeaderSize) * ItemsCount);

    unsigned short ListCount = DataH.NumberOfLists[D] - 1;
    unsigned int TitleSize = Lists[ListIndex][0].TitleSize;
    free(Lists[ListIndex][0].Title); free(Lists[ListIndex]);
    unsigned int SegmentSize = SSizes.PointerSize * (ListCount - ListIndex);
    memmove(&Lists[ListIndex], &Lists[ListIndex + 1], SegmentSize);

    free(Items[ListIndex]);
    memmove(&Items[ListIndex], &Items[ListIndex + 1], SegmentSize);
    free(ItemIDs[ListIndex]);
    memmove(&ItemIDs[ListIndex], &ItemIDs[ListIndex + 1], SegmentSize);

    DataH.DataSize -= (TitleSize + SSizes.ListHeaderSize);
    DataH.NumberOfLists[D]--;

    if (ListCount % 10 == 0) {
        ls_resizeListArrayD(D);
    }

    return true;
}

bool ls_deleteItemAtIndex(unsigned short ItemIndex, unsigned short ListIndex, unsigned char D) {
    if (ListIndex >= DataH.NumberOfLists[D]) {
        return false;
    }

    ListHeader *List = Data[D].ListHeaders[ListIndex];

    if (ItemIndex >= List[0].NumberOfItems) {
        return false;
    }

    ItemHeader **Items = Data[D].ItemHeaders[ListIndex];

    unsigned short IDIndex;
    IDConfirmation IDConfirmation = ls_idExistsInList(Items[ItemIndex][0].ID, Data[D].ItemIDs[ListIndex], Data[D].ListHeaders[ListIndex][0].NumberOfItems);

    if (IDConfirmation.Exists) {
        IDIndex = IDConfirmation.IDIndex;
    } else {
        return false;
    }

    unsigned short ItemsCount = Data[D].ListHeaders[ListIndex][0].NumberOfItems - 1;
    unsigned int TitleSize = Items[ItemIndex][0].TitleSize;
    free(Items[ItemIndex][0].Title); free(Items[ItemIndex]);
    unsigned int SegmentCount = SSizes.PointerSize * (ItemsCount - ItemIndex);
    memmove(&Items[ItemIndex], &Items[ItemIndex + 1], SegmentCount);

    ChronoHeader **ItemIDs = Data[D].ItemIDs[ListIndex];
    free(ItemIDs[IDIndex]);
    SegmentCount = SSizes.PointerSize * (ItemsCount - IDIndex);
    memmove(&ItemIDs[IDIndex], &ItemIDs[IDIndex + 1], SegmentCount);

    for (unsigned short ItemIncrement = 0; ItemIncrement < ItemsCount; ItemIncrement++) {
        if (ItemIDs[ItemIncrement][0].Index > ItemIndex) {
            ItemIDs[ItemIncrement][0].Index--;
        }
    }

    DataH.DataSize -= (TitleSize + SSizes.ItemHeaderSize + SSizes.ChronoHeaderSize);
    List[0].NumberOfItems--;

    if (ItemsCount % 10 == 0) {
        ls_resizeItemArrayAtIndex(ListIndex, D);
    }

    return true;
}

#pragma mark - Reorder Lists/ItemSSizes.
bool ls_moveListFromIndex(unsigned short FromIndex, unsigned short ToIndex, unsigned char D) {
    if (FromIndex == ToIndex) {
        return false;
    }

    if (FromIndex >= DataH.NumberOfLists[D]) {
        return false;
    }

    if (ToIndex >= DataH.NumberOfLists[D]) {
        return false;
    }

    unsigned int SegmentSize = 0;
    unsigned short F = FromIndex, T = ToIndex;

    if (FromIndex > ToIndex) {
        SegmentSize = SSizes.PointerSize * (FromIndex - ToIndex); F = ToIndex + 1;
    } else {
        SegmentSize = SSizes.PointerSize * (ToIndex - FromIndex); T = FromIndex + 1;
    }

    ListHeader **Lists = Data[D].ListHeaders;
    ItemHeader ***Items = Data[D].ItemHeaders;
    ChronoHeader ***ItemIDs = Data[D].ItemIDs;
    ListHeader *List = Lists[FromIndex];
    ItemHeader **ListItems = Items[FromIndex];
    ChronoHeader **ListItemIDs = ItemIDs[FromIndex];
    memmove(&Lists[F], &Lists[T], SegmentSize);
    memmove(&Items[F], &Items[T], SegmentSize);
    memmove(&ItemIDs[F], &ItemIDs[T], SegmentSize);
    Lists[ToIndex] = List;
    Items[ToIndex] = ListItems;
    ItemIDs[ToIndex] = ListItemIDs;

    return true;
}

bool ls_moveItemInListAtIndex(unsigned short ListIndex, unsigned short FromIndex, unsigned short ToIndex, unsigned char D) {
    if (FromIndex == ToIndex) {
        return false;
    }

    if (ListIndex >= DataH.NumberOfLists[D]) {
        return false;
    }

    ListHeader *List = Data[D].ListHeaders[ListIndex];
    unsigned short ItemsCount = List[0].NumberOfItems, ItemIncrement, IDIndex;

    if (FromIndex >= ItemsCount) {
        return false;
    }

    if (ToIndex >= ItemsCount) {
        return false;
    }

    unsigned int SegmentSize = 0;
    ChronoHeader **ItemIDs = Data[D].ItemIDs[ListIndex];

    if (FromIndex > ToIndex) {
        SegmentSize = SSizes.PointerSize * (FromIndex - ToIndex);

        for (ItemIncrement = 0; ItemIncrement < ItemsCount; ItemIncrement++) {
            IDIndex = ItemIDs[ItemIncrement][0].Index;

            if (IDIndex >= ToIndex && IDIndex < FromIndex) {
                ItemIDs[ItemIncrement][0].Index++;
            } else {
                if (IDIndex == FromIndex) {
                    ItemIDs[ItemIncrement][0].Index = ToIndex;
                }
            }
        }

        IDIndex = ToIndex; ItemIncrement = IDIndex + 1;
    } else {
        SegmentSize = SSizes.PointerSize * (ToIndex - FromIndex);

        for (ItemIncrement = 0; ItemIncrement < ItemsCount; ItemIncrement++) {
            IDIndex = ItemIDs[ItemIncrement][0].Index;

            if (IDIndex <= ToIndex && IDIndex > FromIndex) {
                ItemIDs[ItemIncrement][0].Index--;
            } else {
                if (IDIndex == FromIndex) {
                    ItemIDs[ItemIncrement][0].Index = ToIndex;
                }
            }
        }

        ItemIncrement = FromIndex; IDIndex = ItemIncrement + 1;
    }

    ItemHeader **Items = Data[D].ItemHeaders[ListIndex];
    ItemHeader *Item = Items[FromIndex];
    memmove(&Items[ItemIncrement], &Items[IDIndex], SegmentSize);
    Items[ToIndex] = Item;

    return true;
}

#pragma mark - Initializing/Resizing Lists' memorieSSizes.
bool ls_initializeListArrayD(unsigned char D) {
    unsigned int n = SSizes.PointerSize * (DataH.NumberOfLists[D] + 10);

    Data[D].ListHeaders = malloc(n);
    Data[D].ItemHeaders = malloc(n);
    Data[D].ItemIDs = malloc(n);
    return true;
}

bool ls_initializeItemArrayAtIndex(unsigned short ListIndex, unsigned char D) {
    unsigned int SegmentSize = SSizes.PointerSize * (Data[D].ListHeaders[ListIndex][0].NumberOfItems + 10);

    Data[D].ItemHeaders[ListIndex] = malloc(SegmentSize);
    Data[D].ItemIDs[ListIndex] = malloc(SegmentSize);
    return true;
}

bool ls_resizeListArrayD(unsigned char D) {
    unsigned int SegmentSize = SSizes.PointerSize * (DataH.NumberOfLists[D] + 10);

    Data[D].ListHeaders = realloc(Data[D].ListHeaders, SegmentSize);
    Data[D].ItemHeaders = realloc(Data[D].ItemHeaders, SegmentSize);
    Data[D].ItemIDs = realloc(Data[D].ItemIDs, SegmentSize);
    return true;
}


bool ls_resizeItemArrayAtIndex(unsigned short ListIndex, unsigned char D) {
    unsigned int SegmentSize = SSizes.PointerSize * (Data[D].ListHeaders[ListIndex][0].NumberOfItems + 10);

    Data[D].ItemHeaders[ListIndex] = realloc(Data[D].ItemHeaders[ListIndex], SegmentSize);
    Data[D].ItemIDs[ListIndex] = realloc(Data[D].ItemIDs[ListIndex], SegmentSize);
    
    return true;
}
