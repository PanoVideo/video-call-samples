//
//  SampleHandler.m
//  ScreenSharingExt
//
//  Created by mark on 2021/4/21.
//


#import "SampleHandler.h"
#import <PanoReplayKitExt/PanoReplayKitExt.h>

/**
 * 配置 App Group ID 可以获得更稳定、高效的数据传输。
 * 如何获取 App Group ID？
 * 1. Xcode 工程中进入 Signing & Capabilities
 * 2. Capability 添加 App Groups
 * 3. 点击“+”新增自定义 App Group ID
 */
static NSString *kAppGroupId = <#NSString * Your App Group ID#>;

@interface SampleHandler () <PanoScreenSharingExtDelegate>

@end

@implementation SampleHandler

- (void)broadcastStartedWithSetupInfo:(NSDictionary<NSString *,NSObject *> *)setupInfo {
    // User has requested to start the broadcast. Setup info from the UI extension can be supplied but optional.
    [PanoScreenSharingExt.sharedInstance setupWithAppGroup:kAppGroupId delegate:self];
}

- (void)broadcastPaused {
    // User has requested to pause the broadcast. Samples will stop being delivered.
}

- (void)broadcastResumed {
    // User has requested to resume the broadcast. Samples delivery will resume.
}

- (void)broadcastFinished {
    // User has requested to finish the broadcast.
    [PanoScreenSharingExt.sharedInstance finishScreenSharing];
}

- (void)processSampleBuffer:(CMSampleBufferRef)sampleBuffer withType:(RPSampleBufferType)sampleBufferType {
    
    switch (sampleBufferType) {
        case RPSampleBufferTypeVideo:
            // Handle video sample buffer
            [PanoScreenSharingExt.sharedInstance sendVideoSampleBuffer:sampleBuffer];
            break;
        case RPSampleBufferTypeAudioApp:
            // Handle audio sample buffer for app audio
            break;
        case RPSampleBufferTypeAudioMic:
            // Handle audio sample buffer for mic audio
            break;
            
        default:
            break;
    }
}

//MARK: - PanoScreenSharingExtDelegate

- (void)screenSharingFinished:(PanoScreenSharingResult)reason error:(nullable NSError *)error {
    NSString *log;
    switch (reason) {
        case PanoScreenSharingResultVersionMismatch:
            log = @"PanoReplayKitExt SDK version is mismatch with Pano SDK";
            break;
        case PanoScreenSharingResultCloseByHost:
            log = @"Screen share is closed by host app";
            break;
        case PanoScreenSharingResultDisconnected:
            log = @"The connect with host app is abnormally disconnected";
            break;
        default:
            break;
    }
    
    NSError *err = [NSError errorWithDomain:error.domain code:error.code userInfo:@{ NSLocalizedFailureReasonErrorKey : log }];
    [self finishBroadcastWithError:err];
}

@end
