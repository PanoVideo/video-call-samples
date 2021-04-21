//
//  UserInfo.h
//  BasicVideoCall
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UserInfo : NSObject

@property (assign, nonatomic, readonly) UInt64 userId;
@property (strong, nonatomic, readonly) NSString * _Nullable userName;
@property (assign, nonatomic) BOOL screenEnable;
@property (assign, nonatomic) BOOL screenMute;
@property (strong, nonatomic) UIView * _Nullable screenView;

@end


@interface UserManager : NSObject

- (UserInfo *)addUser:(UInt64)userId withName:(NSString *)userName;
- (UserInfo *)removeUser:(UInt64)userId;
- (UserInfo *)findUser:(UInt64)userId;

- (UserInfo *)findUserWithView:(UIView *)view;
- (UserInfo *)findWatingUser;

@end

NS_ASSUME_NONNULL_END
