
#include "main.h"
#include "revision.h"
#include "keyboard.h"
#include "interface_serial.h"
#include "stdio.h"
#include "math.h"

uint8_t quit_system = 0;
int32_t spi_fd = 0;
pid_t system_control_fork;

#define SERIAL_INDEX_MIN   0
#define SERIAL_INDEX_MAX   1

int serial_port_index = SERIAL_INDEX_MIN;
char serial_port_string[15];

struct interface_struct interface;

serial_class cnc_serial(1);

int main(int argc, char **argv) {
   int socket_addr;
   int wstatus;

   printf("\n----------------------------------------\n");
   printf("\nStarting CNC Interface\n");
   printf("\nVersion %d.%d.%d.%d\n", FW_REV_BETA, FW_REV_MAJOR, FW_REV_MINOR, FW_REV_PATCH);

   init_interface_struct(&interface);
   printf("Everything safe... Starting initialization Process\n");
   while(quit_system == 0){
      switch(interface.state){
         case FORK_INPUT : {
            system_control_fork = fork();
            if(system_control_fork == 0){
               interface.state = CREATE_CHILD_SOCKET;
            } else {
               signal(SIGTERM, &system_shutdown);
               signal(SIGINT, &system_shutdown);
               interface.state = CREATE_SOCKET;
            }
            break;
         }
         case CREATE_SOCKET : {
            socket_addr = create_unix_socket(CONTROL_SOCKET_PATH);
            if(socket_addr == -1){
               printf("socket attempt failed... try again...\n");
            } else {
               printf("Created Unix Socket\n");
               unix_sockets[active_unix] = socket_addr;
               active_unix = active_unix + 1;
               interface.state = INTERFACE_INIT;
            }
            break;
         }
         case CREATE_CHILD_SOCKET : {
            control_socket = connect_unix_socket(CONTROL_SOCKET_PATH);
            if(control_socket == -1){
               interface.state = CREATE_CHILD_SOCKET;
            } else {
               interface.state = CHILD_PROCESS;
            }
            break;
         }
         case INTERFACE_INIT : {
            sprintf(serial_port_string, "/dev/ttyACM%d", serial_port_index);
            printf("Initializing Serial: %s\n", serial_port_string);
            serial_fd = serialOpen(serial_port_string, 115200);
            if(serial_fd != -1){
               interface.state = ENABLE_GPIO;
               printf("Connected %s!\n", serial_port_string);
            } else {
               printf("Couldn't Connect %s: %d\n", serial_port_string, serial_fd);
               serial_port_index = serial_port_index + 1;
               if(serial_port_index > SERIAL_INDEX_MAX){
                  printf("Couldn't Connect Serial, Quitting\n");
                  interface.state = EXIT_INTERFACE;
               }
            }
            sleep(1);
            break;
         }
         case ENABLE_GPIO : {
            printf("\nSetting Up WiringPi\n");
            load_config_file(&interface.machine_config);
            init_interface_gpio();
            sleep(1);
            interface.state = INTERFACE_RUNNING;
            break;
         }
         case INTERFACE_RUNNING : {
            quit_system = interface_main(&interface);
            break;
         }
         case DISABLE_GPIO : {
            disable_interface_gpio();
            if(interface.program.instruction != 0){
               free(interface.program.instruction);
               interface.program.instruction = 0;
            }
            sleep(1);
            interface.state = EXIT_INTERFACE;
            break;
         }
         case EXIT_INTERFACE : {
            kill(system_control_fork, SIGTERM);
            waitpid(system_control_fork, &wstatus, 0);
            quit_system = 1;
            break;
         }
         case CHILD_PROCESS : {
            system_control(control_socket);
            exit(EXIT_SUCCESS); // should never hit this - parent kills child
            break;
         }
         default : {
            interface.state = DISABLE_GPIO;
            quit_system = 0;
            break;
         }
      }
   }

   return 0;
}

void init_interface_struct(struct interface_struct* interface){
   interface->state = FORK_INPUT;
   interface->machine_state = MACHINE_IDLE;
   interface->user_command_set = 0;
   interface->user_command_finished = 0;
   interface->user_command[0] = '\0';
   interface->read_in_progress = 0;
   interface->read_complete = 0;
   interface->write_in_progress = 0;
   interface->write_complete = 0;
   interface->machine_marker = 0;
   interface->status_received = 0;
   interface->instant_done = 0;
   interface->machine_configured = 0;
   interface->instruction_requested = 0;
}

uint8_t interface_main(struct interface_struct* interface){
   handle_cnc_state(interface);
   return 0;
}

void system_shutdown(int sig){
   int wstatus;
   // disable gpio to shutdown safely
   disable_interface_gpio();
   if(interface.program.instruction != 0){
      free(interface.program.instruction);
      interface.program.instruction = 0;
   }
   serialClose(serial_fd);
   kill(system_control_fork, SIGTERM);
   waitpid(system_control_fork, &wstatus, 0);
   exit(0);
}

void system_control(int control_socket){
   char control_char[8];
   char control_count = 0;
   char send_data = 0;
   uint8_t exit_child = 0;
   // execute system shutdown on sigterm
   signal(SIGTERM, &system_control_shutdown);
   signal(SIGINT, &system_control_shutdown);

   while(!exit_child){
      // use getche to echo each input character
      control_char[control_count] = getch();
      // send character to main process
      control_count++;
      if(control_char[0] == 27){
         // escape character
         if(control_char[1] != 0){
            if(control_char[1] == 91){
               if(control_char[2] != 0){
                  if(control_char[2] == 49){
                     if(control_char[3] != 0){
                        if(control_char[3] == 59){
                           if(control_char[4] != 0){
                              if(control_char[4] == 50){
                                 if(control_char[5] != 0){
                                    send_data = 1;
                                 }
                              } else {
                                 send_data = 1;
                              }
                           }
                        } else {
                        send_data = 1;
                        }
                     }
                  } else {
                     send_data = 1;
                  }
               }
            } else {
               send_data = 1;
            }
         }
         if(send_data){
            send(control_socket, &control_char, sizeof(control_char), 0);
            for(control_count = 0; control_count < 8; control_count++){
               control_char[control_count] = 0;
            }
            control_count = 0;
            send_data = 0;
         }
      } else {
         send(control_socket, &control_char[0], sizeof(control_char[0]), 0);
         control_char[0] = 0;
         control_count = 0;
         send_data = 0;
      }
   }
}

void system_control_shutdown(int sig){
   // reset terminal before exiting
   resetTermios();
   exit(0);
}

void print_interface_menu(void){
   printf("\n\n");
   printf("<<--------------------------->>\n");
   printf("<<--User Input Options------->>\n");
   printf("<<--------------------------->>\n");
   printf("<<--q : Shutdown and Quit---->>\n");
   printf("<<--e : Enable Gpio --------->>\n");
   printf("<<--d : Disable Gpio -------->>\n");
   printf("<<--g : Get CNC Status  ----->>\n");
   printf("<<--s : Send Serial String ----->>\n");
   printf("<<--r : Receive Serial String -->>\n");
   printf("<<--u : Update Firmware ----->>\n");
   printf("<<--i : Init Program -------->>\n");
   printf("<<--t : Terminate Program --->>\n");
   printf("<<--v : Read Micro Version -->>\n");
   printf("<<--o : Open GCode File    -->>\n");
   printf("<<--c : Reconnect Spi      -->>\n");
   printf("<<--------------------------->>\n");
   printf("\nType Input and Press Enter:\n");
}

