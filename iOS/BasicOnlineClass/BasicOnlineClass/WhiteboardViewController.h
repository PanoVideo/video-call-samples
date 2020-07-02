//
//  WhiteboardViewController.h
//  BasicOnlineClass
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "PanoRtc/PanoRtcWhiteboard.h"

@interface WhiteboardViewController : UIViewController

@property (weak, nonatomic) PanoRtcWhiteboard * whiteboardEngine;

- (void)close;

@end
