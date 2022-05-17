#!/bin/sh

case $1 in

	rel)
	MANGOHUD_DLSYM=1 nohup mangohud ./CleanEngineGL_release > /dev/null 2>&1 & 
	MANGOHUD_DLSYM=1 nohup mangohud ./CleanEngineVk_release > /dev/null 2>&1 &
	;;
	*)
	MANGOHUD_DLSYM=1 nohup mangohud ./CleanEngineGL_debug > /dev/null 2>&1 & 
	MANGOHUD_DLSYM=1 nohup mangohud ./CleanEngineVk_debug > /dev/null 2>&1 &
	;;
esac
