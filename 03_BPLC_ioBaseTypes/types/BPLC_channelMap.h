#ifndef BPLC_channelMap_h
#define BPLC_channelMap_h

#include "00_ioInterface.h"

#define BPLC_CHANNEL_MAP_MAX_SIZE 8

class BPLC_channelMap
{
    public:
                            BPLC_channelMap      ()
                            {
                                for(uint8_t channel = 0; channel < BPLC_CHANNEL_MAP_MAX_SIZE; channel++)
                                {
                                    this->p_ioObject[channel] = nullptr;
                                }
                            }
    bool                    isChannelInRange     (const uint8_t CHANNEL, const uint8_t CHANNEL_COUNT) const
                            {
                                return (CHANNEL >= 1)
                                    && ((uint8_t)CHANNEL <= CHANNEL_COUNT)
                                    && (CHANNEL_COUNT <= BPLC_CHANNEL_MAP_MAX_SIZE);
                            }
    bool                    isChannelFree        (const uint8_t CHANNEL, const uint8_t CHANNEL_COUNT) const
                            {
                                return this->isChannelInRange(CHANNEL, CHANNEL_COUNT)
                                    && (this->p_ioObject[(uint8_t)CHANNEL - 1] == nullptr);
                            }
    IO_Interface*           get                  (const uint8_t CHANNEL_INDEX) const
                            {
                                if(CHANNEL_INDEX >= BPLC_CHANNEL_MAP_MAX_SIZE)
                                {
                                    return nullptr;
                                }
                                return this->p_ioObject[CHANNEL_INDEX];
                            }
    void                    set                  (const uint8_t CHANNEL_INDEX, IO_Interface* P_IO_OBJECT)
                            {
                                if(CHANNEL_INDEX < BPLC_CHANNEL_MAP_MAX_SIZE)
                                {
                                    this->p_ioObject[CHANNEL_INDEX] = P_IO_OBJECT;
                                }
                            }

    private:
    IO_Interface*           p_ioObject[BPLC_CHANNEL_MAP_MAX_SIZE];
};
#endif
