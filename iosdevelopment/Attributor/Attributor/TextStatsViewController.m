//
//  TextStatsViewController.m
//  Attributor
//
//  Created by Kacey on 6/22/14.
//  Copyright (c) 2014 Kacey. All rights reserved.
//

#import "TextStatsViewController.h"

@interface TextStatsViewController ()
@property (weak, nonatomic) IBOutlet UILabel *colorfulCharacters;
@property (weak, nonatomic) IBOutlet UILabel *outlinedCharacters;

@end

@implementation TextStatsViewController


- (void) setTextToAnalyze:(NSAttributedString *)textToAnalyze {
    _textToAnalyze = textToAnalyze;
    if (self.view.window )[self updateUI];
}

- (void) viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self updateUI];
}
-(void) updateUI {
    self.colorfulCharacters.text = [NSString stringWithFormat:@"%d colorful characters", [[self charactersWithAttribute:NSForegroundColorAttributeName] length]];
    self.outlinedCharacters.text = [NSString stringWithFormat:@"%d outline characters", [[self charactersWithAttribute:NSStrokeWidthAttributeName] length]];
    
}

- (NSAttributedString *)charactersWithAttribute:(NSString *)attributeName {
    NSMutableAttributedString *characters = [[NSMutableAttributedString alloc] init];
    int index = 0;
    while (index < [self.textToAnalyze length]){
        NSRange range;
        id value = [self.textToAnalyze attribute:attributeName atIndex:index effectiveRange:&range];
        if (value) {
            [characters appendAttributedString:[self.textToAnalyze attributedSubstringFromRange:range]];
            index = range.location + range.length;
        } else {
            ++index;
        }
    }
    return characters;
}


@end
