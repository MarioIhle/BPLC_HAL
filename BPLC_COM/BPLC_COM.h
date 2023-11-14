#ifndef BPLC_COM_h
#define BPLC_COM_h

#include "BPLC_Network.h"
#include "BPLC_Port.h"

#include "SpecialFunctions.h"
#include "EEPROM.h"

//---------------------------------------------------------------------------------------
//BERTANET MASTER 
//---------------------------------------------------------------------------------------
//DEBUGGING
//#define DEBUG_MASTER

#ifndef DEBUG_MASTER
#define POLL_DELIMITER  20
#endif

#ifdef  DEBUG_MASTER 
#define POLL_DELIMITER  1000
#endif

#define   POLL_COUNT_MAX  1
#define   PORT_COUNT      255
#define   SLAVE_COUNT_MAX 255
#define   HOST_AND_MASTER_ADRESS 2 

//---------------------------------------------------------------------------------------
class BPLC_MasterNode
{
  public:
  BPLC_MasterNode(uint8_t ADRESS, HardwareSerial* P_SERIAL, uint8_t ENABLE_PIN); 
  void begin();
  void tick ();

  private:
  void              executeCommand  ();

  void              pollSlave();
  void              setupNextPoll();
  uint8_t					  slaveCount;
	uint8_t 				  slaveToPoll;
	uint8_t 				  pollCount;

  void                  saveReceivedPortData  (uint8_t PORT, u_Payload_t P_PAYLOAD);

  s_networkParameter_t  networkParameter;
  BPLC_Network          network;

  BPLC_COM_Port         allPorts[PORT_COUNT];
  bool                  isPortSavedInEeprom[PORT_COUNT];
  void                  recoverPortData();

  Timeout 			        to_poll;
  Timeout               to_attachNewNode;
};







//---------------------------------------------------------------------------------------
//BERTANET SLAVE KLASSE
//---------------------------------------------------------------------------------------
//DEBUGGING
//#define DEBUG_SLAVE
//#define DEBUG_SLAVE_ADVANCED

//---------------------------------------------------------------------------------------
//REQUEST TYPE
#define BUSREQUEST_BUFFER_SIZE 10

typedef struct
{
	e_BPLC_NETWORK_KEY_t key;	    		      
	uint8_t 		destinationAdress;	
	uint8_t 		portIndex;				        
} 
s_busRequest_t;

#define PORT_COUNT_MAX 50

typedef struct 
{
  BPLC_COM_Port* portIndex[PORT_COUNT_MAX];
  uint8_t count;

}s_usedPorts_t;


//---------------------------------------------------------------------------------------
class BPLC_SlaveNode
{
  public:
  BPLC_SlaveNode();
  void begin(uint8_t ADDRESS, HardwareSerial* P_SERIAL, uint8_t ENABLE_PIN, s_usedPorts_t* P_PORTS); 
  void tick   ();  

  private:
  void            resyncNetwork                 ();
  bool            executeCommand                ();
  void 						checkPorts              	    ();
  u_Payload_t     getPortData                   (uint8_t PORT);
  void            saveReceivedPortData          (uint8_t PORT, u_Payload_t PAYLOAD);    

  bool            sendBusRequest			          ();
  bool 			      setBusRequest			            (uint8_t DESTINATION_ADRESS, uint8_t KEY, uint8_t PORT);
  uint8_t         getNextFreeBufferSlot         ();
  bool            isThereAFreeBufferSlot        (); 	
  bool            thereARequestToSend           ();
  uint8_t         skipToNextRequestBufferSlot		(uint8_t SLOT);		
  void						flushRequestAndSkipToNextSlot	();

  s_busRequest_t 	busRequestBuffer[BUSREQUEST_BUFFER_SIZE]; 
  uint8_t 				requestToSend; 

  s_networkParameter_t  networkParameter;
  BPLC_Network          network;
  s_usedPorts_t*        p_ports;
  uint8_t               initializedPorts;
};

#endif
