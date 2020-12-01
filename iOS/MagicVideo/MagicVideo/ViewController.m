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

- (void)handleScalingGesture: (UIPinchGestureRecognizer *)pinchRecognizer {
    CGFloat xScale = 1.0f;
    CGFloat yScale = 1.0f;
    double theSlope;

    if ([pinchRecognizer state] == UIGestureRecognizerStateBegan ||
            [pinchRecognizer state] == UIGestureRecognizerStateChanged) {
        if ([pinchRecognizer numberOfTouches] > 1) {
            UIView *theView = [pinchRecognizer view];
            CGPoint locationOne = [pinchRecognizer locationOfTouch:0 inView:theView];
            CGPoint locationTwo = [pinchRecognizer locationOfTouch:1 inView:theView];
            if (locationOne.x == locationTwo.x) {
                    // perfect vertical line
                    // not likely, but to avoid dividing by 0 in the slope equation
                theSlope = 1000.0;
            }else if (locationOne.y == locationTwo.y) {
                    // perfect horz line
                    // not likely, but to avoid any problems in the slope equation
                theSlope = 0.0;
            }else {
                theSlope = (locationTwo.y - locationOne.y)/(locationTwo.x - locationOne.x);
            }
            double abSlope = (theSlope>0 ? theSlope : -theSlope);
            if (abSlope < 0.5) {
                //  Horizontal pinch - scale in the X
                xScale = pinchRecognizer.scale;
            }else if (abSlope > 1.7) {
                // Vertical pinch - scale in the Y
                yScale = pinchRecognizer.scale;
            } else {
                // Diagonal pinch - scale in both directions
                xScale = yScale = pinchRecognizer.scale;
            }  // else for diagonal pinch
        }  // if numberOfTouches
    }  // StateBegan if

    PanoBuiltinTransformOption * option = self.baseTransformOption;
    option.xScaling = xScale;
    option.yScaling = yScale;
    [self.engineKit setOption:option forType:kPanoOptionBuiltinTransform];
    pinchRecognizer.scale = 1;
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
