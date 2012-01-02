#include "sleepworker.h"

/**
 * \var int _ew_countEvents
 * \brief eventcounter
 */
int _ew_countEvents = 0;

/**
 * \var int _ew_maxfd
 * \brief maximal count of filedescriptors
 */
int _ew_maxfd = 0;

/**
 * \var struct eventStruct _ew_events
 * \brief the eventarray
 *
 * at these array, all events, whitch will be watched are stored with full
 * name and the filedescriptor
 */
struct eventStruct _ew_events[128];

/**
 * \var fd_set _ew_eventWatch
 * \brief all events which would watched
 */
fd_set _ew_eventWatch;

/**
 * \brief initializes the events
 * 
 * This method initialize all mouse and keyboardevents.
 * For it the method scan the PATH for files with "event-mouse"
 * or "event-kbd" in the name (the local "." and rootdir ".."
 * will be ignored)
 *
 * \param
 *
 * \return
 *
 */
void ew_eventInit(void) {

    int tmpfd = 0, errno;
    DIR *dir;
    const char *dir_path = PATH_NAME;
    char *eventName;
    struct dirent *dp;

    _ew_countEvents = 0;
    FD_ZERO(&_ew_eventWatch);
    dir = opendir(dir_path);

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (strstr(dp->d_name, "event-kbd") ||
                    strstr(dp->d_name, "event-mouse")) {
                // copy filename
                eventName = malloc((strlen(PATH_NAME) + strlen(dp->d_name) + 1) *
                        sizeof *eventName);
                strcpy(eventName, PATH_NAME);
                strcat(eventName, dp->d_name);

                tmpfd = open(eventName, O_RDONLY);
                if (tmpfd != -1)
                    FD_SET(tmpfd, &_ew_eventWatch);
                if (FD_ISSET(tmpfd, &_ew_eventWatch)) {
                    _ew_events[_ew_countEvents].fd = tmpfd;
                    //                    strcpy(events[countEvents].fqn,eventName);
                    DEBUG(("event %d is open\n", tmpfd));
                    output(parameter.outputFile, "event %d is open\n", tmpfd);
                } else {
                    DEBUG(("cant open event: ''%s''\n", dp->d_name));
                    output(parameter.errorFile,
                            "[ERROR] error opening event: %s\n",
                            strerror(errno));
                    if (errno == EACCES) {
                        output(parameter.errorFile,
                                "[ERROR] session will be closed\n");
                        exit(EXIT_FAILURE);
                    }
                }

                if (_ew_maxfd < tmpfd)
                    _ew_maxfd = tmpfd;
                _ew_countEvents++;
                free(eventName);
            } // if file is an eventfile
        } //if dp_name != . || ..
    } // while readdir != NULL
    closedir(dir);
    /*
     * codedraft from
     * http://snippets.dzone.com/posts/show/5734
     * date: may 2011
     */
}

/**
 * \brief close the events
 *
 * This method closes all open events
 *
 * \param
 *
 * \return
 *
 */
void ew_eventClose(void) {
    int i = 0;

    for (i = 0; i < _ew_countEvents; i++) {
        close(_ew_events[i].fd);
        DEBUG(("event %d closed\n", _ew_events[i].fd));
    }
}

/**
 * \brief watches at the events
 *
 * This method is polling the mouse and keyboardevents
 * if the time == 0 this method is polling
 * till an event occour
 *
 * else this method is polling till an event occour or the
 * timer is expire. (knode is in mode waiting
 *
 * \param int time: this is the sleeptime for the select
 *
 * \return
 * 
 */
int ew_eventWatch(int time) {
    int i = 0, rc = 0;
    struct timeval sleepTime;

    sleepTime.tv_sec = time;
    sleepTime.tv_usec = 0;

    ew_eventInit();

    /**
     * if there are no events open, close
     */

    if (_ew_countEvents == 0 ) {
	DEBUG(("There are no events to open\n"));
        (parameter.errorFile,
            "[ERROR] session will be closed\n");
	return (-1);
    }

    for (i = 0; i < _ew_countEvents; i++)
        DEBUG(("EventNo %d: FD: %d\n", i, _ew_events[i].fd));

    DEBUG(("MAXFD=%d\t countevents %d\n", _ew_maxfd, _ew_countEvents));

    if (time == 0) /* waiting for an event */
        rc = select(_ew_maxfd + 1, &_ew_eventWatch, NULL, NULL, NULL);
    else /* waiting for an event or timer is down */
        rc = select(_ew_maxfd + 1, &_ew_eventWatch, NULL, NULL, &sleepTime);

    ew_eventClose();

    return (rc);
}
