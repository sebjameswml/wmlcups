/*
 * Functions which are useful in rawprint.c, potentially in cups-lpd.c
 * (though we hav ea copy of these in cups-1.1.23-wml for that) and in
 * the wmlcdl code (or any other CUPs filter code which needs to be
 * able to create new CUPs jobs with minimum fuss).
 */

#include "cups-inetd.h"
#include <cups/language.h>
#include <syslog.h>
#include <errno.h>

int
check_cupsd_for_maxjobs (http_t * connection)
{
	/* Find what MaxJobs is. This is held in the scheduler as a
	 * variable */
	/* Find what current number of jobs is, also from the scheduler */
	/* If we're too close to MaxJobs, return 0 */
	/* Any errors, return -1 */

	ipp_t * rqst;
	ipp_t * rtn;
	ipp_attribute_t * ipp_attributes;
	cups_lang_t * lang;

	int made_connection = 0;

	int i = 0;
	int MyMaxJobs = 0, gotMax = 0;
	int MyNumJobs = 0, gotJobs = 0;

	int cupsd_jobs_ok = 0;

	static const char * jobinfo_attributes[] = {
		"cupsd-maxjobs",
		"cupsd-numjobs"
	};
	int n_attributes = 2; // Should match the above number of entries

	//syslog (LOG_INFO, "At start of %s", __FUNCTION__);

	/*
	 * Setup a connection and request data...
	 */

	if (connection == NULL) {
		if ((connection = httpConnectEncrypt(cupsServer(), ippPort(),
						     cupsEncryption())) == NULL)
		{
			syslog(LOG_ERR, "%s: Unable to connect to server %s: %s",
			       __FUNCTION__, cupsServer(), strerror(errno));
			return -1;
		}
		made_connection = 1;
	}

	rqst = ippNew();
	rqst->request.op.operation_id = CUPS_GET_MAXJOBS;
	rqst->request.op.request_id   = 0x8383;
	lang = cupsLangDefault();

	ippAddString(rqst,
		     IPP_TAG_OPERATION,
		     IPP_TAG_CHARSET,
		     "attributes-charset",
		     NULL,
		     cupsLangEncoding(lang));

	ippAddString(rqst,
		     IPP_TAG_OPERATION,
		     IPP_TAG_LANGUAGE,
		     "attributes-natural-language",
		     NULL,
		     lang->language);

	ippAddStrings(rqst,
		      IPP_TAG_OPERATION,
		      IPP_TAG_KEYWORD,
		      "requested-attributes",
		      n_attributes,
		      NULL,
		      jobinfo_attributes);

	rtn = cupsDoRequest (connection, rqst, "/");

	if (!rtn) {
		syslog (LOG_ERR, "%s: cupsDoRequest() failed: '%s'\n", __FUNCTION__, ippErrorString(cupsLastError()));
		cupsLangFree (lang);
		return -1;
	}

	for (ipp_attributes = rtn->attrs; ipp_attributes != NULL; ipp_attributes = ipp_attributes->next) {

		while (ipp_attributes != NULL && ipp_attributes->group_tag != IPP_TAG_EVENT_NOTIFICATION) {
			// Move on to the next one.
			ipp_attributes = ipp_attributes->next;
		}

		while (ipp_attributes != NULL && ipp_attributes->group_tag == IPP_TAG_EVENT_NOTIFICATION) {

			if (!strcmp(ipp_attributes->name, "cupsd-maxjobs") &&
			    ipp_attributes->value_tag == IPP_TAG_INTEGER) {
				MyMaxJobs = ipp_attributes->values[0].integer;
				gotMax = 1;
				//syslog (LOG_DEBUG, "Got MaxJobs (%d)", MyMaxJobs);
			}

			if (!strcmp(ipp_attributes->name, "cupsd-numjobs") &&
			    ipp_attributes->value_tag == IPP_TAG_INTEGER) {
				MyNumJobs = ipp_attributes->values[0].integer;
				gotJobs = 1;
				//syslog (LOG_DEBUG, "Got NumJobs (%d)", MyNumJobs);
			}

			ipp_attributes = ipp_attributes->next;
		}
		if (gotJobs && gotMax) { break; }
		if (i++ > 1023) {
			syslog (LOG_WARNING, "%s: spinning and failed to get both NumJobs and MaxJobs after 1024 for loops", __FUNCTION__);
			break;
		}
	}

	if (rtn)
		ippDelete(rtn);
	cupsLangFree (lang);

	if (made_connection) {
		httpClose(connection);
	}

	if (MyNumJobs - MyMaxJobs < 0) {
		/* All is well, cupsd has space to receive the job, return 1 */
		cupsd_jobs_ok = 1;
	} else {
		cupsd_jobs_ok = 0;
	}

	return cupsd_jobs_ok;
}

/*
 * 'check_free_space_for_a_duplicate()' - Check we have enough free space
 * to create a duplicate of "file" on the filesystem containing "file".
 *
 * NB: This assumes the cups-lpd temporary file is created in the same
 * directory as the cups temporary file. Is that true? It is for my
 * application, but may not be for all?
 *
 * This check is important on cups systems where the print spool filesystem
 * is limited in size.
 *
 */

int
check_free_space_for_a_duplicate (const char * file)
{
	struct statvfs *tmpstat;        /* Used by statvfs() function */
	struct stat    *buf;            /* Used by stat() function */
	int            statret;         /* To hold the rtn value of stat()
					 * and statvfs() */
	unsigned long  file_size = 0;   /* The size of the file to be printed */
	unsigned long  free_space = 0;  /* The free space available on the
					 * filesystem holding file */

	/* Find disk usage of file */
	buf = malloc (sizeof (struct stat));
	memset (buf, 0, sizeof(struct stat));

	if ((statret = stat (file, buf)) != 0) {
		syslog (LOG_ERR, "%s: stat() returned error %d\n",
			__FUNCTION__, statret);
		free (buf);
		return -1;
	}

	if (S_ISREG(buf->st_mode))
	{
		file_size = buf->st_size;
		free (buf);
	}
	else
	{
		/* Error reading file */
		syslog (LOG_ERR, "%s: Error reading file", __FUNCTION__);
		free (buf);
		return -1;
	}

	/* Find free space available */
	tmpstat = malloc (sizeof (struct statvfs));

	/* Here we have a hardcoded directory - how to get this from cups? */
	if ((statret = statvfs (file, tmpstat)))
	{
		syslog (LOG_ERR, "%s: statvfs() returned error %d\n",
			__FUNCTION__, statret);
		free (tmpstat);
		return -1;
	}

	/* 512 blocks is 2 MB on /tmp on the vortex */
	free_space = tmpstat->f_bfree * tmpstat->f_bsize;

	syslog (LOG_INFO, "file_size = %ld (%2f MB), free_space = %ld (%2f MB)",
		file_size, (float)file_size/1048576,
		free_space, (float)free_space/1048576);

	if (free_space > file_size)
	{
		free (tmpstat);
		return 1;
	}
	else
	{
		free (tmpstat);
		return 0;
	}

}

/* Return length of string found. */
int get_string_from_cupsdconf (const char* var_name,
			       char* buffer,
			       int bufferlength)
{
	char line[1024];
	FILE * ifp = NULL;
	int i = 0;
	int line_is_comment = 0;
	char* p = NULL;
	int value_length = 0;

	ifp = fopen ("/etc/cups/cupsd.conf", "r");
	if (!ifp) {
		syslog (LOG_ERR, "%s: Couldn't open cupsd.conf", __FUNCTION__);
		return -1;
	}

	while (fgets (line, 1024, ifp)) {

		// Is first char a '#'?
		i=0;
		while (i<strlen(line)) {
			// Skip any whitespace
			if (line[i] == '\t' || line[i] == ' ') {
				i++;
				continue;
			}
			if (line[i] == '#') {
				line_is_comment = 1;
				break;
			} else {
				line_is_comment = 0;
				break;
			}
		}
		// If so, move to next line.
		if (line_is_comment) { continue; }

		// Now we have a real line.
		if ((p = strstr (line, var_name)) != NULL) {
			// It's a match for our variable name, p points at start of
			// var_name location.
			p += strlen (var_name);

			while (*p == ' ' || *p == '\t') {
				p++;
			}

			// p now points at our value, so read that in, sans the last
			// char (\n). NB: This doesn't allow for dos newlines, but hey,
			// this is the Central Unix Printing system.
			value_length = snprintf (buffer, strlen(p), "%s", p);

			// Got our value, now break.
			break;
		}
	}

	fclose (ifp);
	return value_length;
}

long get_integer_from_cupsd_conf (const char* var_name)
{
	char line[1024];
	FILE * ifp = NULL;
	long the_value = 0;
	char *units;
	int i = 0;
	int line_is_comment = 0;
	char* p = NULL;

	ifp = fopen ("/etc/cups/cupsd.conf", "r");
	if (!ifp) {
		syslog (LOG_ERR, "%s: Couldn't open cupsd.conf", __FUNCTION__);
		return -1;
	}

	while (fgets (line, 1024, ifp)) {

		// Is first char a '#'?
		i=0;
		while (i<strlen(line)) {
			// Skip any whitespace
			if (line[i] == '\t' || line[i] == ' ') {
				i++;
				continue;
			}
			if (line[i] == '#') {
				line_is_comment = 1;
				break;
			} else {
				line_is_comment = 0;
				break;
			}
		}
		// If so, move to next line.
		if (line_is_comment) { continue; }

		// Now we have a real line.
		if ((p = strstr (line, var_name)) != NULL) {
			// It's a match for our variable name, p points at start of
			// var_name location.
			p += strlen (var_name);

			the_value = strtol(p, &units, 0);

			if (units && *units)
			{
				if (tolower(units[0] & 255) == 'g')
					the_value *= 1024 * 1024 * 1024;
				else if (tolower(units[0] & 255) == 'm')
					the_value *= 1024 * 1024;
				else if (tolower(units[0] & 255) == 'k')
					the_value *= 1024;
				else if (tolower(units[0] & 255) == 't')
					the_value *= 262144;
			}

			// Got our value, now break.
			break;
		}
	}

	fclose (ifp);
	return the_value;
}

/*
 * Seb's "how much free space on /tmp" fn.
 *
 * Is there a cups API function which returns the value of TempDir
 * from cupsd.conf? No. They are all available as globals in cupsd,
 * but not outside. See scheduler/conf.[ch] for more details.
 *
 * This fn gets InputBuffer and TempDir from cupsd.conf as it needs
 * these variables.
 */
int tempdir_free_space (void)
{
	struct statvfs   *tmpstat;
	int              statret;
	long             bytes_limit;
	char             path[128];

	if ((get_string_from_cupsdconf ("TempDir", path, 128)) == -1) {
		snprintf (path, 128, "%s", "/tmp");
	}

	bytes_limit = get_integer_from_cupsd_conf ("InputBufferSize");
	if (bytes_limit <= 0) {
		bytes_limit = BUFFER; /* Our default */
	}

	/*
	  syslog (LOG_DEBUG,
	  "%s: From cupsd.conf: TempDir is '%s'. InputBufferSize is %ld.",
	  __FUNCTION__, path, bytes_limit);
	*/

	tmpstat = malloc (sizeof (struct statvfs));

	/* Before generating each file, test that there is
	   bytes_limit bytes free on the /tmp filesystem */
	if (statvfs (path, tmpstat))
	{
		statret = errno;
		syslog (LOG_ERR, "statvfs returned error %d", statret);
		return -1;
	}

	if ((tmpstat->f_bfree * tmpstat->f_bsize) > bytes_limit)
	{
		free (tmpstat);
		return 1;
	}
	else
	{
		free (tmpstat);
		return 0;
	}
}

int queue_is_enabled (http_t * connection, const char * name)
{
	ipp_t * prqst;
	ipp_t * rtn;
	cups_lang_t * lang;

	static const char * printer_attributes[] = {
		"printer-name",
		"printer-state"
	};
	int n_attributes = 2; // Should match the above number of entries

	ipp_attribute_t * ipp_attributes;
	int qenabled = 0;
	char * printer = NULL;

	//syslog (LOG_INFO, "At start of %s", __FUNCTION__);

	if (connection == NULL) {
		syslog(LOG_ERR, "%s: Unable to connect to server, connection is NULL", __FUNCTION__);
		return -1;
	}

	/*
	 * Setup a request for data
	 */

	prqst = ippNew();
	prqst->request.op.operation_id = CUPS_GET_PRINTERS;
	prqst->request.op.request_id   = 0x8282;

	lang = cupsLangDefault();

	ippAddString(prqst,
		     IPP_TAG_OPERATION,
		     IPP_TAG_CHARSET,
		     "attributes-charset",
		     NULL,
		     cupsLangEncoding(lang));

	ippAddString(prqst,
		     IPP_TAG_OPERATION,
		     IPP_TAG_LANGUAGE,
		     "attributes-natural-language",
		     NULL,
		     lang->language);

	ippAddStrings(prqst,
		      IPP_TAG_OPERATION,
		      IPP_TAG_KEYWORD,
		      "requested-attributes",
		      n_attributes,
		      NULL,
		      printer_attributes);

	rtn = cupsDoRequest (connection, prqst, "/");

	if (!rtn) {
		// Handle error
		syslog (LOG_ERR, "%s: cupsDoRequest() failed: '%s'\n", __FUNCTION__, ippErrorString(cupsLastError()));
		// Perhaps we need to ippDelString for each string we added before calling ippDelete?
		//ippDelete (prqst);
		cupsLangFree (lang);
		return -1;
	}

	for (ipp_attributes = rtn->attrs;
	     ipp_attributes != NULL;
	     ipp_attributes = ipp_attributes->next) {

		while (ipp_attributes != NULL
		       && ipp_attributes->group_tag != IPP_TAG_PRINTER) {
			// Move on to the next one.
			ipp_attributes = ipp_attributes->next;
		}

		while (ipp_attributes != NULL &&
		       ipp_attributes->group_tag == IPP_TAG_PRINTER) {

			if (!strcmp(ipp_attributes->name, "printer-name") &&
			    ipp_attributes->value_tag == IPP_TAG_NAME) {
				printer = ipp_attributes->values[0].string.text;
			}
			if (!strcmp(ipp_attributes->name, "printer-state") &&
			    ipp_attributes->value_tag == IPP_TAG_ENUM) {
				qenabled = ipp_attributes->values[0].integer;
			}

			ipp_attributes = ipp_attributes->next;

		}

		if (printer != NULL) {
			//syslog (LOG_INFO, "Printer %s has status %d", printer, qenabled);
			if (!strcmp (printer, name)) {
				/* this is our printer! */
				if (qenabled == IPP_PRINTER_IDLE || qenabled == IPP_PRINTER_PROCESSING) {
					/* printer is enabled */
					ippDelete (rtn);
					cupsLangFree (lang);
					return 1;

				} else if (qenabled == IPP_PRINTER_STOPPED) {
					ippDelete (rtn);
					cupsLangFree (lang);
					return 0;

				} else {
					/* unknown value for qenabled */
					syslog (LOG_WARNING, "unknown printer status %d", qenabled);
					return -1;
				}
			}
		}
	}

	/* didn't find printer, return 0 (not enabled) */
	ippDelete (rtn);
	cupsLangFree (lang);
	return 0;
}


/*
 * 'print_file()' - Print a file to a printer or class.
 */

int					/* O - Job ID */
print_file(const char    *name,		/* I - Printer or class name */
           const char    *file,		/* I - File to print */
           const char    *title,	/* I - Title of job */
           const char    *docname,	/* I - Name of job file */
           const char    *user,		/* I - Owner of job */
           int           num_options,	/* I - Number of options */
	   cups_option_t *options)	/* I - Options */
{
	http_t	*http;			/* Connection to server */
	ipp_t		*request;		/* IPP request */
	ipp_t		*response;		/* IPP response */
	ipp_attribute_t *attr;		/* IPP job-id attribute */
	char		uri[HTTP_MAX_URI];	/* Printer URI */
	cups_lang_t	*language;		/* Language to use */
	int		jobid;			/* New job ID */
#ifdef CHECK_DISK_SPACE_FEATURE
	int           rtn;                    /* return value of check_free_space... */
#endif

	/*
	 * Setup a connection and request data...
	 */

	if ((http = httpConnectEncrypt(cupsServer(), ippPort(),
				       cupsEncryption())) == NULL)
	{
		syslog(LOG_ERR, "Unable to connect to server %s: %s", cupsServer(),
		       strerror(errno));
		return (0);
	}

	/*
	 * Build a standard CUPS URI for the printer and fill the standard IPP
	 * attributes...
	 */

	if ((request = ippNew()) == NULL)
	{
		syslog(LOG_ERR, "Unable to create request: %s", strerror(errno));
		httpClose(http);
		return (0);
	}

	request->request.op.operation_id = IPP_PRINT_JOB;
	request->request.op.request_id   = 0x8080;

	snprintf(uri, sizeof(uri), "ipp://localhost/printers/%s", name);

	language = cupsLangDefault();

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
		     "attributes-charset", NULL, cupsLangEncoding(language));

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
		     "attributes-natural-language", NULL,
		     language != NULL ? language->language : "C");

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
		     NULL, uri);

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name",
		     NULL, user);

	if (title)
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "job-name", NULL, title);
	if (docname)
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "document-name", NULL, docname);

	/*
	 * Then add all options on the command-line...
	 */

	cupsEncodeOptions(request, num_options, options);

	/*
	 * Do the request...
	 */

	snprintf(uri, sizeof(uri), "/printers/%s", name);

#ifdef CHECK_DISK_SPACE_FEATURE
	/*
	 * Find the size of the file and make sure we have 1 times that amount
	 * of free space prior to processing it, as the cupsDoFileRequest will copy
	 * the file prior to spooling it to the printer.
	 */
	while (!(rtn = check_free_space_for_a_duplicate (file)))
	{
		syslog (LOG_INFO,
			"Ramdisk is nearly full, waiting for space to become available..");
		usleep (5000000); /* 5 sec */
	}

	if (rtn == -1)
		syslog (LOG_ERR, "check_free_space_for_a_duplicate() returned an error.");

#endif /* CHECK_DISK_SPACE_FEATURE */

#ifdef CHECK_QUEUE_IS_ENABLED_FEATURE
	/* Make sure that the queue is enabled before calling cupsDoFileRequest() */
	while (queue_is_enabled (http, name) < 1) {
		syslog (LOG_INFO, "Queue '%s' is disabled, waiting for it to be re-enabled..", name);
		usleep (10000000);
	}
#endif /* CHECK_QUEUE_IS_ENABLED_FEATURE */

	response = cupsDoFileRequest(http, request, uri, file);

	if (response == NULL)
	{
		syslog(LOG_ERR, "Unable to print file (queue %s), null response to cupsDoFileRequest, cupsLastError() returns %s",
		       name, ippErrorString(cupsLastError()));
		jobid = 0;
	}
	else if (response->request.status.status_code > IPP_OK_CONFLICT)
	{
		syslog(LOG_ERR, "Unable to print file (queue %s), response->request.status.status_code = %s",
		       name, ippErrorString(response->request.status.status_code));
		jobid = 0;
	}
	else if ((attr = ippFindAttribute(response, "job-id", IPP_TAG_INTEGER)) == NULL)
	{
		syslog(LOG_ERR, "No job-id attribute found in response from server!");
		jobid = 0;
	}
	else
	{
		jobid = attr->values[0].integer;
		// Question is - at this point, has cupsd copied the file to /tmp/d0000N-001 ?
		syslog(LOG_INFO, "cupsd accepted file as Job %d", jobid);
	}

	if (response != NULL)
		ippDelete(response);

	httpClose(http);
	cupsLangFree(language);

	return (jobid);
}
