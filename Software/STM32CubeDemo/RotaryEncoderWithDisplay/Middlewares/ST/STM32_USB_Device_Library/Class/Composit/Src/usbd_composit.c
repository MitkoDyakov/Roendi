/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc.h"
#include "usbd_customhid.h"
#include "usbd_composit.h"
#include "usbd_custom_hid_if.h"
#include "usbd_cdc_if.h"


void *pClassDataHID = NULL;
void *pClassDataCDC = NULL;

#define USBD_CDC_CMD_INTERFACE    (0u)
#define USBD_CDC_DATA_INTERFACE   (1u)
#define USBD_HID_INTERFACE        (2u)

#define HID_INDATA_NUM             (CUSTOM_HID_EPIN_ADDR & 0x0F)
#define HID_OUTDATA_NUM            (CUSTOM_HID_EPOUT_ADDR & 0x0F)
 
#define CDC_INDATA_NUM             (CDC_IN_EP & 0x0F)
#define CDC_OUTDATA_NUM            (CDC_OUT_EP & 0x0F)
#define CDC_OUTCMD_NUM             (CDC_CMD_EP & 0x0F)

static uint8_t  USBD_COMPOSIT_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);

static uint8_t  USBD_COMPOSIT_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);

static uint8_t  USBD_COMPOSIT_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t  USBD_COMPOSIT_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_COMPOSIT_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_COMPOSIT_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t  *USBD_COMPOSIT_GetFSCfgDesc(uint16_t *length);

static uint8_t  *USBD_COMPOSIT_GetHSCfgDesc(uint16_t *length);

static uint8_t  *USBD_COMPOSIT_GetOtherSpeedCfgDesc(uint16_t *length);

static uint8_t  *USBD_COMPOSIT_GetOtherSpeedCfgDesc(uint16_t *length);

uint8_t         *USBD_COMPOSIT_GetDeviceQualifierDescriptor(uint16_t *length);

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_COMPOSIT_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/* CDC interface class callbacks structure */
USBD_ClassTypeDef  USBD_COMPOSIT =
{
  USBD_COMPOSIT_Init,
  USBD_COMPOSIT_DeInit,
  USBD_COMPOSIT_Setup,
  NULL,                 /* EP0_TxSent, */
  USBD_COMPOSIT_EP0_RxReady,
  USBD_COMPOSIT_DataIn,
  USBD_COMPOSIT_DataOut,
  NULL,
  NULL,
  NULL,
  USBD_COMPOSIT_GetHSCfgDesc,
  USBD_COMPOSIT_GetFSCfgDesc,
  USBD_COMPOSIT_GetOtherSpeedCfgDesc,
  USBD_COMPOSIT_GetDeviceQualifierDescriptor,
};

/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_COMPOSIT_CfgHSDesc[USB_COMPOSIT_CONFIG_DESC_SIZE] __ALIGN_END =
{
  /*Configuration Descriptor*/
  0x09,                             /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  USB_COMPOSIT_CONFIG_DESC_SIZE,     /* wTotalLength:no of returned bytes */
  0x00,
  0x03,   /* bNumInterfaces: 3 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */

    /******** IAD should be positioned just before the CDC interfaces ******
                                  IAD to associate the two CDC interfaces */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x00, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x00, /* iFunction (Index of string descriptor describing this function) */
    /* 08 bytes */
          
    /*---------------------------------------------------------------------------*/

  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */

  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_HS_BINTERVAL,                           /* bInterval: */
  /*---------------------------------------------------------------------------*/

  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                               /* bInterval: ignore for Bulk transfer */
    
  
/*********************************IAD Descriptor*********************************/
  0x08, //Descriptor size
  0x0B, //IAD descriptor type
  0x02,                   //bFirstInterface
  0x01,                   //bInferfaceCount
  0x03,                   //bFunctionClass:CUSTOM_HID
  0x00,                   //bFunctionSubClass
  0x00,                   //bFunctionProtocol
  0x05,                   //iFunction

  
/************** Descriptor of CUSTOM HID interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x02,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: CUSTOM_HID*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of CUSTOM_HID *************************/
  /* 18 */
  0x09,         /*bLength: CUSTOM_HID Descriptor size*/
  CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
  0x11,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  USBD_CUSTOM_HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

  CUSTOM_HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,                 /*bmAttributes: Interrupt endpoint*/
  CUSTOM_HID_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
  0x00,
  CUSTOM_HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
  /* 34 */

  0x07,                     /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: */
  CUSTOM_HID_EPOUT_ADDR,    /*bEndpointAddress: Endpoint Address (OUT)*/
  0x03,                     /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPOUT_SIZE,    /* wMaxPacketSize: 2 Bytes max  */
  0x00,
  CUSTOM_HID_FS_BINTERVAL  /* bInterval: Polling Interval */
  /* 41 */
} ;

/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_COMPOSIT_CfgFSDesc[USB_COMPOSIT_CONFIG_DESC_SIZE] __ALIGN_END =
{
    /*Configuration Descriptor*/
  0x09,                             /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  USB_COMPOSIT_CONFIG_DESC_SIZE,     /* wTotalLength:no of returned bytes */
  0x00,
  0x03,   /* bNumInterfaces: 3 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */

    /******** IAD should be positioned just before the CDC interfaces ******
                                  IAD to associate the two CDC interfaces */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x00, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x00, /* iFunction (Index of string descriptor describing this function) */
    /* 08 bytes */
          
    /*---------------------------------------------------------------------------*/

  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */

  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_HS_BINTERVAL,                           /* bInterval: */
  /*---------------------------------------------------------------------------*/

  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                               /* bInterval: ignore for Bulk transfer */
    
  
/*********************************IAD Descriptor*********************************/
  0x08, //Descriptor size
  0x0B, //IAD descriptor type
  0x02,                   //bFirstInterface
  0x01,                   //bInferfaceCount
  0x03,                   //bFunctionClass:CUSTOM_HID
  0x00,                   //bFunctionSubClass
  0x00,                   //bFunctionProtocol
  0x05,                   //iFunction

  
/************** Descriptor of CUSTOM HID interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x02,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: CUSTOM_HID*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of CUSTOM_HID *************************/
  /* 18 */
  0x09,         /*bLength: CUSTOM_HID Descriptor size*/
  CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
  0x11,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  USBD_CUSTOM_HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

  CUSTOM_HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,                 /*bmAttributes: Interrupt endpoint*/
  CUSTOM_HID_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
  0x00,
  CUSTOM_HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
  /* 34 */

  0x07,                     /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: */
  CUSTOM_HID_EPOUT_ADDR,    /*bEndpointAddress: Endpoint Address (OUT)*/
  0x03,                     /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPOUT_SIZE,    /* wMaxPacketSize: 2 Bytes max  */
  0x00,
  CUSTOM_HID_FS_BINTERVAL  /* bInterval: Polling Interval */
  /* 41 */
} ;

__ALIGN_BEGIN uint8_t USBD_COMPOSIT_OtherSpeedCfgDesc[USB_COMPOSIT_CONFIG_DESC_SIZE] __ALIGN_END =
{
    /*Configuration Descriptor*/
  0x09,                             /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  USB_COMPOSIT_CONFIG_DESC_SIZE,     /* wTotalLength:no of returned bytes */
  0x00,
  0x03,   /* bNumInterfaces: 3 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */

    /******** IAD should be positioned just before the CDC interfaces ******
                                  IAD to associate the two CDC interfaces */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x00, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x00, /* iFunction (Index of string descriptor describing this function) */
    /* 08 bytes */
          
    /*---------------------------------------------------------------------------*/

  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */

  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_HS_BINTERVAL,                           /* bInterval: */
  /*---------------------------------------------------------------------------*/

  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                               /* bInterval: ignore for Bulk transfer */
    
  
/*********************************IAD Descriptor*********************************/
  0x08, //Descriptor size
  0x0B, //IAD descriptor type
  0x02,                   //bFirstInterface
  0x01,                   //bInferfaceCount
  0x03,                   //bFunctionClass:CUSTOM_HID
  0x00,                   //bFunctionSubClass
  0x00,                   //bFunctionProtocol
  0x05,                   //iFunction

  
/************** Descriptor of CUSTOM HID interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x02,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: CUSTOM_HID*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of CUSTOM_HID *************************/
  /* 18 */
  0x09,         /*bLength: CUSTOM_HID Descriptor size*/
  CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
  0x11,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  USBD_CUSTOM_HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

  CUSTOM_HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,                 /*bmAttributes: Interrupt endpoint*/
  CUSTOM_HID_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
  0x00,
  CUSTOM_HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
  /* 34 */

  0x07,                     /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: */
  CUSTOM_HID_EPOUT_ADDR,    /*bEndpointAddress: Endpoint Address (OUT)*/
  0x03,                     /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPOUT_SIZE,    /* wMaxPacketSize: 2 Bytes max  */
  0x00,
  CUSTOM_HID_FS_BINTERVAL  /* bInterval: Polling Interval */
  /* 41 */
};

/**
  * @brief  USBD_COMPOSIT_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_COMPOSIT_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = 0U;

  pdev->pUserData = (void *)&USBD_Interface_fops_FS;
  ret = USBD_CDC.Init(pdev, cfgidx);
  pClassDataCDC = pdev->pClassData;
  
  if(USBD_OK != ret)
  {
      return ret;
  }
  
  pdev->pUserData = (void *)&USBD_CustomHID_fops_FS;
  ret = USBD_CUSTOM_HID.Init(pdev, cfgidx);
  pClassDataHID = pdev->pClassData;
  
  if(USBD_OK != ret)
  {
      return ret;
  }
  
  return ret;
}

/**
  * @brief  USBD_COMPOSIT_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_COMPOSIT_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = 0U;
  
  USBD_CDC.DeInit(pdev, cfgidx);
  USBD_CUSTOM_HID.DeInit(pdev, cfgidx);
  
  return ret;
}

/**
  * @brief  USBD_COMPOSIT_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_COMPOSIT_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    uint8_t ret = USBD_OK;

    switch (req->bmRequest & USB_REQ_RECIPIENT_MASK)
    {
     case USB_REQ_RECIPIENT_INTERFACE:
       switch(req->wIndex)
        {
           case USBD_CDC_DATA_INTERFACE:
           case USBD_CDC_CMD_INTERFACE:
                pdev->pUserData = (void *)&USBD_Interface_fops_FS;
                return(USBD_CDC.Setup(pdev, req));

           case USBD_HID_INTERFACE:
                pdev->pUserData = (void *)&USBD_CustomHID_fops_FS;
                return(USBD_CUSTOM_HID.Setup (pdev, req));

           default:
              break;
       }
       break;
       
     case USB_REQ_RECIPIENT_ENDPOINT:
       switch(req->wIndex)
       {
           case CDC_IN_EP:
           case CDC_OUT_EP:
           case CDC_CMD_EP:
                pdev->pUserData = (void *)&USBD_Interface_fops_FS;
                return(USBD_CDC.Setup(pdev, req));

           case CUSTOM_HID_EPIN_ADDR:
           case CUSTOM_HID_EPOUT_ADDR:
                pdev->pUserData = (void *)&USBD_CustomHID_fops_FS;
                return(USBD_CUSTOM_HID.Setup (pdev, req));

           default:
              break;
       }
       break;
    }
    
    return ret;
}

/**
  * @brief  USBD_COMPOSIT_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_COMPOSIT_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  switch(epnum)
  {
      case CDC_INDATA_NUM:
          pdev->pUserData = (void *)&USBD_Interface_fops_FS;
          return(USBD_CDC.DataIn(pdev,epnum));
 
      case HID_INDATA_NUM:
          pdev->pUserData = (void *)&USBD_CustomHID_fops_FS;
          return(USBD_CUSTOM_HID.DataIn(pdev,epnum));
 
      default:
         break;
 
  }
  
  return USBD_FAIL; 
}

/**
  * @brief  USBD_COMPOSIT_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_COMPOSIT_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  switch(epnum)
  {
      case CDC_OUTDATA_NUM:
      case CDC_OUTCMD_NUM:
          pdev->pUserData = (void *)&USBD_Interface_fops_FS;
          return(USBD_CDC.DataOut(pdev,epnum));
 
      case HID_OUTDATA_NUM:
          pdev->pUserData = (void *)&USBD_CustomHID_fops_FS;
          return(USBD_CUSTOM_HID.DataOut(pdev,epnum));
      default:
         break;
 
  }
  
  return USBD_FAIL;
}

/**
  * @brief  USBD_COMPOSIT_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_COMPOSIT_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
    pdev->pUserData = (void *)&USBD_Interface_fops_FS;
    
        USBD_CDC.EP0_RxReady(pdev);
    
    
    pdev->pUserData = (void *)&USBD_CustomHID_fops_FS;
    
      USBD_CUSTOM_HID.EP0_RxReady(pdev);
    
    
    return USBD_OK;
}

/**
  * @brief  USBD_COMPOSIT_GetFSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_COMPOSIT_GetFSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_COMPOSIT_CfgFSDesc);
  return USBD_COMPOSIT_CfgFSDesc;
}

/**
  * @brief  USBD_COMPOSIT_GetHSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_COMPOSIT_GetHSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_COMPOSIT_CfgHSDesc);
  return USBD_COMPOSIT_CfgHSDesc;
}

/**
  * @brief  USBD_COMPOSIT_GetCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_COMPOSIT_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_COMPOSIT_OtherSpeedCfgDesc);
  return USBD_COMPOSIT_OtherSpeedCfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_COMPOSIT_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = sizeof(USBD_COMPOSIT_DeviceQualifierDesc);
  return USBD_COMPOSIT_DeviceQualifierDesc;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
