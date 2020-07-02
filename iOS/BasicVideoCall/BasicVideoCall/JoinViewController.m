//
//  JoinViewController.m
//  BasicVideoCall
//
//  Copyright © 2020 Pano. All rights reserved.
//

#import "JoinViewController.h"
#import "ChannelInfo.h"

@interface JoinViewController () <UITextFieldDelegate>

@property (strong, nonatomic) IBOutlet UITextField * channelName;
@property (strong, nonatomic) IBOutlet UITextField * userName;
@property (strong, nonatomic) IBOutlet UISegmentedControl * channelMode;

@end

@implementation JoinViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    _channelName.delegate = self;
    _userName.delegate = self;
}

- (BOOL)shouldPerformSegueWithIdentifier:(NSString *)identifier sender:(nullable id)sender {
    return _channelName.text.length > 0 ? YES : NO;
}

- (IBAction)changeChannelMode:(id)sender {
    PanoChannelMode mode = _channelMode.selectedSegmentIndex == 0 ? kPanoChannel1v1 : kPanoChannelMeeting;
    [ChannelInfo setChannelMode:mode];
}

- (void)textFieldDidEndEditing:(UITextField *)textField {
    if (textField == _channelName) {
        [ChannelInfo setChannelName:_channelName.text];
    } else if (textField == _userName) {
        [ChannelInfo setUserName:_userName.text];
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
