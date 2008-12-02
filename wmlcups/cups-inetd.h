/*!
 * Functions that go together with cups-1.1.23-wml.
 */

#ifndef _CUPS_INETD_H_
#define _CUPS_INETD_H_

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cups/cups.h>
#include <cups/ipp.h>
#include <wmlppctrl/cupsd_numjobs.h>

/* BUFFER is the smallest amount of free space allowed on the cups
   temporary file store before cups-lpd will start to pause while
   processing jobs. BUFFER should be given in bytes. BUFFER should be
   twice as big as the largest print job which is expected to be
   handled by cups-lpd. If BUFFER is set to 8 MB, a print file of 4 MB
   would be accepted and the data received, but a following job (of
   any size) would wait.  The accepted job would then be duplicated as
   it's passed to the cupsd via the print_file() function.

   NB! NB! If a print file of 4MB<size<=8MB is sent to cups-lpd.c, it
   would be received, but never printed, so this code really does
   depend on your knowing the size of your print jobs. It's intended
   for those implementing commercial printing systems with limited
   storage space for print jobs. */

/* This is a default. The input Buffer is now user configurable via
 * the inputBufferSize parameter in cupsd.conf. Seb 20080201.  */
#define BUFFER      8388608 /* 8 MB */

/*!
 * A utility function to get a string value from cupsd.conf
 */
int get_string_from_cupsdconf (const char* var_name,
			       char* buffer,
			       int bufferlength);
/*!
 * A utility function to read an integer value from cupsd.conf
 */
long get_integer_from_cupsd_conf (const char* var_name);

/*!
 * Do a check on allowable free space for a duplicate of the file.
 */
int check_free_space_for_a_duplicate (const char * file);

/*!
 * Return the free space available in the /tmp directory, in bytes.
 */
int tempdir_free_space (void);

/*!
 * Check whether the given queue is enabled (as far as CUPS is
 * concerned).
 */
int queue_is_enabled (http_t * connection, const char * name);

/*!
 * Check if we have reached the maximum number of jobs which we allow
 * CUPS to process at one time.
 */
int check_cupsd_for_maxjobs (http_t * connection);

/*!
 * Print the file "file" to the queue "name" with title "title", job
 * name "docname", as if printed by user "user". You can pass in CUPS
 * options in "options", with num_options set correctly. Return the
 * Job ID printed.
 */
int print_file(const char *name, const char *file,
	       const char *title, const char *docname,
	       const char *user, int num_options,	
	       cups_option_t *options);	

#endif /* _CUPS_INETD_H_ */
