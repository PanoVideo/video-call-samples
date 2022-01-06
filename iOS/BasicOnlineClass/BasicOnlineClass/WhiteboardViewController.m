//
//  WhiteboardViewController.m
//  BasicOnlineClass
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "WhiteboardViewController.h"

static UInt32 kDefaultLineWidth = 5;
static UInt32 kDefaultFontSize = 15;
static NSString * kDefaultImageUrl = @"https://www.pano.video/assets/img/optimized_logo-pano.png";

@interface WhiteboardViewController () <PanoRtcWhiteboardDelegate>

@property (strong, nonatomic) IBOutlet UIView *drawView;

@property (strong, nonatomic) IBOutlet UIBarButtonItem * selectItem;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * drawItem;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * textItem;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * eraserItem;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * styleItem;

@property (strong, nonatomic) IBOutlet UIToolbar * drawtToolbar;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * pathItem;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * lineItem;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * rectItem;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * ellipseItem;
@property (strong, nonatomic) IBOutlet UIBarButtonItem * arrowItem;

@property (strong, nonatomic) IBOutlet UIView * styleView;
@property (strong, nonatomic) IBOutlet UITextField * lineWidth;
@property (strong, nonatomic) IBOutlet UIStepper * lineWidthStepper;
@property (strong, nonatomic) IBOutlet UIButton * boldStyle;
@property (strong, nonatomic) IBOutlet UIButton * italicStyle;
@property (strong, nonatomic) IBOutlet UITextField * fontSize;
@property (strong, nonatomic) IBOutlet UIStepper * fontSizeStepper;

@property (strong, nonatomic) IBOutlet UILabel * pageNumber;
@property (strong, nonatomic) IBOutlet UILabel * zoomScale;
@property (strong, nonatomic) IBOutlet UILabel * message;

@property (strong, nonatomic) UIColor * selectColor;
@property (strong, nonatomic) UIColor * unselectColor;

@property (assign, nonatomic) PanoWBPageNumber curPage;
@property (assign, nonatomic) UInt32 totalPages;

@end

@implementation WhiteboardViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.selectColor = UIColor.greenColor;
    self.unselectColor = nil;

    [self initToolType];
    [self initLineWidth];
    [self initForegroundColor];
    [self initFontStyle];
    [self initFontSize];
    [self initPageNumber];
    [self initZoomScale];
    [self.whiteboardEngine setDelegate:self];
    [self.whiteboardEngine open:self.drawView];
}

- (void)close {
    [self.whiteboardEngine close];
    [self.whiteboardEngine setDelegate:nil];
    [self dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark IBActions

- (IBAction)exitWhiteboard:(id)sender {
    [self close];
}

- (IBAction)setSelectTool:(id)sender {
    [self hideAllSubToolViews];
    if (self.selectItem.tintColor != self.selectColor) {
        [self clearAllToolItemSelectColor];
        self.selectItem.tintColor = self.selectColor;
        [self.whiteboardEngine setToolType:kPanoWBToolSelect];
    }
}

- (IBAction)showDrawToolbar:(id)sender {
    [self hideAllSubToolViews];
    self.drawtToolbar.hidden = NO;
}

- (IBAction)setPathTool:(id)sender {
    if ([self selectDrawItem:self.pathItem]) {
        [self.whiteboardEngine setToolType:kPanoWBToolPath];
    }
}

- (IBAction)setLineTool:(id)sender {
    if ([self selectDrawItem:self.lineItem]) {
        [self.whiteboardEngine setToolType:kPanoWBToolLine];
    }
}

- (IBAction)setRectTool:(id)sender {
    if ([self selectDrawItem:self.rectItem]) {
        [self.whiteboardEngine setToolType:kPanoWBToolRect];
    }
}

- (IBAction)setEllipseTool:(id)sender {
    if ([self selectDrawItem:self.ellipseItem]) {
        [self.whiteboardEngine setToolType:kPanoWBToolEllipse];
    }
}

- (IBAction)setArrowTool:(id)sender {
    if ([self selectDrawItem:self.arrowItem]) {
        [self.whiteboardEngine setToolType:kPanoWBToolArrow];
    }
}

- (IBAction)setTextTool:(id)sender {
    [self hideAllSubToolViews];
    if (self.textItem.tintColor != self.selectColor) {
        [self clearAllToolItemSelectColor];
        self.textItem.tintColor = self.selectColor;
        [self.whiteboardEngine setToolType:kPanoWBToolText];
    }
}

- (IBAction)setEraserTool:(id)sender {
    [self hideAllSubToolViews];
    if (self.eraserItem.tintColor != self.selectColor) {
        [self clearAllToolItemSelectColor];
        self.eraserItem.tintColor = self.selectColor;
        [self.whiteboardEngine setToolType:kPanoWBToolDelete];
    }
}

- (IBAction)showStyleView:(id)sender {
    self.drawtToolbar.hidden = YES;
    self.styleView.hidden = !self.styleView.hidden;
}

- (IBAction)setImage:(id)sender {
    [self hideAllSubToolViews];
    [self.whiteboardEngine setBackgroundImageScalingMode:kPanoWBImageScalingFit];
    [self.whiteboardEngine setBackgroundImage:kDefaultImageUrl];
}

- (IBAction)snapshotWhitboard:(id)sender {
    NSString * documentPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    if (documentPath) {
        [self.whiteboardEngine snapshot:kPanoWBSnapshotAll path:documentPath];
    }
}

- (IBAction)undoOperation:(id)sender {
    [self hideAllSubToolViews];
    [self.whiteboardEngine undo];
}

- (IBAction)redoOperation:(id)sender {
    [self hideAllSubToolViews];
    [self.whiteboardEngine redo];
}

- (IBAction)selectColor:(UIButton*)button {
    self.styleItem.tintColor = button.backgroundColor;
    PanoWBColor * fgColor = [self convertWBColor:button.backgroundColor];
    [self.whiteboardEngine setForegroundColor:fgColor];
}

- (IBAction)changeLineWidth:(UIStepper*)stepper {
    self.lineWidth.text = [NSString stringWithFormat:@"%u", (unsigned int)stepper.value];
    [self.whiteboardEngine setLineWidth:stepper.value];
}

- (IBAction)selectFontStyle:(UIButton *)button {
    button.selected = !button.selected;
    [self.whiteboardEngine setFontStyle:[self getFontStyle]];
}

- (IBAction)changeFontSize:(UIStepper *)stepper {
    self.fontSize.text = [NSString stringWithFormat:@"%u", (unsigned int)stepper.value];
    [self.whiteboardEngine setFontSize:stepper.value];
}

- (IBAction)prevPage:(UIButton *)button {
    [self.whiteboardEngine prevPage];
}

- (IBAction)nextPage:(UIButton *)button {
    [self.whiteboardEngine nextPage];
}

- (IBAction)addPage:(UIButton *)button {
    [self.whiteboardEngine addPage:YES];
}

- (IBAction)removePage:(UIButton *)button {
    [self.whiteboardEngine removePage:self.curPage];
}

#pragma mark Callback Methods

- (void)onPageNumberChanged:(PanoWBPageNumber)curPage withTotalPages:(UInt32)totalPages {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.curPage = curPage;
        self.totalPages = totalPages;
        self.pageNumber.text = [NSString stringWithFormat:@"%u of %u", (unsigned int)curPage, (unsigned int)totalPages];
    });
}

- (void)onImageStateChanged:(PanoWBImageState)state withUrl:(NSString * _Nonnull)url {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"The image state change to %ld", (long)state]];
    });
}

- (void)onViewScaleChanged:(Float32)scale {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.zoomScale.text = [NSString stringWithFormat:@"%d%%", (int)(scale * 100)];
    });
}

- (void)onSnapshotComplete:(PanoResult)result name:(NSString *)filename {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self displayMessage:[NSString stringWithFormat:@"The whiteboard snapshot complete with %ld", (long)result]];
        if (kPanoResultOK == result) {
            UIImage *uiImage = [UIImage imageWithContentsOfFile:filename];
            if (uiImage) {
                UIImageWriteToSavedPhotosAlbum(uiImage, nil, nil, nil);
            }
        }
    });
}

#pragma mark Private

- (void)initToolType {
    [self selectDrawItem:self.pathItem];
    [self.whiteboardEngine setToolType:kPanoWBToolPath];
}

- (void)initLineWidth {
    self.lineWidth.text = [NSString stringWithFormat:@"%u", (unsigned int)kDefaultLineWidth];
    self.lineWidthStepper.value = kDefaultLineWidth;
    [self.whiteboardEngine setLineWidth:kDefaultLineWidth];
}

- (void)initForegroundColor {
    self.styleItem.tintColor = UIColor.blackColor;
    PanoWBColor * fgColor = [self convertWBColor:UIColor.blackColor];
    [self.whiteboardEngine setForegroundColor:fgColor];
}

- (void)initFontStyle {
    self.boldStyle.selected = NO;
    self.italicStyle.selected = NO;
    [self.whiteboardEngine setFontStyle:[self getFontStyle]];
}

- (void)initFontSize {
    self.fontSize.text = [NSString stringWithFormat:@"%u", (unsigned int)kDefaultFontSize];
    self.fontSizeStepper.value = kDefaultFontSize;
    [self.whiteboardEngine setFontSize:kDefaultFontSize];
}

- (void)initPageNumber {
    self.curPage = [self.whiteboardEngine getCurrentPageNumber];
    self.totalPages = [self.whiteboardEngine getTotalNumberOfPages];
    self.pageNumber.text = [NSString stringWithFormat:@"%u of %u", (unsigned int)self.curPage, (unsigned int)self.totalPages];
}

- (void)initZoomScale {
    Float32 scale = [self.whiteboardEngine getCurrentScaleFactor];
    self.zoomScale.text = [NSString stringWithFormat:@"%d%%", (int)(scale * 100)];
}

- (BOOL)selectDrawItem:(UIBarButtonItem *)item {
    BOOL select = NO;
    self.drawtToolbar.hidden = YES;
    if (item.tintColor != self.selectColor) {
        [self clearAllToolItemSelectColor];
        item.tintColor = self.selectColor;
        self.drawItem.title = item.title;
        self.drawItem.image = item.image;
        self.drawItem.tintColor = self.selectColor;
        select = YES;
    }
    return select;
}

- (PanoWBFontStyle)getFontStyle {
    PanoWBFontStyle style = kPanoWBFontNormal;
    BOOL bold = self.boldStyle.selected;
    BOOL italic = self.italicStyle.selected;
    if (bold && italic) {
        style = kPanoWBFontBoldItalic;
    } else if (bold) {
        style = kPanoWBFontBold;
    } else if (italic) {
        style = kPanoWBFontItalic;
    }
    return style;
}

- (void)clearAllToolItemSelectColor {
    self.selectItem.tintColor = self.unselectColor;
    self.drawItem.tintColor = self.unselectColor;
    self.pathItem.tintColor = self.unselectColor;
    self.lineItem.tintColor = self.unselectColor;
    self.rectItem.tintColor = self.unselectColor;
    self.ellipseItem.tintColor = self.unselectColor;
    self.arrowItem.tintColor = self.unselectColor;
    self.textItem.tintColor = self.unselectColor;
    self.eraserItem.tintColor = self.unselectColor;
}

- (void)hideAllSubToolViews {
    self.drawtToolbar.hidden = YES;
    self.styleView.hidden = YES;
}

- (void)displayMessage:(NSString *)message {
    self.message.text = message;
    self.message.hidden = NO;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        self.message.hidden = YES;
    });
}

- (PanoWBColor *)convertWBColor:(UIColor *)color {
    CGFloat red = 0.0, green = 0.0, blue = 0.0, alpha = 0.0;
    [color getRed:&red green:&green blue:&blue alpha:&alpha];
    PanoWBColor * wbColor = [[PanoWBColor alloc] init];
    wbColor.red = red;
    wbColor.green = green;
    wbColor.blue = blue;
    wbColor.alpha = alpha;
    return wbColor;
}

@end
