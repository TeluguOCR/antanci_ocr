#! /bin/bash
echo "LINKING "
g++ -I"../leptonica/src" -o ${1%.cpp} $1 -L"../leptonica/Debug" -L/usr/lib/x86_64-linux-gnu -lleptonica -lpng -ltiff -lz -ljpeg


