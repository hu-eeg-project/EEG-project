#pragma once
#include <stdint.h>
#include <stddef.h>

// Error codes that may be send to the pc.
#define ERROR_UNKOWN_CMD    0x01
#define ERROR_BAD_SIGNAL    0x02
#define ERROR_BAD_CHECKSUM  0x03
#define ERROR_PACKET_LEN    0x04
#define ERROR_CREATING_TASK 0x05

/** 
 * \brief The funtion that receives and parses all the data from the Froce Trainer.
 * The function parses data from the ThinkGear communication protocol. Documentation on this protocol can
 * be found here: http://developer.neurosky.com/docs/doku.php?id=thinkgear_communications_protocol
 */
void handle_force_trainer();

//The next functions have to ported.

/** 
 * \brief This function should read an amount of bytes specified bij len. Should also be blocking.
 * 
 * @param data The array the data will be stored in.
 * @param len Amount of bytes to read in data.
 */
void read_bytes_from_ft(uint8_t* data, const size_t len);

 
/** 
 * \brief Function for sending bytes to the PC. Works the same as printf.
 *
 * @param format C string that contains the text to be written to the pc. Can contain format specifiers. 
 */
void write_bytes_pc(const char* format, ...);
 
/** 
 * \brief Function for getting the amount of microseconds since start of program. 
 *
 * @return 
 */
int64_t timer_get_us();


