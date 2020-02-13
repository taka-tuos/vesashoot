set path=%path%;..\watcom\binnt
set watcom=..\watcom
set include=..\watcom\h
wcl386 -bt=dos -l=dos4g -fe=vesa-a main.c xgrp.c
