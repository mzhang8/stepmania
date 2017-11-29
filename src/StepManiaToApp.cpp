/* StepManiaToApp
 * - Adapted from bsmulders' StepmaniaToSolver.cpp.
 */

#include "global.h"
#include "StepManiaToApp.h"
#include "RageLog.h"
#include "RageTimer.h"
#include "RageThreads.h"
#include "InputFilter.h"

#include <ctime>
#if defined(_WINDOWS)
#include <windows.h>
#endif
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

StepManiaToApp* STA;	// global and accessable from anywhere in our program

static RageThread AppThread;
static bool app_shutdown;
static int app_sockfd;

int StartToAppThread(void *p)
{    
	while (!app_shutdown) {
        char buf[1024];
        bzero(buf, 1024);
        int n = read(app_sockfd, buf, 1024);

        if (n < 0) {
            LOG->Warn("Error reading from socket to application.");
        }
        
        RageTimer rtimer = RageTimer(0, 0);
        rtimer.Touch();
        DeviceInput dipUp = DeviceInput(DEVICE_KEYBOARD, KEY_UP, rtimer);
        DeviceInput dipDown = DeviceInput(DEVICE_KEYBOARD, KEY_DOWN, rtimer);
        DeviceInput dipLeft = DeviceInput(DEVICE_KEYBOARD, KEY_LEFT, rtimer);
        DeviceInput dipRight = DeviceInput(DEVICE_KEYBOARD, KEY_RIGHT, rtimer);
        
        switch(buf[0]) {
            case 'l':
                LOG->Info("Left pressed from application.");
                dipLeft.bDown = true;
                dipLeft.level = 1;
                dipLeft.z = 0;
                INPUTFILTER->ButtonPressed(dipLeft);
                rtimer.Touch();
                dipLeft.bDown = false;
                dipLeft.level = 0;
                INPUTFILTER->ButtonPressed(dipLeft);
                break;
            case 'd':
                LOG->Info("Down pressed from application.");
                dipDown.bDown = true;
                dipDown.level = 1;
                dipDown.z = 0;
                INPUTFILTER->ButtonPressed(dipDown);
                rtimer.Touch();
                dipDown.bDown = false;
                dipDown.level = 0;
                INPUTFILTER->ButtonPressed(dipDown);
                break;                
            case 'u':
                LOG->Info("Up pressed from application.");
                dipUp.bDown = true;
                dipUp.level = 1;
                dipUp.z = 0;
                INPUTFILTER->ButtonPressed(dipUp);
                rtimer.Touch();
                dipUp.bDown = false;
                dipUp.level = 0;
                INPUTFILTER->ButtonPressed(dipUp);
                break;                
            case 'r':
                LOG->Info("Right pressed from application.");
                dipRight.bDown = true;
                dipRight.level = 1;
                dipRight.z = 0;
                INPUTFILTER->ButtonPressed(dipRight);
                rtimer.Touch();
                dipRight.bDown = false;
                dipRight.level = 0;
                INPUTFILTER->ButtonPressed(dipRight);
                break;
        }
	}
    
	return 0;
}

StepManiaToApp::StepManiaToApp()
{    
    // Set up network
    int portno;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    
    hostname = "localhost";
    portno = 5005;
    
    // Create socket
    app_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (app_sockfd < 0) {
        LOG->Warn("Error opening socket to application.");
    }
    
    // Get the hostname
    server = gethostbyname(hostname);
    if (server == NULL) {
        LOG->Warn("Error, no such host as %s to application.", hostname);
        exit(0);
    }
    
    // Build the internet address
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
    
    // Try to connect
    if (connect(app_sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        LOG->Warn("Error connecting to server to application.");
    }
    
    // Report for duty
    LOG->Info("StepManiaToApp is ready.");
    Sync();
    
    // Setup thread for receiving network
    app_shutdown = false;
	AppThread.SetName("StepMania to App");
	AppThread.Create(StartToAppThread, this);
}

StepManiaToApp::~StepManiaToApp()
{
    app_shutdown = true;
    close(app_sockfd);
}

void StepManiaToApp::Sync()
{
    Send("COMMS_SYNC");
}

void StepManiaToApp::Send(const char* message)
{
    int n = write(app_sockfd, message, strlen(message));
    if (n < 0) {
    	LOG->Warn("Error writing to socket to application.");
    }
}