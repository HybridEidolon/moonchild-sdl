#import <AVFoundation/AVFoundation.h>
#import "GameLoopView.h"
#import "GameLoopTouchView.h"

@class MPMoviePlayerController;
@interface GameLoopViewController : UIViewController <UIGestureRecognizerDelegate>
{
    GameLoopView *oglView;
    GameLoopTouchView *touchView;
    UIButton *exitButton;
    UIImageView *gameOverPic;
    MPMoviePlayerController *moviePlayer;
    
    CADisplayLink *displayLink;
    BOOL running;

#if DEBUGON
    UIButton *editorButton;
    UIButton *saveButton;
    UIButton *patButton;
    UIButton *clickButton;
#endif
}

@property (nonatomic, retain) GameLoopView *oglView;
@property (nonatomic, retain) GameLoopTouchView *touchView;
@property (nonatomic, retain) UIButton *exitButton;
@property (nonatomic, retain) UIImageView *gameOverPic;

@property (nonatomic) BOOL joyStickEnabled;

@property (nonatomic, retain) UIImageView *joystick;
@property (nonatomic, retain) UIImageView *joyreach;

#if DEBUGON
@property (nonatomic, retain) UIButton *patButton;
@property (nonatomic, retain) UIButton *editorButton;
@property (nonatomic, retain) UIButton *saveButton;
@property (nonatomic, retain) UIButton *clickButton;
#endif

- (void) startGame;
- (void)updateFrame:(id)sender;
//- (CVPixelBufferRef) pixelBufferFromBuffer;
- (void) pixelBufferFromBuffer:(CVPixelBufferRef)pxbuffer;
- (CVPixelBufferRef)createPixelBuffer;

-(void)moonChildTick;
-(void) initMoonChild;
-(void) checkPlayMovie;
- (void) moviePlayBackDidFinish:(NSNotification*)notification;


@end
