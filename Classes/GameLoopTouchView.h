//
//  GameLoopTouchView.h
//  GameLoop
//
//  Created by reinier Van Vliet on 11/20/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface GameLoopTouchView : UIView
{
    CGPoint curTouchPos;
    BOOL mousePressed;
    BOOL mousePressing;
}

@property (nonatomic, assign) CGPoint curTouchPos;
@property (nonatomic, assign) CGPoint curTouchPosOrg;
@property (nonatomic, assign) BOOL mousePressed;
@property (nonatomic, assign) BOOL mousePressing;

-(CGPoint)pointTransform:(CGPoint)pnt;

@end
