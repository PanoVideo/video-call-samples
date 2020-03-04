//
//  ChannelInfo.h
//  PanoVideoCall
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PanoRtc/PanoRtcEngineKit.h"

NS_ASSUME_NONNULL_BEGIN

@interface ChannelInfo : NSObject

+ (NSString *)appId;
+ (NSString *)server;
+ (NSString *)token;
+ (NSString *)channelName;
+ (PanoChannelMode)channelMode;
+ (UInt64)userId;
+ (NSString *)userName;

+ (void)setChannelName:(NSString *)channelName;
+ (void)setChannelMode:(PanoChannelMode)channelMode;
+ (void)setUserName:(NSString *)userName;

@end

NS_ASSUME_NONNULL_END
