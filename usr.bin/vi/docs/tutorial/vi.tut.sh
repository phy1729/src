#!/bin/sh
#	$OpenBSD: vi.tut.csh,v 1.2 2001/01/29 01:58:40 niklas Exp $

#
# This makes the user's EXINIT variable set to the 'correct' things.
# I don't know what will happen if they also have a .exrc file!
#
# XXX
# Make sure that user is using a 24 line window!!!

if [ "$1" != beginner ] && [ "$1" != advanced ]; then
	echo Usage: $0 beginner or $0 advanced
	exit 1
fi

EXINIT='se ts=4 wm=8 sw=4' vi vi.$1
