#ifndef SOFT_I2C_1_H
#define SOFT_I2C_1_H
#include "main.h"

#define SDA1_IN()                 \
    {                            \
        GPIOB->CRH &= ~(3 << 28); \
        GPIOB->CRH |= 0 << 28;    \
    }
#define SDA1_OUT()                \
    {                            \
        GPIOB->CRH &= ~(3 << 28); \
        GPIOB->CRH |= 1 << 28;    \
    }
#define IIC_SCL(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define IIC_SDA(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define READ_SDA1 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void Write_IIC_Command(uint8_t* command);
void Read_IIC_Data(uint8_t *data, uint8_t count);


#endif
