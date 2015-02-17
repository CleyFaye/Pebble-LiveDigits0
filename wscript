
#
# This file is the default set of rules to compile a Pebble project.
#
# Feel free to customize this to your needs.
#

import os.path
import sys
sys.path.insert(0, os.path.join('extra', 'config'))
import makecfg

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')
    makecfg.main([
            '--ifile', 'config/livedigits0.cfg', 
            '--cfile', os.path.join('src', 'config.c'),
            '--hfile', os.path.join('src', 'config.h'),
            '--ofile', os.path.join('html', 'livedigits0.htm') ])

def build(ctx):
    ctx.load('pebble_sdk')

    ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'),
                    target='pebble-app.elf')

    if os.path.exists('worker_src'):
        ctx.pbl_worker(source=ctx.path.ant_glob('worker_src/**/*.c'),
                        target='pebble-worker.elf')
        ctx.pbl_bundle(elf='pebble-app.elf',
                        worker_elf='pebble-worker.elf',
                        js=ctx.path.ant_glob('src/js/**/*.js'))
    else:
        ctx.pbl_bundle(elf='pebble-app.elf',
                        js=ctx.path.ant_glob('src/js/**/*.js'))
