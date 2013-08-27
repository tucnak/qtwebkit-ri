# The MIT License (MIT)
#
# Copyright (c) 2013 Illya Kovalevskyy (illya.kovalevskyy@gmail.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

QT += core webkitwidgets

lessThan(QT_MAJOR_VERSION, 5): QT += gui webkit

TARGET = qtwebkit-ri
TEMPLATE = app
DESTDIR = bin

# Build Destination
MOC_DIR = build/moc
OBJECTS_DIR = build/obj
RCC_DIR = build/rcc
UI_HEADERS_DIR = build/uih

# Source

HEADERS += \
           src/networkreplyproxy.h \
    example/mainwindow.h \
    src/interceptormanager.h

SOURCES += \
    src/networkreplyproxy.cpp \
    example/mainwindow.cpp \
    example/main.cpp \
    src/interceptormanager.cpp

FORMS += \
    example/mainwindow.ui
