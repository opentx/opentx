#!/bin/bash  
mogrify -modulate 100,100,70 *.png
mogrify +level-colors "#000066","#0000cc" *.png
