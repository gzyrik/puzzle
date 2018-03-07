#import <UIKit/UIKit.h>
#include "PickImage.h"

@interface iOSImagePicker : NSObject
@end
static iOSImagePicker* _picker;
@interface iOSImagePicker () <UINavigationControllerDelegate, UIImagePickerControllerDelegate>
@end
@implementation iOSImagePicker {
    UIViewController* _mainViewController;
    UIWindow* _mainWindow;
    void (^_completion)(cocos2d::Texture2D*);
}
-(instancetype)init:(void (^)(void))transition completion:(void (^)(cocos2d::Texture2D*))completion
{
    self = [super init];
    if (!self)
        return nil;
    _completion = completion;
    _mainWindow = [UIApplication sharedApplication].keyWindow;
    UIViewController* viewCtrl = _mainWindow.rootViewController;
    NSArray* windows = [UIApplication sharedApplication].windows;
    for(int i=(int)windows.count-1; i >= 0 && !viewCtrl; --i){
        _mainWindow = windows[i];
        viewCtrl = _mainWindow.rootViewController;
    }
    
    _mainViewController = viewCtrl;
    UIImagePickerController *pickerImage = [[UIImagePickerController alloc] init];
    if([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeSavedPhotosAlbum]) {
        pickerImage.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
        //pickerImage.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
        pickerImage.mediaTypes = [UIImagePickerController availableMediaTypesForSourceType:pickerImage.sourceType];
    }
    pickerImage.delegate = self;
    pickerImage.allowsEditing = YES;
    [viewCtrl dismissViewControllerAnimated:YES completion:^{
        if (transition) {
            transition();
        }
    }];
    _mainWindow.rootViewController = pickerImage;
    [UIView transitionWithView:_mainWindow
                      duration:0.5
                       options:UIViewAnimationOptionTransitionFlipFromLeft
                    animations:nil
                    completion:nil];
    return self;
}

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingImage:(UIImage *)image editingInfo:(NSDictionary *)editingInfo
{
    CGImageRef imageRef = [image CGImage];
    int width = (int)CGImageGetWidth(imageRef);
    int height = (int)CGImageGetHeight(imageRef);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    int data_len = height * width * 4;
    unsigned char* rawData = (unsigned char*) calloc(data_len, sizeof(unsigned char));
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    NSUInteger bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(rawData, width, height, bitsPerComponent,
                                                 bytesPerRow, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageRef);
    CGContextRelease(context);
    cocos2d::Texture2D *pickedImage = new cocos2d::Texture2D();
    pickedImage->initWithData(rawData, data_len, cocos2d::Texture2D::PixelFormat::RGBA8888, width, height, cocos2d::Size(width, height));
    pickedImage->autorelease();
    [self finished:picker result:pickedImage];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [self finished:picker result:nullptr];
}
-(void) finished:(UIImagePickerController *)picker result:(cocos2d::Texture2D*)tex2D
{
    [picker dismissViewControllerAnimated:YES completion:nil];
    _mainWindow.rootViewController = _mainViewController;
    _completion(tex2D);
    [UIView transitionWithView:_mainWindow
                      duration:0.5
                       options:UIViewAnimationOptionTransitionFlipFromRight|UIViewAnimationOptionAllowAnimatedContent
                    animations:nil
                    completion:nil];
    _mainViewController = nil;
    _mainWindow = nil;
    _picker = nil;

}
@end

void pickImage(const std::function<void()>& transition,
               const std::function<void(cocos2d::Texture2D*)>& completion)
{
    std::function<void()> middle(transition);
    std::function<void(cocos2d::Texture2D*)> finish(completion);

    _picker = [[iOSImagePicker alloc] init:^{
                                    middle();
                                }
                                completion:^(cocos2d::Texture2D* tex2D){
                                    finish(tex2D);
                                }];
}