/**
  ******************************************************************************
  * @file    usbd_msc.c
  * @author  MCD Application Team
  * @brief   This file provides all the PRINTER core functions.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  * @verbatim
  *
  *          ===================================================================
  *                                PRINTER Class  Description
  *          ===================================================================
  *           This module manages the PRINTER class V1.0 following the "Universal
  *           Serial Bus Mass Storage Class (PRINTER) Bulk-Only Transport (BOT) Version 1.0
  *           Sep. 31, 1999".
  *           This driver implements the following aspects of the specification:
  *             - Bulk-Only Transport protocol
  *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
  *
  *  @endverbatim
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
- "stm32xxxxx_{eval}{discovery}{adafruit}_sd.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup PRINTER_CORE
  * @brief Mass storage core module
  * @{
  */

/** @defgroup PRINTER_CORE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup PRINTER_CORE_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup PRINTER_CORE_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup PRINTER_CORE_Private_FunctionPrototypes
  * @{
  */
uint8_t USBD_PRINTER_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_PRINTER_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t USBD_PRINTER_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
uint8_t USBD_PRINTER_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t USBD_PRINTER_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

#ifndef USE_USBD_COMPOSITE
uint8_t *USBD_PRINTER_GetHSCfgDesc(uint16_t *length);
uint8_t *USBD_PRINTER_GetFSCfgDesc(uint16_t *length);
uint8_t *USBD_PRINTER_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_PRINTER_GetDeviceQualifierDescriptor(uint16_t *length);
#endif /* USE_USBD_COMPOSITE */
/**
  * @}
  */


/** @defgroup PRINTER_CORE_Private_Variables
  * @{
  */


USBD_ClassTypeDef  USBD_PRINTER =
{
  USBD_PRINTER_Init,
  USBD_PRINTER_DeInit,
  USBD_PRINTER_Setup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  USBD_PRINTER_DataIn,
  USBD_PRINTER_DataOut,
  NULL, /*SOF */
  NULL,
  NULL,
#ifdef USE_USBD_COMPOSITE
  NULL,
  NULL,
  NULL,
  NULL,
#else
  USBD_PRINTER_GetHSCfgDesc,
  USBD_PRINTER_GetFSCfgDesc,
  USBD_PRINTER_GetOtherSpeedCfgDesc,
  USBD_PRINTER_GetDeviceQualifierDescriptor,
#endif /* USE_USBD_COMPOSITE */
};

/* USB Mass storage device Configuration Descriptor */
#ifndef USE_USBD_COMPOSITE
/* USB Mass storage device Configuration Descriptor */
/* All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN static uint8_t USBD_PRINTER_CfgDesc[USB_PRINTER_CONFIG_DESC_SIZ]  __ALIGN_END =
{
  0x09,                                            /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,                     /* bDescriptorType: Configuration */
  USB_PRINTER_CONFIG_DESC_SIZ,

  0x00,
  0x01,                                            /* bNumInterfaces: 1 interface */
  0x01,                                            /* bConfigurationValue */
  0x04,                                            /* iConfiguration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                            /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                            /* bmAttributes: Bus Powered according to user configuration */
#endif /* USBD_SELF_POWERED */
  USBD_MAX_POWER,                                  /* MaxPower (mA) */

  /********************  Mass Storage interface ********************/
  0x09,                                            /* bLength: Interface Descriptor size */
  0x04,                                            /* bDescriptorType: */
  0x00,                                            /* bInterfaceNumber: Number of Interface */
  0x00,                                            /* bAlternateSetting: Alternate setting */
  0x02,                                            /* bNumEndpoints */
  0x07,                                            /* bInterfaceClass: PRINTER Class */
  0x01,                                            /* bInterfaceSubClass : PRINTER */
  0x02,                                            /* nInterfaceProtocol : Bi-directional */
  0x05,                                            /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,                                            /* Endpoint descriptor length = 7 */
  0x05,                                            /* Endpoint descriptor type */
  PRINTER_EPIN_ADDR,                                   /* Endpoint address (IN, address 1) */
  0x02,                                            /* Bulk endpoint type */
  LOBYTE(PRINTER_MAX_FS_PACKET),
  HIBYTE(PRINTER_MAX_FS_PACKET),
  0x00,                                            /* Polling interval in milliseconds */

  0x07,                                            /* Endpoint descriptor length = 7 */
  0x05,                                            /* Endpoint descriptor type */
  PRINTER_EPOUT_ADDR,                                  /* Endpoint address (OUT, address 1) */
  0x02,                                            /* Bulk endpoint type */
  LOBYTE(PRINTER_MAX_FS_PACKET),
  HIBYTE(PRINTER_MAX_FS_PACKET),
  0x00                                             /* Polling interval in milliseconds */
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_PRINTER_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]  __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  PRINTER_MAX_FS_PACKET,
  0x01,
  0x00,
};
#endif /* USE_USBD_COMPOSITE */

uint8_t PRINTERInEpAdd  = PRINTER_EPIN_ADDR;
uint8_t PRINTEROutEpAdd = PRINTER_EPOUT_ADDR;

/**
  * @}
  */


/** @defgroup PRINTER_CORE_Private_Functions
  * @{
  */

/**
  * @brief  USBD_PRINTER_Init
  *         Initialize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t USBD_PRINTER_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_PRINTER_BOT_HandleTypeDef *hmsc;

  hmsc = (USBD_PRINTER_BOT_HandleTypeDef *)USBD_malloc(sizeof(USBD_PRINTER_BOT_HandleTypeDef));

  if (hmsc == NULL)
  {
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassDataCmsit[pdev->classId] = (void *)hmsc;
  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  PRINTERInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  PRINTEROutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, PRINTEROutEpAdd, USBD_EP_TYPE_BULK, PRINTER_MAX_HS_PACKET);
    pdev->ep_out[PRINTEROutEpAdd & 0xFU].is_used = 1U;

    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, PRINTERInEpAdd, USBD_EP_TYPE_BULK, PRINTER_MAX_HS_PACKET);
    pdev->ep_in[PRINTERInEpAdd & 0xFU].is_used = 1U;
  }
  else
  {
    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, PRINTEROutEpAdd, USBD_EP_TYPE_BULK, PRINTER_MAX_FS_PACKET);
    pdev->ep_out[PRINTEROutEpAdd & 0xFU].is_used = 1U;

    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, PRINTERInEpAdd, USBD_EP_TYPE_BULK, PRINTER_MAX_FS_PACKET);
    pdev->ep_in[PRINTERInEpAdd & 0xFU].is_used = 1U;
  }

  /* Init the BOT  layer */
  PRINTER_BOT_Init(pdev);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_PRINTER_DeInit
  *         DeInitialize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t USBD_PRINTER_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  PRINTERInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  PRINTEROutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Close PRINTER EPs */
  (void)USBD_LL_CloseEP(pdev, PRINTEROutEpAdd);
  pdev->ep_out[PRINTEROutEpAdd & 0xFU].is_used = 0U;

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, PRINTERInEpAdd);
  pdev->ep_in[PRINTERInEpAdd & 0xFU].is_used = 0U;

  /* Free PRINTER Class Resources */
  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
  {
    /* De-Init the BOT layer */
    PRINTER_BOT_DeInit(pdev);

    (void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
    pdev->pClassDataCmsit[pdev->classId]  = NULL;
    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_PRINTER_Setup
  *         Handle the PRINTER specific requests
  * @param  pdev: device instance
  * @param  req: USB request
  * @retval status
  */
uint8_t USBD_PRINTER_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_PRINTER_BOT_HandleTypeDef *hmsc = (USBD_PRINTER_BOT_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
  USBD_StatusTypeDef ret = USBD_OK;
  uint32_t max_lun;
  uint16_t status_info = 0U;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  PRINTERInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  PRINTEROutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (hmsc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    /* Class request */
    case USB_REQ_TYPE_CLASS:
      switch (req->bRequest)
      {
        case BOT_GET_MAX_LUN:
          if ((req->wValue  == 0U) && (req->wLength == 1U) &&
              ((req->bmRequest & 0x80U) == 0x80U))
          {
            max_lun = (uint32_t)((USBD_StorageTypeDef *)pdev->pUserData[pdev->classId])->GetMaxLun();
            hmsc->max_lun = (max_lun > PRINTER_BOT_MAX_LUN) ? PRINTER_BOT_MAX_LUN : max_lun;
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hmsc->max_lun, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case BOT_RESET :
          if ((req->wValue  == 0U) && (req->wLength == 0U) &&
              ((req->bmRequest & 0x80U) != 0x80U))
          {
            PRINTER_BOT_Reset(pdev);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;
    /* Interface & Endpoint request */
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hmsc->interface, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hmsc->interface = (uint8_t)(req->wValue);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            if (req->wValue == USB_FEATURE_EP_HALT)
            {
              /* Flush the FIFO */
              (void)USBD_LL_FlushEP(pdev, (uint8_t)req->wIndex);

              /* Handle BOT error */
              PRINTER_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
            }
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return (uint8_t)ret;
}

/**
  * @brief  USBD_PRINTER_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
uint8_t USBD_PRINTER_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  PRINTER_BOT_DataIn(pdev, epnum);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_PRINTER_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
uint8_t USBD_PRINTER_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  PRINTER_BOT_DataOut(pdev, epnum);

  return (uint8_t)USBD_OK;
}
#ifndef USE_USBD_COMPOSITE
/**
  * @brief  USBD_PRINTER_GetHSCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t *USBD_PRINTER_GetHSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_PRINTER_CfgDesc, PRINTER_EPIN_ADDR);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_PRINTER_CfgDesc, PRINTER_EPOUT_ADDR);

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = PRINTER_MAX_HS_PACKET;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = PRINTER_MAX_HS_PACKET;
  }

  *length = (uint16_t)sizeof(USBD_PRINTER_CfgDesc);
  return USBD_PRINTER_CfgDesc;
}

/**
  * @brief  USBD_PRINTER_GetFSCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t *USBD_PRINTER_GetFSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_PRINTER_CfgDesc, PRINTER_EPIN_ADDR);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_PRINTER_CfgDesc, PRINTER_EPOUT_ADDR);

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = PRINTER_MAX_FS_PACKET;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = PRINTER_MAX_FS_PACKET;
  }

  *length = (uint16_t)sizeof(USBD_PRINTER_CfgDesc);
  return USBD_PRINTER_CfgDesc;
}

/**
  * @brief  USBD_PRINTER_GetOtherSpeedCfgDesc
  *         return other speed configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t *USBD_PRINTER_GetOtherSpeedCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_PRINTER_CfgDesc, PRINTER_EPIN_ADDR);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_PRINTER_CfgDesc, PRINTER_EPOUT_ADDR);

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = PRINTER_MAX_FS_PACKET;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = PRINTER_MAX_FS_PACKET;
  }

  *length = (uint16_t)sizeof(USBD_PRINTER_CfgDesc);
  return USBD_PRINTER_CfgDesc;
}
/**
  * @brief  DeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t *USBD_PRINTER_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_PRINTER_DeviceQualifierDesc);

  return USBD_PRINTER_DeviceQualifierDesc;
}
#endif /* USE_USBD_COMPOSITE */
/**
  * @brief  USBD_PRINTER_RegisterStorage
  * @param  fops: storage callback
  * @retval status
  */
uint8_t USBD_PRINTER_RegisterStorage(USBD_HandleTypeDef *pdev, USBD_StorageTypeDef *fops)
{
  if (fops == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  pdev->pUserData[pdev->classId] = fops;

  return (uint8_t)USBD_OK;
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

