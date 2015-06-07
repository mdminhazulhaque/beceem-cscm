#define __OS_WIN

#include <stdio.h>
#include <stdlib.h>
#ifdef __OS_WIN
#include <windows.h>
#include <winbase.h>
#include <time.h>
#else
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

enum {
    CLI_END,
    OMA_DM_START = 5,
    OMA_DM_CLOSE
};

struct cli_header{
    unsigned short type;
    unsigned short data_len;
};


/*
 * Need two arguments:
 * 1. Port -- OMA DM listen port
 * 2. Type -- 5: OMA_DM_START 6: OMA_DM_CLOSE
 */
int call_OMA_cli(int port, int type)
{
    int res;
    int fd = -1;
    struct sockaddr_in cli;

#ifdef __OS_WIN
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 1, 1 );

    res = WSAStartup( wVersionRequested, &wsaData );
    if ( res!= 0 ) {
		return -1;
    }

    if ( LOBYTE( wsaData.wVersion ) != 1 ||
	    HIBYTE( wsaData.wVersion ) != 1 ) {
		WSACleanup( );
		return -1; 
    }

#endif

	printf("Start to call oma cli...\n");
    memset(&cli, 0, sizeof(cli));
    cli.sin_family = AF_INET;
    cli.sin_port = htons((short)port);
    cli.sin_addr.s_addr = inet_addr("127.0.0.1");
    fd = (int) socket(AF_INET, SOCK_STREAM, 0);

    if(fd >= 0 && connect(fd, (struct sockaddr *)&cli, sizeof(cli)) == 0) {
		struct cli_header header;
		header.type = htons((short)type);
		header.data_len = htons(0);
		send(fd, (const char *)&header, sizeof(header), 0);
		printf("Send oma cli...\n");
		closesocket(fd);
		printf("Close oma cli\n");
	} else {
		printf("Call oma cli error\n");
		return -1;
	}

    return 0;
}

