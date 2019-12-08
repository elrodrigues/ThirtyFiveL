#include "randlib.h"
#include <stdlib.h>
#include <stdio.h>
// extern void __attribute__((__constructor__)) initialize(void);
// extern void __attribute__((constructor)) finialize(void);

static FILE *urandstream;

extern unsigned long long
rand64 (void)
{
  unsigned long long int x;
  if (fread (&x, sizeof x, 1, urandstream) != 1)
    abort ();
  return x;
}

__attribute__((__constructor__))
extern void
initialize (void)
{
  urandstream = fopen ("/dev/urandom", "r");
  if (! urandstream)
    abort ();
}

__attribute__((__destructor__))
extern void
finialize (void)
{
  fclose (urandstream);
}
