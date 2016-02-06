#include "tsl_user.h"

// Channel IOs definition
#define CHANNEL_0_SRC              ((uint32_t)(GR2))
#define CHANNEL_0_DEST             (0)
#define CHANNEL_0_SAMPLE_CONFIG    TSL_GROUP2_IO2
#define CHANNEL_0_CHANNEL_CONFIG   TSL_GROUP2_IO1

#define CHANNEL_1_SRC              ((uint32_t)(GR9))
#define CHANNEL_1_DEST             (1)
#define CHANNEL_1_SAMPLE_CONFIG    TSL_GROUP9_IO2
#define CHANNEL_1_CHANNEL_CONFIG   TSL_GROUP9_IO1

#define CHANNEL_2_SRC              ((uint32_t)(GR3))
#define CHANNEL_2_DEST             (2)
#define CHANNEL_2_SAMPLE_CONFIG    TSL_GROUP3_IO2
#define CHANNEL_2_CHANNEL_CONFIG   TSL_GROUP3_IO1

// Banks definition
#define BANK_0_NBCHANNELS          (3)
#define BANK_0_INDEX               (0) // Index of 1st channel used
#define BANK_0_SHIELD_SAMPLE       (0)
#define BANK_0_SHIELD_CHANNEL      (0)

// User Parameters
extern TSL_ObjectGroup_T MyObjGroup;
extern CONST TSL_Object_T MyObjects[];
extern CONST TSL_Bank_T MyBanks[];
extern CONST TSL_LinRot_T MyLinRots[];

void MyLinRots_ErrorStateProcess(void);
void MyLinRots_OffStateProcess(void);

TSL_Status_enum_T TSL_user_Action(void);


int isTouchingSensor = 0;

//==============================================================================
// Channels
//==============================================================================

// Channel Source and Configuration: Always placed in ROM
const TSL_ChannelSrc_T MyChannels_Src[TSLPRM_TOTAL_CHANNELS] = {
    { CHANNEL_0_SRC, CHANNEL_0_SAMPLE_CONFIG, CHANNEL_0_CHANNEL_CONFIG },
    { CHANNEL_1_SRC, CHANNEL_1_SAMPLE_CONFIG, CHANNEL_1_CHANNEL_CONFIG },
    { CHANNEL_2_SRC, CHANNEL_2_SAMPLE_CONFIG, CHANNEL_2_CHANNEL_CONFIG }
};

// Channel Destination: Always placed in ROM
const TSL_ChannelDest_T MyChannels_Dest[TSLPRM_TOTAL_CHANNELS] = {
    { CHANNEL_0_DEST },
    { CHANNEL_1_DEST },
    { CHANNEL_2_DEST }
};

// Channel Data: Always placed in RAM
TSL_ChannelData_T MyChannels_Data[TSLPRM_TOTAL_CHANNELS];

//------
// Banks
//------

// Always placed in ROM
CONST TSL_Bank_T MyBanks[TSLPRM_TOTAL_BANKS] = {
    {&MyChannels_Src[0], &MyChannels_Dest[0], MyChannels_Data, BANK_0_NBCHANNELS, BANK_0_SHIELD_SAMPLE, BANK_0_SHIELD_CHANNEL}
};

//==============================================================================
// Linear and Rotary sensors
//==============================================================================

// Data (RAM)
TSL_LinRotData_T MyLinRots_Data[TSLPRM_TOTAL_LINROTS];

// Parameters (RAM)
TSL_LinRotParam_T MyLinRots_Param[TSLPRM_TOTAL_LINROTS];

// State Machine (ROM)
CONST TSL_State_T MyLinRots_StateMachine[] = {
    // Calibration states
    /*  0 */ { TSL_STATEMASK_CALIB,              TSL_linrot_CalibrationStateProcess },
    /*  1 */ { TSL_STATEMASK_DEB_CALIB,          TSL_linrot_DebCalibrationStateProcess },
    // Release states
    /*  2 */ { TSL_STATEMASK_RELEASE,            TSL_linrot_ReleaseStateProcess },
#if TSLPRM_USE_PROX > 0
    /*  3 */ { TSL_STATEMASK_DEB_RELEASE_PROX,   TSL_linrot_DebReleaseProxStateProcess },
#else
    /*  3 */ { TSL_STATEMASK_DEB_RELEASE_PROX,   0 },
#endif
    /*  4 */ { TSL_STATEMASK_DEB_RELEASE_DETECT, TSL_linrot_DebReleaseDetectStateProcess },
    /*  5 */ { TSL_STATEMASK_DEB_RELEASE_TOUCH,  TSL_linrot_DebReleaseTouchStateProcess },
#if TSLPRM_USE_PROX > 0
    // Proximity states
    /*  6 */ { TSL_STATEMASK_PROX,               TSL_linrot_ProxStateProcess },
    /*  7 */ { TSL_STATEMASK_DEB_PROX,           TSL_linrot_DebProxStateProcess },
    /*  8 */ { TSL_STATEMASK_DEB_PROX_DETECT,    TSL_linrot_DebProxDetectStateProcess },
    /*  9 */ { TSL_STATEMASK_DEB_PROX_TOUCH,     TSL_linrot_DebProxTouchStateProcess },
#else
    /*  6 */ { TSL_STATEMASK_PROX,               0 },
    /*  7 */ { TSL_STATEMASK_DEB_PROX,           0 },
    /*  8 */ { TSL_STATEMASK_DEB_PROX_DETECT,    0 },
    /*  9 */ { TSL_STATEMASK_DEB_PROX_TOUCH,     0 },
#endif
    // Detect states
    /* 10 */ { TSL_STATEMASK_DETECT,             TSL_linrot_DetectStateProcess },
    /* 11 */ { TSL_STATEMASK_DEB_DETECT,         TSL_linrot_DebDetectStateProcess },
    // Touch state
    /* 12 */ { TSL_STATEMASK_TOUCH,              TSL_linrot_TouchStateProcess },
    // Error states
    /* 13 */ { TSL_STATEMASK_ERROR,              MyLinRots_ErrorStateProcess },
    /* 14 */ { TSL_STATEMASK_DEB_ERROR_CALIB,    TSL_linrot_DebErrorStateProcess },
    /* 15 */ { TSL_STATEMASK_DEB_ERROR_RELEASE,  TSL_linrot_DebErrorStateProcess },
    /* 16 */ { TSL_STATEMASK_DEB_ERROR_PROX,     TSL_linrot_DebErrorStateProcess },
    /* 17 */ { TSL_STATEMASK_DEB_ERROR_DETECT,   TSL_linrot_DebErrorStateProcess },
    /* 18 */ { TSL_STATEMASK_DEB_ERROR_TOUCH,    TSL_linrot_DebErrorStateProcess },
    // Other states
    /* 19 */ { TSL_STATEMASK_OFF,                MyLinRots_OffStateProcess }
};

// Methods for "extended" type (ROM)
CONST TSL_LinRotMethods_T MyLinRots_Methods = {
    TSL_linrot_Init,
    TSL_linrot_Process,
    TSL_linrot_CalcPos
};

// Delta Normalization Process
// The MSB is the integer part, the LSB is the real part
// Examples:
// - To apply a factor 1.10:
//   0x01 to the MSB
//   0x1A to the LSB (0.10 x 256 = 25.6 -> rounded to 26 = 0x1A)
// - To apply a factor 0.90:
//   0x00 to the MSB
//   0xE6 to the LSB (0.90 x 256 = 230.4 -> rounded to 230 = 0xE6)
CONST uint16_t MyLinRot_DeltaCoeff[3] = {0x0200, 0x0100, 0x0300};

// LinRots list (ROM)
CONST TSL_LinRot_T MyLinRots[TSLPRM_TOTAL_LINROTS] = {
    {
        &MyLinRots_Data[0], &MyLinRots_Param[0], &MyChannels_Data[CHANNEL_0_DEST],
        3, // Number of channels
        MyLinRot_DeltaCoeff,
        (TSL_tsignPosition_T*)TSL_POSOFF_3CH_LIN_H,
        TSL_SCTCOMP_3CH_LIN_H,
        TSL_POSCORR_3CH_LIN_H,
        MyLinRots_StateMachine,
        &MyLinRots_Methods
    }
};

//----------------
// Generic Objects
//----------------

// List (ROM)
CONST TSL_Object_T MyObjects[TSLPRM_TOTAL_OBJECTS] = {
    { TSL_OBJ_LINEAR, (TSL_LinRot_T*)& MyLinRots[0] }
};

// Group (RAM)
TSL_ObjectGroup_T MyObjGroup = {
    &MyObjects[0],        // First object
    TSLPRM_TOTAL_OBJECTS, // Number of objects
    0x00,                 // State mask reset value
    TSL_STATE_NOT_CHANGED // Current state
};

//-------------------------------------------
// TSL Common Parameters placed in RAM or ROM
// --> external declaration in tsl_conf.h
//-------------------------------------------

TSL_Params_T TSL_Params = {
    TSLPRM_ACQ_MIN,
    TSLPRM_ACQ_MAX,
    TSLPRM_CALIB_SAMPLES,
    TSLPRM_DTO,
#if TSLPRM_TOTAL_TKEYS > 0
    MyTKeys_StateMachine,   // Default state machine for TKeys
    &MyTKeys_Methods,       // Default methods for TKeys
#endif
#if TSLPRM_TOTAL_LNRTS > 0
    MyLinRots_StateMachine, // Default state machine for LinRots
    &MyLinRots_Methods      // Default methods for LinRots
#endif
};

/* Private functions prototype -----------------------------------------------*/

void TSL_user_InitGPIOs(void);
void TSL_user_SetThresholds(void);

/* Global variables ----------------------------------------------------------*/

TSL_tTick_ms_T ECS_last_tick; // Hold the last time value for ECS

/**
  * @brief  Initialize the STMTouch Driver
  * @param  None
  * @retval None
  */
void TSL_user_Init(void)
{
#if TSLPRM_USE_SHIELD == 0
    TSL_user_InitGPIOs();
#endif
    TSL_obj_GroupInit(&MyObjGroup); // Init Objects
    TSL_Init(MyBanks); // Init timing and acquisition modules
    TSL_user_SetThresholds(); // Init thresholds for each object individually
}

/**
  * @brief  Executed when a sensor is in Error state
  * @param  None
  * @retval None
  */
void MyLinRots_ErrorStateProcess(void)
{
    // Add here your own processing when a sensor is in Error state
    TSL_linrot_SetStateOff();
}


/**
  * @brief  Executed when a sensor is in Off state
  * @param  None
  * @retval None
  */
void MyLinRots_OffStateProcess(void)
{
    // Add here your own processing when a sensor is in Off state
}

/**
  * @brief  Initializes the TouchSensing GPIOs.
  * @param  None
  * @retval None
  */
void TSL_user_InitGPIOs(void)
{
    // Configure the Shield IO (PX.y) to ground when not used.
}


/**
  * @brief  Set thresholds for each object (optional).
  * @param  None
  * @retval None
  */
void TSL_user_SetThresholds(void)
{
    // Example: Decrease the Detect thresholds for the TKEY 0
    //MyTKeys_Param[0].DetectInTh -= 10;
    //MyTKeys_Param[0].DetectOutTh -= 10;
}

/**
  * @brief  Execute STMTouch Driver main State machine
  * @param  None
  * @retval status Return TSL_STATUS_OK if the acquisition is done
  */
TSL_Status_enum_T TSL_user_Action(void)
{
    static uint8_t idx_bank=0;
    static uint8_t ConfigDone=0;
    TSL_Status_enum_T status;
    if (!ConfigDone) {
        TSL_acq_BankConfig(idx_bank);
        TSL_acq_BankStartAcq();
        ConfigDone = 1;
    }
    // Check Bank End of Acquisition
    if (TSL_acq_BankWaitEOC() == TSL_STATUS_OK) {
        TSL_acq_BankGetResult(idx_bank, 0, 0);
        ConfigDone = 0;
        idx_bank++;
    }
    if (idx_bank == TSLPRM_TOTAL_BANKS) {
        idx_bank = 0;
        TSL_obj_GroupProcess(&MyObjGroup);
        // ECS every 100ms
        // ECS = Environment Change System. Adapts to slow changes in power supply voltage, temperature and air humidity.
        // Current config never returns TSL_STATUS_OK.
        if (TSL_tim_CheckDelay_ms(100, &ECS_last_tick) == TSL_STATUS_OK) {
            // Not getting here with current config.
            if (TSL_ecs_Process(&MyObjGroup) == TSL_STATUS_OK) {
                isTouchingSensor = 0;
            }
            else {
                isTouchingSensor = 1;
            }
        }
        // All banks have been acquired and sensors processed
        status = TSL_STATUS_OK;
    }
    else {
        status = TSL_STATUS_BUSY;
    }
    return status;
}

int getTouchSliderPercent(void)
{
    // Run one step of the STMTouch state machine and exit if we're not in a valid state yet.
    if (TSL_user_Action() != TSL_STATUS_OK) {
        return -1;
    }

    // Sensor not being touched.
    if (TSL_linrot_GetStateMask() != TSL_STATEMASK_DETECT) {
        return -1;
    }

//    uint16_t maxValue = 256 / (9 - TSLPRM_LINROT_RESOLUTION);
    int percentValue = MyLinRots[0].p_Data->Position;
//    percentValue *= 10000;
//    percentValue /= maxValue;
    return percentValue;
}

int getTouchButtons(void)
{
    // Sadly, when a button touch is first detected (and the TSL_STATEMASK_DETECT state is first entered), the Position
    // is still what it was when the button was last released. It then changes to the correct position after a few
    // moments. So, as an easy way to hide the spurious value, the button id is only returned when the
    // button is released (TSL_STATEMASK_RELEASE state is first entered), at which time it will have settled to the
    // true value. If the finger slides over multiple buttons before release, the last button that was touched is
    // returned on release.

    static int buttonId = -1;

    // Run one step of the STMTouch state machine and exit if we're not in a valid state yet.
    if (TSL_user_Action() != TSL_STATUS_OK) {
        return -1;
    }

    // Return the button that was pressed when it's released.
    if (TSL_linrot_GetStateMask() == TSL_STATEMASK_RELEASE) {
        int retButtonId = buttonId;
        buttonId = -1;
        return retButtonId;
    }

    // Find which button is pressed.
    if (TSL_linrot_GetStateMask() != TSL_STATEMASK_DETECT) {
        return -1;
    }

#ifdef STM32L1XX_MDP

    // Medium density plus device.
    if (MyLinRots[0].p_Data->Position > 118) {
        buttonId = 3;
    }
    else if (MyLinRots[0].p_Data->Position > 80) {
        buttonId = 2;
    }
    else if (MyLinRots[0].p_Data->Position > 5) {
        buttonId = 1;
    }
    else {
        buttonId = 0;
    }

#else

    // Medium density device.
    if (MyLinRots[0].p_Data->Position >= 127) {
        return -1;
    }

    if (MyLinRots[0].p_Data->Position > 110) {
        buttonId = 3;
    }
    else if (MyLinRots[0].p_Data->Position > 80) {
        buttonId = 2;
    }
    else if (MyLinRots[0].p_Data->Position > 5) {
        buttonId = 1;
    }
    else {
        buttonId = 0;
    }

#endif

    return -1;
}
