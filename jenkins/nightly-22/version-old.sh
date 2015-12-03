#!/bin/bash

release=`grep "VERSION =" ./opentx/radio/src/Makefile | cut -d' ' -f3 | cut -d'$' -f1`
version=22
branch=next
winboxurl=http://winbox.open-tx.org/voice-builds

