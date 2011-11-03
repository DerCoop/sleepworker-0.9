/*
 * netsniff-ng - the packet sniffing beast
 * By Daniel Borkmann <daniel@netsniff-ng.org>
 * Copyright 2011 Daniel Borkmann.
 * Subject to the GPL.
 */

#include <unistd.h>
#include <sys/types.h>
//#include <stdio.h>


void check_for_root_maybe_die(void)
{
	if (geteuid() != 0 || geteuid() != getuid())
		printf("Uhhuh, not root?!\n");
	else
		printf("PUhh, u r root!!\n");
}

void main ()
{
	check_for_root_maybe_die();
	printf("alles OK \n");
}
