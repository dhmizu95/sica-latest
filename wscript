# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('sica', ['core','wifi','mesh'])
    module.source = [
        'model/sica.cc',
        'helper/sica-helper.cc',
        'model/sica-queue.cc',
        'model/sica-packet.cc',
        'model/sica-neighbor.cc',
        'model/sica-channel.cc',
        'model/channel-emulation.cc',
        'model/sica-rtable.cc'
        ]

    module_test = bld.create_ns3_module_test_library('sica')
    module_test.source = [
        'test/sica-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'sica'
    headers.source = [
        'model/sica.h',
        'helper/sica-helper.h',
        'model/sica-queue.h',
        'model/sica-packet.h',
        'model/sica-neighbor.h',
        'model/sica-channel.h',
        'model/channel-emulation.h',
        'model/sica-rtable.h'
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

