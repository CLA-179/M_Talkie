// #include <stdio.h>
#include "Soft_i2c1.h"
#include "tim.h"
#include "M_talk.h"

#define ADD_W 0x90
#define ADD_R 0x91
#define SQ_TH 0x0100u
#define PTT 0
#define RX 0
#define TX 1

// uint32_t tx_freq[16];
// uint32_t rx_freq[16];
// uint8_t rt;
// uint8_t tx_first;

uint16_t BK_Read(uint8_t reg)
{
    uint16_t ret = 0;
    IIC_Start();
    IIC_Send_Byte(ADD_W);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(ADD_R);
    IIC_Wait_Ack();
    ret = IIC_Read_Byte(1);
    ret = ret << 8;
    ret |= IIC_Read_Byte(0);
    IIC_Stop();
    return ret;
}
void BK_Write(uint8_t reg, uint16_t data)
{
    IIC_Send_Byte(ADD_W);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Send_Byte((data >> 8) & 0xffu);
    IIC_Wait_Ack();
    IIC_Send_Byte(data & 0xffu);
    IIC_Wait_Ack();
    IIC_Stop();
}

void BK_Idle(void)
{
    BK_Write(0x04, 0xffbf);
}

void init_r()
{
    uint16_t r1 = 0;
    do
    {
        r1 = BK_Read(0x1B);
    } while (r1 != 0x4802u);

    BK_Write(0x04, 0x0b00);
    BK_Write(0x17, 0x04ff);
    BK_Write(0x04, 0x0b00);
    BK_Write(0x05, 0x0c04);
    BK_Write(0x06, 0xf140);
    BK_Write(0x07, 0xed00);
    BK_Write(0x08, 0x3fe0);
    BK_Write(0x09, 0xe0e0);
    BK_Write(0x0a, 0x8543);
    BK_Write(0x0b, 0x0700);
    BK_Write(0x0c, 0xa066);
    BK_Write(0x0d, 0xffff);
    BK_Write(0x0e, 0xffe0);
    BK_Write(0x0f, 0x061f);
    BK_Write(0x10, 0x9e3c);
    BK_Write(0x11, 0x1f00);
    BK_Write(0x12, 0xd1d1);
    BK_Write(0x13, 0x240f);
    BK_Write(0x14, 0x01ff);
    BK_Write(0x15, 0xe000);
    BK_Write(0x16, 0x0c00);
    BK_Write(0x20, 0x01ff); // 初始化各个寄存器
}

void init_t()
{
    uint16_t r1 = 0;
    do
    {
        r1 = BK_Read(0x1B);
    } while (r1 != 0x4802u);

    BK_Write(0x04, 0x7c00);
    // BK_Write(0x17, 0x04ff);
    // BK_Write(0x04, 0x0b00);
    BK_Write(0x05, 0x0004);
    BK_Write(0x06, 0xf140);
    BK_Write(0x07, 0xed00);
    BK_Write(0x08, 0x17e0);
    BK_Write(0x09, 0xe0e0);
    BK_Write(0x0a, 0x8543);
    BK_Write(0x0b, 0x0700);
    BK_Write(0x0c, 0xa066);
    BK_Write(0x0d, 0xffff);
    BK_Write(0x0e, 0xffe0);
    BK_Write(0x0f, 0x061f);
    BK_Write(0x10, 0x9e3f);
    BK_Write(0x11, 0x1fc0);
    BK_Write(0x12, 0xd1d1);
    BK_Write(0x13, 0x240f);
    BK_Write(0x14, 0x01ff);
    BK_Write(0x15, 0xe000);
    BK_Write(0x16, 0x0c00); // 初始化各个寄存器
    // BK_Write(0x20, 0x01ff);
}

void write_freq(uint32_t freq)
{
    // 写频
    BK_Write(0x02, 0x0000);
    BK_Write(0x01, freq & 0xffffu);
    BK_Write(0x00, (freq >> 16) & 0xffffu);
}

void BK_Set_RX(uint32_t freq)
{
    init_r();
    write_freq(freq);
    delay_ms(20);
}
void BK_Set_TX(uint32_t freq)
{
    // if (tx_first == 1)
    // {
    //     BK_Idle();
    //     delay_ms(350);
    //     tx_first = 0;
    // }

    init_t();
    write_freq(freq);
    BK_Write(0x17, 0x06ff);
}

uint16_t BK_Get_SQ()
{
    return BK_Read(0x1a);
}

void BK_Open_Sound()
{
    BK_Write(0x04, 0x0300);
}

void BK_Close_Sound()
{
    BK_Write(0x04, 0x0B00);
}

// int main()
// {
    
//     while (1)
//     {
//         // if (rt == RX)
//         // {
//         //     r2 = BK_Read(0x1a);
//         //     if (r2 < SQ_TH)
//         //     {
//         //         /* code */
//         //     }
//         // }
        
//     }
// }
