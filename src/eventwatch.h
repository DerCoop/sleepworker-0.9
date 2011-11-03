/*
 * File:   eventwatch.h
 * Author: Cooper
 *         < dercoop@users.sf.net >
 *
 * Created on July 30, 2011, 2:54 AM
 * Last modified on August 15, 2011
 */

#ifndef _EVENTWATCH_H
#define	_EVENTWATCH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>

void ew_eventInit();
void ew_eventClose();
int ew_eventWatch(int);


#ifdef	__cplusplus
}
#endif

#endif	/* _EVENTWATCH_H */
