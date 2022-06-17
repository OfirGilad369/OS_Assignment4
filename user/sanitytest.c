#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

//OFRY added this whole file to test implementation 

int
main()
{
  char buf[BSIZE];
  int num_blocks, fd;
  int direct_size, single_direct_size, double_direct_size;
  direct_size = 12;
  single_direct_size = 12 + 256;
  double_direct_size = 12 + 256 + 256 * 256;

  fd = open("sanitytest.txt", O_CREATE | O_WRONLY);
  if(fd < 0){
    printf("sanitytest: Failed opening file sanitytest.txt for writing\n");
    exit(-1);
  }

  num_blocks = 0;
  while(1){
    *(int*)buf = num_blocks;
    int cc = write(fd, buf, sizeof(buf));
    if(cc <= 0)
      break;
    num_blocks++;
    if(num_blocks == direct_size){
        printf("Finished writing 12KB (direct)\n");
    }
    if(num_blocks == single_direct_size){
        printf("Finished writing 268KB (single direct)\n");
    }
    if(num_blocks == double_direct_size){
        printf("Finished writing 10MB\n");
    }
    if(num_blocks % 1000 == 0){
      printf("Finished writing an addition of 1000 blocks\n");
    }
  }

  printf("\nA Total of %d blocks were written\n", num_blocks);
  if(num_blocks != 65804){
    printf("sanitytest: File sanitytest.txt size is too small\n");
    exit(-1);
  }

  close(fd);
  fd = open("sanitytest.txt", O_RDONLY);
  if(fd < 0){
    printf("sanitytest: Failed re-opening sanitytest.txt for reading\n");
    exit(-1);
  }

  int i; 
  for(i = 0; i < num_blocks; i++){
    int cc = read(fd, buf, sizeof(buf));
    if(cc <= 0){
      printf("sanitytest: Encountered error while reading block %d\n", i);
      exit(-1);
    }
    if(*(int*)buf != i){
      printf("sanitytest: Encountered the wrong data (%d) while reading block %d\n", *(int*)buf, i);
      exit(-1);
    }
  }

  printf("sanitytest: OK\n"); 

  exit(0);
}