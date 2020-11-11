qmake -project
echo QT += core gui widgets >> *.pro
qmake
make
