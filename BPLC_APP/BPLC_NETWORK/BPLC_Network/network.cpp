#include "BertaNetwork.h"

/*#################################################################################
  BUSHANDLING
  #################################################################################*/
BertaNetwork::BertaNetwork()
{}

//---------------------------------------------------------------------------------------
void BertaNetwork::begin(s_networkParameter_t PARAMETER)	
{
	//Netzwerk Parameter
	this->networkParameter = PARAMETER;
	#ifdef DEBUG
	this->networkParameter.debuggingActive = true;
	#endif	
	this->setNetworkTiming(this->networkParameter.debuggingActive);

	//SCHNITTSTELLE inititalisieren
	pinMode(this->networkParameter.enablePin, OUTPUT);	
	this->networkParameter.serial->begin(NETWORK_BAUDRATE);	
	
	this->initInterface();

	#ifdef DEBUG_NETWORK
	Serial.begin(DEBUG_BAUDRATE);
	#endif		
}

//---------------------------------------------------------------------------------------
void BertaNetwork::initInterface()	
{
	//Netzwerk Parameter
	memset(&this->mailbox, 0, sizeof(this->mailbox));
	this->inMessageCount 		= 0;	
	this->lastKnownMessageIndex = 0;	
	
	this->networkParameter.serial->flush();
	this->flushInBuffer();

	this->networkState		= networkState_init;
}

//---------------------------------------------------------------------------------------
void BertaNetwork::startBussystem()
{
	this->networkState = networkState_running;	
}

//---------------------------------------------------------------------------------------
void BertaNetwork::stopBussystem()
{
	this->networkState = networkState_stopped;	
}

//---------------------------------------------------------------------------------------
void BertaNetwork::setNetworkTiming(bool DEBUGGING_ACTIVE)
{
	if(DEBUGGING_ACTIVE || this->networkParameter.debuggingActive)
	{
		this->networkParameter.debuggingActive = true;
		this->to_busDelmiter.setInterval(DEBUG_BUS_DELIMITER);
		this->to_response.setInterval(DEBUG_RECORDING_TIME_MAX);		

		this->networkParameter.serial->setTimeout(DEBUG_RECORDING_TIME_MAX);
	}
	else
	{
		this->networkParameter.debuggingActive = false;
		this->to_busDelmiter.setInterval(BUS_DELIMITER);
		this->to_response.setInterval(RECORDING_TIME_MAX);			
		this->networkParameter.serial->setTimeout(RECORDING_TIME_MAX);
	}
}

//---------------------------------------------------------------------------------------
e_networkState_t BertaNetwork::getNetworkState()
{
	return this->networkState;
}

//---------------------------------------------------------------------------------------
void  BertaNetwork::setNetworkState(e_networkState_t STATE)
{
	this->networkState = STATE;
}

/*#################################################################################
  NETZWERKFUNKTIONEN
  #################################################################################*/
bool BertaNetwork::checkMailbox(bool WAIT_FOR_RESPONSE)
{
	this->to_response.reset();
	bool THERE_IS_A_VALID_MESSAGE_FOR_THIS_NODE = false;

	#ifdef DEBUG_MAILBOX
	if(WAIT_FOR_RESPONSE)
	{		
		Serial.println("WAITING_FOR_RESPONSE"); 	
	}
	#endif
	do
	{
		THERE_IS_A_VALID_MESSAGE_FOR_THIS_NODE = (bool)(this->thereIsAnewValidMessageForThisNode());
		
		if(THERE_IS_A_VALID_MESSAGE_FOR_THIS_NODE)
		{	
			WAIT_FOR_RESPONSE = false;
			#ifdef DEBUG_MAILBOX 
			Serial.println("THERE_IS_A_NEW_MESSAGE_FOR_THIS_NODE");  
			#endif			
		}
		else if(this->to_response.check())
		{		
			WAIT_FOR_RESPONSE = false;
			#ifdef DEBUG_MAILBOX 
			Serial.println("NO_RESPONSE_EXIT_MAILBOX_AFTER_TIMEOUT"); 	
			#endif
		}		

	}while (WAIT_FOR_RESPONSE);	
	
	#ifdef DEBUG_MAILBOX 
	if(THERE_IS_A_VALID_MESSAGE_FOR_THIS_NODE)
	{
		Serial.println("MAILBOX_CONTENT:"); 	
		for(int m=0; m<MAILBOX_SIZE; m++)
		{		
			Serial.print("SLOT: "); Serial.println(m);
			for(int i=0;i<11;i++)
			{
				Serial.print(this->mailbox[m].DATA[i]); Serial.print("_");
			}	
			Serial.println();
		}
	}
	#endif
	
	return THERE_IS_A_VALID_MESSAGE_FOR_THIS_NODE;
}

//---------------------------------------------------------------------------------------
u_Sequence_t* BertaNetwork::getLastSequneceInMailbox()
{	
	u_Sequence_t* P_LAST_MAIL = &this->mailbox[this->inMessageCount - 1];
	return P_LAST_MAIL;
}

//---------------------------------------------------------------------------------------
void BertaNetwork::flushLastSequenceInMailbox()
{
	this->inMessageCount--;
	memset(&this->mailbox[inMessageCount], 0, sizeof(u_Sequence_t));	
}

//---------------------------------------------------------------------------------------
bool BertaNetwork::checkMessageIndex(u_Sequence_t* P_SEQUENCE)
{
	this->lastKnownMessageIndex++;	
	const bool INDEX_IS_AS_EXPECTED = (bool)(P_SEQUENCE->extract.head.messageIndex == lastKnownMessageIndex);

	return INDEX_IS_AS_EXPECTED;
}	

//---------------------------------------------------------------------------------------
bool BertaNetwork::thereIsAnewValidMessageForThisNode()
{
	bool THERE_IS_A_VALIDE_NEW_MESSAGE = false;

	if(this->networkParameter.serial->available() > 0) 
	{
		#ifdef DEBUG_RECORDING 
		Serial.print("THERE IS SOMETHING IN THE RX BUFFER"); 
		#endif	
		const bool THERE_IS_A_NEW_MESSAGE = this->recordEvent();		

		if(THERE_IS_A_NEW_MESSAGE)
		{
			THERE_IS_A_VALIDE_NEW_MESSAGE = this->validateReceivedMessage();

			if(THERE_IS_A_VALIDE_NEW_MESSAGE == false)
			{
				this->flushLastSequenceInMailbox();
			}
		}		
	}	
	return THERE_IS_A_VALIDE_NEW_MESSAGE;
}

//---------------------------------------------------------------------------------------
//Checksumme Validiert Nachricht, auch wenn eine Nachricht verpasst wurde und der Index deshalb nicht ok ist, kann wenn die Nachricht unbeschädgt ist ausgewertet werden
//Ist aber Checksumme falsch, ist Nachricht wertlos und muss erneut angefragt werden.
bool BertaNetwork::validateReceivedMessage()
{
	u_Sequence_t*	P_SEQUENCE 					= this->getLastSequneceInMailbox();						
	const bool 		CHECKSUM_OK					= this->checkFCS(P_SEQUENCE);
	bool 			MESSAGE_IS_FOR_THIS_NODE 	= false;		

	if(CHECKSUM_OK)
	{
		#ifdef DEBUG_RECORDING 
		Serial.println("CHECKSUM VALID!");  
		#endif		

		MESSAGE_IS_FOR_THIS_NODE = (bool)(P_SEQUENCE->extract.head.destinationAdress == this->networkParameter.adress || P_SEQUENCE->extract.head.destinationAdress == BROADCAST);				
		#ifdef DEBUG_RECORDING 
		if(MESSAGE_IS_FOR_THIS_NODE)
		{
			Serial.println("MESSAGE FOR THIS NODE"); 
		}
		else
		{
			Serial.println("MESSAGE NOT FOR THIS NODE");
		}			
		#endif					
		
		const bool	MESSAGE_INDEX_OK = this->checkMessageIndex(P_SEQUENCE);			
		if(MESSAGE_INDEX_OK)
		{
			#ifdef DEBUG_RECORDING 
			Serial.println("NETWORK IS IN SYNC");  
			#endif
			this->networkParameter.networkInSync 	= true;							
		}	
		else
		{
			#ifdef DEBUG_RECORDING 
			Serial.println("NETWORK IS OUT OF SYNC");  
			#endif
			this->networkState 						= networkState_init;
			this->networkParameter.networkInSync 	= false;
			this->lastKnownMessageIndex				= P_SEQUENCE->extract.head.messageIndex;
		}	
	} 			
	else
	{
		#ifdef DEBUG_RECORDING 
		Serial.println("MESSAGE CORRUPT!");  
		#endif
		this->initInterface();								
	}		

	const bool MESSAGE_IS_VALIDE_AND_FOR_THIS_NODE = (bool)(MESSAGE_IS_FOR_THIS_NODE == true && CHECKSUM_OK == true);
	Serial.print("MESSAGE_IS_VALIDE_AND_FOR_THIS_NODE: ");
	Serial.println(MESSAGE_IS_VALIDE_AND_FOR_THIS_NODE);
	
	return MESSAGE_IS_VALIDE_AND_FOR_THIS_NODE;
}

//---------------------------------------------------------------------------------------
bool BertaNetwork::recordEvent()
{
	uint8_t byteCount = 0;
	u_Sequence_t inSequence;
	memset(&inSequence, 0, SEQUENCE_LENGTH);	
		
	byteCount = this->networkParameter.serial->readBytes(inSequence.DATA, SEQUENCE_LENGTH);

#ifdef DEBUG_RECORDING
	Serial.println();
	Serial.println("IN_SEQUENCE EXTRACTED: ");
	Serial.print("DA: 	"); 	Serial.println(inSequence.extract.head.destinationAdress);
	Serial.print("SA: 	"); 	Serial.println(inSequence.extract.head.senderAdress);
	Serial.print("LEN: 	"); 	Serial.println(inSequence.extract.head.length);
	Serial.print("KEY: 	"); 	Serial.println(inSequence.extract.head.key);
	Serial.print("PORT:	"); 	Serial.println(inSequence.extract.head.portIndex);
	Serial.print("INDEX:"); 	Serial.println(inSequence.extract.head.messageIndex);
	Serial.print("res:	"); 	Serial.println(inSequence.extract.head.reserve);
	Serial.print("res: 	"); 	Serial.println(inSequence.extract.head.reserve_1);
	Serial.print("P1: 	"); 	Serial.println(inSequence.extract.payload.DATA[0]);
	Serial.print("P2: 	"); 	Serial.println(inSequence.extract.payload.DATA[1]);
	Serial.print("P3: 	"); 	Serial.println(inSequence.extract.payload.DATA[2]);
	Serial.print("P4: 	"); 	Serial.println(inSequence.extract.payload.DATA[3]);
	Serial.print("LO: 	"); 	Serial.println(inSequence.extract.checksum.loByte);
	Serial.print("HI: 	"); 	Serial.println(inSequence.extract.checksum.hiByte);
	Serial.print("byteCount: ");Serial.println(byteCount);
#endif

	this->to_busDelmiter.reset();	
	bool SEQUENCE_COMPLETE = (bool)(byteCount == SEQUENCE_LENGTH);
	
	if(SEQUENCE_COMPLETE)
	{
#ifdef DEBUG_RECORDING
		Serial.println("SEQUENCE IS COMPLETE");
#endif
		this->mailbox[this->inMessageCount] = inSequence;	
		this->inMessageCount++;	
		return true;
	}
	else
	{
#ifdef DEBUG_RECORDING
		Serial.println("TIMEOUT! SEQUENCE NOT COMPLETE");
#endif
		return false;
	}
}

//---------------------------------------------------------------------------------------
void BertaNetwork::flushInBuffer()
{	
	while (this->networkParameter.serial->available() > 0) 
	{
		this->networkParameter.serial->read();		
	}
}

/*#################################################################################
  SEQUENZ BAUEN UND VERSENDEN
  #################################################################################*/  
void BertaNetwork::buildAndSendCommand(uint8_t DESTINATION_ADRESS, uint8_t KEY, uint8_t PORT)
{
	//Sequenzkopf übertragen
	s_SequenceHead_t HEAD;
	memset(&HEAD, 0, sizeof(s_SequenceHead_t));	
	HEAD.senderAdress  		= this->networkParameter.adress;
	HEAD.destinationAdress 	= DESTINATION_ADRESS;
	HEAD.length			 	= 4;
	HEAD.key 				= KEY;
	HEAD.portIndex			 	= PORT;

	u_Payload_t PAYLOAD;	
	memset(&PAYLOAD, 0, sizeof(u_Payload_t));

	if(KEY == getSoftwareVersion)
	{
		delay(this->networkParameter.adress * 10);				
		PAYLOAD.DATA[0] = SoftwareVersion_major;
		PAYLOAD.DATA[1] = SoftwareVersion_minor;	
	}
	else if(KEY == busTiming)
	{
		PAYLOAD.DATA[0] = this->networkParameter.debuggingActive;
		PAYLOAD.DATA[1] = this->networkParameter.recordingTimeout_ms;
		PAYLOAD.DATA[2] = this->networkParameter.busDelimiter_ms;
		PAYLOAD.DATA[3] = this->networkParameter.responseTimeout_ms;
	}
	//CRC berechnen und Sequenz versenden
	calulateChecksumAndSendSequence(HEAD, PAYLOAD);
}

//---------------------------------------------------------------------------------------
void BertaNetwork::buildAndSendCommandWithPayload(uint8_t DESTINATION_ADRESS, uint8_t KEY, uint8_t PORT, u_Payload_t PAYLOAD)
{
	//Sequenzkopf übertragen
	s_SequenceHead_t HEAD;
	memset(&HEAD, 0, sizeof(s_SequenceHead_t));	
	HEAD.senderAdress  		= this->networkParameter.adress;
	HEAD.destinationAdress 	= DESTINATION_ADRESS;
	HEAD.length			 	= 4;
	HEAD.key 				= KEY;
	HEAD.portIndex			 	= PORT;
	//CRC berechnen und Sequenz versenden
	this->calulateChecksumAndSendSequence(HEAD, PAYLOAD);
}

//---------------------------------------------------------------------------------------
void BertaNetwork::calulateChecksumAndSendSequence(s_SequenceHead_t HEAD, u_Payload_t PAYLOAD)
{ 
	u_Sequence_t OUT_SEQUENCE;
	memset(&OUT_SEQUENCE, 0, sizeof(u_Sequence_t));	

	OUT_SEQUENCE.extract.head = HEAD;
	this->lastKnownMessageIndex++;
	OUT_SEQUENCE.extract.head.messageIndex 	= this->lastKnownMessageIndex;
	OUT_SEQUENCE.extract.head.reserve 		= 0;
	OUT_SEQUENCE.extract.head.reserve_1 	= 0;

	for(uint8_t i=0; i<PAYLOAD_LENGTH; i++)
	{
		OUT_SEQUENCE.extract.payload	= PAYLOAD;
	}

	OUT_SEQUENCE.extract.checksum = calculateFCS(OUT_SEQUENCE.DATA, SEQUENCE_DATABYTES_COUNT); 	
	

	#ifdef DEBUG_SENDING
	Serial.println("COMMAND TO SEND: ");
	Serial.print("DA: 	"); 	Serial.println(HEAD.destinationAdress);
	Serial.print("SA: 	"); 	Serial.println(HEAD.senderAdress);
	Serial.print("LEN: 	"); 	Serial.println(HEAD.length);
	Serial.print("KEY: 	"); 	Serial.println(HEAD.key);
	Serial.print("PORT: "); 	Serial.println(HEAD.portIndex);
	Serial.print("INDEX	"); 	Serial.println(HEAD.messageIndex);
	Serial.print("res: 	"); 	Serial.println(HEAD.reserve);
	Serial.print("res:	"); 	Serial.println(HEAD.reserve_1);
	Serial.println("PAYLOAD: ");
	Serial.print("1.byte: "); 	Serial.println(PAYLOAD.DATA[0]);
	Serial.print("2.byte: "); 	Serial.println(PAYLOAD.DATA[1]);
	Serial.print("3.byte: "); 	Serial.println(PAYLOAD.DATA[2]);
	Serial.print("4.byte: "); 	Serial.println(PAYLOAD.DATA[3]);
	Serial.println("CHECKSUM: ");
	Serial.print("CRC LO: "); 	Serial.println(OUT_SEQUENCE.extract.checksum.loByte);
	Serial.print("CRC HI: "); 	Serial.println(OUT_SEQUENCE.extract.checksum.hiByte);
	#endif

	while(to_busDelmiter.check() != true)
	{
#ifdef DEBUG_SENDING
	Serial.println("WAIT_FOR_BUS_DELIMTER_TIME");
#endif
	}

	digitalWrite(this->networkParameter.enablePin, HIGH);	
	this->networkParameter.serial->write(OUT_SEQUENCE.DATA, SEQUENCE_LENGTH);
	this->networkParameter.serial->flush();
	digitalWrite(this->networkParameter.enablePin, LOW);	
	to_busDelmiter.reset();		//Nach empfangen oder senden von Daten resetten
}

//---------------------------------------------------------------------------------------
bool BertaNetwork::checkFCS(u_Sequence_t* P_SEQUENCE)
{
	const s_CRC16_t FCS = calculateFCS(P_SEQUENCE->DATA, SEQUENCE_DATABYTES_COUNT);
	//Berechnete mit zuprüfende Checksumme vergleichen
	const bool FCS_OK = (bool)(FCS.loByte == P_SEQUENCE->extract.checksum.loByte && FCS.hiByte == P_SEQUENCE->extract.checksum.hiByte);

	#ifdef DEBUG_RECORDING
	Serial.print("RECEIVED LoByte: "); Serial.print(P_SEQUENCE->extract.checksum.loByte);Serial.print(", CALCULATED LoByte: "); Serial.println(FCS.loByte);
	Serial.print("RECEIVED HiByte: "); Serial.print(P_SEQUENCE->extract.checksum.hiByte);Serial.print(", CALCULATED HiByte: "); Serial.println(FCS.hiByte);
	#endif
	
	return FCS_OK;
}

//---------------------------------------------------------------------------------------
s_CRC16_t BertaNetwork::calculateFCS(uint8_t *DATA, uint8_t LENGTH)
{
	static unsigned char CRC_16Rev_Lo[] = 
	{   //CRC16
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
  };

	static unsigned char CRC_16Rev_Hi[] = 
	{   //CRC16
		0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2,
		0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
		0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
		0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
		0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
		0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
		0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
		0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
		0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
		0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
		0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE,
		0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
		0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA,
		0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
		0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
		0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62,
		0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
		0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE,
		0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
		0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
		0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
		0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76,
		0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
		0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
		0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
		0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
		0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
		0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A,
		0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86,
		0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
	};
	byte uchCRCHi = 0x00 ;        /* high CRC byte initialized */
	byte uchCRCLo = 0x00 ;        /* low CRC byte initialized */
	unsigned uIndex ;             /* will index into CRC lookup */

	//pass through message buffer
	while (LENGTH--)
	{
		uIndex = uchCRCLo ^ *DATA++ ;
		uchCRCLo = uchCRCHi ^ CRC_16Rev_Lo[uIndex] ;
		uchCRCHi = CRC_16Rev_Hi[uIndex] ;
	}
	//Checksumme an Kommando anhängen
	s_CRC16_t FCS;
	FCS.loByte = uchCRCLo;
	FCS.hiByte = uchCRCHi;
	return FCS;
}