#import <QuartzCore/QuartzCore.h>
#import "GameLoopViewController.h"
#import <MediaPlayer/MediaPlayer.h>
#import <string.h>
#import "GameLoopAppDelegate.h"
#include <zlib.h>


#define _IN_MAIN
#import "Frm_int.hpp"
#import "globals.hpp"
#import "prefs.hpp"
#import "MoonPlication.h"

#define IS_IPHONE5 (([[UIScreen mainScreen] bounds].size.width-568)?NO:YES)

#define ZOOMED
static inline double radians (double degrees) { return degrees * (M_PI / 180); }

void framework_EventHandle(int event, int param);
char *FullPath( char *a_File );
char *FullWritablePath( char *a_File );

extern HEARTBEAT_FN framework_InitGame(Cvideo *video, Caudio *audio, Ctimer *timer, Cmovie *movie);
void checkPlayMovie();

@implementation GameLoopViewController

#define BYTES_PER_PIXEL 4
    int WIDTH = 640;
    int HEIGHT = 480;
#define GAMEINTERVAL (1)

#define JOYSTICKDELTA (16)

unsigned char *pixelBuffer;
extern PREFS *prefs;

int					g_SettingsFlg;
int					g_ReqKeyFlg;		// is a hardware key being requested?
short			   *g_ReqKeyPtr;		// which key is being requested?
short				g_CurKey;
int					g_KeyTimeOut;		// tijdelijk werkt options key niet

char  filenameBuf[512];
char  rootPath[512];
char  rootWritablePath[512];

#define MOUSETRESHOLD (32)

int g_MouseFlg;         //deze bevat niet altijd of ie ingedrukt is
int g_MouseActualFlg;   //bevat altijd of ie wel of niet ingdrukt is
int g_MouseXDown;
int g_MouseYDown;
int g_MouseXCurrent;
int g_MouseYCurrent;

int g_CurDeltaX;

int g_LastSpdX;
int g_LastSpdY;

int frmwrk_CenterX;
int frmwrk_CenterY;

unsigned short *SettingsPic;
unsigned short *ButPic[10];
unsigned short *SwitchPic[6];
unsigned short *SpeakerPic[2];
unsigned short *LoadingPic;
unsigned short *TempPic;

//moet weer extern worden!
extern unsigned short levelloadedflg;
extern unsigned short puzzleactiveflg;
extern unsigned short showdeadsequence;



@synthesize oglView, touchView, exitButton, gameOverPic;

#if DEBUGON
@synthesize saveButton, editorButton, patButton, clickButton;
#endif

- (void)cleanup
{
	[oglView release];
	[touchView release];
	[exitButton release];
	[gameOverPic release];
	oglView = nil;
	touchView = nil;
    exitButton = nil;
    gameOverPic = nil;
}

- (void)dealloc 
{
	[self cleanup];
    [displayLink release];
    
	[super dealloc];
}

- (void)viewDidUnload 
{
	[super viewDidUnload];
    
	[self cleanup];
}


- (void)viewDidLoad 
{
	[super viewDidLoad];

	self.oglView = [[[GameLoopView alloc] initWithFrame:CGRectMake(0.0f, 0.0f, WIDTH, HEIGHT)] autorelease];
    CGAffineTransform transform;
    if([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad)
    {
        gameOverPic = [[UIImageView  alloc] initWithImage:[UIImage imageNamed:@"gameover_ipad"]];
    }
    else
    {
        gameOverPic = [[UIImageView  alloc] initWithImage:[UIImage imageNamed:@"gameover"]];
    }
//    float screenHeight = [[UIScreen mainScreen] bounds].size.height;
//    HEIGHT = (int)screenHeight;
//    float scalingHeight = screenHeight / HEIGHT;
//    float screenWidth = [[UIScreen mainScreen] bounds].size.width;
//    WIDTH = (int)screenWidth;
//    float scalingWidth = screenWidth / WIDTH;
//    float minScale = MIN(scalingWidth, scalingHeight);
//    transform = CGAffineTransformIdentity;
//    transform = CGAffineTransformScale(transform, minScale, minScale);
//
    CGSize contentSize = CGSizeMake(640.0f, 480.0f);
    CGSize containerSize = self.view.bounds.size;

    CGFloat scaleX = containerSize.width  / contentSize.width;
    CGFloat scaleY = containerSize.height / contentSize.height;

    CGFloat scale = MIN(scaleX, scaleY);

    // Apply transform
    self.oglView.transform = CGAffineTransformMakeScale(scale, scale);

    // Center it
    self.oglView.center = CGPointMake(containerSize.width * 0.5f,
                                      containerSize.height * 0.5f);


    [self.view addSubview:self.oglView];
	self.touchView = [[[GameLoopTouchView alloc] initWithFrame:CGRectMake(0.0f, 0.0f, self.view.frame.size.width, self.view.frame.size.height)] autorelease];
    [self.view addSubview:self.touchView];
    
    exitButton = [[UIButton alloc] initWithFrame:CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-64, 48, 48)];
    [exitButton setImage:[UIImage imageNamed:@"exitbutton.png"] forState:UIControlStateNormal];
    exitButton.frame = CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-64, 48, 48);
    NSLog(@"w=%f", self.view.frame.size.width-64);
    exitButton.alpha = 0.0f;
    exitButton.enabled = false;
    [exitButton addTarget:self action:@selector(exitButtonPressed) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:exitButton];
    
    gameOverPic.frame = CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height);
    gameOverPic.hidden = YES;
    [self.view addSubview:gameOverPic];


#if DEBUGON
    editorButton = [[UIButton alloc] initWithFrame:CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-64, 48, 48)];
    [editorButton setImage:[UIImage imageNamed:@"exitbutton.png"] forState:UIControlStateNormal];
    editorButton.frame = CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-128, 48, 48);
    editorButton.alpha = 1.0f;
    editorButton.enabled = true;
    [editorButton addTarget:self action:@selector(editorButtonPressed) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:editorButton];
    
    patButton = [[UIButton alloc] initWithFrame:CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-64, 48, 48)];
    [patButton setImage:[UIImage imageNamed:@"exitbutton.png"] forState:UIControlStateNormal];
    patButton.frame = CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-192, 48, 48);
    patButton.alpha = 1.0f;
    patButton.enabled = true;
    [patButton addTarget:self action:@selector(patButtonPressed) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:patButton];
    
    clickButton = [[UIButton alloc] initWithFrame:CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-64, 48, 48)];
    [clickButton setImage:[UIImage imageNamed:@"exitbutton.png"] forState:UIControlStateNormal];
    clickButton.frame = CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-256, 48, 48);
    clickButton.alpha = 1.0f;
    clickButton.enabled = true;
    [clickButton addTarget:self action:@selector(clickButtonPressed) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:clickButton];
    
    saveButton = [[UIButton alloc] initWithFrame:CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-64, 48, 48)];
    [saveButton setImage:[UIImage imageNamed:@"exitbutton.png"] forState:UIControlStateNormal];
    saveButton.frame = CGRectMake(self.view.frame.size.width-64, self.view.frame.size.height-320, 48, 48);
    saveButton.alpha = 1.0f;
    saveButton.enabled = true;
    [saveButton addTarget:self action:@selector(saveButtonPressed) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:saveButton];
#endif    

    self.joyStickEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"input_preference"];
    self.joystick = [[[UIImageView alloc] initWithImage:[UIImage imageNamed:@"joystick_dark"]] autorelease];
    self.joyreach = [[[UIImageView alloc] initWithImage:[UIImage imageNamed:@"joyreach_inactive"]] autorelease];
    self.joyreach.alpha = 0.75f;
    self.joystick.hidden = YES;
    self.joyreach.hidden = YES;
    [self.view addSubview:self.joyreach];
    [self.view addSubview:self.joystick];
    
    [self startGame];

}


- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];

}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    if(UIInterfaceOrientationIsLandscape(interfaceOrientation)) return YES;
    return NO;
}

- (void) startGame
{
	if (!running)
	{
        //root path
        NSString *rootDir = [[NSBundle mainBundle] resourcePath];
        const char *rootDirPntr = [rootDir UTF8String];
        strcpy(rootPath, rootDirPntr);
        
        //document path
        NSArray *paths = NSSearchPathForDirectoriesInDomains(
                                                             NSDocumentDirectory, 
                                                             NSUserDomainMask, YES
                                                             ); 
        NSString* docDir = [paths objectAtIndex:0];
        const char *docDirPntr = [docDir UTF8String];
        strcpy(rootWritablePath, docDirPntr);
        
        
        pixelBuffer = new unsigned char[WIDTH*HEIGHT*BYTES_PER_PIXEL];
        
        [self initMoonChild];
        
        memset(pixelBuffer, 0, WIDTH*HEIGHT*BYTES_PER_PIXEL);
        
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(updateFrame:)];
        [displayLink setFrameInterval:GAMEINTERVAL];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	}
}

static CVPixelBufferRef pixelBuf = nil;

- (void)updateFrame:(id)sender
{
    [self checkPlayMovie];
    [self moonChildTick];
    if( pixelBuf == nil) pixelBuf = [self createPixelBuffer];
    [self pixelBufferFromBuffer:pixelBuf];
    [self.oglView displayPixelBuffer:pixelBuf];
//    CVPixelBufferRelease(pixelBuf);
}


- (CVPixelBufferRef)createPixelBuffer
{
    NSDictionary *options = @{
            (id)kCVPixelBufferIOSurfacePropertiesKey : @{},
            (id)kCVPixelBufferOpenGLESCompatibilityKey : @YES,
            (id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA),
            (id)kCVPixelBufferWidthKey : @(WIDTH),
            (id)kCVPixelBufferHeightKey : @(HEIGHT),
        };

    CVPixelBufferRef pxbuffer = NULL;
    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault, WIDTH,
                                          HEIGHT, kCVPixelFormatType_32BGRA, (CFDictionaryRef) options, 
                                          &pxbuffer);
    NSParameterAssert(status == kCVReturnSuccess && pxbuffer != NULL);
    return pxbuffer;
}

- (void)pixelBufferFromBuffer:(CVPixelBufferRef)pxbuffer
{
    CVPixelBufferLockBaseAddress(pxbuffer, 0);

    size_t h = CVPixelBufferGetHeight(pxbuffer);
    size_t dstBPR = CVPixelBufferGetBytesPerRow(pxbuffer);
    size_t srcBPR = CVPixelBufferGetWidth(pxbuffer) * 4;

    uint8_t *dst = (uint8_t *)CVPixelBufferGetBaseAddress(pxbuffer);
    uint8_t *src = (uint8_t *)pixelBuffer;

    for (size_t y = 0; y < h; ++y) {
        memcpy(dst, src, srcBPR);
        dst += dstBPR;
        src += srcBPR;
    }

    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);
}

UIView *movieClickView = nil;

-(void) checkPlayMovie
{
    if(moviePlayer)
    {
        if(globalTouchDetected)
        {
            [self cleanupVideoPlayer];
            return;
        }
    }
    if(lmovie->videoFilename != NULL)
    {
        NSString *videoFilename = [NSString stringWithCString:lmovie->videoFilename encoding:NSASCIIStringEncoding];
        NSLog(@"trying to play movie: %@", videoFilename);
        lmovie->videoFilename = NULL;
    
        NSURL *url = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:videoFilename ofType:@"mov"]];  
        moviePlayer = [[MPMoviePlayerController alloc] initWithContentURL:url];
        
        // Register to receive a notification when the movie has finished playing.  
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(moviePlayBackDidFinish:)
                                                     name:MPMoviePlayerPlaybackDidFinishNotification
                                                   object:moviePlayer];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(moviePlayBackChangedPlayState:)
                                                     name:MPMoviePlayerPlaybackStateDidChangeNotification
                                                   object:moviePlayer];
        
        
        moviePlayer.useApplicationAudioSession = YES;
        if([moviePlayer respondsToSelector:@selector(setAllowsAirPlay:)])
        {
            [moviePlayer setAllowsAirPlay:NO];
        }
        // Use the new 3.2 style API
        moviePlayer.controlStyle = MPMovieControlStyleNone;
        moviePlayer.shouldAutoplay = YES;
        [self.view addSubview:moviePlayer.view];
        [moviePlayer setFullscreen:YES animated:YES];

//        BOOL musicEnabled= [[NSUserDefaults standardUserDefaults] boolForKey:@"music_preference"];
//        [[MPMusicPlayerController applicationMusicPlayer] setVolume:(musicEnabled) ? 1.0f : 0.0f];
//        movieClickView = [[[UIView alloc] initWithFrame:self.view.bounds] autorelease];
//
        

//        [self.view addSubview:movieClickView];
        

    }
    return;
}

- (void) endVideo
{
    [self cleanupVideoPlayer];
}

- (void)moviePlayBackChangedPlayState:(NSNotification*)notification
{
    if(moviePlayer.playbackState == MPMoviePlaybackStatePlaying)
    {
        UITapGestureRecognizer *oneFingerOneTap =
        [[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(endVideo)] autorelease];
        
        // Set required taps and number of touches
        [oneFingerOneTap setNumberOfTapsRequired:1];
        [oneFingerOneTap setNumberOfTouchesRequired:1];
        
        //remove all gesture recognizers from the movieplayer
        UIView *gestureView = moviePlayer.view.subviews[0];
        for(UIGestureRecognizer *gr in gestureView.gestureRecognizers)
        {
            [gestureView removeGestureRecognizer:gr];
        }
        [gestureView addGestureRecognizer:oneFingerOneTap];
        
    }
}

- (void) moviePlayBackDidFinish:(NSNotification*)notification
{
    NSLog(@"Movie finished notification");
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:MPMoviePlayerPlaybackDidFinishNotification
                                                  object:moviePlayer];
    
    [self performSelectorOnMainThread:@selector(cleanupVideoPlayer) withObject:nil waitUntilDone:NO];
}

- (void)cleanupVideoPlayer
{
    [moviePlayer setFullscreen:NO animated:NO];
    moviePlayer.fullscreen = NO;
    [moviePlayer stop];
    // If the moviePlayer.view was added to the view, it needs to be removed
    if ([moviePlayer respondsToSelector:@selector(setFullscreen:animated:)]) {
        [moviePlayer.view removeFromSuperview];
    }
    
//    [movieClickView removeFromSuperview];
//    [movieClickView release];
//    movieClickView = nil;
    
    [moviePlayer release];
    moviePlayer = nil;
    lmovie->videoReady = true;
    
    //    [[MPMusicPlayerController applicationMusicPlayer] setVolume:(musicEnabled) ? 1.0f : 0.0f];
    
    
}



-(void) initMoonChild
{
	lvideo = new Cvideo();
	
//	LoadSettingsPics();
//	LoadConfig();
//	BlitConfig();
    
	laudio = new Caudio();  // create audio AFTER window is created!
	ltimer = new Ctimer();  // Create timer facilities
	lmovie = new Cmovie(laudio);  // Initiate movie playback features
	
	g_SettingsFlg = 0;	//we starten met het settings window
	g_ReqKeyFlg = 0;
	g_KeyTimeOut = 0;
    
	frmwrk_CenterX = 0;
	frmwrk_CenterY = 0;
	
	gbGameLoop = TRUE;
	
	heartbeat = NULL;
    
    
    
    if ( !lvideo->on(pixelBuffer, WIDTH, HEIGHT, 256) )
    {
        //EXIT!
        return;
    }
    
    heartbeat = framework_InitGame(lvideo, laudio, ltimer, lmovie);
    
    if (heartbeat == NULL)
    {
        //EXIT!
        return;
    }
    
    //read progress
    GameLoopAppDelegate *delegate = (GameLoopAppDelegate *) [[UIApplication sharedApplication]delegate];
    [delegate readProgress];
                                
    
    
    //check for cheat or reset
    int i;
    BOOL settingsReset = [[NSUserDefaults standardUserDefaults] boolForKey:@"reset_preference"];
    if(settingsReset)
    {
        //reset code
        maxlevel = 0;
        
        for(i=0; i<13; i++)
        {
            blacksperlevel[i] = 0;
            scoreblacksperlevel[i] = 0;
        }
        
        //set switch back to off
        [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"reset_preference"];
    }
    
    BOOL settingsCheat = [[NSUserDefaults standardUserDefaults] boolForKey:@"cheat_preference"];
    if(settingsCheat)
    {
        //cheat code
        maxlevel = 12;
        
        for(i=0; i<13; i++)
        {
            blacksperlevel[i] = 0;
            scoreblacksperlevel[i] = 0;
        }
        
        //set switch back to off
        [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"cheat_preference"];
    }
}

#define VK_ESCAPE 'Q'

-(void) exitButtonPressed
{
    framework_EventHandle(FW_KEYDOWN,(int) VK_ESCAPE);  //firekey
}

#if DEBUGON

-(void) editorButtonPressed
{
    framework_EventHandle(FW_KEYDOWN,(int) 'E');  //firekey
}

-(void) patButtonPressed
{
    framework_EventHandle(FW_KEYDOWN,(int) 'P');  //firekey
}

-(void) clickButtonPressed
{
    mouselbut ^= 1;
    mouselchng = 1;
    clickButton.alpha = (mouselbut == 1) ? 0.5f : 1.0f; 
}

-(void) saveButtonPressed
{
    framework_EventHandle(FW_KEYDOWN,(int) 'U');  //firekey
}
#endif
static float curAlpha=0.4f;
static float alphaSpd = 0.01f;

-(void)moonChildTick
{
//    return;
    if(showdeadsequence)
    {
        gameOverPic.hidden = NO;
        if(touchView.mousePressing)
        {
            touchView.mousePressing = 0;
            showdeadsequence = 0;
            gameOverPic.hidden = YES;
            return;
        }
    }
    
    laudio->checkVolume();
    
    if(levelloadedflg || puzzleactiveflg)
    {
        exitButton.alpha = curAlpha;
        exitButton.enabled = YES;
    }
    else
    {
        exitButton.alpha = 0.0f;
        exitButton.enabled = NO;
    }
    
    curAlpha += alphaSpd;
    if(curAlpha > 0.8f || curAlpha < 0.4f)
    {
        alphaSpd = -alphaSpd;
    }
    
    
    
    
    if(self.joyStickEnabled && levelloadedflg==1)  //nieuwe joystick mehod alleen enabled in levels
    {
        static int joyFireAnimTimer=0;
        g_MouseFlg = NO;
        //during level selection
        if(levelloadedflg==0 && touchView.mousePressed)
        {
            self.joystick.hidden = YES;
            self.joyreach.hidden = YES;
            touchView.mousePressed = NO;
            framework_EventHandle(FW_KEYDOWN,(int) prefs->shootkey);  //firekey
            g_MouseFlg = touchView.mousePressing;
            g_MouseXCurrent = touchView.curTouchPos.x;
            g_MouseYCurrent = touchView.curTouchPos.y;
            g_MouseXDown = g_MouseXCurrent;
            g_MouseYDown = g_MouseYCurrent;
            g_MouseActualFlg = touchView.mousePressing;
            
        }
        //during game
        if(levelloadedflg==1)
        {
            static int touchTimeCounter = 0;
            static float joyAlphaAnim = 0.0f;
            self.joystick.hidden = NO;
            self.joyreach.hidden = NO;

            if(touchView.mousePressed)
            {
                touchView.mousePressed = NO;
                framework_EventHandle(FW_KEYDOWN,prefs->shootkey);
                joyFireAnimTimer = 5;
//                if(touchTimeCounter>0)
//                {
//                    touchTimeCounter = 0;
//                }
            }
            if(joyFireAnimTimer>0)
            {
                joyFireAnimTimer--;
                self.joystick.image = [UIImage imageNamed:@"joystick_light"];
            }
            else
            {
                self.joystick.image = [UIImage imageNamed:@"joystick_dark"];
            }
            
            if(touchView.mousePressing)
            {
                joyAlphaAnim = 1.0f;
                if(g_MouseXCurrent==-1) //first time press?
                {
                    g_MouseXCurrent = touchView.curTouchPosOrg.x;
                    g_MouseYCurrent = touchView.curTouchPosOrg.y;
                    touchTimeCounter = 5;
                }
                touchTimeCounter--;

                int maxDelta = 38;
                if(![[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad)
                {
                    maxDelta = 19;
                }
                
                //determine direction
                int deltaX = touchView.curTouchPosOrg.x - g_MouseXCurrent;
                int deltaY = touchView.curTouchPosOrg.y - g_MouseYCurrent;
                
                long length = deltaX*deltaX +deltaY*deltaY;
                int centerX = touchView.curTouchPosOrg.x;
                int centerY = touchView.curTouchPosOrg.y;
                length = sqrt(length);
                if(length>maxDelta)
                {
                    deltaX *= maxDelta;
                    deltaX /= length;
                    deltaY *= maxDelta;
                    deltaY /= length;
                    centerX = g_MouseXCurrent + deltaX;
                    centerY = g_MouseYCurrent + deltaY;
                }
                
                self.joyreach.center = CGPointMake(g_MouseXCurrent, g_MouseYCurrent);
                self.joystick.center = CGPointMake(centerX, centerY);
                
                bool right = false;
                bool left = false;
                bool down = false;
                bool up = false;
                int joystickDelta = JOYSTICKDELTA;
                if(![[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad)
                {
                    joystickDelta/=2;
                }
                NSString *joyReachImage = @"joyreach_inactive";
                if(length>joystickDelta )
                {
                    //determine keypresses
                    float angle = atan2(-deltaY,-deltaX)/M_PI*180 + 180;
                    
                    if(angle < 22.5)
                    {
                        joyReachImage = @"joyreach_active_right";
                        right = true;
                    }
                    else if(angle<45+22.5)
                    {
                        joyReachImage = @"joyreach_active_bottom-right";
                        right = true;
                        down = true;
                    }
                    else if(angle<90+22.5)
                    {
                        joyReachImage = @"joyreach_active_bottom";
                        down = true;
                    }
                    else if(angle<135+22.5)
                    {
                        joyReachImage = @"joyreach_active_bottom-left";
                        left = true;
                        down = true;
                    }
                    else if(angle<180+22.5)
                    {
                        joyReachImage = @"joyreach_active_left";
                        left = true;
                    }
                    else if(angle<225+22.5)
                    {
                        left = true;
                        up = true;
                        joyReachImage = @"joyreach_active_top-left";
                    }
                    else if(angle<270+22.5)
                    {
                        up = true;
                        joyReachImage = @"joyreach_active_top";
                    }
                    else if(angle<315+22.5)
                    {
                        joyReachImage = @"joyreach_active_top-right";
                        right = true;
                        up = true;
                    }
                    else
                    {
                        joyReachImage = @"joyreach_active_right";
                        right = true;
                    }
                    
                }
                self.joyreach.image = [UIImage imageNamed:joyReachImage];
                if(up) framework_EventHandle(FW_KEYDOWN,prefs->upkey);
                else framework_EventHandle(FW_KEYUP,prefs->upkey);
                if(down) framework_EventHandle(FW_KEYDOWN,prefs->downkey);
                else framework_EventHandle(FW_KEYUP,prefs->downkey);
                if(left) framework_EventHandle(FW_KEYDOWN,prefs->leftkey);
                else framework_EventHandle(FW_KEYUP,prefs->leftkey);
                if(right) framework_EventHandle(FW_KEYDOWN,prefs->rightkey);
                else framework_EventHandle(FW_KEYUP,prefs->rightkey);
                self.joystick.alpha = joyAlphaAnim;
                self.joyreach.alpha = joyAlphaAnim*0.75f;
            }
            else
            {
                g_MouseXCurrent = -1;
                g_MouseYCurrent = -1;
                framework_EventHandle(FW_KEYUP,prefs->upkey);
                framework_EventHandle(FW_KEYUP,prefs->downkey);
                framework_EventHandle(FW_KEYUP,prefs->leftkey);
                framework_EventHandle(FW_KEYUP,prefs->rightkey);
                
                if(joyAlphaAnim>=0.0f)
                {
                    joyAlphaAnim-= 0.05f;
                    if(joyAlphaAnim < 0.0f) joyAlphaAnim = 0.0f;
                    self.joystick.alpha = joyAlphaAnim;
                    self.joyreach.alpha = joyAlphaAnim*0.75f;
                }
            }
        }
    }
    else
    {
        self.joystick.hidden = YES;
        self.joyreach.hidden = YES;
        g_MouseXCurrent = touchView.curTouchPos.x;
        g_MouseYCurrent = touchView.curTouchPos.y;
        g_MouseXDown = g_MouseXCurrent;
        g_MouseYDown = g_MouseYCurrent;
        g_MouseActualFlg = touchView.mousePressing;
        g_MouseFlg = touchView.mousePressing;
        
        if(levelloadedflg==0 && touchView.mousePressed)
        {
            touchView.mousePressed = NO;
            framework_EventHandle(FW_KEYDOWN,(int) prefs->shootkey);  //firekey
        }
        else if(g_MouseFlg && levelloadedflg==1)	//alleen tijdens levels!
        {
            int DeltaX,DeltaY;
            int SpdX,SpdY;
            SpdX = SpdY = 0;
            DeltaX = (g_MouseXCurrent/2) - (frmwrk_CenterX/2);
            DeltaY = (g_MouseYCurrent/2) - (frmwrk_CenterY/2);
            if(DeltaX > MOUSETRESHOLD) SpdX = 1;
            if(DeltaX < -MOUSETRESHOLD) SpdX = -1;
            if(DeltaY > MOUSETRESHOLD) SpdY = 1;
            if(DeltaY < -MOUSETRESHOLD) SpdY = -1;
            
            if(touchView.mousePressed)
            {
                framework_EventHandle(FW_KEYDOWN,(int) prefs->shootkey);  //firekey
                touchView.mousePressed = NO;
            }
            
            g_CurDeltaX = DeltaX;
            if(SpdX != g_LastSpdX)
            {
                switch(SpdX)
                {
                    case -1:
                        framework_EventHandle(FW_KEYUP,prefs->rightkey);		//rechts
                        framework_EventHandle(FW_KEYDOWN,prefs->leftkey);		//links
                        break;
                    case 0:
                        framework_EventHandle(FW_KEYUP,prefs->rightkey);		//rechts
                        framework_EventHandle(FW_KEYUP,prefs->leftkey);         //links
                        break;
                    case 1:
                        framework_EventHandle(FW_KEYUP,prefs->leftkey);         //links
                        framework_EventHandle(FW_KEYDOWN,prefs->rightkey);		//rechts
                        break;
                }
            }
            if(SpdY != g_LastSpdY)
            {
                switch(SpdY)
                {
                    case -1:
                        framework_EventHandle(FW_KEYUP,prefs->downkey);		//down
                        framework_EventHandle(FW_KEYDOWN,prefs->upkey);		//up
                        break;
                    case 0:
                        framework_EventHandle(FW_KEYUP,prefs->upkey);		//up
                        framework_EventHandle(FW_KEYUP,prefs->downkey);		//down
                        break;
                    case 1:
                        framework_EventHandle(FW_KEYUP,prefs->upkey);		//up
                        framework_EventHandle(FW_KEYDOWN,prefs->downkey);		//down
                        break;
                }
            }
            g_LastSpdX = SpdX;
            g_LastSpdY = SpdY;
        }
        else if(g_MouseFlg==0 && levelloadedflg==1)
        {

            if(g_LastSpdX != 0)
            {
                framework_EventHandle(FW_KEYUP,prefs->rightkey);   //rechts
                framework_EventHandle(FW_KEYUP,prefs->leftkey); //links
            }
            if(g_LastSpdY != 0)
            {
                framework_EventHandle(FW_KEYUP,prefs->upkey); //up
                framework_EventHandle(FW_KEYUP,prefs->downkey); //down
            }
            g_LastSpdX = 0;
            g_LastSpdY = 0;
            
            
        }
    }
    
    if(g_SettingsFlg)	//settings screen op het beeld?
    {
        //				unsigned char buf[4] = {1,2,3,0};
        //				int getal;
        //				getal = g_KeyConfig.LeftKey;
        //				buf[2] = getal/100;
        //				getal = getal - ((getal/100)*100);
        //				buf[1] = getal/10;
        //				buf[0] = getal%10;
        lvideo->DrawSettings();
        //				lvideo->DisplayChars2(buf, 5, 5);
        
    }
    else
    {
//        lmovie->movieplay();            // if movie is playing this routine will handle frame advancement
        if (heartbeat != NULL)
        {
            heartbeat = (HEARTBEAT_FN) heartbeat();
            if(heartbeat == NULL)  // No heartbeat anymore, Let's close
            {
                //exit the game
            }
            
            
        }
    }
}

@end



char *FullPath( char *a_File )
{
    strcpy(filenameBuf, rootPath);
    strcat(filenameBuf, "/");
    strcat(filenameBuf, a_File);
	return filenameBuf;
}

char *FullWritablePath( char *a_File )
{
    strcpy(filenameBuf, rootWritablePath);
    strcat(filenameBuf, "/");
    strcat(filenameBuf, a_File);
	return filenameBuf;
}

unsigned short*LoadTGA(char *FileName)
{
	char logbuf[100];
	sprintf(logbuf,"loading: %s\n",FileName);
//	LOG(logbuf);
    
	// load targa file
	BYTE* tgabuff = new BYTE[20];
	bool OK = true;
	gzFile tga = gzopen( FullPath(FileName), "rb" );
	if (!tga) return 0; 
	gzread(tga, tgabuff, 20);
	gzclose( tga );
    
//	LOG("open succeeded\n");
    
	int TgaIDLen;
	int TgaCMapType;
	int TgaImgType;
	int TgaCMapOrig;
	int TgaCMapLen;
	int TgaCMapSize;
	int TgaXPos;
	int TgaYPos;
	int TgaWidth;
	int TgaHeight;
	int TgaPixSize;
	TgaIDLen		= *tgabuff;
	TgaCMapType	= *(tgabuff + 1);
	TgaImgType	= *(tgabuff + 2);
	TgaCMapOrig	= *(tgabuff + 3) + 256 * *(tgabuff + 4);
	TgaCMapLen	= *(tgabuff + 5) + 256 * *(tgabuff + 6);
	TgaCMapSize	= *(tgabuff + 7);
	TgaXPos		= *(tgabuff + 8) + 256 * *(tgabuff + 9);
	TgaYPos		= *(tgabuff + 10) + 256 * *(tgabuff + 11);
	TgaWidth	= *(tgabuff + 12) + 256 * *(tgabuff + 13);
	TgaHeight	= *(tgabuff + 14) + 256 * *(tgabuff + 15);
	TgaPixSize	= *(tgabuff + 16);
	delete [] tgabuff;
    
	int w,h;
	w = TgaWidth;
	h = TgaHeight;
	unsigned short *dest;
	int size = w * 4 * h + 20;
	tgabuff = new BYTE[size];
	dest = new unsigned short[w*h];  // hier komt uitgepakte plaatje
    
	tga = gzopen( FullPath(FileName), "rb" );
	if (!tga)
	{
		delete [] tgabuff;
		delete [] dest;
		return 0;
	}
	int read = gzread( tga, tgabuff, size );
	gzclose( tga );
	
	if (TgaImgType == 1)
	{
		// Palettized image
		unsigned short* pal = new unsigned short[256];
		for ( int i = 0; i < 256; i++ )
		{
			int b = *(tgabuff + 18 + i * 3);
			int g = *(tgabuff + 18 + i * 3 + 1);
			int r = *(tgabuff + 18 + i * 3 + 2);
			pal[i] = (unsigned short)(((r >> 3) << 11) + ((g >> 2) << 5) + (b >> 3));
		}
		unsigned char* src = tgabuff + 18 + 768 + (h - 1) * w;
		unsigned short* dst = (unsigned short*)dest;
		for ( int y = 0; y < h; y++ )
		{
			for ( int x = 0; x < w; x++ )
			{
				int idx = *(src + x);
				*(dst + x) = pal[idx];
			}
			dst += w;
			src -= w;
		}
	}
	else
	{
		// Store the data at the specified target address
		unsigned char* src = (tgabuff + 18) + (((h - 1) * w)*4);
		unsigned short* dst = (unsigned short*)dest;
		for ( int i = 0; i < h; i++ )
		{
			for ( int x = 0; x < w; x++ )
			{
				int r,g,b,a;
				unsigned short rgba;
				b= *src++;
				g= *src++;
				r= *src++;
				a= *src++;
				rgba = ((r>>3)<<11)+((g>>2)<<5)+(b>>3);
				*(dst + x) = rgba; //*(src + x);
			}
			dst += w;
			src -= (w*8);
		}
	}
    
	delete [] tgabuff;
    
//	LOG("tga success\n");
	return dest;
}


void framework_usefastfile(bool offon)
{
    frmwrk_usefastfile = offon;
}

void ShowPicture(char *FileName)
{
	TempPic   = LoadTGA(FileName);
    
	lvideo->DrawTempPic();
    
	delete [] TempPic;
}




