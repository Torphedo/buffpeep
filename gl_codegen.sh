#!/bin/sh

# Some sed/regex black magic to put quotes at the beginning and end of every
# line. Also adds "\n" before each closing quote, so the shaders compile.
sed '/^ *$/d;s/.*/"&\\n"/' src/gl/vertex.glsl > src/gl/vertex.h
sed '/^ *$/d;s/.*/"&\\n"/' src/gl/fragment.glsl > src/gl/fragment.h

