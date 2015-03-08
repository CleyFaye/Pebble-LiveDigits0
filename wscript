# vi:syntax=python
#
# This file is the default set of rules to compile a Pebble project.
#
# Feel free to customize this to your needs.
#

import os.path
import sys
sys.path.insert(0, os.path.join('extra', 'config'))
sys.path.insert(0, os.path.join('extra', 'mbp'))
import makecfg
import pngtombp

top = '.'
out = 'build'

def createAppInfo():
    with open('appinfo.json.in', 'r') as inFile:
        with open('appinfo.json', 'w') as outFile:
            outFile.write(inFile.read() % { 'appKeys': makecfg.main([
            '--ifile', 'config/livedigits0.cfg', 
            '--cfile', os.path.join('src', 'config.c'),
            '--hfile', os.path.join('src', 'config.h'),
            '--ofile', os.path.join('html', 'livedigits0.htm') ]) })

def convertMBP(ctx):
# Basically... convert all png in /mbp to /resources/mbp/*.mbp
    pngListForMBP = ctx.path.ant_glob('mbp/*.png')
    if len(pngListForMBP) > 0:
        try:
            os.makedirs(os.path.join('resources', 'mbp'))
        except OSError:
            pass
    for src in pngListForMBP:
        fileName = str(src)
        baseName, ext = os.path.splitext(fileName)
        srcFilePath = os.path.join('mbp', fileName)
        dstFilePath = os.path.join('resources', 'mbp', '%s.mbp' % baseName)
        if os.path.isfile(dstFilePath) and os.path.getmtime(dstFilePath) > os.path.getmtime(srcFilePath):
            continue
        with open(srcFilePath, 'r') as inPNG:
            with open(dstFilePath, 'w') as outMBP:
                print 'Converting %s to MBP' % baseName
                pngtombp.PNGtoMBP(inPNG, outMBP)

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')
    createAppInfo()
    convertMBP(ctx)


def build(ctx):
    ctx.load('pebble_sdk')

    build_worker = os.path.exists('worker_src')
    binaries = []

    for p in ctx.env.TARGET_PLATFORMS:
        ctx.set_env(ctx.all_envs[p])
        app_elf='{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'),
        target=app_elf)

        if build_worker:
            worker_elf='{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': p, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_worker(source=ctx.path.ant_glob('worker_src/**/*.c'),
            target=worker_elf)
        else:
            binaries.append({'platform': p, 'app_elf': app_elf})

    ctx.pbl_bundle(binaries=binaries, js=ctx.path.ant_glob('src/js/**/*.js'))
