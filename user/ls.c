#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

char*
linkfmtname(char *path)
{
  static char buf[DIRSIZ+1];
  // Return blank-padded name.
  if(strlen(path) >= DIRSIZ) {
      return path;
  }
  memmove(buf, path, strlen(path));
  memset(buf+strlen(path), ' ', DIRSIZ-strlen(path));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_NOFOLLOW)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  struct stat symlink_target;
  switch(st.type){
  case T_SYMLINK:
    readlink(path, buf, 128);
    stat(buf, &symlink_target);

    // Appending the paths strings
    char symlinkpath[256];
    int i = 2, j = 0;
    while(path[i] != '\0') {
      symlinkpath[j] = path[i];
      i++;
      j++;
    }
    symlinkpath[j] = '-';
    j++;
    symlinkpath[j] = '>';
    j++;
    i = 0;
    while (buf[i] != '\0') {
      symlinkpath[j] = buf[i];
      i++;
      j++;
    }
    symlinkpath[j] = '\0';

    printf("%s %d %d 0\n", linkfmtname(symlinkpath), symlink_target.type, st.ino);
    break;
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      if(st.type == T_SYMLINK){
        char target[128];
        readlink(buf, target, 128);
        stat(target, &symlink_target);

        // Appending the paths strings
        char symlinkpath[256];
        int i = 2, j = 0;
        while(buf[i] != '\0') {
          symlinkpath[j] = buf[i];
          i++;
          j++;
        }
        symlinkpath[j] = '-';
        j++;
        symlinkpath[j] = '>';
        j++;
        i = 0;
        while (target[i] != '\0') {
          symlinkpath[j] = target[i];
          i++;
          j++;
        }
        symlinkpath[j] = '\0';

        printf("%s %d %d 0\n", linkfmtname(symlinkpath), symlink_target.type, st.ino);
      }
      else{
        printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
