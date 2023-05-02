//
//  ViewController.m
//  ListEngine
//
//  Created by Wael Youssef on 30/04/2023.
//

#import "ViewController.h"
#import "EngineIO.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    EngineIO *io = [[EngineIO alloc] init];
    
    [io setupIEngine];
    
    NSLog(@"%@", [io titleForListAtIndex: 0]);
    
    // Do any additional setup after loading the view.
}


@end
