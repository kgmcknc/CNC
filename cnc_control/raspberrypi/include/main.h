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

#define MAX_FUNCTION_STRING 40
#define MAX_FILE_STRING 200

void init_system(void);
void shutdown_system(void);
int create_unix_socket(char path[MAX_FILE_STRING]);
int connect_unix_socket(char path[MAX_FILE_STRING]);
void system_control(int control_socket);
int socket_handler(void);
