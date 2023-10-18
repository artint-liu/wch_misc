/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : 串口1收发演示
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH58x_common.h"

uint8_t TxBuff[] = "This is a tx exam\r\n";
uint8_t RxBuff[100];
uint8_t trigB;

// PWR_LVL PA3
// EN_VDD  PA7
// PWM     PB19
// LATCH   PB7
// BTN     PB17

BOOL bLightOn = TRUE;

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */
int main()
{
    uint8_t len;

    SetSysClock(CLK_SOURCE_PLL_60MHz);


    // EN_VDD
    GPIOA_ResetBits(GPIO_Pin_7);
    GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);

    // PWM
    bLightOn = TRUE;
    GPIOB_SetBits(GPIO_Pin_19);
    GPIOB_SetBits(GPIO_Pin_7);
    GPIOB_ResetBits(GPIO_Pin_0);
    GPIOB_ModeCfg(GPIO_Pin_0 | GPIO_Pin_19 | GPIO_Pin_7, GPIO_ModeOut_PP_5mA);

    GPIOB_ModeCfg(GPIO_Pin_17, GPIO_ModeIN_Floating);

    DelayMs(200);

    while(1)
    {
        if(GPIOB_ReadPortPin(GPIO_Pin_17))
        {
            if(bLightOn)
            {
                GPIOB_ResetBits(GPIO_Pin_19);
                GPIOB_ResetBits(GPIO_Pin_7);
                bLightOn = FALSE;
            }
            else
            {
                GPIOB_SetBits(GPIO_Pin_19);
                GPIOB_SetBits(GPIO_Pin_7);
                bLightOn = TRUE;
            }
            DelayMs(200);
        }
    }
}

/*********************************************************************
 * @fn      UART1_IRQHandler
 *
 * @brief   UART1中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void UART1_IRQHandler(void)
{
    volatile uint8_t i;

    switch(UART1_GetITFlag())
    {
        case UART_II_LINE_STAT: // 线路状态错误
        {
            UART1_GetLinSTA();
            break;
        }

        case UART_II_RECV_RDY: // 数据达到设置触发点
            for(i = 0; i != trigB; i++)
            {
                RxBuff[i] = UART1_RecvByte();
                UART1_SendByte(RxBuff[i]);
            }
            break;

        case UART_II_RECV_TOUT: // 接收超时，暂时一帧数据接收完成
            i = UART1_RecvString(RxBuff);
            UART1_SendString(RxBuff, i);
            break;

        case UART_II_THR_EMPTY: // 发送缓存区空，可继续发送
            break;

        case UART_II_MODEM_CHG: // 只支持串口0
            break;

        default:
            break;
    }
}
