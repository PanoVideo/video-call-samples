//
//  ViewController.m
//  MagicVideo
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "ViewController.h"
#import "PanoRtc/PanoRtcEngineKit.h"

/* Please go to Pano Control Panel (https://console.pano.video/) to register your account,
   create your applicaiton, then get App ID and Temp Token. */
static NSString * _appId = @"<#Your App ID#>";
static NSString * _token = @"<#Token#>";
/* Please refer to Glossary to understand the meaning of App ID, Channel ID, Token, User ID, and User Name: https://developer.pano.video/getting-started/terms/
   请参考 名词解释 了解 App ID、Channel ID、Token、User ID、User Name 的含义：https://developer.pano.video/getting-started/terms/

   You can use temporary token for temporary testing: https://developer.pano.video/getting-started/firstapp/#14-%E7%94%9F%E6%88%90%E4%B8%B4%E6%97%B6token
   可以使用 临时token 来进行临时测试：https://developer.pano.video/getting-started/firstapp/#14-%E7%94%9F%E6%88%90%E4%B8%B4%E6%97%B6token
*/
static NSString * _server = @"api.pano.video";

@interface ViewController () <PanoRtcEngineDelegate>

@property (strong, nonatomic) PanoRtcEngineKit * engineKit;
@property (assign, nonatomic) BOOL magicEnabled;

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
    UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handleScalingGesture:)];
    [self.view addGestureRecognizer:pinchGesture];

    UIRotationGestureRecognizer *rotateGesture = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotateZGesture:)];
    [self.view addGestureRecognizer:rotateGesture];

    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotateXYGesture:)];
    panGesture.maximumNumberOfTouches = 1;
    panGesture.minimumNumberOfTouches = 1;
    [self.view addGestureRecognizer:panGesture];

    UIPanGestureRecognizer *panGestureTwo = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleProjectionDepthGesture:)];
    panGestureTwo.maximumNumberOfTouches = 2;
    panGestureTwo.minimumNumberOfTouches = 2;
    [self.view addGestureRecognizer:panGestureTwo];

    UITapGestureRecognizer *doubleTapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleResetTransParamsGesture:)];
    doubleTapGesture.numberOfTapsRequired = 2;
    doubleTapGesture.numberOfTouchesRequired = 1;
    [self.view addGestureRecognizer:doubleTapGesture];
}

- (void)handleScalingGesture: (UIPinchGestureRecognizer *)sender {
    PanoBuiltinTransformOption * option = self.baseTransformOption;
    option.scaling = sender.scale;
    [self.engineKit setOption:option forType:kPanoOptionBuiltinTransform];
    sender.scale = 1;
}

- (void)handleRotateZGesture: (UIRotationGestureRecognizer *) sender {
    PanoBuiltinTransformOption * option = self.baseTransformOption;
    option.zRotation = sender.rotation;
    [self.engineKit setOption:option forType:kPanoOptionBuiltinTransform];
}

- (void)handleRotateXYGesture: (UIPanGestureRecognizer *) sender {
    CGPoint dist = [sender translationInView:self.view];
    PanoBuiltinTransformOption * option = self.baseTransformOption;
    option.xRotation = dist.x;
    option.yRotation = dist.y;
    [self.engineKit setOption:option forType:kPanoOptionBuiltinTransform];
    [sender setTranslation:CGPointZero inView:self.view];
}

- (void)handleProjectionDepthGesture: (UIPanGestureRecognizer *) sender {
    CGPoint dist = [sender translationInView:self.view];
    PanoBuiltinTransformOption * option = self.baseTransformOption;
    if (dist.x>0) {
        option.xProjection = 0.05;
    } else if (dist.x<0) {
        option.xProjection = -0.05;
    }
    if (dist.y>0) {
        option.yProjection = 0.05f;
    } else if (dist.y<0) {
        option.yProjection = -0.05f;
    }
    [self.engineKit setOption:option forType:kPanoOptionBuiltinTransform];
    [sender setTranslation:CGPointZero inView:self.view];
}

-(void)handleResetTransParamsGesture: (UITapGestureRecognizer *) sender {
    PanoBuiltinTransformOption * option = self.baseTransformOption;
    option.bReset = true;
    [self.engineKit setOption:option forType:kPanoOptionBuiltinTransform];
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
