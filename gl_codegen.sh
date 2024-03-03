#!/bin/sh

sed '/^ *$/d;s/.*/"&\\\n"/' src/gl/vertex.glsl > src/gl/vertex.h
sed '/^ *$/d;s/.*/"&\\\n"/' src/gl/fragment.glsl > src/gl/fragment.h

