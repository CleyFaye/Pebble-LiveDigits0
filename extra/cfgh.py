#!/usr/bin/python

import cfgdef

def generateCHeader(config, defines):
    entries = config['ENTRIES']
    output = open(config['OUTHFILE'], 'w')
    if not config['APPMESSAGE']:
        appMessage = 'bool cfg_handle_app_message(Tuple* tuple);\n'
    else:
        appMessage = ''
    output.write('/* AUTO-GENERATED HEADER DO NOT MODIFY */\n'
                 '#ifndef __INCL_CONFIG_H\n'
                 '#define __INCL_CONFIG_H\n'
                 '\n'
                 '#include <pebble.h>\n'
                 '\n'
                 '%(defines)s'
                 '\n'
                 'typedef void(*config_callback_t)(void* data);\n'
                 '\n'
                 '%(funcHeaders)s'
                 '\n'
                 'void cfg_init(config_callback_t callback, void* data);\n'
                 'void cfg_clear(void);\n'
                 '%(appMessageHandler)s'
                 '\n'
                 '#endif\n'
                 % { 'defines': getHeaderDefines(defines),
                     'funcHeaders': getHeaderFunctions(entries),
                     'appMessageHandler': appMessage})
    output.close()

def getHeaderDefines(defines):
    "Prepare all #define"
    allDefines = cfgdef.getAllDefines(defines)
    result = ''
    for define in sorted(allDefines.keys()):
        result += "#define %s %i\n" % (define.upper(), allDefines[define])
    return result

def getHeaderFunctions(entries):
    "Prepare all function declarations"
    result = ''
    generators = {'slider': generateBoolean,
                  'checkbox': generateBoolean,
                  'select': generateEnum,
                  'radio': generateEnum,
                  'oneshot': generateBoolean}
    for section in entries:
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            result += generators[entryType](entryDefine)
    return result

def generateBoolean(label):
    return 'bool cfg_get_%(label)s(void);\n' % { 'label': label }

def generateEnum(label):
    return 'int cfg_get_%(label)s(void);\n' % { 'label': label }

if __name__ == '__main__':
    print 'This file is a library and must not be run separately'

