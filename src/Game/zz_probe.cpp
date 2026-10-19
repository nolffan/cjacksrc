#include "stdafx.h"
#include ""
#include <stddef.h>
struct ZZProbe : public  { static void f() {
  char (*psize)[sizeof()+1] = 1;
} };
