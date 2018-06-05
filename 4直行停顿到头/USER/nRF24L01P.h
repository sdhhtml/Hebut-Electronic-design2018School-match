#ifndef __NRF24L01_H
#define __NRF24L01_H

#include "stm32f10x.h"


//NRF24L01ʹ�ܿ��Ʋ���
#define NRF24L01_MB_CE       GPIO_Pin_8
#define GPIO_NRF24L01_MB_CE  GPIOG
#define RCC_NRF24L01_MB_CE   RCC_APB2Periph_GPIOG

//NRF24L01 SPI�ӿ�CS�ź�
#define NRF24L01_MB_CS       GPIO_Pin_7
#define GPIO_NRF24L01_MB_CS  GPIOG
#define RCC_NRF24L01_MB_CS   RCC_APB2Periph_GPIOG

//NRF24L01 �����ж��ź�
#define NRF24L01_MB_IRQ      GPIO_Pin_6
#define GPIO_NRF24L01_MB_IRQ GPIOG
#define RCC_NRF24L01_MB_IRQ  RCC_APB2Periph_GPIOG

//NRF24L01ģ��ָʾ�ƿ���
#define NRF24L01_MB_LED      GPIO_Pin_5
#define GPIO_NRF24L01_MB_LED GPIOB
#define RCC_NRF24L01_MB_LED  RCC_APB2Periph_GPIOB

//nRF2401Pʹ�ܲ���
#define CE_MB_HIGH      {GPIO_SetBits(GPIO_NRF24L01_MB_CE, NRF24L01_MB_CE);}
#define CE_MB_LOW       {GPIO_ResetBits(GPIO_NRF24L01_MB_CE, NRF24L01_MB_CE);}

//SPIƬѡ�ź�	
#define CS_MB_HIGH      {GPIO_SetBits(GPIO_NRF24L01_MB_CS, NRF24L01_MB_CS);}
#define CS_MB_LOW       {GPIO_ResetBits(GPIO_NRF24L01_MB_CS, NRF24L01_MB_CS);}


#define READ_NRF24L01_MB_IRQ    (GPIO_ReadInputDataBit(GPIO_NRF24L01_MB_IRQ,NRF24L01_MB_IRQ))


//�����շ���ַ���(�ֽ���)
#define TX_ADDR_WIDTH 5
#define RX_ADDR_WIDTH 5

//�����շ����ݳ���(�ֽ���)
#define TX_PLOAD_WIDTH 1
#define RX_PLOAD_WIDTH 1
/****************************************************************************************************/
//NRF24L01�Ĵ�����������
#define SPI_READ_REG    0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define SPI_WRITE_REG   0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF  //�ղ���,����������״̬�Ĵ���	 
//SPI(NRF24L01)�Ĵ�����ַ
#define CONFIG          0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
                              //bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��
#define EN_AA           0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR       0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define SETUP_AW        0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR      0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH           0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;
#define RF_SETUP        0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define STATUS          0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
                              //bit5:���ݷ�������ж�;bit6:���������ж�;
#define MAX_TX  	    0x10    //�ﵽ����ʹ����ж�
#define TX_OK       	0x20    //TX��������ж�
#define RX_OK   	    0x40    //���յ������ж�

#define OBSERVE_TX      0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD              0x09  //�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0      0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1      0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2      0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3      0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4      0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5      0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR         0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0        0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define FIFO_STATUS     0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
                              //bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;
															
															
extern  void Init_NRF24L01_MB(void);															
extern  void Set_TxMode_MB(void);		
extern  void Set_RxMode_MB(void);		
extern  u8 NRF24L01_TxPacket_MB(u8 *txbuf);	
extern  u8 NRF24L01_RxPacket_MB(u8 *rxbuf);
#endif
