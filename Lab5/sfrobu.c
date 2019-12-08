#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <ctype.h>

int SET_F_FLAG = 0;

int frobcmp(void const *a, void const *b)
{
  int diff = 0;
  char *a_char = *(char **)a;
  char *b_char = *(char **)b;
  int nullPos = 0;
  for(int l = 0; a_char[l] != '\0' && b_char[l] != '\0'; l++){
      int a_D = a_char[l] ^ 42;
      int b_D = b_char[l] ^ 42;
      if(SET_F_FLAG){
	if(a_D >= SCHAR_MIN && a_D <= SCHAR_MAX)
	  a_D = toupper((unsigned char) a_D);
	if(b_D >= SCHAR_MIN && b_D <= SCHAR_MAX)
	  b_D = toupper((unsigned char) b_D);
      }
      diff = a_D - b_D;
      if (diff != 0)
	return diff;
      nullPos++;
  }
  if(a_char[nullPos] == '\0' && b_char[nullPos] != '\0')
    return -1;
  else if(b_char[nullPos] == '\0' && a_char[nullPos] != '\0')
    return 1;
  return 0;
}

int main(int argc, char **argv){
  int c = 0;
  size_t k = 8;
  char *input = NULL;
  char malFlag = '\0';
  ssize_t inputSz;
  struct stat fbuf;
  if(argc > 2){
    fprintf(stderr, "Bad arguments\n");
    exit(1);
  }
  else if(argc == 2){
    if((argv[1])[0] == '-' && (argv[1])[1] == 'f' && (argv[1])[2] == '\0')
      SET_F_FLAG = 1;
    else{
      fprintf(stderr, "Bad arguments\n");
      exit(1);
    }
  }
  if(isatty(fileno(stdin))){
    input = (char *) malloc(MAX_INPUT);
    if(!input)
      exit(1);
    malFlag = 'A';
    inputSz = MAX_INPUT;
  }
  else{
    if(fstat(STDIN_FILENO, &fbuf)){
      fprintf(stderr, "Bad read\n");
      exit(1);
    }
    if(!S_ISREG(fbuf.st_mode)){
      fprintf(stderr, "Must be a regular file\n");
      exit(1);
    }
    input = (char *) malloc(fbuf.st_size);
    if(!input)
      exit(1);
    inputSz = fbuf.st_size;
  }
  
  long size = -1;
  size = read(STDIN_FILENO, input, inputSz);
  
  if(size == 0){ // For special cases
    char c = '\0';
    size = read(STDIN_FILENO, &c, 1);
    if(size == 0)
      exit(0);
    int inputSz = 8;
    int cnt = 0;
    char *new_inp = NULL;
    new_inp = (char *) realloc(input, inputSz);
    if(!new_inp)
      exit(1);
    input = new_inp;
    while(size > 0){
      if(cnt == inputSz - 1){
	inputSz = (inputSz << 1);
	new_inp = (char *) realloc(input, inputSz);
	if(!new_inp)
	  exit(1);
	input = new_inp;
      }

      input[cnt] = c;
      cnt = cnt + 1;
      size = read(STDIN_FILENO, &c, 1);
    }
    size = cnt;
  }
  
  // Splicing
  int k_A = 8 * sizeof(char**);
  char** ptrArr = (char **) malloc(k_A);
  if(!ptrArr)
    exit(1);
  int c_A = 0;

  int k_n = 8;
  int c_n = 0;
  ptrArr[0] = (char *) malloc(8);
  if(!ptrArr[0])
    exit(1);

  char* new_ptr = NULL;
  char** new_ptr2 = NULL;
  while(size > 0){
    for(int i = 0; i < size; i++){
      if(c_A == k_A - 1){
	k_A = (k_A << 1);
	new_ptr2 = (char **) realloc(ptrArr, k_A);
	if(!new_ptr2)
	  exit(1);
	ptrArr = new_ptr2;
      }
      if(c_n == k_n - 1){
	k_n = (k_n << 1);
	k_A = k_A + k_n;
	new_ptr2 = (char **) realloc(ptrArr, k_A);
	if(!new_ptr2)
	  exit(1);
	ptrArr = new_ptr2;
	new_ptr = (char *) realloc(ptrArr[c_A], k_n);
	if(!new_ptr)
	  exit(1);
	ptrArr[c_A] = new_ptr;
      }

      if(input[i] == ' '){
	(ptrArr[c_A])[c_n] = '\0';
	c_n = 0;
	k_n = 8;
	c_A = c_A + 1;
	k_A = k_A + 8;
	new_ptr2 = (char **) realloc(ptrArr, k_A);
	if(!new_ptr2)
	  exit(1);
	ptrArr = new_ptr2;
	ptrArr[c_A] = (char *) malloc(8);
	(ptrArr[c_A])[0] = '\0';
      }
      else{
	(ptrArr[c_A])[c_n] = input[i];
	c_n = c_n + 1;
      }
    }
    if(c_n == k_n - 1){
      k_n = (k_n << 1);
      new_ptr = (char *) realloc(ptrArr[c_A], k_n);
      if(!new_ptr)
	exit(1);
      ptrArr[c_A] = new_ptr;
    }
    
    size = read(STDIN_FILENO, input, inputSz);
    if(ferror(stdin) || size == -1)
      exit(1);
  }
  (ptrArr[c_A])[c_n] = '\0';
  c_A = c_A + 1;

  // fprintf(stderr, "Tokens: %d\n", c_A);

  // Sort and Output
  qsort(ptrArr, c_A, 8, frobcmp);
  int ret_code = 0;
  char whitesp = ' ';
  for(int i = 0; i < c_A; i++){
    char *temp = ptrArr[i];
    int j;
    for(j = 0; temp[j] != '\0'; j++);
    if(write(STDOUT_FILENO, temp, j) < 0){
      perror("write()");
      exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, &whitesp, 1);
    free(ptrArr[i]);
  }
  
  // Deallocate
  free(ptrArr);
  free(input);
  return 0;
}
