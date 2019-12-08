#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char **argv){
  // Error Checking Arguments
  if(argc != 3){
    fprintf(stderr, "Bad Arguments: need 2 arguments\n");
    exit(1);
  }
  char* from = argv[1];
  char* to = argv[2];
  if(from[0] == '\0' || to[0] == '\0'){
    fprintf(stderr, "Bad Arguments: empty arguments not allowed\n");
    exit(1);
  }
  char* cset = (char *) calloc(256, 256);
  char* toset = (char *) malloc(256);
  int i;
  for(i = 0; from[i] != '\0' && to[i] != '\0'; i++){
    if(cset[from[i]] == 'a'){
      fprintf(stderr, "Bad Arguments: duplicate in First Argument\n");
      exit(1);
    }
    if(!cset[from[i]]){
      cset[from[i]] = 'a';
      toset[from[i]] = to[i];
    }
  }
  if(from[i] == '\0' && to[i] != '\0'){
    fprintf(stderr, "Bad Arguments: arguments not the same length\n");
    exit(1);
  }
  else if(from[i] != '\0' && to[i] == '\0'){
    fprintf(stderr, "Bad Arguments: arguments not the same length\n");
    exit(1);
  }

  // Parsing
  char c;
  int size = read(STDIN_FILENO, &c, 1);
  char temp;
  while(size > 0){
    if((int)c < 0)
      write(STDOUT_FILENO, &c, 1);
    else if(cset[c] == 'a'){
      temp = toset[c];
      write(STDOUT_FILENO, &temp, 1);
    }
    else
      write(STDOUT_FILENO, &c, 1);
    if(ferror(stdin))
      exit(1);
    size = read(STDIN_FILENO, &c, 1);
    if(size < 0)
      fprintf(stderr, "Error reading\n");
  }
  if(size != 0)
    exit(1);

  // Freeing
  free(toset);
  free(cset);
  return 0;
}
