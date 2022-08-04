@echo off
SetLocal EnableDelayedExpansion
(set QT_VERSION=5.15.2)
(set QT_VER=5.15)
(set QT_VERSION_TAG=5152)
(set QT_INSTALL_DOCS=D:/ENVIRONMENT/QT/QT5.15.2/Docs/Qt-5.15.2)
(set BUILDDIR=F:/MyPro/MQTT/qtmqtt/src/mqtt)
D:\ENVIRONMENT\QT\QT5.15.2\5.15.2\MSVC2019_64\bin\qdoc.exe %*
EndLocal
