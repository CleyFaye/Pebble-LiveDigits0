#!/usr/bin/python

def readDefines(entries):
    "Generate defines from configuration entries"
    defines = {}
    addDefine(defines, "CONFIG", "CONFIG")
    for section in entries:
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            defines = addDefine(defines, "CONFIG", entryDefine)
            if entryType in ('select', 'radio'):
                for valuePair in entryValues.split('|')[1:]:
                    valueDef = valuePair.split('=')[0]
                    addDefine(defines, entryDefine, enumDef(entryDefine, valueDef))
    return defines

def enumDef(enum, value):
    "Build an enum def"
    return "%s_%s" % (enum, value)

def addDefine(defines, group, define):
    "Add a define to the list"
    if group not in defines:
        defines[group] = {define: 0}
    else:
        defines[group][define] = len(defines[group])
    return defines

def getDefine(defines, group, define):
    "Return the numerical value of a define"
    return defines[group][define]

def getAllDefines(defines):
    "Return all defines flattened (without keys)"
    result = {}
    for group in defines:
        if group == 'CONFIG':
            continue
        for define in defines[group]:
            result[define] = defines[group][define]
    return result

def getAllKeys(defines):
    "Return all keys"
    result = {}
    for define in defines['CONFIG']:
        result[define] = defines['CONFIG'][define]
    return result

if __name__ == '__main__':
    print 'This file is a library and must not be run separately'

