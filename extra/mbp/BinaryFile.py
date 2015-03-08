#!/usr/bin/env python

class BinaryFile():
    accumulatorByte = 0
    accumulatorLength = 0
    totalSize = 0
    ownOutputFile = False
    outputFile = None
    def __init__(self, outputFile):
        if isinstance(outputFile, file):
            self.outputFile = outputFile
            self.ownOutputFile = False
        else:
            if outputFile == 'NUL':
                self.outputFile = None
                self.ownOutputFile = False
            else:
                self.outputFile = open(outputFile, 'w')
                self.ownOutputFile = True
    def __enter__(self):
        return self
    def __exit__(self, type, value, tb):
        self.checkAccumulator(True)
        if self.ownOutputFile:
            self.outputFile.close()
    def writeBits(self, *values):
        self.totalSize += len(values)
        for valueIndex in range(0, len(values)):
            if values[valueIndex]:
                self.accumulatorByte = self.accumulatorByte + (1 << self.accumulatorLength)
            self.accumulatorLength += 1
            self.checkAccumulator()
    def writeBytes(self, *values):
        for valueIndex in range(0, len(values)):
            self.writeByte(values[valueIndex])
    def writeByte(self, byte, bitCount = 8):
        for bitIndex in range(0, bitCount):
            self.writeBits((byte & 1 << bitIndex) != 0)
    def checkAccumulator(self, pad = False):
        if self.accumulatorLength == 8 or pad:
            if self.outputFile is not None:
                self.outputFile.write(chr(self.accumulatorByte))
            self.accumulatorByte = 0
            self.accumulatorLength = 0
    def size(self):
        return (self.totalSize + 7) / 8

if __name__ == '__main__':
    print('This file is a library and can not be run independently.')

