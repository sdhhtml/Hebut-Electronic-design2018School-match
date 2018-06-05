#include "nRF24l01P.h"
#include "stm32f10x.h"

const u8 TX_ADDRESS_MB[TX_ADDR_WIDTH]={0xE1,0xE2,0xE3,0xE4,0xE5}; //发送地址
const u8 RX_ADDRESS_MB[RX_ADDR_WIDTH]={0xE1,0xE2,0xE3,0xE4,0xE5}; //接收地址		
/***************************************************************************
 * 描  述 : SPI读写数据函数，读写一个字节
 * 入  参 : Dat：写入的数据
 * 返回值 : 读取的数据
 **************************************************************************/
u8 SPI2_ReadWriteByte(u8 Dat)                                       
{		
	u8 retry=0;				 	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)      //发送缓存标志位为空
	{
		retry++;
		if(retry>200)return 0;
	}			  
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI2, Dat);                                      //通过外设SPI1发送一个数据
	retry=0;
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)   //接收缓存标志位不为空
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);                                 //通过SPI1返回接收数据				    
}

/***************************************************************************
 * 描  述 : 配置用于和NRF24L01模块通讯的SPI总线
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
void NRF24L01_MB_SPI_Init(void)
{	 
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
    
  // Enable SPI2 and GPIO clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //SPI2时钟使能
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);


  //SPI2口初始化，配置PB13和PB15为：push-pull - SCK,MOSI
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	//配置PB14为输入 - MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);          //PB13/14/15上拉
	SPI_Cmd(SPI2, DISABLE); 
	/* SPI2 configuration */                                              //初始化SPI结构体
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //SPI设置为双线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //设置SPI为主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //SPI时钟空闲时为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                        //第一个时钟沿开始采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS信号由软件（使用SSI位）管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;   //SPI2波特率预分频值为32
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRC值计算的多项式


	SPI_Init(SPI2, &SPI_InitStructure);      //根据SPI_InitStruct中指定的参数初始化外设SPI2寄存器
	SPI_Cmd(SPI2, ENABLE);                   //使能SPI2外设
	SPI2_ReadWriteByte(0xff);  
} 

/***************************************************************************
 * 描  述 : NRF24L01初始化函数，初始化连接NRF24L01模块的管脚，调用SPI初始化函数
 *          完成和NRF24L01模块通讯的SPI总线的初始化
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
void Init_NRF24L01_MB(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 配置SPI2片选 */
	RCC_APB2PeriphClockCmd(RCC_NRF24L01_MB_CS, ENABLE);   //使能GPIO的时钟
  GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_CS;      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_CS, &GPIO_InitStructure);
	
	/* NRF24L01使能控制管脚 */
	RCC_APB2PeriphClockCmd(RCC_NRF24L01_MB_CE, ENABLE);    //使能GPIO的时钟
  GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_CE;          //NRF24L01 模块片选信号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_CE, &GPIO_InitStructure);
	
	/* 配置NRF24L01模块指示灯控制管脚 */
  RCC_APB2PeriphClockCmd( RCC_NRF24L01_MB_LED , ENABLE); 						 			
	GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_LED;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_LED, &GPIO_InitStructure);
	
	CE_MB_HIGH;                                    //初始化时先拉高
  CS_MB_HIGH;                                    //初始化时先拉高

	/* 配置获取NRF24L01模块IRQ的管脚 */
	GPIO_InitStructure.GPIO_Pin = NRF24L01_MB_IRQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_NRF24L01_MB_IRQ, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_NRF24L01_MB_IRQ,NRF24L01_MB_IRQ);

	NRF24L01_MB_SPI_Init();                      //初始化SPI
	CE_MB_LOW; 	                                 //使能NRF24L01
	CS_MB_HIGH;                                  //SPI片选取消
}

/***************************************************************************
 * 描  述 : NRF24L01寄存器写函数
 * 入  参 : regaddr：要写的寄存器地址；data：写入到寄存器的数据
 * 返回值 : 读取的状态值
 **************************************************************************/
u8 NRF24L01_MB_Write_Reg(u8 regaddr,u8 dat)
{
	u8 status;	
	
  CS_MB_LOW;                             //使能SPI传输
  status =SPI2_ReadWriteByte(regaddr); //发送寄存器地址
  SPI2_ReadWriteByte(dat);             //写入寄存器的值
  CS_MB_HIGH;                            //禁止SPI传输	   
  return(status);       		          //返回状态值
}

/***************************************************************************
 * 描  述 : NRF24L01寄存器读函数
 * 入  参 : regaddr:要读取的寄存器地址
 * 返回值 : 读取的寄存器的值
 **************************************************************************/
u8 NRF24L01_MB_Read_Reg(u8 regaddr)
{
	u8 reg_val;	  
  
 	CS_MB_LOW;                          //使能SPI传输		
  SPI2_ReadWriteByte(regaddr);      //发送寄存器号
  reg_val=SPI2_ReadWriteByte(0XFF); //读取寄存器内容
  CS_MB_HIGH;                         //禁止SPI传输		    
  return(reg_val);                 //返回读取的值
}	

/***************************************************************************
 * 描  述 : 从指定地址读出指定长度的数据
 * 入  参 : pBuf:读出数据的存放地址；datlen：读出的数据字节数
 * 返回值 : 读取的寄存器的值
 **************************************************************************/
u8 NRF24L01_ReadBuf_MB(u8 regaddr,u8 *pBuf,u8 datlen)
{
	u8 status,u8_ctr;	       
  CS_MB_LOW;                               //使能SPI传输
  status=SPI2_ReadWriteByte(regaddr);    //发送寄存器值(位置),并读取状态值   	   
 	for(u8_ctr=0;u8_ctr<datlen;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
  CS_MB_HIGH;                              //关闭SPI传输
  return status;                        //返回读到的状态值
}

/***************************************************************************
 * 描  述 : 向指定地址写入指定长度的数据
 * 入  参 : pBuf:读出数据的存放地址；datlen：读出的数据字节数
 * 返回值 : 读取的状态寄存器值
 **************************************************************************/
u8 NRF24L01_Write_Buf(u8 regaddr, u8 *pBuf, u8 datalen)
{
	u8 status,u8_ctr;	    
 	CS_MB_LOW;                                              //使能SPI传输
  status = SPI2_ReadWriteByte(regaddr);                 //发送寄存器值(位置),并读取状态值
  for(u8_ctr=0; u8_ctr<datalen; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据	 
  CS_MB_HIGH;                                             //关闭SPI传输
  return status;                                       //返回读到的状态值
}	

/***************************************************************************
 * 描  述 : 启动NRF24L01发送一次数据
 * 入  参 : buf:待发送数据首地址
 * 返回值 : 发送完成状况
 **************************************************************************/
u8 NRF24L01_TxPacket_MB(u8 *buf)
{
	u8 state;   
	
	CE_MB_LOW;
  NRF24L01_Write_Buf(WR_TX_PLOAD,buf,TX_PLOAD_WIDTH);  //写数据到TX BUF 
 	CE_MB_HIGH;                                             //启动发送	   
	while(READ_NRF24L01_MB_IRQ!=0);                         //等待发送完成
	state=NRF24L01_MB_Read_Reg(STATUS);                     //读取状态寄存器的值	   
	NRF24L01_MB_Write_Reg(SPI_WRITE_REG+STATUS,state);      //清除TX_DS或MAX_RT中断标志
	if(state&MAX_TX)                                     //达到最大重发次数
	{
		NRF24L01_MB_Write_Reg(FLUSH_TX,0xff);                 //清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(state&TX_OK)                                      //发送完成
	{
		return TX_OK;
	}
	return 0xff;                                         //其他原因发送失败
}

/***************************************************************************
 * 描  述 : 启动NRF24L01读取一次数据
 * 入  参 : buf:待发送数据首地址
 * 返回值 : 0：接收到数据；1：未接收到数据
 **************************************************************************/
u8 NRF24L01_RxPacket_MB(u8 *buf)
{
	u8 state;		    
	
	state=NRF24L01_MB_Read_Reg(STATUS);                //读取状态寄存器的值    	 
	NRF24L01_MB_Write_Reg(SPI_WRITE_REG+STATUS,state); //清除TX_DS或MAX_RT中断标志
	if(state&RX_OK)                                 //接收到数据
	{
		NRF24L01_ReadBuf_MB(RD_RX_PLOAD,buf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_MB_Write_Reg(FLUSH_RX,0xff);          //清除RX FIFO寄存器 
		return RX_OK; 
	}	   
	return 0;                                      //没收到任何数据
}

/***************************************************************************
 * 描  述 : 初始化NRF24L01到RX模式，配置相关参数，CE变高后,即进入RX模式
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
void Set_RxMode_MB(void)
{
	CE_MB_LOW;	  
  //写RX节点地址
  NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_MB,RX_ADDR_WIDTH);

  //使能通道0的自动应答    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);    
  //使能通道0的接收地址  	 
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
  //设置RF通信频率		  
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_CH,55);	     
  //选择通道0的有效数据宽度 	    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);
  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX接收模式 
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+CONFIG, 0x0f); 
  //CE为高,进入接收模式 
  CE_MB_HIGH;                                
}			

/***************************************************************************
 * 描  述 : 初始化NRF24L01到TX模式，配置相关参数
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
void Set_TxMode_MB(void)
{														 
	CE_MB_LOW;	    
  //写TX节点地址 
  NRF24L01_Write_Buf(SPI_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS_MB,TX_ADDR_WIDTH);    
  //设置TX节点地址,主要为了使能ACK	  
  NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_MB,RX_ADDR_WIDTH); 

  //使能通道0的自动应答    
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);     
  //使能通道0的接收地址  
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01); 
  //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+SETUP_RETR,0x21);
  //设置RF通道为55
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_CH,55);       
  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);  
  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX发送模式,开启所有中断
  NRF24L01_MB_Write_Reg(SPI_WRITE_REG+CONFIG,0x0e);                                  
}		  
/*********************************END FILE************************************/







