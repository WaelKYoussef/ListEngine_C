//
//  StructureDeclrations.h
//  ListEngine
//
//  Created by Wael Youssef on 30/04/2023.
//

#ifndef StructureDeclrations_h
#define StructureDeclrations_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#import <UIKit/UIKit.h>

#pragma mark - InterfaceValues, ColorThemes & InfoOptions.

typedef struct _BufferResult{
    void * pointer;
    unsigned int size;
}BufferResult;

typedef struct _Date{
    unsigned short Year;
    unsigned char Month,Day,Hour,Minute,Second;
    unsigned int NSecond;
}Date;

typedef struct _ShortDate{
    unsigned short Year;
    unsigned char Month,Day;
}ShortDate;

typedef struct _InterfaceValuesHeader{
    unsigned char NumberOfInfoTitles;
}InterfaceValuesHeader;

typedef struct _InterfaceValues{
    bool IsTablet;
    float SlotS,SlotO,SlotH,ISlotO,ISlotH;
    float AVBtnSize,IBSize;
    float BlockButtonY,CBrBtnSize,ABSize;
    float FontSize,DateBlockFontSize,CBrBtnFontSize;
    float CPHeight,CPWidth,CPIconHeight,CPIconWidth;
    float DtMIconSize,DtMItemSize;
}InterfaceValues;

typedef struct _InfoTitle{
    bool IsTitle;
    unsigned char Index,TitleSize;
    void*Title;
}InfoTitle;

typedef struct _InfoTitleList{
    unsigned char TitlesCount,Size;
    InfoTitle*Titles;
}InfoTitleList;

typedef struct _ColorValues{
    float CThemes[4][36];
    float GThemes[4][72];
    float SColors[30];
}ColorValues;

typedef struct _InfoBackgorundOptions{
    unsigned char Option,Color,Gradient,GradientDirection;
}InfoBackgorundOptions;
typedef struct _InfoPlusOptions{
    unsigned char AddFromTop,IsButton,Color,Size;
}InfoPlusOptions;
typedef struct _InfoDateOptions{
    unsigned char DateFormat,TimeFormat;
}InfoDateOptions;
typedef struct _InfoDefaultsOptions{
    int StripeColor;
    int StartUpListTitleSize;
    char*StartUpListTitle;
}InfoDefaultsOptions;

typedef struct _InfoOptions2{
    Date AppDeviceID;
    InfoBackgorundOptions Backgorund;
    InfoPlusOptions Plus;InfoDateOptions Date;
    unsigned char ColorTheme,Animation,Font,Reminders,Syncing;
}InfoOptions2;

typedef struct _InfoOptions21{
    Date AppDeviceID;
    InfoBackgorundOptions Backgorund;
    InfoBackgorundOptions Overlay;
    InfoDefaultsOptions Defaults;
    InfoPlusOptions Plus;InfoDateOptions Date;
    unsigned char ColorTheme,Animation,Font,Reminders,Syncing;
}InfoOptions21;

#pragma mark - Database Structures.
typedef struct _StaticSizes{
    unsigned short PointerSize,ChronoHeaderSize,ListHeaderSize;
    unsigned short ItemHeaderSize,DataHeaderSize,InterfaceHeaderSize;
    unsigned short ColorThemesSize,InterfaceValuesSize,InfoOptionsSize;
    unsigned short InfoTitleListSize,InfoTitleSize;
}StaticSizes;

typedef struct _DataHeader{
    unsigned int DataSize;
    unsigned short NumberOfLists[2];
}DataHeader;

typedef struct _ListHeader{
    unsigned short NumberOfItems;
    unsigned char Color;
    unsigned int TitleSize;
    void*Title;
}ListHeader;

typedef struct _ItemHeader{
    Date ID,LastChange;
    ShortDate ItemDate;
    unsigned char Color, Notification;
    bool Done,Selected;
    unsigned int TitleSize;
    void*Title;
}ItemHeader;

typedef struct _ChronoHeader{
    Date ID;
    unsigned short Index;
}ChronoHeader;

typedef struct _DataSystem{
    ListHeader**ListHeaders;
    ItemHeader***ItemHeaders;
    ChronoHeader***ItemIDs;
}DataSystem;

#pragma mark - Misc Structures.
typedef struct _IDConfirmation{
    bool Exists;
    unsigned short IDIndex, Index;
}IDConfirmation;

typedef struct _TitleConfirmation{
    bool Exists;
    unsigned short D, ListIndex;
}TitleConfirmation;

typedef struct _ItemPath{
    bool Exists;
    unsigned short D,ListIndex,ItemIndex;
}ItemPath;

typedef struct _CleanS{
    char*S;
    unsigned int SLength;
    bool IsList,HasText;
}CleanS;


#endif /* StructureDeclrations_h */
