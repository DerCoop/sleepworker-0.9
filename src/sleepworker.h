/*
 * File:   sleepdaemon.h
 * Author: Cooper
 *         < dercoop@users.sf.net >
 *
 * Created on April 30, 2011, 4:54 PM
 * Last modified on May 30, 2011
 */

#ifndef _SLEEPWORKER_H
#define	_SLEEPWORKER_H

#ifdef	__cplusplus
extern "C" {
#endif

//#include <stdio.h>
//#include <stdlib.h>
#include <stdarg.h>
//#include <string.h>
//#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

#include "initSleepworker.h"
#include "eventwatch.h"

#define PID_FILE "/var/run/sleepworker.pid" /* speziefy the pid-file */
#define PATH_NAME "/dev/input/by-path/" /* speziefy the path */
#define SLEEPFILE "/tmp/sleepfile" /* speziefy the sleepfile, created by default */
#define DEFAULT_SLEEP_TIME 3600 /* time to wait when sleeping */
#define DEFAULT_WAIT_TIME 600	/* timer to check interrupts for waituing for sleep */
#define DEFAULT_DISABLEFILE "/tmp/disableSleepworker" /* spezify the default disablefile */

/*
 * priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 *
 * priorities (these are ordered)
 */
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */

/*
 * define states for the daemon,
 * one of these states are reached each time
 */
#define WAITING     1   /* node is working, waiting for sleep */
#define SLEEPING    2   /* node is sleeping, monitor for evengts */
#define WAKEUP      3   /* node is waking up, state will be implement soon */


#ifdef _DEBUG
#define DEBUG(msg) stderr_printf msg
#include <stdarg.h>

void stderr_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
//	fflush(stderr);
}
#else
#define DEBUG(msg)
#endif /* _DEBUG*/

/*
 * the typedefinition for an boolean
 */
typedef enum {
    FALSE = 0, TRUE = 1
} boolean;

/*
 * a structure to represent all global parameter
 */
typedef struct { 
    /*@{*/
    int timeToSleep; /**<the time in which nothing should happen (in seconds) */
    int timeToWait; /**<the time which the program sleep after an event occured
                     * to the next poll (in seconds)*/
    char* outputFilename; /**<the name of the outputfile */
    FILE* outputFile; /**<the filedescriptor of the outputfile */
    FILE* errorFile; /**<the filedescriptor of the errorfile */
    char* inputFileName; /**<the name of the inputfile */
    boolean verbose; /**<if false, the output is short, else it is brief */
    double loadavg1; /**<the maximum value of the load average 1 min */
    char* sleepCommand; /**<the command which will execute if state changes
                         * from waiting to sleeping */
    char* wakeUpCommand; /**<the command which will execute if an event occure
                          * while state is sleeping */
    char* initCommand; /**<the command which will execute if an event occure
                          * while state is sleeping */
    char* cleanUpCommand; /**<the command which will execute if an event occure
                          * while state is sleeping */
    int sleepFD; /**<the filedescriptor for the sleepfile*/
	char* disableFilename; /**<the name of the file to disable sleepfunction */
    /*@}*/
}params_t;
extern params_t parameter;

/*
 * a structure to represent the events
 */
struct eventStruct {
    /*@{*/
    int fd; /**<the filedescriptor from the event*/
    char fqn[256]; /**<the full qualified name of the event*/
    /*@}*/
};

void output(FILE* , const char *fmt, ...);
void cleanUp();
void initialize(int , char**);
void sleepf();
void wakeupf();
void sigHandler(int);


#ifdef	__cplusplus
}
#endif

#endif	/* _SLEEPWORKER_H */

