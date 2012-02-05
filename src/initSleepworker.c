#include "sleepworker.h"

/**
 * \brief for printing the usage of the program
 *
 * \param FILE* stream: pointer to the outputstream
 * \param int exit_code: the code with which the method exit
 * \param char* program_name: the name of THIS program
 *
 * \return
 */
void is_printUsage(FILE* stream, int exit_code, char* program_name) {
    fprintf(stream, "\nUsage : %s options \n\n", program_name);
    fprintf(stream,
            " -h --help\t\t\tDisplay this usage information.\n"
            " -I --initcommand COMMAND\tSet the command for initialization\n"
            " -S --sleepcommand COMMAND\tSet the command for sleeping\n"
            " -W --wakeupcommand COMMAND\tSet the command for wakeUp\n"
            " -C --cleanupcommand COMMAND\tSet the command for cleanUp\n"
            " -s --sleeptime TIME\t\tSet the SLEEPTIME in seconds, "
            "default is 3600 sec\n"
            " -w --waittime TIME\t\tSet the WAITTIME in seconds, "
            "default is 600 sec\n"
            " -l --loadavg VALUE\t\tSet the maximum loadavg 1min, "
            "default is 0.0 (disabled)\n"
            " -o --outputfile FILENAME\tWrite output to file. "
            "If file exists, it will be overwritten\n"
            " -v --verbose\t\t\tPrint verbose messages.\n"
            " -i --inputfile FILENAME\tGet aruments via an inputfile "
            "(all other arguments will be ignored)\n"
            "use 'CTRL - C' to close the program\n\n");
    exit(exit_code);

    return;
}

/**
 * \brief initialize arguments
 *
 * In these method, all arguments were set to default value
 *
 * \param
 *
 * \return
 * 
 */
void is_initArgs() {
    parameter.timeToSleep = DEFAULT_SLEEP_TIME;
    parameter.timeToWait = DEFAULT_WAIT_TIME;
    parameter.verbose = FALSE;
    parameter.loadavg1 = 0.0;
    parameter.initCommand = NULL;
    parameter.wakeUpCommand = NULL;
    parameter.sleepCommand = NULL;
    parameter.cleanUpCommand = NULL;
    parameter.outputFilename = NULL;
    parameter.outputFile = stdout;
    parameter.errorFile = stderr;
	parameter.disableFilename = DEFAULT_DISABLEFILE;

    return;
}

/**
 * \brief open the output file an test for errors
 *
 * these method opens the output file. if it is not posible,
 * it will be written to stderr an the outputfile will be set to
 * stdout (default value)
 *
 * \param
 *
 * \return
 *
 */
void is_openOutputFile() {

    parameter.outputFile = fopen(parameter.outputFilename, "w+");
    if (parameter.outputFile == 0) {
        output(parameter.errorFile,
                "[WARNING] error opening outputfile: %s\n",
                strerror(errno));
        parameter.outputFile = stdout;
        output(parameter.outputFile, "error opening file,"
                "set outputfile to stdout\n");
    }

    return;
}

/**
 * \brief read arguments from file
 *
 * this method read all arguments from file.
 * attention, this method will overwrite all arguments are written
 * befor on commandline. if there are further arguments after
 * this, these will overwrite the arguments are written from file
 *
 * \param
 *
 * \return
 */
void is_getArgsFromFile() {

    FILE *inputFD;
    char line[255];
    char *tag;
    char *value;

    output(parameter.errorFile, "[INFO] found an inputfile, all "
            "commandline arguments will be ignored\n");

    /* close outputfile, if open and set all arguments to default */
    if (parameter.outputFile != stdout)
        fclose(parameter.outputFile);

    is_initArgs(); /* set all arguments to default */

    inputFD = fopen(parameter.inputFileName, "r");

    if (inputFD == 0) {
        output(parameter.errorFile,
                "[WARNING] cant open inputfile: %s\n",
                strerror(errno));
    } else {
        while (fgets(line, sizeof (line), inputFD)) {
	
	    line[strcspn( line, "\n")] = '\0'; /* removes newline from line */
	    if ( strlen(line) == 0 ) { 
		continue; /* empty line */ 	   
	    }

	    if (strncmp(line, "#", 1) == 0) {
                continue; /* skip this line, because it is an comment */
            }
	    
            tag = strtok(line, "="); // cat line to TAG=<value>
            if (strstr(tag, "INITCOMMAND")) {
                value = strtok(NULL, "=");
                parameter.initCommand = strdup(value);
            } else if (strstr(tag, "SLEEPCOMMAND")) {
                value = strtok(NULL, "=");
                parameter.sleepCommand = strdup(value);
            } else if (strstr(tag, "WAKEUPCOMMAND")) {
                value = strtok(NULL, "=");
                parameter.wakeUpCommand = strdup(value);
            } else if (strstr(tag, "CLEANUPCOMMAND")) {
                value = strtok(NULL, "=");
                parameter.cleanUpCommand = strdup(value);
            } else if (strstr(tag, "SLEEPTIME")) {
                value = strtok(NULL, "=");
                parameter.timeToSleep = atoi(value);
            } else if (strstr(tag, "WAITTIME")) {
                value = strtok(NULL, "=");
                parameter.timeToWait = atoi(value);
            } else if (strstr(tag, "LOADAVG")) {
                value = strtok(NULL, "=");
                parameter.loadavg1 = atof(value);
            } else if (strstr(tag, "VERBOSE")) {
                value = strtok(NULL, "=");
                if (strstr(value, "on"))
                    parameter.verbose = TRUE;
                else
                    parameter.verbose = FALSE;
            } else if (strstr(tag, "OUTPUT")) {
                value = strtok(NULL, "=");
                parameter.outputFilename = strdup(value);
                is_openOutputFile();
            } else {
                output(parameter.errorFile, "[WARNING] there is an illegal command in the inputfile\n"
					"[WARNING] ignore illegal command %s\n",value);
	    }
        }
    }
    fclose(inputFD);

    return;
}

/**
 * \brief parses the commandline arguments
 *
 * This method parses all commandline arguements and save the variables
 *
 * \param int argc: number of arguments
 * \param char **argv: the commandline arguments
 *
 * \return
 *
 */
void is_parseCommandLine(int argc, char **argv) {
    int next_option;

    /* A string listing valid short options letters. */
    const char* const short_options = "hI:S:W:C:s:w:l:o:vi:";
    /* An array describing valid long options. */
    const struct option long_options[] = {
        { "help", 0, NULL, 'h'},
        { "initcommand", 1, NULL, 'I'},
        { "sleepcommand", 1, NULL, 'S'},
        { "wakeupcommand", 1, NULL, 'W'},
        { "cleanupcommand", 1, NULL, 'C'},
        { "sleeptime", 1, NULL, 's'},
        { "waittime", 1, NULL, 'w'},
        { "loadavg", 1, NULL, 'l'},
        { "outputfile", 1, NULL, 'o'},
        { "verbose", 0, NULL, 'v'},
        { "inputfile", 1, NULL, 'i'},
        { NULL, 0, NULL, 0} /* Required at end of array.*/
    };
    /* The name of the file to receive program output, or NULL for
    standard output. */

    /* Whether to display verbose messages. */
    do {
        next_option = getopt_long(argc, argv, short_options,
                long_options, NULL);
        switch (next_option) {
            case 'h': /* -h or --help */
                /* User has requested usage information. Print it to standard
                 * output, and exit with exit code zero (normal termination). */
                is_printUsage(stdout, 0, argv[0]);

            case 'I': /* -I or --initcommand */
                /* This option takes an argument, the command which will be
                 * executed on initialization.*/
                parameter.initCommand = strdup(optarg);
                DEBUG(("initCommand is set to %s\n", parameter.initCommand));
                break;

            case 'S': /* -S or --sleepcommand */
                /* This option takes an argument, the command which will be
                 * executed if the node is idle.*/
                parameter.sleepCommand = strdup(optarg);
                DEBUG(("sleepCommand is set to %s\n", parameter.sleepCommand));
                break;

            case 'W': /* -W or --wakeupcommand */
                /* This option takes an argument, the command which will be
                 * executed if the node is idle.*/
                parameter.wakeUpCommand = strdup(optarg);
                DEBUG(("wakeUpCommand is set to %s\n", parameter.wakeUpCommand));
                break;

            case 'C': /* -C or --cleanupcommand */
                /* This option takes an argument, the command which will be
                 * executed if the programm is closed.*/
                parameter.cleanUpCommand = strdup(optarg);
                DEBUG(("cleanUpCommand is set to %s\n", parameter.cleanUpCommand));
                break;

            case 's': /* -s or --sleeptime */
                /* This option takes an argument, the name of the output file.*/
                parameter.timeToSleep = atoi(optarg);
                DEBUG(("timeToSleep is set to %d\n",
                        parameter.timeToSleep));
                break;

            case 'w': /* -w or --waittime */
                /* This option takes an argument, the name of the output file.*/
                parameter.timeToWait = atoi(optarg);
                DEBUG(("timeToWait is set to %d\n",
                        parameter.timeToWait));
                break;

            case 'l': /* -l or --loadavg */
                /* this option takes the loadavg value from commandline
                 * default value is 1.5*/
                parameter.loadavg1 = atof(optarg);
                DEBUG(("max loadavg is set to %lf \n", parameter.loadavg1));
                break;

            case 'o': /* -o or --outputfile */
                /* This option takes an argument, the name of the output file.*/
                parameter.outputFilename = strdup(optarg);
                DEBUG(("the outputfile is set to ''%s''\n",
                        parameter.outputFilename));
                is_openOutputFile();
                break;

            case 'v': /* -v or --verbose*/
                parameter.verbose = TRUE;
                DEBUG(("verbose is switched on\n"));
                break;

            case 'i': /* -i or --inputfile */
                parameter.inputFileName = strdup(optarg);
                is_getArgsFromFile();
                return;
                break;

            case '?': /* The user specified an invalid option. */
                /* Print usage information to standard error, and exit with exit
                code one (indicating abnormal termination). */
                is_printUsage(stderr, 1, argv[0]);

            case -1:
                break; /* Done with options.  */

            default: /* Something else: unexpected.*/
                output(parameter.errorFile, "\nit seems there is an unknown "
                        "commandLineArgument\n");
                abort();
        }
    } while (next_option != -1);

    return;
}

/**
 * \brief print the delivered arguments (from file and/or from commandline)
 *
 * This method is only for printing the arguments
 * it pritns ALL arguments, if you dont commit an
 * argument, it prints the default value
 *
 * \param
 *
 * \return
 */
void is_printArguments() {
    output(parameter.outputFile, "initCommand is set to ''%s''\n", parameter.initCommand);
    output(parameter.outputFile, "sleepCommand is set to ''%s''\n", parameter.sleepCommand);
    output(parameter.outputFile, "wakeUpCommand is set to ''%s''\n", parameter.wakeUpCommand);
    output(parameter.outputFile, "cleanUpCommand is set to ''%s''\n", parameter.cleanUpCommand);
    output(parameter.outputFile, "timeToSleep is set to %d\n",
            parameter.timeToSleep);
    output(parameter.outputFile, "timeToWait is set to %d\n",
            parameter.timeToWait);
    output(parameter.outputFile, "max loadavg is set to %lf \n", parameter.loadavg1);
    output(parameter.outputFile, "the outputfile is set to ''%s''\n",
            parameter.outputFilename);
    if (parameter.verbose)
        output(parameter.outputFile, "verbose is on\n");
    else
        output(parameter.outputFile, "verbose is off\n");

    return;
}
