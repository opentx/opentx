#!/bin/bash

set -x
set -e

ffmpeg -i $1 -an -vb 1000k -s 960x540 `echo $1 | sed 's/\(.*\.\)mp4/\1mov/'`

