#include "../../include/DhGloablHead.h"

typedef struct {                                   /*!< RADIO Structure                                                       */
  volatile	u4  TASKS_TXEN;                        /*!< Enable radio in TX mode.                                              */
  volatile	u4  TASKS_RXEN;                        /*!< Enable radio in RX mode.                                              */
  volatile	u4  TASKS_START;                       /*!< Start radio.                                                          */
  volatile	u4  TASKS_STOP;                        /*!< Stop radio.                                                           */
  volatile	u4  TASKS_DISABLE;                     /*!< Disable radio.                                                        */
  volatile	u4  TASKS_RSSISTART;                   /*!< Start the RSSI and take one sample of the receive signal strength.    */
  volatile	u4  TASKS_RSSISTOP;                    /*!< Stop the RSSI measurement.                                            */
  volatile	u4  TASKS_BCSTART;                     /*!< Start the bit counter.                                                */
  volatile	u4  TASKS_BCSTOP;                      /*!< Stop the bit counter.                                                 */
  volatile	u4  RESERVED0[55];
  volatile	u4  EVENTS_READY;                      /*!< Ready event.                                                          */
  volatile	u4  EVENTS_ADDRESS;                    /*!< Address event.                                                        */
  volatile	u4  EVENTS_PAYLOAD;                    /*!< Payload event.                                                        */
  volatile	u4  EVENTS_END;                        /*!< End event.                                                            */
  volatile	u4  EVENTS_DISABLED;                   /*!< Disable event.                                                        */
  volatile	u4  EVENTS_DEVMATCH;                   /*!< A device address match occurred on the last received packet.          */
  volatile	u4  EVENTS_DEVMISS;                    /*!< No device address match occurred on the last received packet.         */
  volatile	u4  EVENTS_RSSIEND;                    /*!< Sampling of the receive signal strength complete. A new RSSI
                                                         sample is ready for readout at the RSSISAMPLE register.               */
  volatile	u4  RESERVED1[2];
  volatile	u4  EVENTS_BCMATCH;                    /*!< Bit counter reached bit count value specified in BCC register.        */
  volatile	u4  RESERVED2[53];
  volatile	u4  SHORTS;                            /*!< Shortcuts for the radio.                                              */
  volatile	u4  RESERVED3[64];
  volatile	u4  INTENSET;                          /*!< Interrupt enable set register.                                        */
  volatile	u4  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  volatile	u4  RESERVED4[61];
  volatile	u4  CRCSTATUS;                         /*!< CRC status of received packet.                                        */
  volatile	u4  RESERVED5;
  volatile	u4  RXMATCH;                           /*!< Received address.                                                     */
  volatile	u4  RXCRC;                             /*!< Received CRC.                                                         */
  volatile	u4  DAI;                               /*!< Device address match index.                                           */
  volatile	u4  RESERVED6[60];
  volatile	u4  PACKETPTR;                         /*!< Packet pointer. Decision point: START task.                           */
  volatile	u4  FREQUENCY;                         /*!< Frequency.                                                            */
  volatile	u4  TXPOWER;                           /*!< Output power.                                                         */
  volatile	u4  MODE;                              /*!< Data rate and modulation.                                             */
  volatile	u4  PCNF0;                             /*!< Packet configuration 0.                                               */
  volatile	u4  PCNF1;                             /*!< Packet configuration 1.                                               */
  volatile	u4  BASE0;                             /*!< Radio base address 0. Decision point: START task.                     */
  volatile	u4  BASE1;                             /*!< Radio base address 1. Decision point: START task.                     */
  volatile	u4  PREFIX0;                           /*!< Prefixes bytes for logical addresses 0 to 3.                          */
  volatile	u4  PREFIX1;                           /*!< Prefixes bytes for logical addresses 4 to 7.                          */
  volatile	u4  TXADDRESS;                         /*!< Transmit address select.                                              */
  volatile	u4  RXADDRESSES;                       /*!< Receive address select.                                               */
  volatile	u4  CRCCNF;                            /*!< CRC configuration.                                                    */
  volatile	u4  CRCPOLY;                           /*!< CRC polynomial.                                                       */
  volatile	u4  CRCINIT;                           /*!< CRC initial value.                                                    */
  volatile	u4  TEST;                              /*!< Test features enable register.                                        */
  volatile	u4  TIFS;                              /*!< Inter Frame Spacing in microseconds.                                  */
  volatile	u4  RSSISAMPLE;                        /*!< RSSI sample.                                                          */
  volatile	u4  RESERVED7;
  volatile	u4  STATE;                             /*!< Current radio state.                                                  */
  volatile	u4  DATAWHITEIV;                       /*!< Data whitening initial value.                                         */
  volatile	u4  RESERVED8[2];
  volatile	u4  BCC;                               /*!< Bit counter compare.                                                  */
  volatile	u4  RESERVED9[39];
  volatile	u4  DAB[8];                            /*!< Device address base segment.                                          */
  volatile	u4  DAP[8];                            /*!< Device address prefix.                                                */
  volatile	u4  DACNF;                             /*!< Device address match configuration.                                   */
  volatile	u4  RESERVED10[56];
  volatile	u4  OVERRIDE0;                         /*!< Trim value override register 0.                                       */
  volatile	u4  OVERRIDE1;                         /*!< Trim value override register 1.                                       */
  volatile	u4  OVERRIDE2;                         /*!< Trim value override register 2.                                       */
  volatile	u4  OVERRIDE3;                         /*!< Trim value override register 3.                                       */
  volatile	u4  OVERRIDE4;                         /*!< Trim value override register 4.                                       */
  volatile	u4  RESERVED11[561];
  volatile	u4  POWER;                             /*!< Peripheral power control.                                             */
}BlkNrfRadio;

#define NRF_RADIO                       ((BlkNrfRadio          *) NRF_RADIO_ADDRESS)

	
