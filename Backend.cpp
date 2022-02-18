#include "Backend.h"

#define SLEEP_ms(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

#define DO_READ 1

BackEnd::BackEnd(QObject *parent) : QObject(parent){
    initObjectDictionary();

    m_canFrameWrite.can_dlc = 1;

};

void BackEnd::initObjectDictionary()
{


}

void BackEnd::initCANConnection()
{
    if (m_swCAN == true)
    {
        m_socketCan = socket(PF_CAN, SOCK_RAW, CAN_RAW); //Open a socket for communicating over a CAN network

        struct sockaddr_can addr;

        struct ifreq ifr;

        strcpy(ifr.ifr_name, "can0");

        ioctl(m_socketCan, SIOCGIFINDEX, &ifr);

        addr.can_family = AF_CAN;

        addr.can_ifindex = ifr.ifr_ifindex;

        bind(m_socketCan, (struct sockaddr *) &addr, sizeof(addr));

        qDebug() << "init connection\n";

    }else{
        qDebug() << "Turn on the CAN connection switch";
    }


}

float BackEnd::getAcceleration()
{
   return m_acceleration;
}

float BackEnd::getSteer()
{
   return m_steer;
}

void BackEnd::setAcceleration(float acc)
{
    m_acceleration = acc;
    // Para mandar o valor para CAN, como o intervalo do slide é de -1 a 1, 
    // soma-se 1 ao valor e multiplica-se por 10 para eliminar o passo. 
    m_acceleration = (m_acceleration + 1) * 10 ;
    qDebug() << "Backend acc " << (u_int32_t) m_acceleration; 

    sendAcceleration((u_int32_t) m_acceleration);
}

void BackEnd::setSteer(float str)
{
    m_steer = str;
    // Para mandar o valor para CAN, como o intervalo do slide é de -1 a 1, 
    // soma-se 1 ao valor e multiplica-se por 10 para eliminar o passo. 
    m_steer = (str + 1) * 10 ;
    qDebug() << "CAN steer is " << (uint32_t) m_steer;
    sendSteer((uint32_t) m_steer);
    
}

bool BackEnd::getSw()
{
    return m_swCAN;
}

void BackEnd::setSw(bool sw)
{
    m_swCAN = sw;
    qDebug() << "Backend str " << m_steer;
}

// Functions to manage CAN messages
void BackEnd::sendCAN(struct can_frame& canFrame)
{
    if(m_swCAN)
    {
        write(m_socketCan, &canFrame, sizeof(struct can_frame));
    }
}

void BackEnd::sendAcceleration(uint32_t value)
{
    if(m_swCAN)
    {
        qDebug() << "sending profilePosition" << value;
        can_frame m_canAcceleration { .can_id = 0x001 , .can_dlc = m_canFrameWrite.can_dlc};
        m_canAcceleration.data[0] = ( value & 0xFF );
        sendCAN(m_canAcceleration);
        SLEEP_ms(10);
    }
}

void BackEnd::sendSteer(uint32_t value) //steer
{
    if(m_swCAN == true)
    {
        qDebug() << "sending profilePosition" << value;
        can_frame m_canSteer { .can_id = 0x002 , .can_dlc = m_canFrameWrite.can_dlc};
        m_canSteer.data[0] = ( value & 0xFF );
        sendCAN(m_canSteer);
        SLEEP_ms(10);
    }
}

void BackEnd::resetPosition() //reset position
{
    can_frame m_restPos {.can_id = 0x003, .can_dlc = 1};
    m_restPos.data[0] = 0xFF;
    for(int i = 1; i < 8; i++)
    {
        m_restPos.data[i] = 0;
    }
    sendCAN(m_restPos);
    SLEEP_ms(10);
    qDebug() << "Position (x,y) = (0,0)";
}