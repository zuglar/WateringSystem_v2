/*  !!! All information are in DataSheets folder !!!
*
*   Connection:
*   MicroSD_M pin  NodeMCU-32S pin
*       GND --------------------------- Power GND
*       Vcc --------------------------- Power 5 V
*       MOSI ------ MOSI-GPIO23
*       MISO ------ MISO-GPIO19
*       SCK  ------ VSPI SCK-GPIO18
*       CS   ------ VSPI SS-GPIO5    
*/
/* SDCard header */
#ifndef __SDCARD_H__
#define __SDCARD_H__

class SDCard
{
private:
    /* data */
public:
    SDCard(/* args */);
    ~SDCard();
};


#endif /* __SDCARD_H__ */