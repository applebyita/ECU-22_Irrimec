#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <keepon.h>

void KeepON::setTimerDisabled() {
    [[UIApplication sharedApplication] setIdleTimerDisabled: YES];
}
