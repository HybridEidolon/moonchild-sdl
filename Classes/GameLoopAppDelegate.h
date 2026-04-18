#import <UIKit/UIKit.h>

@class GameLoopViewController;

@interface GameLoopAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    GameLoopViewController *mainViewController;
    
    BOOL progressRead;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet GameLoopViewController *mainViewController;

-(void) writeProgress;
-(void) readProgress;

@end

