//
//  ChannelViewController.m
//  BasicVideoCall
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "ChannelViewController.h"
#import "ChannelInfo.h"
#import "UserInfo.h"

@interface ChannelViewController () <PanoRtcEngineDelegate>

@property (strong, nonatomic) IBOutlet UIView * remoteView;
@property (strong, nonatomic) IBOutlet UIView * localView;
@property (strong, nonatomic) IBOutlet UIView * thumbnail1View;
@property (strong, nonatomic) IBOutlet UIView * thumbnail2View;
@property (strong, nonatomic) IBOutlet UILabel * message;

@property (strong, nonatomic) PanoRtcEngineKit * engineKit;
@property (strong, nonatomic) UserManager * userManager;

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

- (IBAction)enableAudio:(UIButton *)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
        [self.engineKit unmuteAudio];
    } else {
        [self.engineKit muteAudio];
    }
}

- (IBAction)switchSpeaker:(UIButton *)sender {
    sender.selected = !sender.selected;
    [self.engineKit setLoudspeakerStatus:sender.selected];
}

- (IBAction)enableVideo:(UIButton *)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
        [self startVideo];
    } else {
        [self.engineKit stopVideo];
    }
}

- (IBAction)switchCamera:(UIButton *)sender {
    [self.engineKit switchCamera];
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
            // Switch speaker and start audio.
            [self.engineKit setLoudspeakerStatus:YES];
            [self.engineKit startAudio];
            // Switch front camera and start video.
            if ([self.engineKit isFrontCamera] == NO) {
                [self.engineKit switchCamera];
            }
            [self startVideo];
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

- (void)onUserJoinIndication:(UInt64)userId
                    withName:(NSString * _Nullable)userName {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) join channel.", userId, userName]];
        [self.userManager addUser:userId withName:userName];
    });
}

- (void)onUserLeaveIndication:(UInt64)userId
                   withReason:(PanoUserLeaveReason)reason {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager removeUser:userId];
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) leave channel with reason: %ld.", userId, user.userName, reason]];
        if (user.videoView) {
            // Find a waiting user and subscribe it.
            UserInfo * waitingUser = [self.userManager findWatingUser];
            if (waitingUser) {
                waitingUser.videoView = user.videoView;
                [self subscribeVideo:waitingUser.userId withView:waitingUser.videoView];
            }
            // Clear video view of the leave user.
            user.videoView = nil;
        }
    });
}

- (void)onUserAudioStart:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:userId];
        user.audioEnable = YES;
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) start audio.", userId, user.userName]];
    });
}

- (void)onUserAudioStop:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:userId];
        user.audioEnable = NO;
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) stop audio.", userId, user.userName]];
    });
}

- (void)onUserAudioMute:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:userId];
        user.audioMute = YES;
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) mute audio.", userId, user.userName]];
    });
}

- (void)onUserAudioUnmute:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:userId];
        user.audioMute = NO;
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) unmute audio.", userId, user.userName]];
    });
}

- (void)onUserVideoStart:(UInt64)userId
          withMaxProfile:(PanoVideoProfileType)maxProfile {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:userId];
        user.videoEnable = YES;
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) start video.", userId, user.userName]];
        user.videoView = [self findIdleVideoView];
        if (user.videoView) {
            [self subscribeVideo:userId withView:user.videoView];
        }
    });
}

- (void)onUserVideoStop:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:userId];
        user.videoEnable = NO;
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) stop video.", userId, user.userName]];
        if (user.videoView) {
            // Find a waiting user and subscribe it.
            UserInfo * waitingUser = [self.userManager findWatingUser];
            if (waitingUser) {
                waitingUser.videoView = user.videoView;
                [self subscribeVideo:waitingUser.userId withView:waitingUser.videoView];
            }
            // Clear video view of the stoped video user.
            user.videoView = nil;
        }
    });
}

- (void)onUserVideoMute:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:userId];
        user.videoMute = YES;
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) mute video.", userId, user.userName]];
    });
}

- (void)onUserVideoUnmute:(UInt64)userId {
    dispatch_async(dispatch_get_main_queue(), ^{
        UserInfo * user = [self.userManager findUser:userId];
        user.videoMute = NO;
        [self displayMessage:[NSString stringWithFormat:@"User %llu(%@) unmute video.", userId, user.userName]];
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
    channelConfig.userName = [@"iOS_" stringByAppendingString:@(ChannelInfo.userId).stringValue];
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

- (PanoVideoProfileType)getSubscribeProfile:(UIView *)videoView {
    PanoVideoProfileType profile = kPanoProfileLow;
    if (videoView == self.remoteView) {
        profile = kPanoProfileMax;
    }
    return profile;
}

- (PanoResult)startVideo {
    PanoRtcRenderConfig * renderConfig = [[PanoRtcRenderConfig alloc] init];
    renderConfig.profileType = kPanoProfileHD720P;
    renderConfig.scalingMode = kPanoScalingCropFill;
    renderConfig.mirror = YES;
    return [self.engineKit startVideoWithView:self.localView config:renderConfig];
}

- (PanoResult)subscribeVideo:(UInt64)userId withView:view {
    PanoRtcRenderConfig * renderConfig = [[PanoRtcRenderConfig alloc] init];
    renderConfig.profileType = [self getSubscribeProfile:view];
    renderConfig.scalingMode = kPanoScalingCropFill;
    return [self.engineKit subscribeVideo:userId withView:view config:renderConfig];
}

@end
