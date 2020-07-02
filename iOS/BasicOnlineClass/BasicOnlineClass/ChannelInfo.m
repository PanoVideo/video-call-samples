//
//  ChannelInfo.m
//  BasicOnlineClass
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "ChannelInfo.h"

/* Please go to Pano Control Panel (https://console.pano.video/) to register your account,
   create your applicaiton, then get App ID and Temp Token. */
static NSString * _appId = @"<#Your App ID#>";
static NSString * _server = @"api.pano.video";
static NSString * _token = @"<#Temp Token#>";
static NSString * _channelName = nil;
static PanoChannelMode _channelMode = kPanoChannel1v1;
static NSString * _userName = nil;

@implementation ChannelInfo

+ (NSString *)appId {
    return _appId;
}

+ (NSString *)server {
    return _server;
}

+ (NSString *)token {
    return _token;
}

+ (NSString *)channelName {
    return _channelName;
}

+ (PanoChannelMode)channelMode {
    return _channelMode;
}

+ (UInt64)userId {
    return arc4random();
}

+ (NSString *)userName {
    return _userName;
}

+ (void)setChannelName:(NSString *)channelName {
    _channelName = channelName;
}

+ (void)setChannelMode:(PanoChannelMode)channelMode {
    _channelMode = channelMode;
}

+ (void)setUserName:(NSString *)userName {
    _userName = userName;
}

@end
