//
//  GameLoopTouchView.m
//  GameLoop
//
//  Created by reinier Van Vliet on 11/20/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#define ZOOMED

#import "GameLoopTouchView.h"

@implementation GameLoopTouchView

@synthesize curTouchPos, mousePressed, mousePressing;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    
    self.backgroundColor = [UIColor clearColor];
    self.multipleTouchEnabled = YES;
    return self;
}

int firstTouchHash;
bool nothingTouching = YES;

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{   
    UITouch *touch = [touches anyObject];

    if(nothingTouching)
    {
        nothingTouching = NO;
        
        // gets the coordinats of the touch with respect to the specified view.
        self.curTouchPos = [self pointTransform:[touch locationInView:self]];
        self.curTouchPosOrg = [touch locationInView:self];
        self.mousePressed = YES;
        self.mousePressing = YES;
        [self checkValidTouch];
    }
    else{
        self.mousePressed = YES;
    }
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    {
            // gets the coordinats of the touch with respect to the specified view.
            self.curTouchPos = [self pointTransform:[touch locationInView:self]];
            self.curTouchPosOrg = [touch locationInView:self];
        [self checkValidTouch];
    }
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *firstTouch = nil;;
    UITouch *touch = [touches anyObject];
    {
            // gets the coordinats of the touch with respect to the specified view.
            self.curTouchPos = [self pointTransform:[firstTouch locationInView:self]];
            self.curTouchPosOrg = [firstTouch locationInView:self];
            self.mousePressing = NO;
            firstTouchHash = 0;
        [self checkValidTouch];
    }

    if ([touches count] == [[event touchesForView:self] count]) {
        nothingTouching = YES;
    }
}

-(void) checkValidTouch {
    if (self.curTouchPos.x < 0 || self.curTouchPos.x >= 640 || self.curTouchPos.y <0 || self.curTouchPos.y >= 480) {
        nothingTouching = YES;
    }
}

-(CGPoint)pointTransform:(CGPoint)pnt
{
    CGPoint newPoint;
    float screenHeight = [[UIScreen mainScreen] bounds].size.height;
    float scalingHeight = screenHeight / 480;
    float screenWidth = [[UIScreen mainScreen] bounds].size.width;
    float scalingWidth = screenWidth / 480;
    float minScale = MIN(scalingWidth, scalingHeight);

    float x = pnt.x - screenWidth / 2.0;
    float y = pnt.y - screenHeight / 2.0;
    x = x / minScale;
    y = y / minScale;
    newPoint = CGPointMake(x + 320.0, y + 240.0);

    return newPoint;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
