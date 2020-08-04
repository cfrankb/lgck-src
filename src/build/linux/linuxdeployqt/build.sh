#!/bin/bash

#BAD./linuxdeployqt-7-x86_64.AppImage lgck/usr/share/applications/ -appimage -no-translations  -no-copy-copyright-files -qmake=/home/cfrankb/Qt/5.15.0/gcc_64/bin/qmake
#./linuxdeployqt-7-x86_64.AppImage usr/bin/lgck-builder -no-translations  -no-copy-copyright-files  -executable=usr/bin/lgck-builder -qmake=/home/cfrankb/Qt/5.15.0/gcc_64/bin/qmake 
#./linuxdeployqt-7-x86_64.AppImage lgck/usr/share/applications/*.desktop -appimage -no-translations  -no-copy-copyright-files  -executable=usr/bin/lgck-builder -qmake=/home/cfrankb/Qt/5.15.0/gcc_64/bin/qmake 
./linuxdeployqt-7-x86_64.AppImage lgck/usr/share/applications/lgck-builder.desktop -appimage -no-translations  -no-copy-copyright-files -qmake=/home/cfrankb/Qt/5.15.0/gcc_64/bin/qmake
