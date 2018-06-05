#include "nRF24l01P.h"
#include "stm32f10x.h"

const u8 TX_ADDRESS_MB[TX_ADDR_WIDTH]={0xE1,0xE2,0xE3,0xE4,0xE5}; //���͵�ַ
const u8 RX_ADDRESS_MB[RX_ADDR_WIDTH]={0xE1,0xE2,0xE3,0xE4,0xE5}; //���յ�ַ		
/***************************************************************************
 * ��  �� : SPI��д���ݺ�������дһ���ֽ�
 * ��  �� : Dat��д�������
 * ����ֵ : ��ȡ������
 **************************************************************************/
u8 SPI2_ReadWriteByte(u8 Dat)                                       
{		
	u8 retry=0;				 	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)      //���ͻ����־λΪ��
	{
		retry++;
		if(retry>200)return 0;
	}			  
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI2, Dat);                                      //ͨ������SPI1����һ������
	retry=0;
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)   //���ջ����־λ��Ϊ��
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);                                 //ͨ��SPI1���ؽ�������				    
}

/***************************************************************************
 * ��  �� : �������ں�NRF24L01ģ��ͨѶ��SPI����
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void NRF24L01_MB_SPI_Init(void)
{	 
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
    
  // Enable SPI2 and GPIO clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //SPI2ʱ��ʹ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);


  //SPI2�ڳ�ʼ��������PB13��PB15Ϊ��push-pull - SCK,MOSI
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	//����PB14Ϊ���� - MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);          //PB13/14/15����
	SPI_Cmd(SPI2, DISABLE); 
	/* SPI2 configuration */                                              //��ʼ��SPI�ṹ��
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //SPI����Ϊ˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //����SPIΪ��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //SPIʱ�ӿ���ʱΪ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                        //��һ��ʱ���ؿ�ʼ��������
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS�ź��������ʹ��SSIλ������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;   //SPI2������Ԥ��ƵֵΪ32
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRCֵ����Ķ���ʽ


	SPI_Init(SPI2, &SPI_InitStructure);      //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPI2�Ĵ���
	SPI_Cmd(SPI2, ENABLE);                   //ʹ��SPI2����
	SPI2_ReadWriteByte(0xff);  
} 

/***************************************************************************
 * ��  �� : NRF24L01��ʼ����������ʼ������NRF24L01ģ��Ĺܽţ�����SPI��ʼ������
 *          ��ɺ�NRF24L01ģ��ͨѶ��SPI���ߵĳ�ʼ��
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void Init_NRF24L01_MB(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* ����SPI2Ƭѡ */
	RCC_APB2PeriphClockCmd(RCC_NRF24L01_MB_CS, ENABLE);   //ʹ��GPIO��ʱ��
  GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_CS;      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_CS, &GPIO_InitStructure);
	
	/* NRF24L01ʹ�ܿ��ƹܽ� */
	RCC_APB2PeriphClockCmd(RCC_NRF24L01_MB_CE, ENABLE);    //ʹ��GPIO��ʱ��
  GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_CE;          //NRF24L01 ģ��Ƭѡ�ź�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_CE, &GPIO_InitStructure);
	
	/* ����NRF24L01ģ��ָʾ�ƿ��ƹܽ� */
  RCC_APB2PeriphClockCmd( RCC_NRF24L01_MB_LED , ENABLE); 						 			
	GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_LED;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_LED, &GPIO_InitStructure);
	
	CE_MB_HIGH;                                    //��ʼ��ʱ������
  CS_MB_HIGH;                                    //��ʼ��ʱ������

	/* ���û�ȡNRF24L01ģ��IRQ�Ĺܽ� */
	GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_IRQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_IRQ, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_NRF24L01_MB_IRQ,NRF24L01_MB_IRQ);

	NRF24L01_MB_SPI_Init();                      //��ʼ��SPI
	CE_MB_LOW; 	                                 //ʹ��NRF24L01
	CS_MB_HIGH;                                  //SPIƬѡȡ��
}

/***************************************************************************
 * ��  �� : NRF24L01�Ĵ���д����
 * ��  �� : regaddr��Ҫд�ļĴ�����ַ��data��д�뵽�Ĵ���������
 * ����ֵ : ��ȡ��״ֵ̬
 **************************************************************************/
u8 NRF24L01_MB_Write_Reg(u8 regaddr,u8 dat)
{
	u8 status;	
	
  CS_MB_LOW;                             //ʹ��SPI����
  status =SPI2_ReadWriteByte(regaddr); //���ͼĴ�����ַ
  SPI2_ReadWriteByte(dat);             //д��Ĵ�����ֵ
  CS_MB_HIGH;                            //��ֹSPI����	   
  return(status);       		          //����״ֵ̬
}

/***************************************************************************
 * ��  �� : NRF24L01�Ĵ���������
 * ��  �� : regaddr:Ҫ��ȡ�ļĴ�����ַ
 * ����ֵ : ��ȡ�ļĴ�����ֵ
 **************************************************************************/
u8 NRF24L01_MB_Read_Reg(u8 regaddr)
{
	u8 reg_val;	  
  
 	CS_MB_LOW;                          //ʹ��SPI����		
  SPI2_ReadWriteByte(regaddr);      //���ͼĴ�����
  reg_val=SPI2_ReadWriteByte(0XFF); //��ȡ�Ĵ�������
  CS_MB_HIGH;                         //��ֹSPI����		    
  return(reg_val);                 //���ض�ȡ��ֵ
}	

/***************************************************************************
 * ��  �� : ��ָ����ַ����ָ�����ȵ�����
 * ��  �� : pBuf:�������ݵĴ�ŵ�ַ��datlen�������������ֽ���
 * ����ֵ : ��ȡ�ļĴ�����ֵ
 **************************************************************************/
u8 NRF24L01_ReadBuf_MB(u8 regaddr,u8 *pBuf,u8 datlen)
{
	u8 status,u8_ctr;	       
  CS_MB_LOW;                               //ʹ��SPI����
  status=SPI2_ReadWriteByte(regaddr);    //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<datlen;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//��������
  CS_MB_HIGH;                              //�ر�SPI����
  return status;                        //���ض�����״ֵ̬
}

/***************************************************************************
 * ��  �� : ��ָ����ַд��ָ�����ȵ�����
 * ��  �� : pBuf:�������ݵĴ�ŵ�ַ��datlen�������������ֽ���
 * ����ֵ : ��ȡ��״̬�Ĵ���ֵ
 **************************************************************************/
u8 NRF24L01_Write_Buf(u8 regaddr, u8 *pBuf, u8 datalen)
{
	u8 status,u8_ctr;	    
 	CS_MB_LOW;                                              //ʹ��SPI����
  status = SPI2_ReadWriteByte(regaddr);                 //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(u8_ctr=0; u8_ctr<datalen; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //д������	 
  CS_MB_HIGH;                                             //�ر�SPI����
  return status;                                       //���ض�����״ֵ̬
}	

/***************************************************************************
 * ��  �� : ����NRF24L01����һ������
 * ��  �� : buf:�����������׵�ַ
 * ����ֵ : �������״��
 **************************************************************************/
u8 NRF24L01_TxPacket_MB(u8 *buf)
{
	u8 state;   
	
	CE_MB_LOW;
  NRF24L01_Write_Buf(WR_TX_PLOAD,buf,TX_PLOAD_WIDTH);  //д���ݵ�TX BUF 
 	CE_MB_HIGH;                                             //��������	   
	while(READ_NRF24L01_MB_IRQ!=0);                         //�ȴ��������
	state=NRF24L01_MB_Read_Reg(STATUS);                     //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_MB_Write_Reg(SPI_WRITE_REG+STATUS,state);      //���TX_DS��MAX_RT�жϱ�־
	if(state&MAX_TX)                                     //�ﵽ����ط�����
	{
		NRF24L01_MB_Write_Reg(FLUSH_TX,0xff);                 //���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(state&TX_OK)                                      //�������
	{
		return TX_OK;
	}
	return 0xff;                                         //����ԭ����ʧ��
}

/***************************************************************************
 * ��  �� : ����NRF24L01��ȡһ������
 * ��  �� : buf:�����������׵�ַ
 * ����ֵ : 0�����յ����ݣ�1��δ���յ�����
 **************************************************************************/
u8 NRF24L01_RxPacket_MB(u8 *buf)
{
	u8 state;		    
	
	state=NRF24L01_MB_Read_Reg(STATUS);                //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_MB_Write_Reg(SPI_WRITE_REG+STATUS,state); //���TX_DS��MAX_RT�жϱ�־
	if(state&RX_OK)                                 //���յ�����
	{
		NRF24L01_ReadBuf_MB(RD_RX_PLOAD,buf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_MB_Write_Reg(FLUSH_RX,0xff);          //���RX FIFO�Ĵ��� 
		return RX_OK; 
	}	   
	return 0;                                      //û�յ��κ�����
}

/***************************************************************************
 * ��  �� : ��ʼ��NRF24L01��RXģʽ��������ز�����CE��ߺ�,������RXģʽ
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void Set_RxMode_MB(void)
{
	CE_MB_LOW;	  
  //дRX�ڵ��ַ
  NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_MB,RX_ADDR_WIDTH);

  //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);    
  //ʹ��ͨ��0�Ľ��յ�ַ  	 
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
  //����RFͨ��Ƶ��		  
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_CH,55);	     
  //ѡ��ͨ��0����Ч���ݿ�� 	    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
  //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);
  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX����ģʽ 
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+CONFIG, 0x0f); 
  //CEΪ��,�������ģʽ 
  CE_MB_HIGH;                                
}			

/***************************************************************************
 * ��  �� : ��ʼ��NRF24L01��TXģʽ��������ز���
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void Set_TxMode_MB(void)
{														 
	CE_MB_LOW;	    
  //дTX�ڵ��ַ 
  NRF24L01_Write_Buf(SPI_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS_MB,TX_ADDR_WIDTH);    
  //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
  NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_MB,RX_ADDR_WIDTH); 

  //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);     
  //ʹ��ͨ��0�Ľ��յ�ַ  
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01); 
  //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+SETUP_RETR,0x21);
  //����RFͨ��Ϊ55
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_CH,55);       
  //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);  
  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX����ģʽ,���������ж�
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+CONFIG,0x0e);                                  
}		  
/*********************************END FILE************************************/







