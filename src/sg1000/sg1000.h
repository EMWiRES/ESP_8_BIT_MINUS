
#ifndef _SG1000_H_
#define _SG1000_H_

/* SG1000 context */
typedef struct {
    uint8 ram[0x400];    
    uint8 *cartridge;
    uint8 irq;
}t_sg1000;

/* Global data */
extern t_sg1000 sg1000;

/* Function prototypes */
void sg1000_frame(int skip_render);
void sg1000_init(void);
void sg1000_reset(void);
void cpu_reset(void);
int  sg1000_irq_callback(int param);

unsigned char *sg1000_get_video(void);
#endif
