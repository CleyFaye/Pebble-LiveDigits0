#!/usr/bin/env python

import os.path
import sys
import getopt
import cgi

import cfgdef
import cfghtml
import cfgh
import cfgc

def addConfigEntry(entries, entrySection, entryDefine, entryLabel, entryType, entryValue):
    "Add an entry to the configuration values"
    for section in entries:
        if section[0] == entrySection:
            section.append([entryDefine, entryLabel, entryType, entryValue])
            return
    entries.append([entrySection])
    addConfigEntry(entries, entrySection, entryDefine, entryLabel, entryType, entryValue)

def makePathRelativeTo(baseFile, filePath):
    "Rebase filePath relative to the path of baseFile"
    if os.path.isabs(filePath):
        return filePath
    basePath = os.path.dirname(os.path.abspath(baseFile))
    return os.path.normpath(os.path.join(basePath, filePath))

def readConfig(inFile):
    "Read the configuration file"
    configFile = open(inFile, 'r')
    entries = []
    customConfig = {}
    customConfigString = readALine(configFile)
    for customConfigPair in customConfigString.split('|'):
        customConfigKey, customConfigValue = customConfigPair.split('=')
        customConfig[customConfigKey] = customConfigValue
    while True:
        entrySection = readALine(configFile)
        entryDefine = readALine(configFile)
        entryLabel = readALine(configFile)
        entryType = readALine(configFile)
        entryValue = readALine(configFile)
        if entryValue is None:
            break
        addConfigEntry(entries, entrySection, entryDefine, entryLabel, entryType, entryValue)
    configFile.close()
    customConfig['ENTRIES'] = entries
    # File path are relative to config script
    for testEntry in ['OUTCFILE', 'OUTHFILE', 'OUTHTMLFILE', 'HEADER', 'FOOTER']:
        if testEntry in customConfig:
            customConfig[testEntry] = makePathRelativeTo(inFile, customConfig[testEntry])
    return customConfig

def printUsage():
    "Display a usage message on the output"
    print 'Usage:'
    print '  make-cfg.py -i <input file> [-t <header>] [-b <footer>] [-c <C output>] [-x <C header output>] [-o <HTML output>]'

def parseParams(argv):
    "Parse input parameters"
    cliConfig = {}
    try:
        opts, args = getopt.getopt(argv,"hi:t:b:c:x:o:",["ifile=","cfile=","hfile=","ofile=","top=","bottom="])
    except getopt.GetoptError:
        printUsage()
        sys.exit(1)
    for opt, arg in opts:
        if opt in ("-i", "--ifile"):
            cliConfig['INFILE'] = arg
        elif opt in ("-t", "--top"):
            cliConfig['HEADER'] = arg
        elif opt in ("-b", "--bottom"):
            cliConfig['FOOTER'] = arg
        elif opt in ("-c", "--cfile"):
            cliConfig['OUTCFILE'] = arg
        elif opt in ("-x", "--hfile"):
            cliConfig['OUTHFILE'] = arg
        elif opt in ("-o", "--ofile"):
            cliConfig['OUTHTMLFILE'] = arg
        else:
            printUsage()
            sys.exit(1)
    if 'INFILE' not in cliConfig:
        print 'Input file is mandatory!'
        sys.exit(1)
    return cliConfig

def openFiles(fileNames):
    "Open all files returned by parseParams()"
    inputFileName, outputCFileName, outputHFileName, outputHTMLFileName, inputHeaderFileName, inputFooterFileName = fileNames
    if inputFileName == '--':
        inFile = sys.stdin
    else:
        inFile = open(inputFileName, 'r')
    outCFile = open(outputCFileName, 'w')
    outHFile = open(outputHFileName, 'w')
    outHTMLFile = open(outputHTMLFileName, 'w')
    return inFile, outCFile, outHFile, outHTMLFile, inputHeaderFileName, inputFooterFileName

def closeFiles(inFile, outCFile, outHFile, outHTMLFile):
    "Close files opened by openFiles()"
    inFile.close()
    outCFile.close()
    outHFile.close()
    outHTMLFile.close()

def readALine(inFile):
    "Read a line from a file (ignoring empty lines and lines starting with a #"
    while True:
        line = inFile.readline()
        if line == '':
            return None
        if line[0] == '#':
            continue
        result = line.strip()
        if result == '':
            continue
        return result

def pumpFile(sourceName, destination):
    "Read every line of source into destination"
    if sourceName == '':
        return
    source = open(sourceName, "r")
    pumpFileFromDescriptor(source, destination)
    source.close()

def pumpFileFromDescriptor(source, destination):
    "Copy the content from one file to another"
    if source is None:
        return
    while True:
        readLine = source.readline()
        if readLine == '':
            return
        destination.write(readLine)

def sanitizeConfig(config):
    if 'APPNAME' not in config:
        config['APPNAME'] = 'Watchface'
    if 'OUTCFILE' not in config:
        config['OUTCFILE'] = 'config.c'
    if 'OUTHFILE' not in config:
        config['OUTHFILE'] = 'config.h'
    if 'OUTHTMLFILE' not in config:
        config['OUTHTMLFILE'] = 'config.htm'
    if 'HEADER' not in config:
        config['HEADER'] = None
    if 'FOOTER' not in config:
        config['FOOTER'] = None
    if 'APPMESSAGE' not in config:
        config['APPMESSAGE'] = True
    else:
        config['APPMESSAGE'] = config['APPMESSAGE'] == 'true'

def main(argv):
    "Application entry point"
    cliConfig = parseParams(argv)
    configFileConfig = readConfig(cliConfig['INFILE'])
    # Merge config file and cli config
    appConfig = configFileConfig.copy()
    appConfig.update(cliConfig)
    sanitizeConfig(appConfig)
    defines = cfgdef.readDefines(appConfig['ENTRIES'])
    # Generate outputs
    cfghtml.generateHTML(appConfig, defines)
    cfgh.generateCHeader(appConfig, defines)
    cfgc.generateCSource(appConfig, defines)
    # JS Ids
    firstKey = True
    result = ''
    for key in defines['CONFIG']:
        if firstKey:
            firstKey = False
            result = '"%(key)s": %(value)s' % { 'key': key, 'value': defines['CONFIG'][key] }
        else:
            result += ',\n"%(key)s": %(value)s' % { 'key': key, 'value': defines['CONFIG'][key] }
    return result

if __name__ == '__main__':
    print main(sys.argv[1:])

