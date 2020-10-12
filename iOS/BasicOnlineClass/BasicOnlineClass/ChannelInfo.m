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
static NSString * _token = @"<#Your Token#>";
/* Please refer to Glossary to understand the meaning of App ID, Channel ID, Token, User ID, and User Name: https://developer.pano.video/getting-started/terms/
   请参考 名词解释 了解 App ID、Channel ID、Token、User ID、User Name 的含义：https://developer.pano.video/getting-started/terms/

   You can use temporary token for temporary testing: https://developer.pano.video/getting-started/firstapp/#14-%E7%94%9F%E6%88%90%E4%B8%B4%E6%97%B6token
   可以使用 临时token 来进行临时测试：https://developer.pano.video/getting-started/firstapp/#14-%E7%94%9F%E6%88%90%E4%B8%B4%E6%97%B6token
*/
static NSString * _server = @"api.pano.video";
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
