#!/bin/bash

set -eof
# flag meanings: -e exit on error


echo "using camera: "$camera

bazel build -c opt --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 desktop:main --verbose_failures --//desktop:camera=$camera
