#include "MacFileDialog.h"
#import <AppKit/AppKit.h>

// allowedFileTypes is deprecated in favor of UTType-based allowedContentTypes
// on newer SDKs, but it's still functional and keeps this compatible with
// the older deployment target this project already builds against.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

std::string macShowSaveDialog(const std::string &defaultName) {
    @autoreleasepool {
        NSSavePanel *panel = [NSSavePanel savePanel];
        panel.allowedFileTypes = @[@"json"];
        panel.nameFieldStringValue = [NSString stringWithUTF8String:defaultName.c_str()];
        panel.canCreateDirectories = YES;

        std::string result;
        if ([panel runModal] == NSModalResponseOK) {
            NSURL *url = panel.URL;
            if (url) result = std::string([[url path] UTF8String]);
        }
        return result;
    }
}

std::string macShowOpenDialog() {
    @autoreleasepool {
        NSOpenPanel *panel = [NSOpenPanel openPanel];
        panel.allowedFileTypes = @[@"json"];
        panel.canChooseFiles = YES;
        panel.canChooseDirectories = NO;
        panel.allowsMultipleSelection = NO;

        std::string result;
        if ([panel runModal] == NSModalResponseOK) {
            NSArray<NSURL *> *urls = panel.URLs;
            if (urls.count > 0) result = std::string([[urls.firstObject path] UTF8String]);
        }
        return result;
    }
}

#pragma clang diagnostic pop
