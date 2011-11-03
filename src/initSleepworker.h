/*
 * File:   initSleepworker.h
 * Author: Cooper
 *         < dercoop@users.sf.net >
 *
 * Created on August 09, 2011, 2:54 AM
 * Last modified on August 15, 2011
 */

#ifndef _INITSLEEPWORKER_H
#define	_INITSLEEPWORKER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <getopt.h>

void is_printUsage(FILE* , int, char* );
void is_initArgs();
void is_openOutputFile();
void is_getArgsFromFile();
void is_parseCommandLine(int , char**);
void is_printArguments();


#ifdef	__cplusplus
}
#endif

#endif	/* _INITSLEEPWORKER_H */
