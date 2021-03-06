
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/time.h>

#define CONTROL_SOCKET_PATH "./cnc_socket"
#define MAX_UNIX_SOCKETS 1
#define MAX_FUNCTION_STRING 40
#define MAX_FILE_STRING 200

int socket_handler(uint8_t* command_ready, char* system_command);
int create_unix_socket(const char socket_path[MAX_FILE_STRING]);
int connect_unix_socket(const char socket_path[MAX_FILE_STRING]);

extern pid_t system_control_fork;
extern int control_socket;
extern int unix_sockets[MAX_UNIX_SOCKETS];
extern int active_sockets;
extern int active_unix;
extern fd_set all_sockets;
extern int max_fds;

