#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
  // Error Checking Arguments
  if(argc != 3){
    fprintf(stderr, "Bad Arguments: need 2 arguments\n");
    exit(1);
  }
  char* from = (char *) argv[1];
  char* to = (char *) argv[2];
  if(from[0] == '\0' || to[0] == '\0'){
    fprintf(stderr, "Bad Arguments: empty arguments not allowed\n");
    exit(1);
  }
  char* cset = calloc(256, 256);
  char* toset = malloc(256);
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
  int c = getchar();
  while(c != EOF){
    if(cset[c] == 'a')
      putchar(toset[c]);
    else
      putchar(c);
    if(ferror(stdin))
      exit(1);
    c = getchar();
  }
  if(!feof(stdin))
    exit(1);

  // Freeing
  free(toset);
  free(cset);
  return 0;
}
