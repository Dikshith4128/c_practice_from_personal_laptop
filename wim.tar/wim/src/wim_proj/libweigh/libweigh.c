#define _GNU_SOURCE
#include <config.h>
#include <stdio.h>

#include <pthread.h>

/* Arete Linux SDK */
#include <alsdk.h>

/* WIM Application */
#include "main.h"
#include "libweigh.h"

int libweigh_hello(void)
{
  printf("%s", "libweigh World!\n");
  return 0;
}
