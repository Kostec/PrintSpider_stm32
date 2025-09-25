/**
  ******************************************************************************
  * @file    usbd_printer_if.c
  * @author  MCD Application Team
  * @brief   Generic media access Layer.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_printer_if.h"
#include "LOG/LOG.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_PRNT
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_PRNT_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_PRNT_Private_Defines
  * @{
  */

  extern USBD_HandleTypeDef hUsbDeviceFS;
  
/**
  * @}
  */


/** @defgroup USBD_PRNT_Private_Macros
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_PRNT_Private_FunctionPrototypes
  * @{
  */

static int8_t PRNT_Init(void);
static int8_t PRNT_DeInit(void);
static int8_t PRNT_Control_req(uint8_t req, uint8_t *pbuf, uint16_t *length);
static int8_t PRNT_Receive(uint8_t *pbuf, uint32_t *Len);

/*printer Private function prototypes*/
void PRNT_PageEndManager(uint8_t *Buf, uint32_t Len);

USBD_PRNT_ItfTypeDef USBD_PRNT_fops_FS =
{
  PRNT_Init,
  PRNT_DeInit,
  PRNT_Control_req,
  PRNT_Receive
};

static char PRNT_DEVICE_ID[] = "MFG:STM32;MDL:DIY_Printer;CMD:RAW;CLS:PRINTER;";

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  PRNT_Init
  *         Initializes the PRNT media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t PRNT_Init(void)
{
  /*
     Add your initialization code here
  */
  return (USBD_OK);
}

/**
  * @brief  PRNT_DeInit
  *         DeInitializes the PRNT media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t PRNT_DeInit(void)
{
  /*
     Add your deinitialization code here
  */
  return (USBD_OK);
}


/**
  * @brief  PRNT_Receive
  *         Data received over USB OUT endpoint are sent over PRNT interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on PRNT interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t PRNT_Receive(uint8_t *Buf, uint32_t *Len)
{
  char _buf[64] = {0};
  for (uint32_t i = 0; i < *Len; i++) {
    _buf[i] = Buf[i];
  }
  _buf[*Len] = '\r';
  _buf[*Len + 1] = '\n';
  LOG_Debug("%s", _buf);

  return (USBD_OK);
}


/**
  * @brief  PRNT_Control_req
  *         Manage the  PRNT class requests
  * @param  req: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t PRNT_Control_req(uint8_t req, uint8_t *pbuf, uint16_t *length)
{
  /* Check on the setup request value */
  switch (req)
  {
    /* Get Printer Device ID request */
    case PRNT_GET_DEVICE_ID:
      /* Not using for loop here due to MISRA-C2012-Rule-16.1 */
      pbuf[0] = (sizeof(PRNT_DEVICE_ID) - 1) >> 8;
      pbuf[1] = (sizeof(PRNT_DEVICE_ID) - 1) & 0xFF;
      memcpy(&pbuf[2], PRNT_DEVICE_ID, sizeof(PRNT_DEVICE_ID) - 1);
      *length = sizeof(PRNT_DEVICE_ID) + 2;
      break;

    /* Get Printer current status */
    case PRNT_GET_PORT_STATUS:
      pbuf[0] =  PRNT_STATUS_PAPER_EMPTY |
                 PRNT_STATUS_SELECTED |
                 PRNT_STATUS_NO_ERROR;

      *length = 1U;
      break;

    /* Printer SOFT RESET request: cleanup pending tasks */
    case PRNT_SOFT_RESET:
      // сбросить приёмные буферы
      USBD_PRNT_HandleTypeDef *hPRNT = (USBD_PRNT_HandleTypeDef *)hUsbDeviceFS.pClassDataCmsit[hUsbDeviceFS.classId];
      USBD_LL_PrepareReceive(&hUsbDeviceFS, PRNT_OUT_EP, hPRNT->RxBuffer, PRNT_DATA_FS_OUT_PACKET_SIZE);
      *length = 0;
      break;

    default:
      /* Unknown commands are not managed */
      break;
  }

  return (USBD_OK);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

