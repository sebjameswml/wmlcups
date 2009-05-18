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

#include "config.h"

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
 * Return the free space available in the /tmp directory, in bytes.
 */
int tempdir_free_space (void);

/*!
 * Check whether the given queue is enabled (as far as CUPS is
 * concerned).
 */
int queue_is_enabled (http_t * connection, const char * name);

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

/*!
 * Functions to allow us to create a job, hod it, add files, then
 * release it.
 */
int ipp_create_job (http_t * http,
		    const char * name,
		    const char * title,
		    const char * docname,
		    const char * user,
		    int num_options,
		    cups_option_t * options);

int ipp_set_job_attributes (http_t * http,
			    int jobid,
			    const char * name,
			    const char * title,
			    const char * docname,
			    const char * user,
			    int num_options,
			    cups_option_t * options);

int ipp_hold_job (http_t * http,
		  int job_id,
		  const char * user);

int ipp_release_job (http_t * http,
		     int job_id,
		     const char * user);

int ipp_send_doc (http_t * http,
		  int job_id,
		  const char * file,
		  const char * docname,
		  const char * user,
		  const char * format,
		  int last);



#endif /* _CUPS_INETD_H_ */
