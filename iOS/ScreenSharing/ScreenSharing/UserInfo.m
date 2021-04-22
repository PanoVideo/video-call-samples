//
//  UserInfo.m
//  BasicVideoCall
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "UserInfo.h"

@implementation UserInfo

- (instancetype)initWithId:(UInt64)userId name:(NSString *)userName {
    self = [super init];
    if (nil != self) {
        _userId = userId;
        _userName = userName;
        _screenEnable = NO;
        _screenMute = NO;
        _screenView = nil;
    }
    return self;
}

@end


@interface UserManager ()

@property (strong, nonatomic) NSMutableArray<UserInfo *> * userInfos;

@end

@implementation UserManager

- (instancetype)init {
    self = [super init];
    if (nil != self) {
        self.userInfos = [NSMutableArray arrayWithCapacity:10];
    }
    return self;
}

- (UserInfo *)addUser:(UInt64)userId withName:(NSString *)userName {
    UserInfo * userInfo = [[UserInfo alloc] initWithId:userId name:userName];
    [self.userInfos addObject:userInfo];
    return userInfo;
}

- (UserInfo *)removeUser:(UInt64)userId {
    UserInfo * userInfo = nil;
    for (userInfo in self.userInfos) {
        if (userInfo.userId == userId) {
            [self.userInfos removeObject:userInfo];
            break;
        }
    }
    return userInfo;
}

- (UserInfo *)findUser:(UInt64)userId {
    UserInfo * userInfo = nil;
    for (userInfo in self.userInfos) {
        if (userInfo.userId == userId) {
            break;
        }
    }
    return userInfo;
}

- (UserInfo *)findUserWithView:(UIView *)view {
    UserInfo * userInfo = nil;
    for (userInfo in self.userInfos) {
        if (userInfo.screenView == view) {
            break;
        }
    }
    return userInfo;
}

- (UserInfo *)findWatingUser {
    UserInfo * userInfo = nil;
    for (userInfo in self.userInfos) {
        if (userInfo.screenEnable && userInfo.screenView == nil) {
            break;
        }
    }
    return userInfo;
}

@end
