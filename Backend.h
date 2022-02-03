#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJSValueList>
#include <QQuickItem>
#include <chrono>
#include <mutex>
#include <unistd.h>
#include <memory.h>
#include <future>
#include <cmath>

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float acceleration READ getAcceleration WRITE setAcceleration)
    Q_PROPERTY(float steer READ getSteer WRITE setSteer)
    Q_PROPERTY(bool swCAN READ getSw WRITE setSw)
    
    
public:
    explicit BackEnd(QObject *parent = nullptr);
    float getAcceleration();
    float getSteer();
    bool getSw();

    void setAcceleration(float acc);
    void setSteer(float stier);
    void setSw(bool sw);

    Q_INVOKABLE void resetPosition();
    Q_INVOKABLE void initCANConnection();

private:
    void initObjectDictionary();

    float m_acceleration;
    float m_steer;
    bool m_swCAN = false;
    int  m_socketCan;
    uint8_t m_data[8];

    void sendCAN(struct can_frame&);
    struct can_frame createSDO(const uint8_t address, const bool &doWrite, int indexSubindex, int dataType, const long &value);
    struct can_frame m_canFrameWrite;

    void sendSteer(uint32_t value);
    void sendAcceleration(uint32_t value);
    

};

#endif // BACKEND_H