#define _GNU_SOURCE
#include <config.h>
#include <stdio.h>

#include <pthread.h>

/* Arete Linux SDK */
#include <alsdk.h>

/* WIM Application */
#include "main.h"
#include "project.h"
#include "libwim.h"

int libwim_hello(void)
{
  printf("%s", "libwim World!\n");
  return 0;
}
