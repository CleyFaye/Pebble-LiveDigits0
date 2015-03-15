#!/usr/bin/env python

import sys
import Image
from BinaryFile import BinaryFile

# MBP file format:
# An MBP can store a "monochrome" bitmap, at most 256*256 pixels wide, with 4 level of transparency.
# It's purpose is to provide single-color bitmap with anti-aliasing capabilities.
# - uint8_t: version (1 = Horizontal RLE, 2 = Vertical RLE, 3 = Raw 2bit pixels)
# - uint8_t: width
# - uint8_t: height
# - uint8_t[]: data
# In data, info are stored at the bit-level. The first bit read determine what's coming next.
# In Horiz. RLE, pixels are read row by row, starting at the upper-left pixel.
# In Vert. RLE, pixels are read column by column, starting at the upper-left pixel.
# In Raw 2bit mode, every two bits ir a pixel, row by row, starting at the upper-left pixel.
# In RLE mode, the following apply:
# - Reading a 0 indicate a direct color. The next two bits are the opacity of the next pixel.
# - Reading a 1 indicate a RLE stripe. The next 6 bits indicate how many pixels compose this stripe (+3), the next two bits indicate the opacity of the stripe.
# Note: stripe are always 9 bits long. They always have at least 4 pixels, since representing 3 pixels without encoding take the same space. This mean that if the 6 length bits are 0, the stripe is 4 pixels long. A stripe can store at most 67 pixels.

def normalizeColor(inColor):
    if inColor >= 192:
        return 3
    elif inColor >= 128:
        return 2
    elif inColor >= 64:
        return 1
    else:
        return 0

def addNewStripe(outFile, color, length, head = True):
    if color == 3:
        opacity = (True, True)
    elif color == 2:
        opacity = (True, False)
    elif color == 1:
        opacity = (False, True)
    else:
        opacity = (False, False)
    if length < 4:
        for pixelId in range(0, length):
            if head:
                outFile.writeBits(False, *opacity)
            else:
                outFile.writeBits(*opacity)
    else:
        if length > 67:
            stripeLength = 67
        else:
            stripeLength = length
        outFile.writeBits(True)
        outFile.writeByte(stripeLength - 4, 6)
        outFile.writeBits(*opacity)
        if length > stripeLength:
            addNewStripe(outFile, color, length - stripeLength)

def PNGtoHorizRLE(img, outFile):
    sizeX, sizeY = img.size
    with BinaryFile(outFile) as out:
        out.writeBytes(1, sizeX, sizeY)
        for y in range(0, sizeY):
            for x in range(0, sizeX):
                if x == 0 and y == 0:
                    currentR, currentG, currentB = img.getpixel((x, y))
                    currentR = normalizeColor(currentR)
                    currentLength = 1
                else:
                    newR, newG, newB = img.getpixel((x, y))
                    newR = normalizeColor(newR)
                    if newR != currentR:
                        addNewStripe(out, currentR, currentLength)
                        currentR, currentG, currentB = newR, newG, newB
                        currentLength = 1
                    else:
                        currentLength += 1
        addNewStripe(out, currentR, currentLength);
        return out.size()

def PNGtoVertRLE(img, outFile):
    sizeX, sizeY = img.size
    with BinaryFile(outFile) as out:
        out.writeBytes(2, sizeX, sizeY)
        for x in range(0, sizeX):
            for y in range(0, sizeY):
                if x == 0 and y == 0:
                    currentR, currentG, currentB = img.getpixel((x, y))
                    currentR = normalizeColor(currentR)
                    currentLength = 1
                else:
                    newR, newG, newB = img.getpixel((x, y))
                    newR = normalizeColor(newR)
                    if newR != currentR:
                        print currentR, newR
                        addNewStripe(out, currentR, currentLength)
                        currentR, currentG, currentB = newR, newG, newB
                        currentLength = 1
                    else:
                        currentLength += 1
        addNewStripe(out, currentR, currentLength);
        return out.size()

def PNGtoRaw(img, outFile):
    sizeX, sizeY = img.size
    with BinaryFile(outFile) as out:
        out.writeBytes(3, sizeX, sizeY)
        for y in range(0, sizeY):
            for x in range(0, sizeX):
                addNewStripe(out, normalizeColor(img.getpixel((x, y))[0]), 1, False)
        return out.size()

def PNGtoMBP(inFile, outFile):
    img = Image.open(inFile).convert('RGB')
    horizRLESize = PNGtoHorizRLE(img, 'NUL')
    vertRLESize = PNGtoVertRLE(img, 'NUL')
    rawSize = PNGtoRaw(img, 'NUL')
    print 'HRLE:%i' % horizRLESize
    print 'VRLE:%i' % vertRLESize
    print 'RAW:%i' % rawSize
    if horizRLESize <= vertRLESize and horizRLESize <= rawSize:
        print 'Save HRLE'
        PNGtoHorizRLE(img, outFile)
    elif vertRLESize <= horizRLESize and vertRLESize <= rawSize:
        print 'Save VRLE'
        PNGtoVertRLE(img, outFile)
    else:
        print 'Save RAW'
        PNGtoRaw(img, outFile)

if __name__ == '__main__':
    inputFile = sys.argv[1]
    outputFile = sys.argv[2]
    with open(inputFile, 'r') as inFile:
        with open(outputFile, 'w') as outFile:
            PNGtoMBP(inFile, outFile)

