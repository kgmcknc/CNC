#include "instructions.h"

unsigned char instruction_state = 0;

void send_instruction(char inst_type, char **inst_data){
	unsigned char checksum_data;
	unsigned int inst_length;
	
	if(inst_type == 0){
		// blank packet
	}
	if(inst_type == 1){
		// request packet
	}
	if(inst_type == 2){
		// packet received
	}
	if(inst_type == 3){
		// resend last packet
	}
	if(inst_type == 4){
		// send command
	}
	if(inst_type == 5){
		// send movement
	}
}

void receive_instruction(void){
	char inst_type;
	
	if(inst_type == 0){
		// blank packet
	}
	if(inst_type == 1){
		// packet received
	}
	if(inst_type == 2){
		// packet received
	}
	if(inst_type == 3){
		// resend last packet
	}
	if(inst_type == 4){
		// command
	}
	if(inst_type == 5){
		// movement
	}
}

void save_last_instruction(void){
	
}

void process_instructions(void){
	
}

void init_instructions(void){
	instruction_state = 0;
}
