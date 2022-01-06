//
//  JoinViewController.m
//  BasicVideoCall
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "JoinViewController.h"
#import "ChannelInfo.h"

@interface JoinViewController () <UITextFieldDelegate>

@property (strong, nonatomic) IBOutlet UITextField * channelId;
@property (strong, nonatomic) IBOutlet UITextField * userId;
@property (strong, nonatomic) IBOutlet UISegmentedControl * channelMode;

@end

@implementation JoinViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    _channelId.delegate = self;
    _userId.delegate = self;
}

- (BOOL)shouldPerformSegueWithIdentifier:(NSString *)identifier sender:(nullable id)sender {
    [self.view endEditing:YES];
    return _channelId.text.length > 0 ? YES : NO;
}

- (IBAction)changeChannelMode:(id)sender {
    PanoChannelMode mode = _channelMode.selectedSegmentIndex == 0 ? kPanoChannel1v1 : kPanoChannelMeeting;
    [ChannelInfo setChannelMode:mode];
}

- (void)textFieldDidEndEditing:(UITextField *)textField {
    if (textField == _channelId) {
        [ChannelInfo setChannelId:_channelId.text];
    } else if (textField == _userId) {
        [ChannelInfo setUserId:@([_userId.text doubleValue]).unsignedLongLongValue];
    }
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self.view endEditing:YES];
}

@end
