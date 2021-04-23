#!/bin/bash
gcc -DRUNTIME -shared -fpic -o ld_mylloc.so ld_mylloc.c -ldl
gcc -o intr int.c
LD_PRELOAD="./ld_malloc.so" ./intr