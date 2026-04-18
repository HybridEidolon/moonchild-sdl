//
//  MoonPlication.m
//  moonchild
//
//  Created by reinier van vliet on 16/5/13.
//
//

#import "MoonPlication.h"

BOOL globalTouchDetected;

@implementation MoonPlication


- (void)sendEvent:(UIEvent*)event {
    //handle the event (you will probably just reset a timer)
    
    [super sendEvent:event];
    
    NSSet *touches = [event allTouches];
    UITouch *touch = [touches anyObject];
    globalTouchDetected = (touch !=nil);
}

@end
