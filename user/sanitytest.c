#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void sanityCheck(void)
{
  char *direct = malloc(12 * 1024);
  char *single_indirect = malloc(268 * 1024);
  char *double_indirect = malloc(10 * 1024 * 1024);

  for(int i = 0; i < 12 * 1024; i++){
    direct[i] = 'a';
  }

  for(int i = 0; i < 268 * 1024; i++){
    single_indirect[i] = 'a';
  }

  for(int i = 0; i < 10 * 1024 * 1024; i++){
    double_indirect[i] = 'a';
  }

  int fd = open("test", O_CREATE | O_RDWR);

  if(write(fd, direct, 12 * 1024) != 12 * 1024){
    printf("error: write to backup file failed\n");
    exit(0);
  }
  else{
    printf("Finished writing 12KB (direct)\n");
  }

  if(write(fd, single_indirect, 268 * 1024) != 268 * 1024){
    printf("error: write to backup file failed\n");
    exit(0);
  }
  else{
    printf("Finished writing 268KB (single indirect)\n");
  }

  if(write(fd, double_indirect, 10 * 1024 * 1024) != 10 * 1024 * 1024){
    printf("error: write to backup file failed\n");
    exit(0);
  }
  else{
    printf("Finished writing 10MB (double indirect)\n");
  }

  close(fd);
  free(direct);
  free(single_indirect);
  free(double_indirect);
}

int
main()
{
  sanityCheck();
  printf("sanitytest: OK\n");
  exit(0);
}