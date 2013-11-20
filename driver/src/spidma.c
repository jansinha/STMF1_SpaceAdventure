#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_dma.h>
#include <stdint.h>
#include "spidma.h"

static const uint16_t speeds [] = {
  [ SPI_SLOW ] = SPI_BaudRatePrescaler_64 ,
  [ SPI_MEDIUM ] = SPI_BaudRatePrescaler_8 ,
  [ SPI_FAST ] = SPI_BaudRatePrescaler_2 };


void spiInit( SPI_TypeDef *SPIx )
{
  SPI_InitTypeDef SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_StructInit( &GPIO_InitStructure );
  SPI_StructInit( &SPI_InitStructure );

  if (SPIx == SPI2) {
    // Enable clocks , configure pins for SPI
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; // SCK
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; // MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; // MOSI
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
  else if (SPIx == SPI1) {
    // Enable clocks , configure pins for SPI
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // SCK
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // MOSI
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
  else {
    return;
  }
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = speeds[ SPI_FAST ];
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init( SPIx, &SPI_InitStructure );
  SPI_Cmd( SPIx, ENABLE );

}


int spiReadWrite( SPI_TypeDef * SPIx, uint8_t *rbuf,
                  const uint8_t *tbuf, int cnt, enum spiSpeed speed)
{
  if (cnt < 4) {
    int i;
    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256 ) | speeds[speed];

    for (i = 0; i < cnt; i++){
      if (tbuf) {
	SPI_I2S_SendData(SPIx, *tbuf++);
      }
      else {
	SPI_I2S_SendData(SPIx, 0xff);
      }
      while( SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET );
      if(rbuf) {
	*rbuf++ = SPI_I2S_ReceiveData(SPIx);
      }
      else {
	SPI_I2S_ReceiveData(SPIx);
      }
    }
    return i;
  }
  else {
    return( xchng_datablock ( SPIx, 0, rbuf, tbuf, cnt) );
  }
}


int spiReadWrite16( SPI_TypeDef * SPIx, uint16_t *rbuf,
		    const uint16_t *tbuf, int cnt, enum spiSpeed speed)
{
  SPI_DataSizeConfig( SPIx, SPI_DataSize_16b );
  SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256 ) | speeds[speed];
  
  int res;

  if (cnt < 4) {
    int i;

    for (i = 0; i < cnt; i++){
      if (tbuf) {
	SPI_I2S_SendData(SPIx, *tbuf++);
      }
      else {
	SPI_I2S_SendData(SPIx, 0xffff);
      }
      while( SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET );
      if(rbuf) {
	*rbuf++ = SPI_I2S_ReceiveData(SPIx);
      }
      else {
	SPI_I2S_ReceiveData(SPIx);
      }
    }

    res = i;
  }
  else {
    res = xchng_datablock ( SPIx, 1, rbuf, tbuf, cnt);
  }

  SPI_DataSizeConfig( SPIx, SPI_DataSize_8b );
  
  return res;
}


int xchng_datablock ( SPI_TypeDef *SPIx, int half,
		      void *rbuf, const void *tbuf, unsigned count)
{
  if( !rbuf ) { // Transmit only
    return ( dmaTxBytes( SPIx, tbuf, count, half ) );
  }
  else if( !tbuf ) { // Read only
    return ( dmaRxBytes( SPIx, rbuf, count, half ) );
  }
  else // Exchange
    return ( dmaRxTxBytes( SPIx, tbuf, rbuf, count, half ) );
}


int dmaRxBytes( SPI_TypeDef *SPIx, void *rbuf, unsigned count, int half) 
{
  uint16_t dummy[] = {0xffff};
  DMA_InitTypeDef DMA_InitStructure;
  /* initialize */
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );

  if (SPIx == SPI1) {
    DMA_DeInit(DMA1_Channel2);
    DMA_DeInit(DMA1_Channel3);
  }
  else {
    DMA_DeInit(DMA1_Channel4);
    DMA_DeInit(DMA1_Channel5);
  }

  // Common to both channels
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPIx->DR));
  DMA_InitStructure.DMA_PeripheralDataSize = (half) ? DMA_PeripheralDataSize_HalfWord : DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = (half) ? DMA_MemoryDataSize_HalfWord : DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_BufferSize = count;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
 
  // Rx Channel
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) rbuf;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

  if (SPIx == SPI1) {
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  }
  else {
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  }
  
  // Tx channel
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) dummy;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

  if (SPIx == SPI1) {
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  }
  else {
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  }
  /* ... */

  // Enable channels
  if (SPIx == SPI1) {
    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);
  }
  else {
    DMA_Cmd(DMA1_Channel4, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
  }

  // Enable SPI TX/RX request
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

  // Wait for completion
  if (SPIx == SPI1) {
    while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);
  }
  else {
    while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);
  }

  // Disable channels
  if (SPIx == SPI1) {
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
  }
  else {
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
  }
  
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);

  return count;
}


int dmaTxBytes( SPI_TypeDef *SPIx, void *tbuf, unsigned count, int half)
{
  uint16_t dummy[] = {0xffff};
  DMA_InitTypeDef DMA_InitStructure;
  /* initialize */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  if (SPIx == SPI1) {
    DMA_DeInit(DMA1_Channel2);
    DMA_DeInit(DMA1_Channel3);
  }
  else {
    DMA_DeInit(DMA1_Channel4);
    DMA_DeInit(DMA1_Channel5);
  }
  
  // Common to both channels
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPIx->DR));
  DMA_InitStructure.DMA_PeripheralDataSize = (half) ? DMA_PeripheralDataSize_HalfWord : DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = (half) ? DMA_MemoryDataSize_HalfWord : DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_BufferSize = count;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
 
  // Rx Channel
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) dummy;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

  if (SPIx == SPI1) {
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  }
  else {
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  }

  // Tx channel
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) tbuf;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

  if (SPIx == SPI1) {
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  }
  else {
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  }
  /* ... */

  // Enable channels
  if (SPIx == SPI1) {
    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);
  }
  else {
    DMA_Cmd(DMA1_Channel4, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
  }

  // Enable SPI TX/RX request
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

  // Wait for completion                                                                                  
  if (SPIx == SPI1) {
    while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);
  }
  else {
    while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);
  }

  // Disable channels                                                                                     
  if (SPIx == SPI1) {
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
  }
  else {
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
  }
  
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);

  return count;
}


int dmaRxTxBytes( SPI_TypeDef *SPIx, void *tbuf, void *rbuf, unsigned count, int half)
{
  DMA_InitTypeDef DMA_InitStructure;
  /* initialize */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  if (SPIx == SPI1) {
    DMA_DeInit(DMA1_Channel2);
    DMA_DeInit(DMA1_Channel3);
  }
  else {
    DMA_DeInit(DMA1_Channel4);
    DMA_DeInit(DMA1_Channel5);
  }
  
  // Common to both channels
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPIx->DR));
  DMA_InitStructure.DMA_PeripheralDataSize = (half) ? DMA_PeripheralDataSize_HalfWord : DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = (half) ? DMA_MemoryDataSize_HalfWord : DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_BufferSize = count;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
 
  // Rx Channel
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) rbuf;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

  if (SPIx == SPI1)
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  else
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

  // Tx channel                                                                                            
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) tbuf;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

  if (SPIx == SPI1)
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  else
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  /* ... */

  // Enable channels
  if (SPIx == SPI1) {
    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel3, ENABLE);
  }
  else {
    DMA_Cmd(DMA1_Channel4, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
  }

  // Enable SPI TX/RX request
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

  // Wait for completion
  if (SPIx == SPI1)
    while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);
  else
    while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);

  // Disable channels 
  if (SPIx == SPI1) {
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
  }
  else {
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
  }
  
  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
  
  return count;
}
