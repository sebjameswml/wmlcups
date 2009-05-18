/*
 * Functions which are useful in WML code which need to access the
 * CUPs API with minimum fuss.
 */

#include "config.h"
#include "cups-inetd.h"
#include <cups/language.h>
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>

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
 *
 * I don't think this function is necessary now, as we correctly
 * handle the case that cupsd has reached maxjobs, and we will set
 * maxjobs to suit the average file size used at a site.
 */
#define BUFFER      8388608 /* 8 MB */
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

/*
 * IPP_CREATE_JOB function
 *
 * name - Printer or class name
 * title - Title of job
 * docname - Name of job file
 * user - Owner of job
 * num_options - Number of options
 * options - CUPS Options
 */
int ipp_create_job(http_t * http, const char * name, const char * title, const char * docname,
		   const char * user, int num_options, cups_option_t * options)
{
	ipp_t * request;           /* IPP request */
	ipp_t * response;          /* IPP response */
	ipp_attribute_t * attr;    /* IPP job-id attribute */
	char uri[HTTP_MAX_URI];    /* Printer URI */
	cups_lang_t * language;    /* Language to use */
	int jobid;                 /* New job ID */

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

	request->request.op.operation_id = IPP_CREATE_JOB;
	request->request.op.request_id = 0x8091;

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
	{
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME,
			     "job-name", NULL, title);
	}
	if (docname)
	{
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME,
			     "document-name", NULL, docname);
	}

	/*
	 * Then add all options on the command-line...
	 */

	cupsEncodeOptions(request, num_options, options);

	/*
	 * Do the request...
	 */

	snprintf(uri, sizeof(uri), "/printers/%s", name);

	response = cupsDoRequest(http, request, uri);

	if (response == NULL)
	{
		syslog(LOG_ERR, "Unable to create job (queue %s), null "
		       "response to cupsDoRequest, cupsLastError() returns %s",
		       name, ippErrorString(cupsLastError()));
		jobid = 0;

	}
	else if (response->request.status.status_code > IPP_OK_CONFLICT)
	{
		syslog(LOG_ERR, "Unable to create job (queue %s), "
		       "response->request.status.status_code = %s",
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
		/* cupsd successfully created a job */
		jobid = attr->values[0].integer;
	}

	if (response != NULL)
	{
		ippDelete(response);
	}

	cupsLangFree(language);

	return (jobid);
}

/*
 * IPP_SET_JOB_ATTRIBUTES function
 *
 * jobid - the CUPS Job ID to set attributes for
 * name - Printer or class name
 * title - Title of job
 * docname - Name of job file
 * user - Owner of job
 * num_options - Number of options
 * options - CUPS Options
 */
int ipp_set_job_attributes(http_t * http, int jobid,
			   const char * name, const char * title, const char * docname,
			   const char * user, int num_options, cups_option_t * options)
{
	ipp_t * request;		/* IPP request */
	ipp_t * response;		/* IPP response */
	char uri[HTTP_MAX_URI];	/* Printer URI */
	cups_lang_t * language;	/* Language to use */
	int rtn = -1;

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

	request->request.op.operation_id = IPP_CREATE_JOB;
	request->request.op.request_id = 0x8095;

	snprintf(uri, sizeof(uri), "ipp://localhost/jobs/%d", jobid);

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
	{
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME,
			     "job-name", NULL, title);
	}
	if (docname)
	{
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME,
			     "document-name", NULL, docname);
	}

	/*
	 * Then add all options on the command-line...
	 */

	cupsEncodeOptions(request, num_options, options);

	/*
	 * Do the request...
	 */

	snprintf(uri, sizeof(uri), "/jobs/%d", jobid);

	response = cupsDoRequest(http, request, uri);

	if (response == NULL)
	{
		syslog(LOG_ERR, "Unable to set job %d attributes, null "
		       "response to cupsDoRequest, cupsLastError() returns %s",
		       jobid, ippErrorString(cupsLastError()));
		rtn = -1;

	}
	else if (response->request.status.status_code > IPP_OK_CONFLICT)
	{
		syslog(LOG_ERR, "Unable to set job %d attributes, "
		       "response->request.status.status_code = %s",
		       jobid, ippErrorString(response->request.status.status_code));
		rtn = -1;

	}
	else
	{
		rtn = 0;
	}

	if (response != NULL)
	{
		ippDelete(response);
	}

	cupsLangFree(language);

	return (rtn);
}

/*
 * IPP_HOLD_JOB function.
 *
 * job_id - The job ID to hold
 * user - Owner of job
 */
int ipp_hold_job(http_t * http, int job_id, const char * user)
{
	ipp_t * request;		/* IPP request */
	ipp_t * response;		/* IPP response */
	char uri[HTTP_MAX_URI];	        /* Printer URI */
	cups_lang_t * language;		/* Language to use */
	int rtn = -1;                   /* Return value -1: Error 0: Success */

	/*
	 * Build a standard CUPS URI for the printer and fill the standard IPP
	 * attributes...
	 */

	if ((request = ippNew()) == NULL)
	{
		syslog(LOG_ERR, "Unable to create request: %s", strerror(errno));
		httpClose(http);
		return (-1);
	}

	request->request.op.operation_id = IPP_HOLD_JOB;
	request->request.op.request_id   = 0x8094;

	snprintf(uri, sizeof(uri), "ipp://localhost/jobs/%d", job_id);

	language = cupsLangDefault();

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
		     "attributes-charset", NULL, cupsLangEncoding(language));

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
		     "attributes-natural-language", NULL,
		     language != NULL ? language->language : "C");

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "job-uri",
		     NULL, uri);

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name",
		     NULL, user);

	/*
	 * Do the request...
	 */

	snprintf(uri, sizeof(uri), "/jobs/%d", job_id);

	response = cupsDoRequest(http, request, uri);

	if (response == NULL)
	{
		syslog(LOG_ERR, "Unable to hold Job %d, "
		       "null response to cupsDoRequest, cupsLastError() returns %s",
		       job_id, ippErrorString(cupsLastError()));
		rtn = -1;

	}
	else if (response->request.status.status_code > IPP_OK_CONFLICT)
	{
		syslog(LOG_ERR, "Unable to hold Job %d, "
		       "response->request.status.status_code = %s",
		       job_id, ippErrorString(response->request.status.status_code));
		rtn = -1;

	}
	else
	{
		rtn = 0;
	}

	if (response != NULL)
	{
		ippDelete(response);
	}

	cupsLangFree(language);

	return rtn;
}

/*
 * IPP_RELEASE_JOB function.
 *
 * job_id - The job ID to release
 * user - Owner of job
 */
int ipp_release_job(http_t * http, int job_id, const char * user)
{
	ipp_t * request;		/* IPP request */
	ipp_t * response;		/* IPP response */
	char uri[HTTP_MAX_URI];	        /* Printer URI */
	cups_lang_t * language;		/* Language to use */
	int rtn = -1;                   /* Return value -1: Error 0: Success */

	/*
	 * Build a standard CUPS URI for the printer and fill the standard IPP
	 * attributes...
	 */

	if ((request = ippNew()) == NULL)
	{
		syslog(LOG_ERR, "Unable to create request: %s", strerror(errno));
		httpClose(http);
		return (-1);
	}

	request->request.op.operation_id = IPP_RELEASE_JOB;
	request->request.op.request_id   = 0x8095;

	snprintf(uri, sizeof(uri), "ipp://localhost/jobs/%d", job_id);

	language = cupsLangDefault();

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
		     "attributes-charset", NULL, cupsLangEncoding(language));

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
		     "attributes-natural-language", NULL,
		     language != NULL ? language->language : "C");

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "job-uri",
		     NULL, uri);

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name",
		     NULL, user);

	/*
	 * Do the request...
	 */

	snprintf(uri, sizeof(uri), "/jobs/%d", job_id);

	response = cupsDoRequest(http, request, uri);

	if (response == NULL)
	{
		syslog(LOG_ERR, "Unable to release Job %d, "
		       "null response to cupsDoRequest, cupsLastError() returns %s",
		       job_id, ippErrorString(cupsLastError()));
		rtn = -1;

	}
	else if (response->request.status.status_code > IPP_OK_CONFLICT)
	{
		syslog(LOG_ERR, "Unable to release Job %d, "
		       "response->request.status.status_code = %s",
		       job_id, ippErrorString(response->request.status.status_code));
		rtn = -1;

	}
	else
	{
		rtn = 0;
	}

	if (response != NULL)
	{
		ippDelete(response);
	}

	cupsLangFree(language);

	return rtn;
}

/*
 * IPP_SEND_DOCUMENT function
 *
 * job_id - The job ID to print this file to
 * file - File to print
 * docname - Name of job file
 * user - Owner of job
 * format - Document format
 * last - 1 means last file in job
 */
int ipp_send_doc(http_t * http, int job_id, const char * file, const char * docname,
		 const char * user, const char * format, int last)
{
	ipp_t * request;		/* IPP request */
	ipp_t * response;		/* IPP response */
	char uri[HTTP_MAX_URI];	        /* Printer URI */
	cups_lang_t * language;		/* Language to use */
	int rtn = -1;                   /* Return value -1: Error 0: Success */

	/*
	 * Build a standard CUPS URI for the printer and fill the standard IPP
	 * attributes...
	 */

	if ((request = ippNew()) == NULL)
	{
		syslog(LOG_ERR, "Unable to create request: %s", strerror(errno));
		httpClose(http);
		return (-1);
	}

	request->request.op.operation_id = IPP_SEND_DOCUMENT;
	request->request.op.request_id   = 0x8092;

	snprintf(uri, sizeof(uri), "ipp://localhost/jobs/%d", job_id);

	language = cupsLangDefault();

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET,
		     "attributes-charset", NULL, cupsLangEncoding(language));

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE,
		     "attributes-natural-language", NULL,
		     language != NULL ? language->language : "C");

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "job-uri",
		     NULL, uri);

	ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name",
		     NULL, user);

	if (docname)
	{
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME,
			     "document-name", NULL, docname);
	}

	if (format)
	{
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_MIMETYPE,
			     "document-format", NULL, format);
	}

	if (last) {
		ippAddBoolean(request, IPP_TAG_OPERATION, "last-document", 1);
	}

	/*
	 * Do the request...
	 */

	snprintf(uri, sizeof(uri), "/jobs/%d", job_id);

	response = cupsDoFileRequest(http, request, uri, file);

	if (response == NULL)
	{
		syslog(LOG_ERR, "Unable to print file (Job %d), "
		       "null response to cupsDoRequest, cupsLastError() returns %s",
		       job_id, ippErrorString(cupsLastError()));
		rtn = -1;

	}
	else if (response->request.status.status_code > IPP_OK_CONFLICT)
	{
		syslog(LOG_ERR, "Unable to print file (Job %d), "
		       "response->request.status.status_code = %s",
		       job_id, ippErrorString(response->request.status.status_code));
		rtn = -1;

	}
	else
	{
		rtn = 0;
	}

	if (response != NULL) {
		ippDelete(response);
	}

	cupsLangFree(language);

	return rtn;
}
