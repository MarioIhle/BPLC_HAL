#ifndef BPLC_Network_h
#define BPLC_Network_h

/**
 * INFO: Wenn nachrichten verloren gehen, RX buffer erhöhren in -> C:\Users\Mario\.platformio\packages\framework-arduino-avr\cores\arduino\HardwareSerial.h  -->Z.53
*/

//---------------------------------------------------------------------------------------
////////////////////////////////Eingbundene Bibiotheken///////Z//////////////////////////
//---------------------------------------------------------------------------------------
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "BPLC_Port.h"

#define SoftwareVersion_major	1
#define SoftwareVersion_minor	3

//---------------------------------------------------------------------------------------
/////////////////////////////////////NETZWERK PARAMETER//////////////////////////////////
//---------------------------------------------------------------------------------------
//RS485 Schnittstelle
#define NETWORK_BAUDRATE 38400			//BAUD Bertanet
//---------------------------------------------------------------------------------------
////////////////////////////////////////DEBUG////////////////////////////////////////////
//---------------------------------------------------------------------------------------
#ifndef SERIAL0
//DEBUG	
//#define	DEBUG_BAUDRATE	115200
//#define 	DEBUG_RECORDING
//#define 	DEBUG_SENDING
//#define 	DEBUG_MAILBOX
#endif
//---------------------------------------------------------------------------------------
///////////////////////////////PRÄPROZESSOR KONSTANTEN///////////////////////////////////
//---------------------------------------------------------------------------------------
//Sequence
#define PAYLOAD_LENGTH					4				//Anzahl Payload Bytes
#define HEAD_LENGTH 					8    	    	//Anzahl bytes von Sequenzkopf
#define FRAME_LENGTH 					7 	       		//Anzahl bytes mit CRC ohne Payload
#define SEQUENCE_DATABYTES_COUNT		9				//Anzahl bytes ohne CRC
#define SEQUENCE_LENGTH					14				//Anzahl bytes von kommpletter Sequenz

//Port
#define NO_PORT		   		    		0				//Platzhalter wenn keine Portdaten vorhanden
//Netzwerk
#define MAX_POLL_VERSUCHE				1				//Anzahl maximaler Kommunikationsversuche
#define HOST_PC							0
#define HOST_PC_CRC_LOW_BYTE			111
#define HOST_PC_CRC_HIGH_BYTE			222
#define MASTER_NODE						1				//Adresse Master Node
#define BROADCAST 						255				//Broadcastadresse
#define MAILBOX_SIZE					5
//Standart Busparameter
#define BUS_DELIMITER					2				//Mindest abstand zwischen Busnachrichten (record<-delimiter->response)
#define RECORDING_TIME_MAX				5				//Zeit bis aufnahme der Sequenz abgebrochen wird
#define RESPONSE_TIME_MAX				50				//Zeit in der Antwort von MASTR/SLAVE erwartet wird, wenn keine Antwort bekommen, dann in nächsten Poll erneut versuchen es gehen keine Daten verloren 
#define DEBUG_BUS_DELIMITER				100					//Mindest abstand zwischen Busnachrichten (record<-delimiter->response)
#define DEBUG_RECORDING_TIME_MAX		255					//Zeit bis aufnahme der Sequenz abgebrochen wird
#define DEBUG_RESPONSE_TIME_MAX			255				//Zeit in der Antwort von MASTER/SLAVE erwartet wird



//---------------------------------------------------------------------------------------
//NETZWERKSTATUS TYPE
typedef enum
{
	networkState_init = 0,
	networkState_resync,
	networkState_debugging,
	networkState_running,
	networkState_stopped,	
}
e_networkState_t;

//---------------------------------------------------------------------------------------
//KEY 
typedef enum
{
	readPort = 0,
	readResponse,
	
	writePort = 10,
	writePushPort,

	poll = 20,
	noRequest,	
	bussystemStart,
	bussystemStop,		
	busTiming,
	softReset,	
	getSoftwareVersion,
	
	HB_ping = 30,
	HB_pong,

	key_ACK = 40,
	key_NAK,

}e_BPLC_NETWORK_KEY_t;

//---------------------------------------------------------------------------------------
//BERTANET SEQUENCE 
//---------------------------------------------------------------------------------------
//SEQUENCEHEAD TYPE
typedef struct 
{
	uint8_t 			destinationAdress;		//Empfängeradresse
	uint8_t 			senderAdress;			//Absenderadresse
	uint8_t 			length;					//Anzahl Nutzdatenbytes (kann auch 0 sein, z.B. bei Leseanfrage)
	uint8_t	 			key;					//Zweck der Sequenz
	uint8_t 			portIndex;					//Index des Ports, falls Nutzdaten enthalten sind
	uint8_t				messageIndex; 			//Laufende Nummer, bei 255 wieder 0
	uint8_t				reserve;
	uint8_t				reserve_1;
}
s_SequenceHead_t;

//---------------------------------------------------------------------------------------
//CHECKSUM TYPE
typedef struct
{
	uint8_t 	loByte;
	uint8_t		hiByte;
} 
s_CRC16_t;

//---------------------------------------------------------------------------------------
//BERTANET SEQUENCE TYPE
#pragma pack (push, 1)
typedef union
{
  uint8_t				DATA[SEQUENCE_LENGTH];
  struct
  {
    s_SequenceHead_t  	head;       	//Kopfdaten von BertaNet Sequnece
    u_Payload_t			payload;    	//Nutzdaten von BertaNet Sequnece
    s_CRC16_t         	checksum;   	//Checksumme von BertaNet Sequnece
  } extract;
} u_Sequence_t;
#pragma pack (pop)

//---------------------------------------------------------------------------------------
//BUSPARAMETER TYPE
typedef struct
{	
	HardwareSerial* serial;
	bool 			debuggingActive;
	bool 			networkInSync;
	uint16_t 		busDelimiter_ms;		
	uint8_t			responseTimeout_ms;
	uint8_t			recordingTimeout_ms; 	
	uint8_t			adress;
	uint8_t			enablePin;
}s_networkParameter_t;




class BPLC_Network
{
	public:
	BPLC_Network			();
	void	begin(s_networkParameter_t PARAMETER);
	void    initInterface();
	//Bushandling
	void 	setNetworkTiming		(bool DEBUGGING_ACTIVE);
	void 	startBussystem			();
	void 	stopBussystem			();
	bool	checkMailbox					(bool WAIT_FOR_RESPONSE);
	void	buildAndSendCommand				(uint8_t DESTINATION_ADRESS, uint8_t KEY, uint8_t PORT);
	void 	buildAndSendCommandWithPayload	(uint8_t DESTINATION_ADRESS, uint8_t KEY, uint8_t PORT, u_Payload_t P_PAYLOAD);

	//getter Funtktionen	
	e_networkState_t		getNetworkState				();
	void 					setNetworkState				(e_networkState_t STATE);
	u_Sequence_t*			getLastSequneceInMailbox	();
	void 					flushLastSequenceInMailbox	();

	private:
	//Sequenz aufnehmen und Prüfen
	bool thereIsAnewValidMessageForThisNode	();
	bool recordEvent			();
	bool validateReceivedMessage();	
	bool checkMessageIndex					(u_Sequence_t* P_SEQUENCE);	
	void flushInBuffer			();	
	bool checkFCS							(u_Sequence_t* 	P_SEQUENCE);

	//Sequenz bauen und versenden	
	void		calulateChecksumAndSendSequence		(s_SequenceHead_t HEAD, u_Payload_t PAYLOAD);	
	s_CRC16_t 	calculateFCS						(uint8_t *DATA, uint8_t LENGTH);
		
	
	//Netzwerk Parameter
	e_networkState_t		networkState;
	s_networkParameter_t 	networkParameter;
	uint8_t					lastKnownMessageIndex;

	u_Sequence_t 			mailbox[MAILBOX_SIZE];			//Es werden nur Sequencen gespeichert, die auch für diese Node bestimmt sind
	uint8_t					inMessageCount;					//Zeiger auf nächster freier Slot
	
	Timeout 				to_response;	
	Timeout					to_busDelmiter;
};
#endif