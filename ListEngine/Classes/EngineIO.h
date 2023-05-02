//
//  EngineIO.h
//  ListEngine
//
//  Created by Wael Youssef on 30/04/2023.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface EngineIO : NSObject

- (void) setupIEngine;

- (int) numberOfLists;
- (int) numberOfItemsInList: (int) listIndex;
- (int) numberOfArchivedLists;
- (int) numberOfItemsInArchivedList: (int) listIndex;
- (NSString *) titleForListAtIndex:(int) listIndex;
- (NSString *) titleForArchivedListAtIndex:(int) listIndex;

@end


NS_ASSUME_NONNULL_END
