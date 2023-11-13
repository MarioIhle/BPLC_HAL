#include "BPLC_Node.h"
//---------------------------------------------------------------------------------------
//SALVE
//---------------------------------------------------------------------------------------
BertaSlave::BertaSlave(uint8_t ADRESS, HardwareSerial* P_SERIAL, uint8_t ENABLE_PIN, s_usedPorts_t* P_PORTS)
{
	memset(&this->networkParameter, 0, sizeof(s_networkParameter_t));	
	this->networkParameter.serial			= P_SERIAL;
	this->networkParameter.adress 			= ADRESS; 
	this->networkParameter.enablePin 		= ENABLE_PIN; 
	this->networkParameter.debuggingActive 	= false;
	this->p_ports     						= P_PORTS;	
	this->initializedPorts					= 0;

	this->requestToSend = 0;  
	memset(this->busRequestBuffer, 0, sizeof(busRequestBuffer));
	for(uint8_t i=0; i< BUSREQUEST_BUFFER_SIZE; i++)
	{
		this->busRequestBuffer[i].key = noRequest;
	}
}
 
//---------------------------------------------------------------------------------------
void BertaSlave::begin()
{
#ifdef DEBUG_SLAVE
	Serial.begin(115200);
  	Serial.println("INIT BERTANETPORT SLAVE");

	for(int i = 0; i<this->p_ports->count; i++)
	{
		Serial.print("INDEX OF PORT ["); Serial.print(i); Serial.print("]:"); Serial.println(this->p_ports->port[i]->getIndex());  		
	}		
	this->networkParameter.debuggingActive = true;
	delay(1500);
#endif	
	//Netwerkparameter übergeben
	this->network.begin(this->networkParameter);	
	this->setBusRequest(BROADCAST, busTiming, NO_PORT);	
	
	//PortCount berechen
	for(uint8_t i = 0; i<PORT_COUNT_MAX; i++)
	{
		if(this->p_ports->port[i]!= NULL)
		{
			this->p_ports->count++;
		}
	}
#ifdef DEBUG_SLAVE
	Serial.print("PORT_COUNT:	"); Serial.print(this->p_ports->count);Serial.print("/");Serial.println(PORT_COUNT_MAX);
	delay(5000);
#endif
}

//---------------------------------------------------------------------------------------
void BertaSlave::tick()
{
	#ifdef DEBUG_SLAVE 
	Serial.print("NETWORKSTATE:"); Serial.println(this->network.getNetworkState());
	#endif
   	switch(this->network.getNetworkState())
  	{
		case networkState_init:
			this->initializedPorts = 0;
			this->network.setNetworkState(networkState_resync);
		break;

		case networkState_resync:
			this->resyncNetwork();
		break;

		case networkState_debugging:
		break;

		case networkState_stopped:
			if(this->network.checkMailbox(false))
			{
				this->executeCommand();
			}		
		break;

		case networkState_running:	 
			//Neue Schreib oder Leseanfrage in Requestbuffer überführen
			this->checkPorts();  

			//Warten auf Masterpoll oder andere Nachricht für diese Node
			if(this->network.checkMailbox(false))	
			{
				//Nachricht für diese Node erhalten
				#ifdef DEBUG_SLAVE 
				Serial.println("THERE_IS_SOMETHING_IN_THE_MAILBOX"); 	
				#endif
				//Kommando ausführen
				if(this->executeCommand())
				{
					#ifdef DEBUG_SLAVE 
					Serial.println("WAITING_FOR_RESPONSE"); 	
					#endif
					//Eventuell auf Antwort warten
					if(this->network.checkMailbox(true))
					{
						this->executeCommand();
					}
				}
			} 
			else
			{
				#ifdef DEBUG_SLAVE 
				Serial.println("THERE_IS_NOTHING_IN_THE_MAILBOX"); 	
				#endif
			}  
		break;
  	}	    
}

//---------------------------------------------------------------------------------------
bool BertaSlave::executeCommand()
{
 	bool WAIT_FOR_RESPONSE 		= false;		
	u_Sequence_t* P_SEQUENCE 	= this->network.getLastSequneceInMailbox();
	
#ifdef DEBUG_SLAVE_ADVANCED
	Serial.print("COMMAND_TO_EXECUTE: ");
	for(int i=0;i<11;i++)
	{
		Serial.print(P_SEQUENCE->DATA[i]); Serial.print("_");
	}		
	Serial.println();
#endif

	switch(P_SEQUENCE->extract.head.key)
	{
		case readPort:	
			this->network.buildAndSendCommandWithPayload(P_SEQUENCE->extract.head.senderAdress, readResponse, P_SEQUENCE->extract.head.port, this->getPortData(P_SEQUENCE->extract.head.port));
		break;

		case readResponse:      
			this->saveReceivedPortData(P_SEQUENCE->extract.head.port, P_SEQUENCE->extract.payload); 
			this->flushRequestAndSkipToNextSlot();		

			if(this->network.getNetworkState() == networkState_resync)
			{				
				this->initializedPorts++;
			}					
		break;

		case writePort:      
			this->saveReceivedPortData(P_SEQUENCE->extract.head.port, P_SEQUENCE->extract.payload);     	
		break;

		case writePushPort:
			this->saveReceivedPortData(P_SEQUENCE->extract.head.port, P_SEQUENCE->extract.payload);     	
		break;

		case poll:			
			if(this->thereARequestToSend())
			{    				
				if(this->sendBusRequest())	//Antwort erwartet?
				{
					WAIT_FOR_RESPONSE = true;
				}
				else
				{
					this->flushRequestAndSkipToNextSlot();
				}
			}
			else
			{				
				this->network.buildAndSendCommand(MASTER_NODE, noRequest, NO_PORT);				
			}
		break;

		case noRequest:
		break;
		
		case key_ACK:
			this->flushRequestAndSkipToNextSlot();					
		break;

		case key_NAK:
		break;

		case getSoftwareVersion:
			this->network.buildAndSendCommand(P_SEQUENCE->extract.head.senderAdress, getSoftwareVersion, NO_PORT);
		break;

		case bussystemStop:
			this->network.stopBussystem();
		break;

		case bussystemStart:
			this->network.startBussystem();
		break;

		case softReset:
			while(true)
			{};
		break;

		case HB_ping:
			this->network.buildAndSendCommand(P_SEQUENCE->extract.head.senderAdress, HB_pong, NO_PORT);
		break;

		case HB_pong:
		break;

		case busTiming:  		
			if(this->networkParameter.debuggingActive == false)
			{
				this->networkParameter.debuggingActive = P_SEQUENCE->extract.payload.DATA[0];				
				this->network.setNetworkTiming(this->networkParameter.debuggingActive); 	
			}							
		break;
	} 
	//Letzte Nachricht wird nicht mehr benötigt, alle Aktionen ausgeführt
	this->network.flushLastSequenceInMailbox();	
	return WAIT_FOR_RESPONSE;
}

//---------------------------------------------------------------------------------------
void BertaSlave::checkPorts()
{	
	//Alle verfügbaren Ports iterieren
	for(uint8_t P= 0; P < this->p_ports->count; P++)
	{		
		bool REQUEST_IS_SAVED = false;
		uint8_t	KEY;

		//Neue Portdaten zu versenden
		if(this->p_ports->port[P]->isNewPortDataAvailableToSend())
		{
			#ifdef DEBUG_SLAVE
			Serial.print("THERE IS NEW PORT DATA TO SEND AT PORTINDEX "); Serial.println(this->p_ports->port[P]->getIndex());
			#endif	
			
			switch(this->p_ports->port[P]->getType())
			{
				case eventPort:
					KEY = writePort;
				break;
				
				case pushPort:
					KEY = writePushPort;
				break;
			}

			REQUEST_IS_SAVED = this->setBusRequest(BROADCAST, KEY, this->p_ports->port[P]->getIndex());

			if(REQUEST_IS_SAVED)
			{
				#ifdef DEBUG_SLAVE 
				Serial.println("AND ITS SAVED IN THE REQUEST BUFFER"); 				
				#endif
				this->p_ports->port[P]->portDataStoredInRequestBuffer();				
			}	
			else
			{
				#ifdef DEBUG_SLAVE 
				Serial.println(" BUT THE REQUEST BUFFER iS FULL"); 
				#endif
				return;//Da Request Buffer voll
			}				
		}		
		//Portdaten von Master lesen
		else if(this->p_ports->port[P]->isPortDataReqeuested())		
		{
			#ifdef DEBUG_SLAVE
			Serial.print("THERE IS NEW PORT DATA REQUESTED AT PORTINDEX "); Serial.println(this->p_ports->port[P]->getIndex());
			#endif	
			REQUEST_IS_SAVED = this->setBusRequest(MASTER_NODE, readPort, this->p_ports->port[P]->getIndex());

			if(REQUEST_IS_SAVED)
			{
				#ifdef DEBUG_SLAVE 
				Serial.println("AND ITS SAVED IN THE REQUEST BUFFER"); 				
				#endif
				this->p_ports->port[P]->requestStoredInRequestBuffer();				
			}	
			else
			{
				#ifdef DEBUG_SLAVE 
				Serial.println(" BUT THE REQUEST BUFFER iS FULL"); 
				#endif
				return;//Da Request Buffer voll
			}			
		}
		else
		{
			//do nothing
		}
	}
	#ifdef DEBUG_SLAVE_ADVANCED 
	for(uint8_t i=0; i< 10; i++)
	{
		Serial.print("REQUEST SLOT: "); Serial.println(i);
		Serial.print("DA: 	"); Serial.println(this->busRequestBuffer[i].destinationAdress);
		Serial.print("KEY: 	"); Serial.println(this->busRequestBuffer[i].key);
		Serial.print("PORT:	"); Serial.println(this->busRequestBuffer[i].port);
	}
	#endif
}

//---------------------------------------------------------------------------------------
u_Payload_t  BertaSlave::getPortData(uint8_t PORT)
{
#ifdef DEBUG_SLAVE 
		Serial.print("SEARCHING PORT: "); Serial.println(PORT);
#endif

	for(uint8_t P= 0; P<this->p_ports->count; P++)
	{		
		#ifdef DEBUG_SLAVE		
		Serial.print("PORT: "); Serial.print(P+1); Serial.print("/"); Serial.println(this->p_ports->count); 
		#endif
		
		if(this->p_ports->port[P]->getIndex() == PORT)
		{ 
			#ifdef DEBUG_SLAVE 
			Serial.println("PORT FOUND!");
			#endif			

			return this->p_ports->port[P]->getPayload();					
		}	
	}	
}

//---------------------------------------------------------------------------------------
void BertaSlave::saveReceivedPortData(uint8_t PORT, u_Payload_t PAYLOAD)
{
	#ifdef DEBUG_SLAVE 
	Serial.print("SEARCH PORT TO SAVE: "); Serial.println(PORT);
	#endif

	for(uint8_t P= 0; P < this->p_ports->count; P++)
	{
		#ifdef DEBUG_SLAVE		
		Serial.print("PORT: "); Serial.print(P+1); Serial.print("/"); Serial.println(this->p_ports->count); 
		#endif

		if(P < this->p_ports->count)
		{
			if(this->p_ports->port[P]->getIndex() == PORT)
			{ 
				#ifdef DEBUG_SLAVE 
				Serial.println("PORT FOUND!");
				#endif				
				this->p_ports->port[P]->setPayload(PAYLOAD);				
				return;
			}					
		}		
	}	  	
}

//---------------------------------------------------------------------------------------
bool BertaSlave::sendBusRequest()
{
	s_busRequest_t* P_REQUEST 			= &this->busRequestBuffer[this->requestToSend];	
	const bool 		COMMAND_HAS_PAYLAOD = (bool)(P_REQUEST->key == writePort || P_REQUEST->key == writePushPort);
	if(COMMAND_HAS_PAYLAOD)
	{
		this->network.buildAndSendCommandWithPayload(P_REQUEST->destinationAdress, P_REQUEST->key, P_REQUEST->port, this->getPortData(P_REQUEST->port));
	}
	else
	{
		this->network.buildAndSendCommand(P_REQUEST->destinationAdress, P_REQUEST->key, P_REQUEST->port);
	}	
	//Antwort erwartet?
	return (bool)(P_REQUEST->key == writePort || P_REQUEST->key == readPort);
}

//---------------------------------------------------------------------------------------
bool BertaSlave::setBusRequest(uint8_t DESTINATION_ADRESS, uint8_t KEY, uint8_t PORT)
{	
	bool REQUEST_IS_SAVED 	= false;
	
	if (this->isThereAFreeBufferSlot())
	{		
		uint8_t REQUEST_SLOT = getNextFreeBufferSlot();

		this->busRequestBuffer[REQUEST_SLOT].destinationAdress 	= DESTINATION_ADRESS;
		this->busRequestBuffer[REQUEST_SLOT].key 				= KEY;	
		this->busRequestBuffer[REQUEST_SLOT].port 				= PORT;	

		#ifdef DEBUG_SLAVE 
		Serial.print("REQUEST STORED AT SLOT: "); Serial.println(REQUEST_SLOT);
		Serial.print("DA: 	"); Serial.println(this->busRequestBuffer[REQUEST_SLOT].destinationAdress);
		Serial.print("KEY: 	"); Serial.println(this->busRequestBuffer[REQUEST_SLOT].key);
		Serial.print("PORT:	"); Serial.println(this->busRequestBuffer[REQUEST_SLOT].port);
		#endif	
		REQUEST_IS_SAVED = true;					
	}	
	return REQUEST_IS_SAVED;
}

//---------------------------------------------------------------------------------------
bool BertaSlave::isThereAFreeBufferSlot()
{
	for(uint8_t i=0; i < BUSREQUEST_BUFFER_SIZE; i++)		
	{
		if(this->busRequestBuffer[i].key == noRequest)
		{
			return true;		
		}
	}
	return false;
}

//---------------------------------------------------------------------------------------
bool BertaSlave::thereARequestToSend()
{
	const bool THERE_IS_A_REQUEST_TO_SEND = (bool)(this->busRequestBuffer[this->requestToSend].key != noRequest);
	
#ifdef DEBUG_SLAVE
	Serial.print("THERE IS "); 	 
	if(THERE_IS_A_REQUEST_TO_SEND)
	{
		Serial.println("A REQUEST TO SEND"); 	
	}
	else
	{
		Serial.println("NO REQUEST TO SEND"); 	
	}
#endif

	return THERE_IS_A_REQUEST_TO_SEND;
}

//---------------------------------------------------------------------------------------
uint8_t BertaSlave::getNextFreeBufferSlot()
{
	uint8_t SLOT = this->requestToSend;

	for(uint8_t i=0; i < BUSREQUEST_BUFFER_SIZE; i++)		
	{
		if(this->busRequestBuffer[SLOT].key == noRequest)
		{
			return SLOT;		
		}
		else
		{
			SLOT = skipToNextRequestBufferSlot(SLOT);
		}
	}
	return SLOT;
}	

//---------------------------------------------------------------------------------------
void BertaSlave::flushRequestAndSkipToNextSlot()
{
	memset(&this->busRequestBuffer[this->requestToSend], 0, sizeof(s_busRequest_t));
	this->busRequestBuffer[this->requestToSend].key = noRequest;
	this->requestToSend = skipToNextRequestBufferSlot(this->requestToSend);
}

//---------------------------------------------------------------------------------------
uint8_t BertaSlave::skipToNextRequestBufferSlot(uint8_t SLOT)
{	
	SLOT++;
	
	if(SLOT >= BUSREQUEST_BUFFER_SIZE)
	{
		SLOT = 0;
	}
	return SLOT;
}

//---------------------------------------------------------------------------------------
void BertaSlave::resyncNetwork()
{
	//Warten auf Masterpoll oder andere Nachricht für diese Node
	if(this->network.checkMailbox(false))	
	{
		//Kommando ausführen
		if(this->executeCommand())	//this->initializedPorts++; ->Wenn readResponse empfangen
		{					
			//Auf Antwort von Master warten
			if(this->network.checkMailbox(true))
			{
				this->executeCommand();
			}
		}
	} 

	//Alle Ports abgefragt, Normalbetrieb starten
	if(this->initializedPorts >= this->p_ports->count)
	{
		this->network.setNetworkState(networkState_running);
#ifdef DEBUG_SLAVE
		Serial.println("ALL PORTS INITIALIZED");
#endif
	}
	//Nächsten Port abfragen
	else if(this->thereARequestToSend() == false)
	{
#ifdef DEBUG_SLAVE
		Serial.print("REQUEST PORT: "); Serial.println(this->initializedPorts);
#endif
		this->setBusRequest(MASTER_NODE, readPort, this->p_ports->port[this->initializedPorts]->getIndex());
	}
}