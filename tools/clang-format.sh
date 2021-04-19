#!/bin/sh

# Automatic indentation of all .c, .cpp and .h-files using clang-format
# based on https://github.com/opentx/opentx/wiki/OpenTX-Code-Style-Guide

# clang-format version 10.0.0

for i in $(find ./ -name "*.c" -or -name "*.cpp" -or -name "*.h")
do
	echo $i
	clang-format -i -style="{BasedOnStyle: LLVM,
				 IndentWidth: 2,
				 UseTab: Never,

				 AllowShortBlocksOnASingleLine: false,
				 AllowShortIfStatementsOnASingleLine: false,
				 AllowShortFunctionsOnASingleLine: false,

				 BreakBeforeBraces: Custom,
				 BraceWrapping: { AfterFunction: true,
						  BeforeElse: true },

				 IndentCaseLabels: true,
				 PenaltyReturnTypeOnItsOwnLine: 0,
				 ColumnLimit: 120}" $i
done
