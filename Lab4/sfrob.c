#include <stdio.h>
#include <stdlib.h>

int frobcmp(void const *a, void const *b)
{
  int diff = 0;
  char *a_char = *(char **)a;
  char *b_char = *(char **)b;
  int nullPos = 0;
  for(int l = 0; a_char[l] != '\0' && b_char[l] != '\0'; l++)
    {
      int a_D = a_char[l] ^ 42;
      int b_D = b_char[l] ^ 42;
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

int main()
{
  // Input
  if(ferror(stdin))
    exit(1);
  if(feof(stdin))
    exit(1);
  int c = 0;
  size_t k = 8;
  char *input = (char *) malloc(8);
  if(!input)
    exit(1);
  char chr = getchar();
  char *new_inp = NULL;
  while(chr != EOF){
    if(c == k - 1)
      {
	k = (k << 1);
	new_inp = (char *) realloc(input, k);
	if(!new_inp)
	  exit(1);
	input = new_inp;
      }
    input[c] = chr;
    c = c + 1;
    chr = getchar();
  }
  if(input[c-1] == ' ')
    c = c - 1;
  
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
  for(int i = 0; i < c; i++)
    {
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
	new_ptr = (char *) realloc(ptrArr[c_A], k_n); //
	if(!new_ptr)
	  exit(1);
	ptrArr[c_A] = new_ptr;
      }
      
      if(input[i] == ' '){
	(ptrArr[c_A])[c_n] = '\0'; //
	c_n = 0;
	k_n = 8;
	c_A = c_A + 1;
	k_A = k_A + 8;
	new_ptr2 = (char **) realloc(ptrArr, k_A);
	if(!new_ptr2)
	  exit(1);
	ptrArr = new_ptr2;
	ptrArr[c_A] = (char *) malloc(8); //
      }
      else{
	(ptrArr[c_A])[c_n] = input[i]; //
	c_n = c_n + 1;
      }
    }

  if(c_n == k_n - 1){
	k_n = (k_n << 1);
	new_ptr = (char *) realloc(ptrArr[c_A], k_n); //
	if(!new_ptr)
	  exit(1);
	ptrArr[c_A] = new_ptr;
  }
  (ptrArr[c_A])[c_n + 1] = '\0';
  c_A = c_A + 1;
  
  // Sort and Output
  qsort(ptrArr, c_A, 8, frobcmp);
  int ret_code = 0;
  for(int i = 0; i < c_A; i++)
    {
      char *temp = ptrArr[i];
      for(int j = 0; temp[j] != '\0'; j++){
	ret_code = putchar(temp[j]);
	if(ret_code == EOF)
	  if(ferror(stdout))
	    {
	      perror("putchar()");
	      exit(EXIT_FAILURE);
	    }
      }
      putchar(' ');
    }

  // Deallocate
  for(int i = 0; i < c_A; i++)
    {
      free(ptrArr[i]);
    }
  free(ptrArr);
  free(input);
  return 0;
}
