//
//  EngineIO.m
//  ListEngine
//
//  Created by Wael Youssef on 30/04/2023.
//

#include <sys/time.h>
#import <UIKit/UIKit.h>
#import "EngineIO.h"
#include "Engine.h"

#pragma mark - DataManager.

@implementation EngineIO

#pragma mark - File Paths.

- (NSString *)pathForDataFile {
    return [[self fileDirectoryPath] stringByAppendingPathComponent:@"file.dat"];
}


- (NSString *)fileDirectoryPath {
    NSArray *PathArray = NSSearchPathForDirectoriesInDomains
            (NSDocumentDirectory, NSUserDomainMask, TRUE);
    return  [PathArray objectAtIndex:0];
}


- (void) setupIEngine {
    NSData * fileData = [NSData dataWithContentsOfFile: [self pathForDataFile]];
    const void * dataBuffer = [fileData bytes];
    ls_setupEngine((void *)dataBuffer);
}

- (int) numberOfLists {
    return ls_dataHeader().NumberOfLists[0];
}

- (int) numberOfItemsInList: (int) listIndex {
    return ls_mainData().ListHeaders[listIndex][0].NumberOfItems;
}

- (int) numberOfArchivedLists {
    return ls_dataHeader().NumberOfLists[1];
}

- (int) numberOfItemsInArchivedList: (int) listIndex {
    return ls_archivedData().ListHeaders[listIndex][0].NumberOfItems;
}

- (NSString *) titleForListAtIndex:(int) listIndex {
    return [[NSString alloc] initWithUTF8String: ls_mainData().ListHeaders[listIndex][0].Title];
}

- (NSString *) titleForArchivedListAtIndex:(int) listIndex {
    return [[NSString alloc] initWithUTF8String: ls_archivedData().ListHeaders[listIndex][0].Title];
}




@end
