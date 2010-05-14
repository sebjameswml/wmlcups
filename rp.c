/*
 * Shamelessly nicked from:
 * http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoClient.c
 *
 * And then modified to send raw data from a file to a print server.
 *
 * Note that this is a CLIENT, not a server. The raw input program
 * used on WML print platform devices is also called rawprint, but is
 * (currently) to be found in the wmlppctrl code module.
 *
 * Seb, Sept 1st 2008.
 *
 */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>     /* for close() */

#define BUFSIZE 4096   /* Size of buffer */

#define SLOW_RAWPRINT_PAUSE 45000000 /* 45 second pause */

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
	int sock;                        /* Socket descriptor */
	struct sockaddr_in printServAddr; /* Print server address */
	unsigned short printServPort = 9100; /* Print server port, set to default of 9100 */
	char *servIP;                    /* Server IP address (dotted quad) */

	FILE * fp;                    /* Input file */
	char buffer[BUFSIZE];    /* Buffer into which file contents are read, before sending */
	unsigned int bufsize;    /* Holds the number of bytes read into buffer */
	unsigned int totalSent = 0;/* The amount of data sent, in total */
	char * printFile;

	struct stat buf;

	if ((argc < 3) || (argc > 4)) {  /* Test for correct number of arguments */
		fprintf (stderr,
			 "Usage: %s <Server IP> <File> [<Port>]\n",
			 argv[0]);
		exit (1);
	}

	servIP = argv[1];             /* First arg: server IP address (dotted quad) */
	printFile = argv[2];         /* Second arg: file to print */
	if (argc == 4) {
		printServPort = atoi (argv[3]); /* Use given port, if any */
	}

	/*
	 * Get the size of printFile here, so we can check if we sent
	 * the entire file or not.
	 */
	memset (&buf, 0, sizeof(struct stat));
	if (stat (printFile, &buf)) {
		DieWithError ("Failed to stat input file.");
	}
	//printFileSize = buf.st_size;

	fp = fopen (printFile, "r");
	if (fp == NULL) {
		DieWithError ("Failed to open input file.");
	}

	/* Create a reliable, stream socket using TCP */
	if ((sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fclose (fp);
		DieWithError ("socket() failed");
	}

	/* Construct the server address structure */
	memset (&printServAddr, 0, sizeof(printServAddr));     /* Zero out structure */
	printServAddr.sin_family      = AF_INET;               /* Internet address family */
	printServAddr.sin_addr.s_addr = inet_addr (servIP);    /* Server IP address */
	printServAddr.sin_port        = htons (printServPort); /* Server port */

	/* Establish the connection to the print server */
	if (connect (sock, (struct sockaddr *) &printServAddr, sizeof(printServAddr)) < 0) {
		fclose (fp);
		DieWithError ("connect() failed");
	}

	while ((bufsize = fread (buffer, sizeof (char), BUFSIZE, fp))) {
		/* Send data to the server */
		if (send (sock, buffer, bufsize, 0) != bufsize) {
			fclose (fp);
			close (sock);
			DieWithError ("send() sent a different number of bytes than expected");
		}
		totalSent += bufsize;
#ifdef SLOW_RAWPRINT_PAUSE
		printf ("Pausing before closing the socket...\n");
		usleep (SLOW_RAWPRINT_PAUSE);
#endif
	}

	if (totalSent != buf.st_size) {
		char err[128];
		snprintf (err, 128,
			  "Sent a different number of bytes (%d) than expected from file size (%ld)",
			  totalSent, buf.st_size);
		DieWithError (err);
	}

	fprintf (stdout,
		 "%s sent to %s, port %d successfully!\n",
		 printFile, servIP, printServPort);

	fclose (fp);
	close (sock);
	exit (0);
}

void DieWithError (char *errorMessage)
{
	fprintf (stderr, "Error: %s\n", errorMessage);
	exit (1);
}
