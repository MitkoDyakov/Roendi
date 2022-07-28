/*
 * CST816S.h
 *
 *  Created on: Jun 21, 2022
 *      Author: Dimitar
 */

#ifndef INC_CST816S_H_
#define INC_CST816S_H_

#define CST816S_ADDRESS            (0x15<<1)

#define CST816S_GESTURE_ID         (0x01u) // none         = 0x00,
                                           // swipe up     = 0x01,
                                           // swipe down   = 0x02,
                                           // swipe left   = 0x03,
                                           // swipe right  = 0x04,
                                           // single click = 0x05,
                                           // double click = 0x0B,
                                           // long press   = 0x0C

#define CST816S_FINGER_NUM         (0x02u) // The number of fingers. 0: No; 1 finger: a finger;

#define CST816S_X_POS_H            (0x03u)
#define CST816S_X_POS_L            (0x04u)

#define CST816S_Y_POS_H            (0x05u)
#define CST816S_Y_POS_L            (0x06u)

#define CST816S_BPC0_H             (0xB0u)
#define CST816S_BPC0_L             (0xB1u)

#define CST816S_BPC1_H             (0xB2u)
#define CST816S_BPC1_L             (0xB3u)

#define CST816S_CHIP_ID            (0xA7u)  // 0xDH

#define CST816S_PROJ_ID            (0xA8u)

#define CST816S_SW_VERSION         (0xA9u)  // Software version

#define CST816S_MOTION_MASK        (0xECu)  // [2] EnConLR Continuous operation can slide around
                                            // [1] EnConUD Slide up and down to enable continuous operation
                                            // [0] EnDClick Enable Double-click action

#define CST816S_IRQ_PLUSE_WIDTH    (0xEDu)  // Unit 0.1ms, optional values: 1-200. The default value is 10.

#define CST816S_NOR_SCAN_PER       (0xEEu)  // Rapid detection of the normal cycle.

#define CST816S_MOTION_SL_ANGLE    (0xEFu)  // Sliding partition gesture detection angle control. Angle = tan (c) * 10 c is a positive x-axis direction as a reference angle.

#define CST816S_LP_SCAN_RAW_1_H    (0xF0u)  // Scanning the reference channel 1 low power 8-bit value is high.
#define CST816S_LP_SCAN_RAW_1_L    (0xF1u)  // The lower 8 bits of the reference value of the low-power scanning No. 1 channel.

#define CST816S_LP_SCAN_RAW_2_H    (0xF2u)  // Scanning the reference channel 1 low power 8-bit value is high.
#define CST816S_LP_SCAN_RAW_2_L    (0xF3u)  // The lower 8 bits of the reference value of the low-power scanning No. 2 channel.

#define CST816S_LP_SCAN_RAW_2_L    (0xF3u)  // The lower 8 bits of the reference value of the low-power scanning No. 2 channel.

#define CST816S_LP_AUTO_WAKE_TIME  (0xF4u)  // Auto-calibration cycle LpAutoWakeTime low power consumption. Unit 1 minute optional value: 1-5. The default is 5.

#define CST816S_LP_SCAN_TH         (0xF5u)  // Low-power scanning wake threshold. The smaller the more sensitive. Optional Value: 1 to 255. The default value is 48.

#define CST816S_LP_SCAN_WIN        (0xF6u)  // Low-power scanning range. The greater the sensitivity, the higher the power consumption. Optional value: 0,1,2,3. The default is 3.

#define CST816S_LP_SCAN_FREQ       (0xF7u)  // Low-power scanning frequency. The smaller the more sensitive. Optional Value: 1 to 255. The default value is 7.

#define CST816S_LP_SCAN_IDAC       (0xF8u)  // Low power scan current. The smaller the more sensitive. Optional Value: 1 to 255.

#define CST816S_AUTO_SLEEP_TIME    (0xF9u)  // When no touch, into low power mode automatically x seconds. Unit 1S, default is 2S.

#define CST816S_IRQ_CTL            (0xFAu)  // [7] EnTest Interrupt pin to test, enable automatic periodic issued after a low pulse.
                                            // [6] EnTouch When a touch is detected, a periodic pulsed Low.
                                            // [5] EnChange Upon detecting a touch state changes, pulsed Low.
                                            // [4] EnMotion When the detected gesture is pulsed Low.
                                            // [0] OnceWLP Press gesture only issue a pulse signal is low.

#define CST816S_AUTO_RESET         (0xFBu)  // X seconds but no valid touch gesture, automatically reset. Unit 1S, do not enable this function is zero. The default is 5.

#define CST816S_LONG_PRESS_TIME    (0xFCu)  // Press automatically reset after x seconds. Unit 1S, do not enable this function is zero. The default is 10.

#define CST816S_IO_CTL             (0xFDu)  // [2] SOFT_RST Master reset by pulling the soft touch IRQ pin implemented. 0: Disable soft reset. 1: Enable soft reset.
                                            // [1] IIC_OD IIC pin drive mode, the default pull-up resistor. 0: pull-up resistor 1: OD
                                            // [0] En1v8 IIC and IRQ pin level selected, the default is the VDD level. 0: VDD 1: 1.8V

#define CST816S_DIS_AUTO_SLEEP     (0xFEu)  // The default is 0, automatically enabled into low power mode. Is a non-zero value, to disable automatic into low power mode.

enum GESTURE
{
    NONE         = 0x00,
    SWIPE_UP     = 0x01,
    SWIPE_DOWN   = 0x02,
    SWIPE_LEFT   = 0x03,
    SWIPE_RIGHT  = 0x04,
    SINGLE_CLICK = 0x05,
    DOUBLE_CLICK = 0x0B,
    LONG_PRESS   = 0x0C
};

typedef struct data_struct
{
    uint8_t gestureID; // Gesture ID
    uint8_t points;    // Number of touch points
    uint16_t x;
    uint16_t y;
    uint8_t hwInfo[3]; // should look like [0xB4 0x00 0x01]
}data_struct;

void read_touch(void);
void touchPanelWakeup(void);
void touchPanelSleep(void);
void touchPanelInit(void);
void touchPanelWork(void);

uint8_t touchPanelWrite(uint8_t devAddress, uint8_t regAddress, uint8_t *data, uint32_t len);
uint8_t touchPanelRead(uint8_t devAddress, uint8_t regAddress, uint8_t *data, uint32_t len);

#endif /* INC_CST816S_H_ */
