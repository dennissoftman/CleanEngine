#!/bin/sh

for f in *.vert *.frag;
do
	glslc "$f" -o "$f".spv
done