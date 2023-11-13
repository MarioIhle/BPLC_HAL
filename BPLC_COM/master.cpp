#include "BPLC_COM.h"
  
//---------------------------------------------------------------------------------------
//MASTER
//---------------------------------------------------------------------------------------
BPLC_MasterNode::BPLC_MasterNode(uint8_t ADRESS, HardwareSerial* P_SERIAL, uint8_t ENABLE_PIN)
{
	memset(&this->networkParameter, 0, sizeof(s_networkParameter_t));	
	this->networkParameter.adress			= ADRESS;
	this->networkParameter.serial			= P_SERIAL;
	this->networkParameter.enablePin 		= ENABLE_PIN;
	this->networkParameter.debuggingActive 	= false;
	
	memset(&this->allPorts, 0, sizeof(this->allPorts));		

	this->slaveCount  					= 0;
	this->to_poll.setInterval(POLL_DELIMITER);
	this->to_attachNewNode.setInterval(5000);	
}

//---------------------------------------------------------------------------------------
void BPLC_MasterNode::begin()
{
#ifdef DEBUG_MASTER
	Serial.begin(115200);
 	Serial.println("MASTER_DEBUGGING");  	
	this->networkParameter.debuggingActive = true;
	delay(1500);
#endif	
	//Netwerkparameter übergeben	
	this->network.begin(this->networkParameter);
	this->network.buildAndSendCommand(BROADCAST, busTiming, NO_PORT);

	//PortDaten aus Flash wiederherstellen
	//this->recoverPortData();

	//SlaveCount bestimmen
	this->slaveCount 		= 0;	//Startadresse der Slave suche
	this->slaveToPoll		= 2;	//Adresse des gepollten Slave
	
	//Weitere Slaves suchen
	bool 			SEARCH_FOR_SLAVES 	= true;
	const uint8_t 	PING_COUNT 			= 3; 

	delay(5000);		//Slaves zeit geben zu booten
	
	while(SEARCH_FOR_SLAVES)
	{	
		const uint8_t PREVIOUS_SLAVE_COUNT = this->slaveCount;

		for(uint8_t anmeldeVersuche= 0; anmeldeVersuche< PING_COUNT; anmeldeVersuche++)
		{
			this->network.buildAndSendCommand((this->slaveCount + HOST_AND_MASTER_ADRESS), HB_ping, NO_PORT);	
			
			if(this->network.checkMailbox(true))
			{				
				//Bei Antworkt von neuer Node, wird SlaveCount um 1 erhöht und die nächste adresse angefragt
				this->executeCommand();
				//Wenn Node gefunden anpingen abbrechen
				if(PREVIOUS_SLAVE_COUNT < this->slaveCount)
				{				
					anmeldeVersuche = PING_COUNT;
				}
			}
		}
		//Kein neuer Slave gefunden oder maximale Anzahl Slaves erreicht, Suche abbrechen
		if(PREVIOUS_SLAVE_COUNT == this->slaveCount || this->slaveCount >= SLAVE_COUNT_MAX)
		{		
			SEARCH_FOR_SLAVES = false;
		}		
	};	



	this->network.setNetworkState(networkState_running);

	#ifdef DEBUG_MASTER
	Serial.print("SLAVE_COUNT:	"); Serial.println(this->slaveCount);
	Serial.print("PORT_COUNT:	"); Serial.println(PORT_COUNT);
	delay(2500);
	#endif
}

//---------------------------------------------------------------------------------------
void BPLC_MasterNode::tick()
{
	switch(this->network.getNetworkState())
  	{
		//Master benötigt keine Syncronisierung
		case networkState_init:
			this->network.setNetworkState(networkState_running);
		break;

		case networkState_resync:
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
			if(this->to_poll.check())
			{					
				this->setupNextPoll();				
				this->to_poll.reset();		

				while(this->pollCount < POLL_COUNT_MAX)	
				{					
					this->pollSlave();	

					if(this->network.checkMailbox(true))
					{
						this->executeCommand();
					}
				}													
			}
			else
			{
				if(this->network.checkMailbox(false))
				{
					#ifdef DEBUG_MASTER 
					Serial.print("THERE_IS_A_COMMAND_TO_EXECUTE"); 
					#endif
					this->executeCommand();
				}
			}
    break;
  	}	  
}

//---------------------------------------------------------------------------------------
void BPLC_MasterNode::pollSlave()
{
	#ifdef DEBUG_MASTER 
	Serial.print("POLL_SLAVE_NODE_NO: "); Serial.print(this->slaveToPoll); 
	Serial.print("  VERSUCH: "); Serial.print(this->pollCount + 1);	Serial.print("/"); Serial.println(POLL_COUNT_MAX);	
	#endif
	this->network.buildAndSendCommand(this->slaveToPoll, poll, NO_PORT);
	this->pollCount++;
}

//---------------------------------------------------------------------------------------
void BPLC_MasterNode::setupNextPoll()
{
	this->slaveToPoll++;
	this->pollCount = 0;
	
	if (this->slaveToPoll < 2)
	{
		this->slaveToPoll = 2;
	}
	if (this->slaveToPoll >= this->slaveCount + 2)	//0=HostPc, 1= Master
	{
		//Alle 5s versuchen new Node anzupingen
		if(this->to_attachNewNode.check())
		{			
			this->to_attachNewNode.reset();			
		}
		else
		{
			this->slaveToPoll = 2;
		}
	}
}

//---------------------------------------------------------------------------------------
void BPLC_MasterNode::executeCommand()
{
	u_Sequence_t* 	P_SEQUENCE = this->network.getLastSequneceInMailbox();

	//Nachricht von Richtiger Node empfangen
	if(P_SEQUENCE->extract.head.senderAdress == this->slaveToPoll)	
	{		
		//Bei neu erkannter Node, Slavecount erhöhen
		if(P_SEQUENCE->extract.head.senderAdress == (this->slaveCount + HOST_AND_MASTER_ADRESS))
		{
			this->slaveCount++;
		}

		//Key auswerten
		switch(P_SEQUENCE->extract.head.key)
		{
			case readPort:	
				this->network.buildAndSendCommandWithPayload(P_SEQUENCE->extract.head.senderAdress, readResponse, P_SEQUENCE->extract.head.portIndex, this->allPorts[P_SEQUENCE->extract.head.portIndex].getPayload());
			break;

			case writePort:      
				this->saveReceivedPortData(P_SEQUENCE->extract.head.portIndex, P_SEQUENCE->extract.payload);  
				this->network.buildAndSendCommand(P_SEQUENCE->extract.head.senderAdress, key_ACK, P_SEQUENCE->extract.head.portIndex);
			break;

			case writePushPort:					
				this->saveReceivedPortData(P_SEQUENCE->extract.head.portIndex, P_SEQUENCE->extract.payload);     	
			break;

			case noRequest:									
			break;
			
			case key_ACK:
			break;

			case key_NAK:
			break;

			case getSoftwareVersion:
				this->network.buildAndSendCommand(P_SEQUENCE->extract.head.senderAdress, getSoftwareVersion, NO_PORT);
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

			case bussystemStop:
				this->network.stopBussystem();
			break;

			case bussystemStart:
				this->network.startBussystem();
			break;
		} 	
		this->pollCount = POLL_COUNT_MAX;	
	}

	this->network.flushLastSequenceInMailbox();
}

//---------------------------------------------------------------------------------------
void BPLC_MasterNode::saveReceivedPortData(uint8_t PORT, u_Payload_t PAYLOAD)
{
	#ifdef DEBUG_MASTER 
	Serial.print("SEARCH PORT TO SAVE: "); Serial.println(PORT);
	#endif

	this->allPorts[PORT].setPayload(PAYLOAD);

	if(this->isPortSavedInEeprom[PORT])
	{	
		for(uint8_t i= 0; i<PAYLOAD_LENGTH; i++)
		{				
			//EEPROM.write(this->allPorts[PORT].getEepromLocation()+i, PAYLOAD.DATA[i]);					
		}	
		//EEPROM.commit();			
	}		  	
}

//---------------------------------------------------------------------------------------
void BPLC_MasterNode::recoverPortData()
{	
	memset(this->isPortSavedInEeprom, 0, sizeof(this->isPortSavedInEeprom));
	//Ports die im EEPROM gespeichert werden
	this->isPortSavedInEeprom[1] = true;

	uint16_t eepromPortDataLocation = 0;	
	for(uint8_t portIndex= 1; portIndex < sizeof(this->isPortSavedInEeprom); portIndex++)
	{
		if(this->isPortSavedInEeprom[portIndex])
		{
			this->allPorts[portIndex].setEepromLocation(eepromPortDataLocation);
			eepromPortDataLocation += PAYLOAD_LENGTH;
		}
	}

	EEPROM.begin(eepromPortDataLocation);
	
	for(uint8_t portIndex= 1; portIndex < sizeof(this->isPortSavedInEeprom); portIndex++)
	{
		if(this->isPortSavedInEeprom[portIndex])
		{				
			//Daten aus EEPROM lesen
			u_Payload_t PORTDATA;
			memset(&PORTDATA, 0, sizeof(u_Payload_t));

			for(uint8_t i=0; i<PAYLOAD_LENGTH; i++)
			{
				PORTDATA.DATA[i] = EEPROM.read(this->allPorts[portIndex].getEepromLocation() + i);
			}
			this->allPorts[portIndex].setPayload(PORTDATA);
			#ifdef DEBUG_MASTER	
			Serial.print("RECOVER DATA OUT OF THE EEPROM FROM PORT: "); Serial.print(portIndex); Serial.print(" | DATA: ");
			Serial.print(PORTDATA.DATA[0]);Serial.print("_");Serial.print(PORTDATA.DATA[1]);Serial.print("_");Serial.print(PORTDATA.DATA[2]);Serial.print("_");Serial.println(PORTDATA.DATA[3]);
			#endif
		}		
	}
}