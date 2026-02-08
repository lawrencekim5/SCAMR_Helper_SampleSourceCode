
/* This module provides the necessary stubs for when dynamic loading is
   not present. */

#include "Python.h"
#include "pycore_importdl.h"
int main(){
   int x,y;
   x=y+1;
   return 0;
}

const char *_PyImport_DynLoadFiletab[] = {NULL};
