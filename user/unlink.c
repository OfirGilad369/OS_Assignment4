#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char* path = argv[1];
  unlink(path);
  exit(0);
}