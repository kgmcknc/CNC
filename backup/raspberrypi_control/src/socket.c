#include "main.h"

pid_t system_control_fork;
int control_socket;
int unix_sockets[MAX_UNIX_SOCKETS] = {0};
int active_sockets = 0;
int active_unix = 0;
fd_set all_sockets;
int max_fds;
struct timeval timeout;
char command_ready = 0;
char system_command[MAX_FUNCTION_STRING];

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)

int create_unix_socket(char path[MAX_FILE_STRING]){
    // Create a unix socket as a server
    int com_fd, com_socket, com_len, com_opt = 1;
    struct sockaddr_un com_addr;
    com_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(com_fd == 0){
        printf("Parent Socket Was 0\n");
        return -1;
    }
    /*if(setsockopt(com_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *) &com_opt, sizeof(com_opt))){
        printf("Parent Socket Opt Failed\n");
        return -1;
    }*/
    printf("SOCKET PATH: %s\n", path);
    memset(&com_addr, 0, sizeof(com_addr));
    com_addr.sun_family = AF_UNIX;
	com_addr.sun_path[0] = 0x00;
    strncpy(com_addr.sun_path+1, path, sizeof(path));
    printf("Doing Bind in Parent\n");
    if(bind(com_fd, (struct sockaddr*) &com_addr, sizeof(com_addr)) < 0){
        printf("Failed in Parent Bind\n");
        handle_error("bind");
        return -1;
    }
    printf("Doing Listen in Parent\n");
    if(listen(com_fd, 1) < 0){
        printf("Failed in Parent Listen\n");
        return -1;
    }
    com_len = sizeof(com_addr);
    printf("Doing Accept in Parent\n");
    com_socket = accept(com_fd, (struct sockaddr*) &com_addr, (socklen_t*) &com_len);
    if(com_socket < 0){
        printf("Failed in Parent Accept\n");
        return -1;
    }
    printf("Finished Create\n");
    return com_socket;
}

int connect_unix_socket(char path[MAX_FILE_STRING]){
    // connect to un//ix_socket as client
    int com_fd, com_socket, com_len, com_opt = 1;
    struct sockaddr_un com_addr;
    com_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&com_addr, 0, sizeof(com_addr));
    com_addr.sun_family = AF_UNIX;
    com_addr.sun_path[0] = 0x00;
    strncpy(com_addr.sun_path+1, path, sizeof(path));
    printf("Doing Connect in Listener\n");
    connect(com_socket, (struct sockaddr *)&com_addr, sizeof(com_addr));
    printf("Finished Connect\n");
    return com_socket;
}

int socket_handler(void){
    int s_count, r_count;
    int activity;
    int new_socket = 0;
    int new_addr_len;
    int num_read;
    int shift;
    char read_data[MAX_FUNCTION_STRING];
    struct sockaddr_in new_addr;
    new_addr_len = sizeof(new_addr);
    
    command_ready = 0;
    
    //printf("Active Sockets: %d\n", active_unix);

    // clear socket set
    FD_ZERO(&all_sockets);
    // always update max file descriptor for select
    max_fds = 0;

    // set unix sockets
    for(s_count=0;s_count<MAX_UNIX_SOCKETS;s_count++){
        if(s_count >= active_unix) break;
        FD_SET(unix_sockets[s_count], &all_sockets);
        if(unix_sockets[s_count] > max_fds) max_fds = unix_sockets[s_count];
    }
    
    // wait for activity on any sockets, timeout in timeout time
    // nfds, readfds, writefds, exeptfds, timeout
    //printf("Starting Select\n");
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    activity = select((max_fds + 1), &all_sockets, NULL, NULL, &timeout);
    if(activity < 0){
        printf("---- ERROR IN SERVER ---\n");
    }
    //printf("Got Something...\n");
    
    // check unix sockets
    for(s_count=0;s_count<MAX_UNIX_SOCKETS;s_count++){
        if(s_count >= active_unix) break;
        if(FD_ISSET(unix_sockets[s_count], &all_sockets)){
            printf("Got Data On Unix:%d\n", s_count);
            num_read = read(unix_sockets[s_count], read_data, sizeof(read_data));
            if(num_read > 0){
                // Got Data
                read_data[num_read] = '\0';
                printf("got: %s\n", read_data);
				command_ready = 1;
                strcpy(system_command, read_data);
            } else {
                if(num_read == 0){
                    // No Data... Closing Connection
                    printf("Closing Unix Socket: %d\n", s_count);
                    close(unix_sockets[s_count]);
                    unix_sockets[s_count] = 0;
                } else {
                    // Error... Closing Connection
                    printf("Error On Unix Socket: %d\n", s_count);
                    close(unix_sockets[s_count]);
                    unix_sockets[s_count] = 0;
                }
            }
        }
    }
    
    // shift down to remove gaps in clients after one closes
    shift = 0;
    for(r_count=0;r_count<active_unix;r_count++){
        if(unix_sockets[r_count] == 0){
            while((unix_sockets[r_count+shift] == 0)){
                if((r_count+shift)<active_unix){
                    shift = shift + 1;
                } else {
                    break;
                }
            }
            unix_sockets[r_count] = unix_sockets[r_count+shift];
            active_unix = active_unix - 1;
        }
    }
    
    return 0;
}