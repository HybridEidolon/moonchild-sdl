#import "GameLoopAppDelegate.h"
#import "GameLoopViewController.h"

#import "Frm_int.hpp"
#import "globals.hpp"

@implementation GameLoopAppDelegate


@synthesize window;
@synthesize mainViewController;


#pragma mark -
#pragma mark Application lifecycle

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    progressRead = NO;
    
    
    
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    // Override point for customization after application launch.
    mainViewController = [[GameLoopViewController alloc] init];
    
    //check defaults
    NSObject *musicEnabled= [[NSUserDefaults standardUserDefaults] objectForKey:@"music_preference"];
    if(musicEnabled == nil)
    {
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"music_preference"];
    }
        
    NSObject *soundEnabled= [[NSUserDefaults standardUserDefaults] objectForKey:@"sound_preference"];
    if(soundEnabled == nil)
    {
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"sound_preference"];
    }
    
    NSObject *joyChoice= [[NSUserDefaults standardUserDefaults] objectForKey:@"input_preference"];
    if(joyChoice == nil)
    {
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"input_preference"];
    }
	
	// Set the main view controller as the window's root view controller and display.
    self.window.rootViewController = self.mainViewController;
    [self.window makeKeyAndVisible];
    

    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    [self writeProgress];
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}


- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}



-(void) writeProgress
{
    //was the progressfile already read?  If not, we can not save state!
    if(progressRead==NO) return;
    
    const int arraySize = 512;
    char progressFileName[arraySize];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(
                                                         NSDocumentDirectory, 
                                                         NSUserDomainMask, YES
                                                         ); 
    NSString* docDir = [paths objectAtIndex:0];
    NSString* file = [docDir stringByAppendingString:@"/progress.dat"];
    
    //    NSLog(file);
    
    //converting to C string
    [file getCString:progressFileName maxLength:arraySize encoding:NSUTF8StringEncoding];
    
    
    FILE *fp;
    
    fp = fopen(progressFileName, "wb+");
    if(fp!=NULL)
    {
        fwrite(&maxlevel, 1, 2, fp);
        int i;
        for(i=0; i<13; i++)
        {
            fwrite(&blacksperlevel[i], 1, 2, fp);
        }
        
        fclose(fp);
    }
}

-(void) readProgress
{
    progressRead = YES;
    const int arraySize = 512;
    char progressFileName[arraySize];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(
                                                         NSDocumentDirectory, 
                                                         NSUserDomainMask, YES
                                                         ); 
    NSString* docDir = [paths objectAtIndex:0];
    NSString* file = [docDir stringByAppendingString:@"/progress.dat"];
    
    //    NSLog(file);
    
    //converting to C string
    [file getCString:progressFileName maxLength:arraySize encoding:NSUTF8StringEncoding];
    
    
    FILE *fp;
    
    fp = fopen(progressFileName, "rb+");
    if(fp!=NULL)
    {
        fread(&maxlevel, 1, 2, fp);
        int i;
        for(i=0; i<13; i++)
        {
            fread(&blacksperlevel[i], 1, 2, fp);
        }
        
        fclose(fp);
    }
}

#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc {
    [mainViewController release];
    [window release];
    [super dealloc];
}

@end
