#!/usr/bin/python

import os.path
import cfglocal
import cgi
import cfgdef
import json

def readFile(srcFile):
    "Return the content of a file"
    if srcFile is None:
        return ''
    f = open(srcFile, 'r')
    result = ''
    while True:
        line = f.readline()
        if line == '':
            break
        result += line
    f.close()
    return result

def generateHTML(config, defines):
    "Generate the HTML configuration page for the given set of config entries"
    output = open(config['OUTHTMLFILE'], 'w')
    entries = config['ENTRIES']
    output.write('<!DOCTYPE html>\n'
                 '<html><head>'
                 '<title>%(appName)s</title>'
                 '<meta charset="utf-8" />'
                 '<meta name="viewport" content="width=device-width, initial-scale=1" />'
                 '<link rel="stylesheet" href="https://code.jquery.com/mobile/1.4.2/jquery.mobile-1.4.2.min.css" />'
                 '<script type="text/javascript" src="https://code.jquery.com/jquery-1.9.1.min.js"></script>'
                 '<script type="text/javascript" src="https://code.jquery.com/mobile/1.4.2/jquery.mobile-1.4.2.min.js"></script>'
                 '<script type="text/javascript">%(javascript)s</script>'
                 '</head>'
                 '<body>'
                 '<div data-role="page" id="main">'
                 '<div data-role="header" class="jqm-header">'
                 '<h1>%(appName)s</h1>'
                 '</div>'
                 '<div data-role="content" id="loading_box">%(loadingLabel)s</div>'
                 '<div data-role="content" id="message_box" style="display: none;">%(messageLabel)s</div>'
                 '<div data-role="content" id="config_box" style="display: none;">'
                 '%(userHeader)s'
                 % { 'appName': htmlEntities(config['APPNAME']),
                     'javascript': javascriptCode(entries, defines),
                     'loadingLabel': cfglocal.loadingLabel,
                     'messageLabel': cfglocal.messageLabel,
                     'userHeader': readFile(config['HEADER'])
                     })
    if len(entries) > 1:
        # Tabbed version
        output.write('<div data-role="tabs" id="tabs">'
                     '<div data-role="navbar">'
                     '<ul>')
        sectionId = 0
        for section in entries:
            output.write('<li><a href="#sec%(sectionId)i" data-ajax="false">%(label)s</a></li>' 
                    % { 'sectionId': sectionId, 
                        'label': htmlEntities(section[0])})
            sectionId += 1
        output.write('</ul>'
                     '</div>')
    # Output sections
    sectionId = 0
    # Available entry generators
    generators = { 'slider': generateSlider,
                   'checkbox': generateCheckbox,
                   'select': generateSelect,
                   'radio': generateRadio,
                   'oneshot': generateCheckbox}
    for section in entries:
        output.write('<div id="sec%i">' % sectionId)
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            generators[entryType](output, entryDefine, entryLabel, entryValues, defines)
        output.write('</div>')
        sectionId += 1
    if len(entries) > 1:
        # Tabbed version
        output.write('</div>')
    output.write('<div class="ui-body ui-body-b">'
                 '<fieldset class="ui-grid-a">'
                 '<div class="ui-block-a"><button type="submit" data-theme="d" id="b-cancel">%(cancelLabel)s</button></div>'
                 '<div class="ui-block-b"><button type="submit" data-theme="a" id="b-submit">%(submitLabel)s</button></div>'
                 '</fieldset>'
                 '</div>'
                 '%(userFooter)s'
                 '</div>'
                 '</div>'
                 '</div>'
                 '</body>'
                 '</html>'
                 % { 'cancelLabel': cfglocal.cancelLabel,
                     'submitLabel': cfglocal.submitLabel,
                     'userFooter': readFile(config['FOOTER'])
                     })
    output.close()

def htmlEntities(msg):
    return cgi.escape(msg).encode('utf-8', 'xmlcharrefreplace')

def generateSlider(output, define, label, values, defines):
    "Generate a slider HTML control"
    output.write('<div data-role="fieldcontain">'
                 '<label for="%(define)s">%(label)s</label>'
                 '<select name="%(define)s" id="%(define)s" data-role="slider">'
                 '<option value="off">%(offLabel)s</option>'
                 '<option value="on">%(onLabel)s</option>'
                 '</select>'
                 '</div>'
                 % { 'define': define,
                     'label': htmlEntities(label),
                     'offLabel': cfglocal.offLabel,
                     'onLabel': cfglocal.onLabel
                     })

def generateCheckbox(output, define, label, values, defines):
    "Generate a checkbox HTML control"
    output.write('<div data-role="fieldcontain">'
                 '<label for="%(define)s">%(label)s</label>'
                 '<input type="checkbox" name="%(define)s" id="%(define)s" />'
                 '</div>'
                 % { 'define': define,
                     'label': htmlEntities(label)
                     })

def generateSelect(output, define, label, values, defines):
    "Generate a select HTML control"
    output.write('<div data-role="fieldcontain">'
                 '<label for="%(define)s">%(label)s</label>'
                 '<select name="%(define)s" id="%(define)s">'
                 % { 'define': define,
                     'label': htmlEntities(label)
                     })
    for option in values.split('|')[1:]:
        optDef, optLabel = option.split("=")
        output.write('<option value="%s">%s</option>' % (cfgdef.getDefine(defines, define, cfgdef.enumDef(define, optDef)), htmlEntities(optLabel)))
    output.write('</select></div>')

def generateRadio(output, define, label, values, defines):
    "Generate a set of radio HTML controls"
    output.write('<fieldset data-role="controlgroup">'
                 '<legend>%s</legend>'
                 % htmlEntities(label))
    controlId = 0
    for option in values.split('|')[1:]:
        value, optionLabel = option.split('=')
        output.write('<input type="radio" name="%(define)s" id="%(define)s_%(id)i" value="%(value)s" />'
                     '<label for="%(define)s_%(id)i">%(label)s</label>'
                     % { 'define': define,
                         'id': controlId,
                         'value': cfgdef.getDefine(defines, define, cfgdef.enumDef(define, value)),
                         'label': htmlEntities(optionLabel)})
        controlId += 1
    output.write('</fieldset>')

def getDefaultValue(defines, entryDefine, entryType, entryValues):
    "Return the default value for an entry"
    if entryType in ('slider', 'checkbox'):
        if entryValues == 'true':
            return '1'
        else:
            return '0'
    elif entryType in ('select', 'radio'):
        return str(cfgdef.getDefine(defines, entryDefine, cfgdef.enumDef(entryDefine, entryValues.split('|')[0])))
    elif entryType in ('oneshot'):
        return '0'
    else:
        raise TypeError('Unknown entry type "%s"' % entryType)

def generateDefaults(entries, defines):
    "Return the javascript loadDefaults() function"
    defaultDict = {}
    for section in entries:
        for entry in section[1:]:
            entryDefine, entryLabel, entryType, entryValues = entry
            defaultDict[entryDefine] = getDefaultValue(defines, entryDefine, entryType, entryValues)
    return 'function loadDefaults() { return %s; }' % json.dumps(defaultDict)

def javascriptCode(entries, defines):
    "Return the javascript stub"
    return readFile(os.path.join(os.path.realpath(os.path.dirname(__file__)), 'cfg.js')) + '\n' + generateDefaults(entries, defines)

if __name__ == '__main__':
    print 'This file is a library and must not be run separately'

