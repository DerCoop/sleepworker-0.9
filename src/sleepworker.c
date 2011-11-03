/*!
 * \file:   sleepdaemon.c
 *
 * Created on April 30, 2011, 4:54 PM
 * Last modified on May 23, 2011
 *
 * \mainpage SLEEPWORKER
 * <center>- no time to sleep -</center>
 *
 * \section Description
 *
 * sleepworker is a UNIX daemon which observes your UNIX machine for idle time.
 * With the help of this daemon you can execute commands or programs
 * during idle time.
 *
 * \section License
 *
 * sleepworker is Free Software and released under the GNU GPL version 2.
 *
 * \section Motivation
 *
 * The motivation for the program sleepworker is based on the characteristics
 * of programs like sleepd, idler, xautolock and other programs, which poll
 * mouse or keyboard events and interrupts. All those programs contain bugs
 * or include testing components, which are not relevant for my intention.
 * Regarding this facts, I decided to write an lightweight
 * eventwatcher - the so-called sleepworker.
 *
 * \author Cooper
 *         < dercoop@users.sf.net > <br/>
 * Homepage
 *         < http://www.sleepworker.sourceforge.net >
 * \version BETA 0.9
 * \date May 2011
 *
 */
//#define _DEBUG
#include "sleepworker.h"

params_t parameter;
/**
 * \brief writes the output to stdout, stderr or to an outputfile
 *
 * the syntax of these method is like normal fprintf,
 * - first argument is the stream, in which the output is to be
 * written (stdot, stderr or outputFilename)
 * - next argument is the plaintext with placeholder for variables
 * - third to x arguments are the variables
 *
 * \param FILE* stream: the stream for the output
 * \param char *fmt: the text which will be written
 * \param ...: the variables which will write to the text dynamicaly
 *
 * \return
 */
void output(FILE* stream, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    if (parameter.verbose || stream == stderr)
        vfprintf(stream, fmt, ap);
    va_end(ap);
    fflush(stream);

    return;
}

/**
 * \brief executes the initcommand
 *
 * \param
 *
 * \return
 *
 */
void initialize(int argc, char** argv) {
    FILE *tmpfd;
    
    /* initialize params */
    is_initArgs();

    is_parseCommandLine(argc, argv);
    is_printArguments();

    /* delete sleepfile, if exists */
    unlink(SLEEPFILE);
    
    /* create PID file with current PID */
    if ((tmpfd=fopen(PID_FILE, "w")) == NULL) {
        output(parameter.errorFile,
	    "[WARNING] cant create PID file: %s\n",
            strerror(errno));
    } else {
	fprintf(tmpfd, "%i\n", getpid());
	fclose(tmpfd);
    }
    
    if (parameter.initCommand) { //user defined initCommand
	if ((system(parameter.initCommand)) != 0) {
	    output(parameter.errorFile,
		"[ERROR] cant execute initCommand: ''%s''\n",
		parameter.initCommand);
	}
    }

    return;
}

/**
 * \brief free mem and close all files
 *
 * the cleanUp method first free all allocated memory an
 * then closes all files
 * 
 * in additional, if the user has given an cleanup command, this
 * command will be executed at the commandline (with system())
 * 
 * \param
 *
 * \return
 *
 */
void cleanUp() {
    
    /* delete the sleepfile, if exists */ 
    if (parameter.sleepCommand) {
        unlink(SLEEPFILE);
    }
     
    /* free mem */
    output(parameter.outputFile, "Free all mem\n");
    free(parameter.sleepCommand);
    free(parameter.wakeUpCommand);
    free(parameter.outputFilename);

    /* delete PID file, if exists */
    output(parameter.outputFile, "delete PID file '%s' \n",
            PID_FILE);
    unlink(PID_FILE);
    
    /* close output and errorfiles */
    output(parameter.outputFile, "close outputfile '%s' \n",
            parameter.outputFilename);
    fclose(parameter.outputFile);
    fclose(parameter.errorFile);

    if (parameter.cleanUpCommand) { //user defined cleanUpCommand
	if ((system(parameter.cleanUpCommand)) != 0) {
	    output(parameter.errorFile,
		"[ERROR] cant execute cleanUpCommand: ''%s''\n",
		parameter.initCommand);
	}
    }

    return;
}

/**
 * \brief executes the sleepcommand
 *
 * \param
 *
 * \return
 *
 */
void sleepf() {
    if (!parameter.sleepCommand) { //default sleepCommand
        int tmpfd;
        tmpfd = creat(SLEEPFILE, S_IRWXU);
        if (tmpfd < 0) { //return -1 if failed
            output(parameter.errorFile,
                    "[ERROR] cant create sleepfile: %s\n",
                    strerror(errno));
        }
        close(tmpfd);
    } else {
	if ((system(parameter.sleepCommand)) != 0) {
	    output(parameter.errorFile,
		"[ERROR] cant execute sleepCommand: ''%s''\n",
		parameter.initCommand);
	}
    }
    return;
}

/**
 * \brief executes the wakeup command
 *
 * \param
 *
 * \return
 *
 */
void wakeupf() {
    if (!parameter.wakeUpCommand) { //default wakeUp command
        int tmpfd;
        tmpfd = unlink(SLEEPFILE);
        if (tmpfd != 0) {
            output(parameter.errorFile,
                    "[ERROR] cant delete sleepfile: %s\nerorcode is %s\n",
                    SLEEPFILE, strerror(errno));
        }
        close(tmpfd);
    } else {
	if ((system(parameter.wakeUpCommand)) != 0) {
	    output(parameter.errorFile,
		"[ERROR] cant execute wakeUpCommand: ''%s''\n",
		parameter.initCommand);
	}
    }
    return;
}

/**
 * \brief to handel signals
 *
 * the signal handler handels only one signal, the SIGINT Signal
 * this handler is only for closing all open files and exit the
 * program.
 *
 * \param int signum: the number of the signal which occur
 *
 * \return
 *
 */
void sigHandler(int signum) {

    switch (signum) {
        case SIGINT:
            output(parameter.errorFile, "\n[INFO] Caught signal %s\n", "SIGINT");
            output(parameter.outputFile, "\nCaught signal %s\n", "SIGINT");
            output(parameter.errorFile, "[INFO] exit sleepworker\n"
                    "[INFO] all files will be closed and all memory will free'd\n");
            cleanUp();
            exit(signum);
            break; // will never reached

        default: // open for other signals
            output(parameter.errorFile, "[INFO] Caught signal %d\n", signum);
            break;
    }
    return; // will never reached
}

/**
 * \brief check for root
 *
 * the executor of the program needs superuser privileges
 * thatswhy this method is checking the UID for root
 *
 * \param 
 *
 * \return
 *
 */
void checkRoot() {
	if (geteuid() != 0 || geteuid() != getuid())
		printf("Uhhuh, not root?!\n");
	else
		printf("Puuhh, u r root!!\n");
}

/**
 * \brief check file for existence
 *
 * this method only check a file for existence
 * if the file exists, it returns true
 * otherwise false will returned
 *
 * \param char * filename: the name of the file to check
 *
 * \return bool: true if file exists, otherwise false
 *
 */
boolean file_exists(const char * filename){
	FILE *tmpfd;
	if (tmpfd = fopen(filename, "r")) {
		fclose(tmpfd);
		return TRUE;
	}
	return FALSE;
}

/**
 * \brief commander for all other actions
 *
 * this is the main method of the program. At these method, first
 * all variables will be set to default value and after,
 * the mainloop is execute. In these loop, all events will be parsed
 * and if something happens, the main will register this an
 * execute the command.
 *
 * \param int argc: the number of commandline arguments
 * \param char** argv: the commandline arguments
 *
 * \return exit(EXIT_SUCCESS): this line will never be reached
 */
int main(int argc, char** argv) {

    int rc = 1;
    double loadavg[4];
    int mode;

    /**
     * \todo implement the chk root method here
     * but attentione, there had to be an argument to disable 
     * these funktion, if the user only checks other thinks??
     * realy?? Whats your opinion?
     */
    /* register Signalhandler */
    signal(SIGINT, sigHandler);

    /* initialize the sleepworker. clean all logfiles
     * and executes user defined funktions */
    initialize(argc, argv); 

    /* set startmode to waiting */
    mode = WAITING;

    sleep(1); /* otherwise, the interrupt from pressing "ENTER" crashes*/

    while (TRUE) {

        if (mode == WAITING) { /* mode is waiting */
            DEBUG(("mode is waiting\n"));
            rc = ew_eventWatch(parameter.timeToSleep);
        } else { /* node can do a job */
            DEBUG(("mode is sleeping\n"));
            rc = ew_eventWatch(0);
        }

        /*get loadvalue */
        getloadavg(loadavg, 4);
        DEBUG(("loadavg 1 min is %lf\n", loadavg[0]));
        if (loadavg[0] <= parameter.loadavg1) { /* tests loadavg */

            if (rc == 0) { /* sleeptime elapsed */
                DEBUG(("rc = %d\n", rc));
		if (file_exists(parameter.disableFilename) == TRUE) {
			output(parameter.outputFile, "File to disable Sleepcommand exists.\n"
				"start new in %d seconds\n",
				parameter.timeToWait);
			sleep(parameter.timeToWait);
		} else {
	                mode = SLEEPING;
	                output(parameter.outputFile, "node is not in use, "
	                        "execute sleepCommand\n");
	                sleepf();
		}
            } else {
                DEBUG(("rc = %d \n", rc));
                output(parameter.outputFile, "an event occurred\n");
                if (mode == SLEEPING) { /* old mode is sleeping */
                    /**
                     * \todo if mode sleeping, there is an other event
                     * than mode is wake up??.. not yet
                     */
                    output(parameter.outputFile,
                            "Node waked up, execute wakeUpCommand\n");
                    mode = WAKEUP;
                    wakeupf();
                }
                mode = WAITING;
                output(parameter.outputFile, "start new in %d seconds\n",
                        parameter.timeToWait);
                sleep(parameter.timeToWait);
            } // if rc == 0

        } else {
            output(parameter.outputFile, "load to hight, start new in %d seconds\n",
                    parameter.timeToWait);
            DEBUG(("load to hight\n"));
            sleep(parameter.timeToWait);
        } // if loadavg <= parameter.loadavg1
    } // while true
    return (EXIT_SUCCESS);
}
