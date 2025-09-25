/**
  ******************************************************************************
  * @file    usbd_printer_if_template.c
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

static int8_t TEMPLATE_Init(void);
static int8_t TEMPLATE_DeInit(void);
static int8_t TEMPLATE_Control_req(uint8_t req, uint8_t *pbuf, uint16_t *length);
static int8_t TEMPLATE_Receive(uint8_t *pbuf, uint32_t *Len);

/*printer Private function prototypes*/
void TEMPLATE_PRNT_PageEndManager(uint8_t *Buf, uint32_t Len);

USBD_PRNT_ItfTypeDef USBD_Printer_fops_FS =
{
  TEMPLATE_Init,
  TEMPLATE_DeInit,
  TEMPLATE_Control_req,
  TEMPLATE_Receive
};

static char PRNT_DEVICE_ID[] = "MFG:STM32:MDL:DIY_Printer;CMD:RAW";

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  TEMPLATE_Init
  *         Initializes the PRNT media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_Init(void)
{
  /*
     Add your initialization code here
  */
  return (USBD_OK);
}

/**
  * @brief  TEMPLATE_DeInit
  *         DeInitializes the PRNT media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_DeInit(void)
{
  /*
     Add your deinitialization code here
  */
  return (USBD_OK);
}


/**
  * @brief  TEMPLATE_Receive
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
static int8_t TEMPLATE_Receive(uint8_t *Buf, uint32_t *Len)
{
  UNUSED(Buf);
  UNUSED(Len);

  return (USBD_OK);
}


/**
  * @brief  TEMPLATE_Control_req
  *         Manage the  PRNT class requests
  * @param  req: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_Control_req(uint8_t req, uint8_t *pbuf, uint16_t *length)
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
      return sizeof(PRNT_DEVICE_ID) + 2;
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

