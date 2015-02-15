#!/usr/bin/python

import cfgdef

def generateCSource(config, defines):
    entries = config['ENTRIES']
    output = open(config['OUTCFILE'], 'w')
    if config['APPMESSAGE']:
        appMessageStatic = 'static'
        appMessageHead = 'static bool cfg_handle_app_message(Tuple* tuple);\n'
        initAppMessage = ('app_message_register_inbox_received(in_recv_handler);\n'
                          'app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());\n')
        appMessageClear = 'app_message_deregister_callbacks();\n'
    else:
        appMessageStatic = ''
        appMessageHead = ''
        initAppMessage = ''
        appMessageClear = ''
    if anyEnumValue(entries):
        atoiImplHead = 'static int str_to_int(const char* str);\n'
        atoiImpl = ('static int str_to_int(const char* str)\n'
                    '{\n'
                    'size_t len = strlen(str);\n'
                    'int res = 0;\n'
                    'for (size_t i = 0; i < len; ++i) {\n'
                    'res += str[i] - \'0\';\n'
                    '}\n'
                    'return res;\n'
                    '}\n')
    output.write('/* GENERATED SOURCE FILE DO NOT MODIFY */\n'
                 '#include <pebble.h>\n'
                 '#include "config.h"\n'
                 '%(keyDefines)s\n'
                 'static config_callback_t config_callback;\n'
                 'static void* config_callback_data;\n'
                 '%(cachedvars)s'
                 'static void in_recv_handler(DictionaryIterator* iterator, void* context);\n'
                 'static void update_config();\n'
                 '%(appMessageHead)s'
                 '%(atoiImplHead)s'
                 'static void in_recv_handler(DictionaryIterator* iterator, void* context)\n'
                 '{\n'
                 'Tuple* tuple = dict_read_first(iterator);\n'
                 'bool handled_a_value = false;\n'
                 'while (tuple) {\n'
                 'handled_a_value |= cfg_handle_app_message(tuple);\n'
                 'tuple = dict_read_next(iterator);\n'
                 '}\n'
                 'if (handled_a_value) { update_config(); }\n'
                 '}\n'
                 '%(atoiImpl)s'
                 'static void update_config(void) { if (config_callback) { config_callback(config_callback_data); } }\n'
                 '%(getters)s'
                 'void cfg_init(config_callback_t callback, void* data)\n'
                 '{\n'
                 'config_callback = callback;\n'
                 'config_callback_data = data;\n'
                 '%(persistload)s'
                 '%(initAppMessage)s'
                 '}\n'
                 'void cfg_clear(void)\n'
                 '{\n'
                 '%(appMessageClear)s'
                 '}\n'
                 '%(appMessageStatic)s bool cfg_handle_app_message(Tuple* tuple)\n'
                 '{\n'
                 'switch (tuple->key) {\n'
                 '%(recvhandlers)s'
                 '}\n'
                 'return false;\n'
                 '}\n'
                 '\n'
                 % { 'cachedvars': getCachedVars(defines, entries),
                     'recvhandlers': getRecvHandlers(defines, entries),
                     'getters': getGetters(defines, entries),
                     'persistload': getLoaders(entries),
                     'appMessageHead': appMessageHead,
                     'appMessageStatic': appMessageStatic,
                     'initAppMessage': initAppMessage,
                     'atoiImplHead': atoiImplHead,
                     'atoiImpl': atoiImpl,
                     'keyDefines': getKeyDefines(defines),
                     'appMessageClear': appMessageClear
                     })
    output.close()

def getKeyDefines(defines):
    "Return all keys #define"
    keyDefines = cfgdef.getAllKeys(defines)
    result = ''
    for define in sorted(keyDefines.keys()):
        result += '#define %s %i\n' % (define.upper(), keyDefines[define])
    return result

def anyEnumValue(entries):
    "Check if any entry is an enum value"
    for section in entries:
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            if entryType in ('select', 'radio'):
                return True
    return False

def getLoaders(entries):
    "Generate loaders"
    types = {
            'slider': 'bool',
            'checkbox': 'bool',
            'select': 'int',
            'radio': 'int',
            'oneshot': 'bool'}
    result = ''
    for section in entries:
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            result += 'if (persist_exists(%(defineUpper)s)) { %(define)s_value = persist_read_%(varType)s(%(defineUpper)s); }\n' % { 'define': entryDefine, 'defineUpper': entryDefine.upper(), 'varType': types[entryType] }
    return result

def getGetters(defines, entries):
    "Generate getters"
    generators = {
            'slider': generateBoolGetter,
            'checkbox': generateBoolGetter,
            'select': generateIntGetter,
            'radio': generateIntGetter,
            'oneshot': generateOneshotGetter}
    result = ''
    for section in entries:
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            result += generators[entryType](entryDefine)
    return result

def generateBoolGetter(define):
    "Generate a bool getter"
    return ('bool cfg_get_%(define)s(void) { return %(define)s_value; }\n' % { 'define': define})

def generateIntGetter(define):
    "Generate an int getter"
    return ('int cfg_get_%(define)s(void) { return %(define)s_value; }\n' % { 'define': define})

def generateOneshotGetter(define):
    "Generate a oneshot getter"
    return ('bool cfg_get_%(define)s(void) {\n'
            '    if (%(define)s_value) {\n'
            '        %(define)s_value = false;\n'
            '        persist_write_bool(%(defineUpper)s, false);\n'
            '        return true;\n'
            '    } else return false;\n'
            '}\n' % { 'define': define, 'defineUpper': define.upper() })

def getRecvHandlers(defines, entries):
    "Generate value receivers"
    generators = {
            'slider': generateBoolRecv,
            'checkbox': generateBoolRecv,
            'select': generateIntRecv,
            'radio': generateIntRecv,
            'oneshot': generateBoolRecv}
    result = ''
    for section in entries:
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            result += 'case %s:\n' % entryDefine.upper()
            result += generators[entryType](defines, entryDefine, entryValues)
    return result

def generateBoolRecv(defines, entryDefine, entryValues):
    "Generate a bool value receiver"
    return ('%(define)s_value = strcmp(tuple->value->cstring, "1") == 0;\n'
            'persist_write_bool(%(defineUpper)s, %(define)s_value);\n'
            'return true;\n' %
            { 'define': entryDefine,
              'defineUpper': entryDefine.upper()})

def generateIntRecv(defines, entryDefine, entryValues):
    "Generate an enum value receiver"
    return ('%(define)s_value = str_to_int(tuple->value->cstring);\n'
            'persist_write_int(%(defineUpper)s, %(define)s_value);\n'
            'return true;\n' %
            { 'define': entryDefine,
              'defineUpper': entryDefine.upper()})

def getDefaultValue(defines, entryDefine, entryType, entryValues):
    "Return the default value for an entry"
    if entryType in ('slider', 'checkbox'):
        if entryValues == 'true':
            return 'true'
        else:
            return 'false'
    elif entryType in ('select', 'radio'):
        return str(cfgdef.getDefine(defines, entryDefine, cfgdef.enumDef(entryDefine, entryValues.split('|')[0])))
    elif entryType in ('oneshot'):
        return 'false'
    else:
        raise UnknownType('Unknown entry type')

def getCachedVars(defines, entries):
    result = ''
    for section in entries:
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            if entryType in ('checkbox', 'slider', 'oneshot'):
                varType = 'bool'
            elif entryType in ('select', 'radio'):
                varType = 'int'
            result += ('static %(varType)s %(define)s_value = %(default)s;\n' % {
                       'varType': varType,
                       'define': entryDefine, 
                       'default': getDefaultValue(defines, entryDefine, entryType, entryValues)})
    return result

if __name__ == '__main__':
    print 'This file is a library and must not be run separately'

