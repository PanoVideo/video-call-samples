//
//  ViewController.m
//  MagicVideo
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "ViewController.h"
#import "PanoRtc/PanoRtcEngineKit.h"

/* Please refer to Glossary to understand the meaning of App ID, Channel ID, Token, User ID, and User Name:
   请参考 名词解释 了解 App ID、Channel ID、Token、User ID、User Name 的含义：
   https://developer.pano.video/getting-started/terms/

   You can use temporary token for temporary testing:
   可以使用 临时token 来进行临时测试：https://developer.pano.video/getting-started/firstapp/#14-%E7%94%9F%E6%88%90%E4%B8%B4%E6%97%B6token
*/
static NSString * _appId = %% Your App ID %%;
static NSString * _token = %% Your Token %%;

static NSString * _server = @"api.pano.video";

@interface ViewController () <PanoRtcEngineDelegate>

@property (strong, nonatomic) PanoRtcEngineKit * engineKit;
@property (assign, nonatomic) BOOL magicEnabled;
@property (assign, nonatomic) PanoQuadIndex quadTransformIndex;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    UIApplication.sharedApplication.idleTimerDisabled = YES;
    self.magicEnabled = NO;
    [self initGestureRecognizers];
    [self createEngineKit];
    [self startPreview];
}

- (void)dealloc {
    [self stopPreview];
    [self destroyEngineKit];
    UIApplication.sharedApplication.idleTimerDisabled = NO;
}

#pragma mark IBAction Methods

- (IBAction)switchCamera:(UIButton *)sender {
    [self.engineKit switchCamera];
    [self restartPreview];
}

- (IBAction)enableMagicVideo:(UIButton *)sender {
    sender.selected = !sender.selected;
    self.magicEnabled = sender.selected;
    PanoBuiltinTransformOption * option = self.baseTransformOption;
    [self.engineKit setOption:option forType:kPanoOptionBuiltinTransform];
}

#pragma mark Gesture Methods

- (void)initGestureRecognizers {
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleQuadMoveGesture:)];
    panGesture.maximumNumberOfTouches = 1;
    panGesture.minimumNumberOfTouches = 1;
    [self.view addGestureRecognizer:panGesture];

    UITapGestureRecognizer *doubleTapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleResetTransParamsGesture:)];
    doubleTapGesture.numberOfTapsRequired = 2;
    doubleTapGesture.numberOfTouchesRequired = 1;
    [self.view addGestureRecognizer:doubleTapGesture];
}

- (void)handleQuadMoveGesture:(UIPanGestureRecognizer *)sender {
    if (sender.state == UIGestureRecognizerStateBegan) {
        CGPoint touchLoc = [sender locationInView:self.view];
        CGFloat halfWidth = self.view.frame.size.width/2.0f;
        CGFloat halfHeight = self.view.frame.size.height/2.0f;
        if (touchLoc.x<halfWidth && touchLoc.y<halfHeight) {
            self.quadTransformIndex = kPanoQuadTopLeft;
        } else if (touchLoc.x>=halfWidth && touchLoc.y<halfHeight) {
            self.quadTransformIndex = kPanoQuadTopRight;
        } else if (touchLoc.x<halfWidth && touchLoc.y>=halfHeight) {
            self.quadTransformIndex = kPanoQuadBottomLeft;
        } else {
            self.quadTransformIndex = kPanoQuadBottomRight;
        }
    } else if (sender.state == UIGestureRecognizerStateChanged) {
        CGPoint dist = [sender translationInView:self.view];
        PanoQuadTransformOption * option = [[PanoQuadTransformOption alloc] init];
        option.enable = self.magicEnabled;
        option.index = self.quadTransformIndex;
        option.xDeltaAxis = dist.x/self.view.frame.size.width;
        option.yDeltaAxis = dist.y/self.view.frame.size.height;
        option.bMirror = [self.engineKit isFrontCamera] ? YES : NO;
        [self.engineKit setOption:option forType:kPanoOptionQuadTransform];
    }

    [sender setTranslation:CGPointZero inView:self.view];
}

-(void)handleResetTransParamsGesture:(UITapGestureRecognizer *)sender {
    PanoQuadTransformOption *option = [[PanoQuadTransformOption alloc] init];
    option.bReset = true;
    [self.engineKit setOption:option forType:kPanoOptionQuadTransform];
}

#pragma mark Private Methods

- (void)createEngineKit {
    PanoRtcEngineConfig * engineConfig = [[PanoRtcEngineConfig alloc] init];
    engineConfig.appId = _appId;
    engineConfig.rtcServer = _server;
    self.engineKit = [PanoRtcEngineKit engineWithConfig:engineConfig delegate:self];
    engineConfig = nil;
}

- (void)destroyEngineKit {
    [self.engineKit destroy];
    self.engineKit = nil;
}

- (void)startPreview {
    [self.engineKit startPreviewWithView:self.view config:[self previewRenderConfig]];
}

- (void)stopPreview {
    [self.engineKit stopPreview];
}

- (void)restartPreview {
    [self.engineKit stopPreview];
    [self.engineKit startPreviewWithView:self.view config:[self previewRenderConfig]];
}

- (PanoRtcRenderConfig *)previewRenderConfig {
    PanoRtcRenderConfig * renderConfig = [PanoRtcRenderConfig new];
    renderConfig.profileType = kPanoProfileHD720P;
    renderConfig.scalingMode = kPanoScalingFit;
    renderConfig.mirror = [self.engineKit isFrontCamera] ? YES : NO;
    return renderConfig;
}

- (PanoBuiltinTransformOption *)baseTransformOption {
    PanoBuiltinTransformOption * option = [PanoBuiltinTransformOption new];
    option.enable = self.magicEnabled;
    return option;
}

@end
