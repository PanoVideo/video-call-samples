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

+ (void)setChannelId:(NSString *)channelId;
+ (void)setChannelMode:(PanoChannelMode)channelMode;
+ (void)setUserId:(UInt64)userId;

@end

NS_ASSUME_NONNULL_END
