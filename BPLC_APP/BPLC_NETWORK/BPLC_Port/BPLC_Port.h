#ifndef BPLC_Port_h
#define BPLC_Port_h
BPLC_Port
#include "Arduino.h"
#include "SpecialFunctions.h"

//--------------------------------------------------------------------------------------- 
//PORT DATATYPE
typedef enum
{
	eventPort, 	
	pushPort,
	readOnlyPort,	
}
e_PortType_t;
//---------------------------------------------------------------------------------------
//PAYLOAD TYPE
typedef union 
{
	uint8_t 	DATA[4];
	float 		FLOAT;
	bool 		BOOL;
	uint8_t		BYTE;
	int 		INT;
	s_RGB_t		RGB;
}
u_Payload_t;


//---------------------------------------------------------------------------------------
//BERTANET SLAVETPORT KLASSE
//---------------------------------------------------------------------------------------
class BertaPort
{
	public:
	//Constructoren
	BertaPort	();
	BertaPort	(e_PortType_t TYPE, uint8_t PORT_INDEX);							//Constructor für Eventport/readonlyport
	BertaPort	(e_PortType_t TYPE, uint8_t PORT_INDEX, uint32_t PUSH_INTERVALL);	//Constructor für Pushport
	//Setter	
	void			setBool		(bool 			PAYLOAD);
	void			setByte		(uint8_t 		PAYLOAD);
	void			setInt		(int 			PAYLOAD);
	void			setFloat	(float 			PAYLOAD);
	void			setRgb		(s_RGB_t 		PAYLOAD);
	void 			setData		(uint8_t BYTE, uint8_t PAYLOAD);
	//Getter	
	bool			getBool		();
	uint8_t			getByte		();
	int				getInt		();
	float			getFloat	();
	s_RGB_t			getRgb		();
	uint8_t			getData		(uint8_t BYTE);
	uint8_t*		getDataLocation();

	//Networkhandling
	void 			read		();	//Frägt Portdaten bei Master an
	u_Payload_t 	getPayload	();	//Für Network handling. Nicht in Appliakation zu nutzen, da flag nicht zurück gesetzt wird!
	void 			setPayload	(u_Payload_t 	PAYLOAD);	//Für Network handling. Nicht in Appliakation zu nutzen, da flag nicht zurück gesetzt wird!
	uint8_t			getIndex	();
	e_PortType_t    getType		();

	//Requestbuffer handling 
	bool 			isNewPortDataAvailableToSend	();	//return f_newPortDataToSend
	bool			isPortDataReqeuested			();	//Port Daten von Master lesen
	void			requestStoredInRequestBuffer	();
	void 			portDataStoredInRequestBuffer	();	//f_newPortDataToSend = false
	//Neue Portdaten verfügbar
	bool			newPortDataAvailable	();			//return f_newPortDataReceived;/ f_newPortDataReceived = false
	//PushPort
	void 	 		pushNow					();

	private:	 
	bool			f_newPortDataReceived;
	bool			f_newPortDataToSend;	
	bool 			f_portDataRequested;
	//Port definition
	uint8_t			index;	
	e_PortType_t	type;
	u_Payload_t		payload;	
	//Timeout für Pushport
	Timeout			to_push;		
};

//---------------------------------------------------------------------------------------
//BERTANET MASTER PORT KLASSE
//---------------------------------------------------------------------------------------
class BertaMasterPort
{
	public:
	BertaMasterPort	();
	//Networkhandling
	void 			setPayload		(u_Payload_t 	PAYLOAD);	
	u_Payload_t 	getPayload		();	
	//EEPROM
	bool			isStoredInEeprom		();
	uint16_t		getEepromLocation		();
	void 			setEepromLocation		(uint16_t EEPROM_LOCATION);
		
	private:	 
	//EEPROM
	uint16_t		eepromLocation;
	bool			storedInEeprom;
	//RAM Portdaten
	u_Payload_t		payload;	
};
#endif
