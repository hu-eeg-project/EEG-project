#include "force-trainer.h"

#define ERROR_UNKOWN_CMD    0x01
#define ERROR_BAD_SIGNAL    0x02
#define ERROR_BAD_CHECKSUM  0x03
#define ERROR_PACKET_LEN    0x04
#define ERROR_CREATING_TASK 0x05

void handle_force_trainer(){
    uint8_t data = 0;
    uint8_t pdata[170];
    uint8_t plength = 0;
    uint8_t pchecksum = 0;

    int64_t st = timer_get_us();
    int64_t batch_timer = st;

    int16_t frame_buffer[20] = {0};
    size_t frame_index = 0;
    uint64_t frame_dur = 0;
    while(1){
        if((timer_get_us()-batch_timer)>1000000){ // every second
            write_bytes_pc("b:1\n");
            batch_timer = timer_get_us();
        }
        read_bytes_from_ft(&data, 1);
        if (data != 0xAA) continue; // No Sync Packet
        read_bytes_from_ft(&data, 1);
        if (data != 0xAA) continue; // No second Sync Byte

        read_bytes_from_ft(&plength, 1);
        uint32_t checksum = 0;
        if(plength > 170){
            write_bytes_pc("r:%d\n", ERROR_PACKET_LEN);
            continue;
        }
        read_bytes_from_ft(pdata, plength);
        read_bytes_from_ft(&pchecksum, 1);
        for (int j = 0; j < plength; j++) {
            checksum += pdata[j];
            checksum &= 0xFF;
        }
        checksum = ~checksum & 0xFF;
        if (checksum != pchecksum) {
            write_bytes_pc("r:%d\n", ERROR_BAD_CHECKSUM);
        }
        for (int j = 0; j < plength; j++) {
            switch (pdata[j])
            {            
            case 0x02: // Signal status
                write_bytes_pc("r:%d:%d\n", ERROR_BAD_SIGNAL, pdata[j + 1]);
                j++;
                continue;
            case 0x03: // Hearthrate
                write_bytes_pc("h:%d\n", pdata[j+1]);
                j++;
                continue;
            case 0x04: //Attention
                write_bytes_pc("a:%d\n", pdata[j+1]);
                j++;
                continue;
            case 0x05: // Meditation
                write_bytes_pc("m:%d\n", pdata[j+1]);
                j++;
                continue;
            case 0x80: ; // C++ wants a statement after a label,
                // so here is a semi-colon for an empty statement.
                // Receive Raw EEG data from Neurosky chip on headset
                uint8_t rlength = pdata[j + 1];
                int16_t value = 0;
                for (int u = 0; u < rlength; u++) {
                    value = value << 8 | pdata[j + 2 + u];
                }
                uint64_t nt = timer_get_us();
                uint64_t dt = nt-st;
                frame_dur += dt;
                frame_buffer[frame_index++] = value;
                if(dt>20000){
                    write_bytes_pc("f:%d:%lld\n", frame_index, frame_dur);
                    for(int i = 0;i<frame_index;i++){
                        write_bytes_pc("d:%d\n", frame_buffer[i]);
                    }
                    frame_index = 0;
                    frame_dur = 0;
                }
                st = nt;
                
                j += 1 + rlength;
                continue;

            case 0x86: ; // printerval. doesnt do much at the moment
                uint16_t interval = (0|pdata[++j])<<8;
                interval |= pdata[++j];
                continue;
                
            case 0x83: ;
                uint32_t waves[8] = {0}; // Delta, theta, low-alpha, high-alpha, low-beta, high-beta, low-gamma and mid-gamma
                for(int i = 0;i<8;i++){
                    waves[i] |= (pdata[++j] << 8);
                    waves[i] |= (pdata[++j] << 8);
                    waves[i] |= pdata[++j];
                }
                write_bytes_pc("w:");
                for(int i = 0;i<6;i++){
                    write_bytes_pc("%d:", waves[i]);
                }
                write_bytes_pc("%d\n", waves[6]);
                continue;

            // unused cases:            
            case 0x01: // battery level
            case 0x06: // 8-bit raw EEG data
            case 0x07:
            case 0xE3:
            case 0x55: // following two are reserved
            case 0xAA:
                j++;
                continue;
                
            default:
                write_bytes_pc("r:%d\n", ERROR_UNKOWN_CMD);
                j++;
                break;
            }
        }
        //fflush(stdout);
    }
}
