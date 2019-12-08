#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "randcpuid.h"

static bool
writebytes (unsigned long long x, int nbytes)
{
  int ndigits = nbytes * 2;
  do
    {
      if (putchar ("0123456789abcdef"[x & 0xf]) < 0)
	return false;
      x >>= 4;
      ndigits--;
    }
  while (0 < ndigits);

  return 0 <= putchar ('\n');
}

int
main (int argc, char **argv)
{
  /* Check arguments.  */
  bool valid = false;
  long long nbytes;
  if (argc == 2)
    {
      char *endptr;
      errno = 0;
      nbytes = strtoll (argv[1], &endptr, 10);
      if (errno)
	perror (argv[1]);
      else
	valid = !*endptr && 0 <= nbytes;
    }
  if (!valid)
    {
      fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
      return 1;
    }


  if (nbytes == 0)
    return 0;

  //void (*initialize) (void);
  unsigned long long (*rand64) (void);
  //void (*finalize) (void);
  // char a = 'a'; // Place-holder
  void *handle;
  char *error = NULL;
  if (rdrand_supported ())
    {
      handle = dlopen("randlibhw.so", RTLD_LAZY);
      if(!handle){
	error = dlerror();
	fprintf(stderr, "Bad link: %s\n", error);
	exit(1);
      }
      rand64 = (unsigned long long (*) (void))dlsym(handle, "rand64");
      if((error = dlerror()) != NULL){
	fprintf(stderr, "Cannot find function: %s\n", error);
	exit(1);
      }
    }
  else
    {
      //initialize = software_rand64_init;
      handle = dlopen("randlibsw.so", RTLD_LAZY);
      if(!handle){
	error = dlerror();
	fprintf(stderr, "Bad link: %s\n", error);
	exit(1);
      }
      rand64 = (unsigned long long (*) (void))dlsym(handle, "rand64");
      if((error = dlerror()) != NULL){
	fprintf(stderr, "Cannot find function: %s\n", error);
	exit(1);
      }
      //finalize = software_rand64_fini;
    }

	// initialize ();
  int wordsize = sizeof rand64 ();
  int output_errno = 0;

  do
    {
      unsigned long long x = rand64 ();
      int outbytes = nbytes < wordsize ? nbytes : wordsize;
      if (!writebytes (x, outbytes))
	{
	  output_errno = errno;
	  break;
	}
      nbytes -= outbytes;
    }
  while (0 < nbytes);

  if (fclose (stdout) != 0)
    output_errno = errno;

  if (output_errno)
    {
      errno = output_errno;
      perror ("output");
      // finalize ();
      dlclose(handle);
      return 1;
    }

  // finalize ();
  dlclose(handle);
  return 0;
}
