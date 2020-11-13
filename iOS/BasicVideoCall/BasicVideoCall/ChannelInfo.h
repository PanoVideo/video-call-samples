//
//  ChannelInfo.h
//  BasicVideoCall
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
+ (NSString *)channelId;
+ (PanoChannelMode)channelMode;
+ (UInt64)userId;
+ (NSString *)userName;

+ (void)setChannelId:(NSString *)channelId;
+ (void)setChannelMode:(PanoChannelMode)channelMode;
+ (void)setUserName:(NSString *)userName;

@end

NS_ASSUME_NONNULL_END
