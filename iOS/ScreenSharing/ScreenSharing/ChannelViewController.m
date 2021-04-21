//
//  ChannelViewController.m
//  BasicVideoCall
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "ChannelViewController.h"
#import "ChannelInfo.h"
#import "UserInfo.h"
#import <ReplayKit/ReplayKit.h>

@interface ChannelViewController () <PanoRtcEngineDelegate>

@property (strong, nonatomic) IBOutlet UIView * remoteView;
@property (strong, nonatomic) IBOutlet UIView * thumbnail1View;
@property (strong, nonatomic) IBOutlet UIView * thumbnail2View;
@property (strong, nonatomic) IBOutlet UILabel * message;

@property (strong, nonatomic) PanoRtcEngineKit * engineKit;
@property (strong, nonatomic) UserManager * userManager;

@property (strong, nonatomic) RPSystemBroadcastPickerView *broadcastPickerView;

@end

@implementation ChannelViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    UIApplication.sharedApplication.idleTimerDisabled = YES;
    [self initUserManager];
    [self createEngineKit];
    [self joinChannel];
}

- (IBAction)enableScreen:(UIButton *)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
        [self.engineKit startScreenWithAppGroupId:[ChannelInfo appGroupId]];
        [self launchBroadcastPickerView];
    } else {
        [self.engineKit stopScreen];
    }
}

- (IBAction)exitChannel:(UIButton *)sender {
    [self leaveChannel];
    [self destroyEngineKit];
    UIApplication.sharedApplication.idleTimerDisabled = NO;
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)onChannelJoinConfirm:(PanoResult)result {
    dispatch_async(dispatch_get_main_queue(), ^{
        if (result == kPanoResultOK) {
            [self displayMessage:[NSString stringWithFormat:@"Join channel successfully."]];
        } else {
            [self displayMessage:[NSString stringWithFormat:@"Join channel failed with error: %ld.", (long)result]];
        }
    });
}

- (void)onChannelLeaveIndication:(PanoResult)result {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Leave channel with indication: %ld.", (long)result]];
    });
}

- (void)onChannelFailover:(PanoFailoverState)state {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Failover channel with state: %ld.", (long)state]];
    });
}

- (void)onScreenStartResult:(PanoResult)result {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"Start Screen Sharing with result: %ld.", (long)result]];
    });
}

- (void)onUserScreenStart:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"User %@ (%lld) start Screen.", nil, userId]];
        UserInfo * user = [self.userManager findUser:userId];
        user.screenEnable = YES;
        user.screenView = [self findIdleVideoView];
        if (user.screenView) {
            [self subscribeScreen:userId withView:user.screenView];
        }
    });
}

- (void)onUserScreenStop:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"User %@ (%lld) stop screen.", nil, userId]];
        UserInfo * user = [self.userManager findUser:userId];
        user.screenEnable = NO;
        if (user.screenView) {
            // Find a waiting user and subscribe it.
            UserInfo * waitingUser = [self.userManager findWatingUser];
            if (waitingUser) {
                waitingUser.screenView = user.screenView;
                [self subscribeScreen:waitingUser.userId withView:waitingUser.screenView];
            }
            // Clear video view of the stoped video user.
            user.screenView = nil;
        }
    });
}

- (void)onUserScreenMute:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"User %@ (%lld) mute screen.", nil, userId]];
        UserInfo * user = [self.userManager findUser:userId];
        user.screenMute = YES;
    });
}

- (void)onUserScreenUnmute:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"User %@ (%lld) unmute screen.", nil, userId]];
        UserInfo * user = [self.userManager findUser:userId];
        user.screenMute = NO;
    });
}

- (void)createEngineKit {
    PanoRtcEngineConfig * engineConfig = [[PanoRtcEngineConfig alloc] init];
    engineConfig.appId = ChannelInfo.appId;
    engineConfig.rtcServer = ChannelInfo.server;
    self.engineKit = [PanoRtcEngineKit engineWithConfig:engineConfig delegate:self];
    engineConfig = nil;
}

- (void)destroyEngineKit {
    [self.engineKit destroy];
    self.engineKit = nil;
}

- (void)joinChannel {
    PanoRtcChannelConfig * channelConfig = [[PanoRtcChannelConfig alloc] init];
    channelConfig.mode = ChannelInfo.channelMode;
    channelConfig.userName = ChannelInfo.userName;
    PanoResult result = [self.engineKit joinChannelWithToken:ChannelInfo.token
                                                   channelId:ChannelInfo.channelId
                                                      userId:ChannelInfo.userId
                                                      config:channelConfig];
    if (result != kPanoResultOK) {
        [self displayMessage:[NSString stringWithFormat:@"Join channel failed with error: %ld.", (long)result]];
    }
}

- (void)leaveChannel {
    [self.engineKit leaveChannel];
}

- (void)initUserManager {
    self.userManager = [[UserManager alloc] init];
}

- (PanoResult)subscribeScreen:(UInt64)userId withView:view {
    return [self.engineKit subscribeScreen:userId withView:view];
}

- (void)displayMessage:(NSString *)message {
    self.message.text = message;
    self.message.hidden = NO;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        self.message.hidden = YES;
    });
}

- (UIView *)findIdleVideoView {
    NSArray<UIView *> *videoViews = @[self.remoteView,
                                      self.thumbnail1View,
                                      self.thumbnail2View];
    UIView * videoView = nil;
    for (videoView in videoViews) {
        if ([self.userManager findUserWithView:videoView] == nil) {
            break;
        }
    }
    return videoView;
}

- (void)launchBroadcastPickerView {
    if (@available(iOS 12.0, *)) {
        if (!self.broadcastPickerView) {
            RPSystemBroadcastPickerView *pickerView = [[RPSystemBroadcastPickerView alloc] initWithFrame:CGRectMake(0, 0, 44, 44)];
            pickerView.showsMicrophoneButton = NO;
            pickerView.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin;
            NSString *pluginPath = [NSBundle mainBundle].builtInPlugInsPath;
            NSArray *contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:pluginPath error:nil];
            for (NSString *content in contents) {
                if (![content hasSuffix:@".appex"]) {
                    continue;
                }
                NSBundle *bundle = [NSBundle bundleWithPath:[[NSURL fileURLWithPath:pluginPath] URLByAppendingPathComponent:content].path];
                if (bundle) {
                    NSString *identifier = [bundle.infoDictionary valueForKeyPath:@"NSExtension.NSExtensionPointIdentifier"];
                    if ([identifier isEqualToString:@"com.apple.broadcast-services-upload"]) {
                        pickerView.preferredExtension = bundle.bundleIdentifier;
                    }
                }
            }
            self.broadcastPickerView = pickerView;
        }
        
        for (UIView *view in self.broadcastPickerView.subviews) {
            if ([view isKindOfClass:[UIButton class]]) {
                [(UIButton *)view sendActionsForControlEvents:UIControlEventAllEvents];
            }
        }
    } else if (@available(iOS 11.0, *)) {
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            
        }];
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:@"iOS 11 Device need go to Control Center and long press Screen Recording, then select ScreenSharingExt to start." preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:cancel];
        [self presentViewController:alertController animated:YES completion:nil];
    } else {
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            
        }];
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:@"Screen Sharing not support below iOS 11 Device." preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:cancel];
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

@end
