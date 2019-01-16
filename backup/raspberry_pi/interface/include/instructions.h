
/*
 * Instruction starts and ends with $
 * Fields are separated by $'s
 * First field is instruction number
 * Last field is checksum (custom checksum including first and last $)
 * Second field is length of transfer
 * Third field is type that defines all other fields until checksum
 * 
 * _____________________________________
 * | Field | Content                    |
 * |------------------------------------|
 * | 0     | Instruction #              |
 * | 1     | Instruction Length (bytes) |
 * | 2     | Instruction Type           |
 * | 3     | Instruction Data           |
 * | ....  | ..........                 |
 * | 4     | Instruction Data           |
 * | 5     | Checksum Value             |
 * |_______|____________________________|
 * 
 * _____________________________________
 * | Instruction   | Content            |
 * |------------------------------------|
 * | 0             | Instruction #      |
 * | 1             | Instruction Length |
 * | 2             | Instruction Type   |
 * | 3             | Instruction Data   |
 * | ....          | ..........         |
 * | 4             | Instruction Data   |
 * | 5             | Checksum Value     |
 * |_______________|____________________|
 * 
*/

void send_instruction(char inst_type, char** inst_data);

void receive_instruction(void);

void save_last_instruction(void);
