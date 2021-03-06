/*
 * telemetryNode.h v1.1
 * Created: 7/10/18 by Andrew Gutierrez and Chris Kjellqvist
 * Modified: 12/15/18
 *
 * Arduino library for telemetry node for URSS. Library is deisnged to
 * Serve as the serial interface between the various "node" boards in the
 * boat and the telemetry server that collects all the data.
 *
 * v1.0
 * Fully functional library for telemetry streaming to
 * Telemtry-Server-SDK. Support for the following boards:
 *  - URSS Alltrax Controller
 *  - URSS VESC Controller
 *  - URSS Motor board
 *  - URSS Battery Board
 *  - URSS GPS/IMU Board
 *  - URSS Throttle Board
 *
 * v1.1
 * Added support for full duplex communication with heartbeat packets.
 * Heartbeats can now send data back to the nodes with data requests.
 * Added unpacking functions for:
 * - URSS Alltrax Controller
 * - URSS VESC Controller
 */
 #ifndef TELEMETRY_NODE_H
 #define TELEMETRY_NODE_H

 #include "Arduino.h"

 struct Packet{
   uint8_t startByte;
   uint8_t data[13];
   uint8_t packetNum;
   uint8_t checksum;
 };

 enum DeviceID {
   DEVICE_ALLTRAX,
   DEVICE_VESC,
   DEVICE_MOTOR_BOARD,
   DEVICE_BATTERY_BOARD,
   DEVICE_GPS_IMU,
   DEVICE_THROTTLE,
   DEVICE_SOLAR
 };

 enum ReciveState {
   HEARTBEAT_WAITING,
   HEARTBEAT_RECEIVING
 };

 class TelemetryNode{
   private:
     const uint8_t CONN_INIT = 0x69;
     const uint8_t CONN_RESPONSE= 0x68;
     const uint8_t CONN_CONFIRM = 0x67;
     const uint8_t CONN_HEARTBEAT = 0x50;

     uint8_t deviceID;
     uint8_t numPackets;
     Serial_ *_serial;
     bool connected;
     unsigned long lastHeartbeat;
     unsigned long hbPacketTimeout; //for sending data back
     ReciveState rState; //for sending data back
     Packet* currentPack;

     virtual void pack(void *p);
     virtual void unpack();
     void checkHeartbeat();
     void connect();
     void sendData();
     void setPacketNum(uint8_t id);

   public:
     TelemetryNode(uint8_t deviceID, Serial_ *serialPort)
      : deviceID (deviceID), _serial (serialPort){
        setPacketNum(deviceID);
        currentPack = new Packet[numPackets];
        hbPacket = new uint8_t[16];
        rState = HEARTBEAT_WAITING;
      };
     uint8_t* hbPacket; //for sending data back
     void begin(long baudrate);
     void update();
     uint8_t getDeviceID() const { return deviceID; };
     uint8_t getNumPackets() const { return numPackets; };
     Packet* getPackets() {return currentPack;};
     bool isConnected() {return connected;};
 };

 class AlltraxNode : public TelemetryNode {
   private:
     const uint8_t PACKET_START = 0xF0;
     void pack(void *p);
     void unpack();
   public:
     uint16_t throt;
     uint16_t diodeTemp;
     uint16_t inVoltage;
     uint16_t outCurrent;
     uint16_t inCurrent;
     uint8_t dutyCycle;
     uint8_t errorCode;
     AlltraxNode(Serial_ *serialPort)
       : TelemetryNode(DEVICE_ALLTRAX, serialPort){};
 };

 class VescNode : public TelemetryNode {
   private:
     const uint8_t PACKET_START = 0xF0;
     void pack(void *p);
     void unpack();
   public:
     uint16_t throt;
     uint16_t fetTemp;
     uint16_t inVoltage;
     uint16_t outCurrent;
     uint16_t inCurrent;
     uint8_t dutyCycle;
     uint8_t faultCode;
     VescNode(Serial_ *serialPort)
       : TelemetryNode(DEVICE_VESC, serialPort){};
 };

 class MotorBoardNode : public TelemetryNode {
   private:
     const uint8_t PACKET_START = 0xF0;
     void pack(void *p);
     void unpack();
   public:
     float motorTemp;
     uint32_t motorRPM;
     uint32_t propRPM;
     MotorBoardNode(Serial_ *serialPort)
       : TelemetryNode(DEVICE_MOTOR_BOARD, serialPort){};
 };

 class GPSIMUNode : public TelemetryNode {
   private:
     const uint8_t PACKET_START = 0xF0;
     void pack(void *p);
     void unpack();
   public:
     float imuPitch;
     float imuRoll;
     float latitude;
     float longitude;
     float speedKnots;
     uint8_t numSatellites;
     uint8_t fix;
     uint8_t heading;
     GPSIMUNode(Serial_ *serialPort)
       : TelemetryNode(DEVICE_GPS_IMU, serialPort){};
 };

 class ThrottleNode : public TelemetryNode{
   private:
     const uint8_t PACKET_START = 0xF0;
     void pack(void *p);
     void unpack();
   public:
     uint16_t throt;
     ThrottleNode(Serial_ *serialPort)
      : TelemetryNode(DEVICE_THROTTLE, serialPort){};
 };

class SolarNode : public SolarNode{
  private:
    const uint8_t PACKET_START = 0xF0;
    void pack(void *p);
    void unpack();
  public:
    float outCurrent1;
    float outCurrent2;
    float totalCurrent;
    SolarNode(Serial_ *serialPort)
     : TelemetryNode(DEVICE_SOLAR, serialPort){};
};
#endif
