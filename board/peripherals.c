/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v11.0
processor: MK64FN1M0xxx12
package_id: MK64FN1M0VLL12
mcu_data: ksdk2_0
processor_version: 12.0.0
board: FRDM-K64F
functionalGroups:
- name: BOARD_InitPeripherals
  UUID: 06b3db46-0fac-4805-9bcd-9b01fffb48b3
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system'
- global_system_definitions:
  - user_definitions: ''
  - user_includes: ''
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'uart_cmsis_common'
- type_id: 'uart_cmsis_common_9cb8e302497aa696fdbb5a4fd622c2a8'
- global_USART_CMSIS_common:
  - quick_selection: 'default'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'gpio_adapter_common'
- type_id: 'gpio_adapter_common_57579b9ac814fe26bf95df0a384c36b6'
- global_gpio_adapter_common:
  - quick_selection: 'default'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * SYSMPU initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'SYSMPU'
- type: 'mpu_utility'
- mode: 'SYSMPU'
- custom_name_enabled: 'false'
- type_id: 'mpu_utility_bc3ea1f6add76edb6050f698d423d163'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'SYSMPU'
- config_sets:
  - sysmpu_init:
    - mpuInit: 'disabled'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/* Empty initialization function (commented out)
static void SYSMPU_init(void) {
} */

/***********************************************************************************************************************
 * NVIC initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'NVIC'
- type: 'nvic'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'nvic_57b5eef3774cc60acaede6f5b8bddc67'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'NVIC'
- config_sets:
  - nvic:
    - interrupt_table:
      - 0: []
    - interrupts: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/* Empty initialization function (commented out)
static void NVIC_init(void) {
} */

/***********************************************************************************************************************
 * CAN0 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'CAN0'
- type: 'flexcan'
- mode: 'interrupts'
- custom_name_enabled: 'false'
- type_id: 'flexcan_a98f4e39f821600f664c89f66f55b020'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'CAN0'
- config_sets:
  - interruptsCfg:
    - messageBufferIrqs: '0'
    - interruptsEnable: ''
    - enable_ored_mb_irq: 'false'
    - interrupt_ored_mb:
      - IRQn: 'CAN0_ORed_Message_buffer_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - enable_busoff_irq: 'false'
    - interrupt_busoff:
      - IRQn: 'CAN0_Bus_Off_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - enable_error_irq: 'false'
    - interrupt_error:
      - IRQn: 'CAN0_Error_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - enable_tx_irq: 'false'
    - interrupt_tx:
      - IRQn: 'CAN0_Tx_Warning_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - enable_rx_irq: 'false'
    - interrupt_rx:
      - IRQn: 'CAN0_Rx_Warning_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - enable_wakeup_irq: 'false'
    - interrupt_wakeup:
      - IRQn: 'CAN0_Wake_Up_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
  - fsl_flexcan:
    - can_config:
      - clockSource: 'kFLEXCAN_ClkSrcPeri'
      - clockSourceFreq: 'BOARD_BootClockRUN'
      - wakeupSrc: 'kFLEXCAN_WakeupSrcUnfiltered'
      - baudRate: '250000'
      - maxMbNum: '16'
      - enableLoopBack: 'false'
      - enableTimerSync: 'true'
      - enableSelfWakeup: 'false'
      - enableIndividMask: 'true'
      - disableSelfReception: 'false'
      - enableListenOnlyMode: 'false'
      - enableSupervisorMode: 'false'
      - timingConfig:
        - propSeg: '2'
        - phaseSeg1: '4'
        - phaseSeg2: '3'
        - rJumpwidth: '2'
        - bitTime: []
    - enableRxFIFO: 'false'
    - rxFIFO:
      - idFilterTable: ''
      - idFilterNum: 'num0'
      - idFilterType: 'kFLEXCAN_RxFifoFilterTypeA'
      - priority: 'kFLEXCAN_RxFifoPrioLow'
    - channels:
      - 0:
        - mbID: '0'
        - mbType: 'mbRx'
        - rxMb:
          - id: '0'
          - format: 'kFLEXCAN_FrameFormatStandard'
          - type: 'kFLEXCAN_FrameTypeData'
      - 1:
        - mbID: '1'
        - mbType: 'mbRx'
        - rxMb:
          - id: '0'
          - format: 'kFLEXCAN_FrameFormatExtend'
          - type: 'kFLEXCAN_FrameTypeData'
      - 2:
        - mbID: '2'
        - mbType: 'mbTx'
        - rxMb:
          - id: '0'
          - format: 'kFLEXCAN_FrameFormatStandard'
          - type: 'kFLEXCAN_FrameTypeData'
      - 3:
        - mbID: '3'
        - mbType: 'mbTx'
        - rxMb:
          - id: '0'
          - format: 'kFLEXCAN_FrameFormatStandard'
          - type: 'kFLEXCAN_FrameTypeData'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const flexcan_config_t CAN0_config = {
  .clkSrc = kFLEXCAN_ClkSrc1,
  .wakeupSrc = kFLEXCAN_WakeupSrcUnfiltered,
  .bitRate = 250000UL,
  .maxMbNum = 16U,
  .enableLoopBack = false,
  .enableTimerSync = true,
  .enableSelfWakeup = false,
  .enableIndividMask = true,
  .disableSelfReception = false,
  .enableListenOnlyMode = false,
  .enableSupervisorMode = false,
  .timingConfig = {
    .preDivider = 23,
    .propSeg = 1,
    .phaseSeg1 = 3,
    .phaseSeg2 = 2,
    .rJumpwidth = 1
  }
};
/* Message buffer 0 configuration structure */
const flexcan_rx_mb_config_t CAN0_rx_mb_config_0 = {
  .id = FLEXCAN_ID_STD(0UL),
  .format = kFLEXCAN_FrameFormatStandard,
  .type = kFLEXCAN_FrameTypeData
};
/* Message buffer 1 configuration structure */
const flexcan_rx_mb_config_t CAN0_rx_mb_config_1 = {
  .id = FLEXCAN_ID_EXT(0UL),
  .format = kFLEXCAN_FrameFormatExtend,
  .type = kFLEXCAN_FrameTypeData
};

static void CAN0_init(void) {
  FLEXCAN_Init(CAN0_PERIPHERAL, &CAN0_config, CAN0_CLOCK_SOURCE);
  /* Message buffer 0 initialization */
  FLEXCAN_SetRxMbConfig(CAN0_PERIPHERAL, 0, &CAN0_rx_mb_config_0, true);
  /* Message buffer 1 initialization */
  FLEXCAN_SetRxMbConfig(CAN0_PERIPHERAL, 1, &CAN0_rx_mb_config_1, true);
  /* Message buffer 2 initialization */
  FLEXCAN_SetTxMbConfig(CAN0_PERIPHERAL, 2, true);
  /* Message buffer 3 initialization */
  FLEXCAN_SetTxMbConfig(CAN0_PERIPHERAL, 3, true);
}

/***********************************************************************************************************************
 * PIT initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'PIT'
- type: 'pit'
- mode: 'LPTMR_GENERAL'
- custom_name_enabled: 'false'
- type_id: 'pit_ab54f91356454adb874dafbb69e655fd'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'PIT'
- config_sets:
  - fsl_pit:
    - enableRunInDebug: 'false'
    - timingConfig:
      - clockSource: 'BusInterfaceClock'
      - clockSourceFreq: 'BOARD_BootClockRUN'
    - channels:
      - 0:
        - channel_id: 'CHANNEL_0'
        - channelNumber: '0'
        - enableChain: 'false'
        - timerPeriod: '10ms'
        - startTimer: 'true'
        - enableInterrupt: 'true'
        - interrupt:
          - IRQn: 'PIT0_IRQn'
          - enable_interrrupt: 'enabled'
          - enable_priority: 'false'
          - priority: '0'
          - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const pit_config_t PIT_config = {
  .enableRunInDebug = false
};

static void PIT_init(void) {
  /* Initialize the PIT. */
  PIT_Init(PIT_PERIPHERAL, &PIT_config);
  /* Set channel 0 period to 10 ms (600000 ticks). */
  PIT_SetTimerPeriod(PIT_PERIPHERAL, PIT_CHANNEL_0, PIT_CHANNEL_0_TICKS);
  /* Enable interrupts from channel 0. */
  PIT_EnableInterrupts(PIT_PERIPHERAL, PIT_CHANNEL_0, kPIT_TimerInterruptEnable);
  /* Enable interrupt PIT_CHANNEL_0_IRQN request in the NVIC */
  EnableIRQ(PIT_CHANNEL_0_IRQN);
  /* Start channel 0. */
  PIT_StartTimer(PIT_PERIPHERAL, PIT_CHANNEL_0);
}

/***********************************************************************************************************************
 * USB0 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'USB0'
- type: 'usb'
- mode: 'device'
- custom_name_enabled: 'false'
- type_id: 'usb_cbf31fb9a3cef21890d93e737c3d2690'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'USB0'
- config_sets:
  - deviceSetting:
    - vendor_id: '0x1FC9'
    - product_id: '0x0094'
    - manufacturer_string: 'NXP'
    - product_string: 'VCOM'
    - self_powered: 'true'
    - suspend_resume: 'false'
    - max_power: '100'
    - interfaces:
      - 0:
        - interface_class: 'kClassCic'
        - setting_cic:
          - interface_name: 'CIC VCOM'
          - subclass: 'kSubclassAcm'
          - protocol: 'kProtocolNone'
          - implementation: 'kImplementationCicVcom'
          - endpoints_settings:
            - 0:
              - setting_name: 'Default'
              - endpoints:
                - 0:
                  - direction: 'kIn'
                  - transfer_type: 'kInterrupt'
                  - synchronization: 'kNoSynchronization'
                  - usage: 'kData'
                  - max_packet_size_fs: 'k64'
                  - polling_interval_fs: '8'
                  - bRefresh: '0'
                  - bSynchAddress: 'NoSynchronization'
          - data_interface_count: '1'
      - 1:
        - interface_class: 'kClassDic'
        - setting_dic:
          - interface_name: 'DIC VCOM'
          - subclass: 'kSubclassNone'
          - protocol: 'kProtocolNone'
          - implementation: 'kImplementationDicVcom'
          - endpoints_settings:
            - 0:
              - setting_name: 'Default'
              - endpoints:
                - 0:
                  - direction: 'kIn'
                  - transfer_type: 'kBulk'
                  - synchronization: 'kNoSynchronization'
                  - usage: 'kData'
                  - max_packet_size_fs: 'k64'
                  - polling_interval_fs: '0'
                  - bRefresh: '0'
                  - bSynchAddress: 'NoSynchronization'
                - 1:
                  - direction: 'kOut'
                  - transfer_type: 'kBulk'
                  - synchronization: 'kNoSynchronization'
                  - usage: 'kData'
                  - max_packet_size_fs: 'k64'
                  - polling_interval_fs: '0'
                  - bRefresh: '0'
                  - bSynchAddress: 'NoSynchronization'
          - quick_selection: 'QS_INTERFACE_DIC_VCOM'
  - commonSettings:
    - mpu_init:
      - mpu_init_component: 'SYSMPU'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

static void USB0_init(void) {
  USB_DeviceApplicationInit();
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
  /* Global initialization */
  /* Disable SYSMPU function */
  SYSMPU_Enable(SYSMPU_PERIPHERAL, 0);

  /* Initialize components */
  CAN0_init();
  PIT_init();
  USB0_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
  BOARD_InitPeripherals();
}
