#!/usr/bin/env python

import numpy

monoColorsOrder = [
        'BLACK',
        'WHITE',
        'RED',
        'ORANGE',
        'YELLOW',
        'GREEN',
        'DARKGREEN',
        'CYAN',
        'BLUE',
        'DEEPBLUE',
        'PURPLE',
        'MAGENTA']

# From 0 to 3
monoColors = {
        'BLACK': (0, 0, 0),
        'WHITE': (3, 3, 3),
        'RED': (3, 0, 0),
        'ORANGE': (3, 1, 0),
        'YELLOW': (3, 3, 0),
        'GREEN': (0, 3, 0),
        'DARKGREEN': (0, 2, 0),
        'CYAN': (0, 3, 3),
        'BLUE': (0, 2, 3),
        'DEEPBLUE': (0, 0, 3),
        'PURPLE': (1, 0, 1),
        'MAGENTA': (3, 0, 3)
        }

transitionOffsets = [
        (-1, -1),
        (-1, 0),
        (0, 0),
        (0, 0),
        (0, 0),
        (1, 0),
        (1, 1)
        ]

pebbleColorNames = {
        'GColorBlack': (0, 0, 0),
        'GColorOxfordBlue': (0, 0, 1),
        'GColorDukeBlue': (0, 0, 2),
        'GColorBlue': (0, 0, 3),
        'GColorDarkGreen': (0, 1, 0),
        'GColorMidnightGreen': (0, 1, 1),
        'GColorCobaltBlue': (0, 1, 2),
        'GColorBlueMoon': (0, 1, 3),
        'GColorIslamicGreen': (0, 2, 0),
        'GColorJaegerGreen': (0, 2, 1),
        'GColorTiffanyBlue': (0, 2, 2),
        'GColorVividCerulean': (0, 2, 3),
        'GColorGreen': (0, 3, 0),
        'GColorMalachite': (0, 3, 1),
        'GColorMediumSpringGreen': (0, 3, 2),
        'GColorCyan': (0, 3, 3),
        'GColorBulgarianRose': (1, 0, 0),
        'GColorImperialPurple': (1, 0, 1),
        'GColorIndigo': (1, 0, 2),
        'GColorElectricUltramarine': (1, 0, 3),
        'GColorArmyGreen': (1, 1, 0),
        'GColorDarkGray': (1, 1, 1),
        'GColorLiberty': (1, 1, 2),
        'GColorVeryLightBlue': (1, 1, 3),
        'GColorKellyGreen': (1, 2, 0),
        'GColorMayGreen': (1, 2, 1),
        'GColorCadetBlue': (1, 2, 2),
        'GColorPictonBlue': (1, 2, 3),
        'GColorBrightGreen': (1, 3, 0),
        'GColorScreaminGreen': (1, 3, 1),
        'GColorMediumAquamarine': (1, 3, 2),
        'GColorElectricBlue': (1, 3, 3),
        'GColorDarkCandyAppleRed': (2, 0, 0),
        'GColorJazzberryJam': (2, 0, 1),
        'GColorPurple': (2, 0, 2),
        'GColorVividViolet': (2, 0, 3),
        'GColorWindsorTan': (2, 1, 0),
        'GColorRoseVale': (2, 1, 1),
        'GColorPurpureus': (2, 1, 2),
        'GColorLavenderIndigo': (2, 1, 3),
        'GColorLimerick': (2, 2, 0),
        'GColorBrass': (2, 2, 1),
        'GColorLightGray': (2, 2, 2),
        'GColorBabyBlueEyes': (2, 2, 3),
        'GColorSpringBud': (2, 3, 0),
        'GColorInchworm': (2, 3, 1),
        'GColorMintGreen': (2, 3, 2),
        'GColorCeleste': (2, 3, 3),
        'GColorRed': (3, 0, 0),
        'GColorFolly': (3, 0, 1),
        'GColorFashionMagenta': (3, 0, 2),
        'GColorMagenta': (3, 0, 3),
        'GColorOrange': (3, 1, 0),
        'GColorSunsetOrange': (3, 1, 1),
        'GColorBrilliantRose': (3, 1, 2),
        'GColorShockingPink': (3, 1, 3),
        'GColorChromeYellow': (3, 2, 0),
        'GColorRajah': (3, 2, 1),
        'GColorMelon': (3, 2, 2),
        'GColorRichBrilliantLavender': (3, 2, 3),
        'GColorYellow': (3, 3, 0),
        'GColorIcterine': (3, 3, 1),
        'GColorPastelYellow': (3, 3, 2),
        'GColorWhite': (3, 3, 3)
        }

def colorName(red, green, blue):
    for name in pebbleColorNames:
        if pebbleColorNames[name] == (red, green, blue):
            return name
    raise Exception('Unknown color: %i,%i,%i' % (red, green, blue))

def colorCode(alpha, red, green, blue):
    return '{.a = %(alpha)s, .r = %(red)s, .g = %(green)s, .b = %(blue)s}' % {
            'alpha': alpha,
            'red': red,
            'green': green,
            'blue': blue
            }

def bakePalettes():
    for frontColor in monoColorsOrder:
        print '// %s' % frontColor
        print '{'
        for backColor in monoColorsOrder:
            offsetR, offsetG, offsetB = numpy.subtract(monoColors[backColor], monoColors[frontColor])
            transitionR = transitionOffsets[offsetR + 3]
            transitionG = transitionOffsets[offsetG + 3]
            transitionB = transitionOffsets[offsetB + 3]
            step1Color = numpy.add(monoColors[frontColor], (transitionR[0], transitionG[0], transitionB[0]))
            step2Color = numpy.add(step1Color, (transitionR[0], transitionG[0], transitionB[0]))
            strMsg = '{%(color3)s, %(color2)s, %(color1)s, %(color0)s}'
            if backColor != 'MAGENTA':
                strMsg += ','
            print strMsg % {
                    'color3': colorCode(0, 0, 0, 0),
                    'color2': colorCode(3, *step2Color),
                    'color1': colorCode(3, *step1Color),
                    'color0': colorCode(3, *monoColors[frontColor])
                    }
        if frontColor != 'MAGENTA':
            print '},'
        else:
            print '}'

if __name__ == '__main__':
    bakePalettes()

