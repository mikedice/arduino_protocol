#include <parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* message = "Hello from Arduino";
void ProcessPacket(Packet* packet){
    char buffer[128];
    
    Serial.println("processing packet");
    sprintf(buffer,"Sig: %s", packet->Sig); 
    Serial.println(buffer);
    
    sprintf(buffer, "Command: %s", packet->Command);
    Serial.println(buffer);

    sprintf(buffer, "DataLen: %d", packet->DataLength);
    Serial.println(buffer);

    
    Serial.println(" ");
    DeletePacket(packet); // done with the packet
    ResetParser();
}

void TestStream(char* stream){
    size_t len = strlen(stream);
    for (size_t i = 0; i < len; i++){
        ProcessStreamByte(stream[i], &ProcessPacket);
    }
}

void setup() {

    Serial.begin(57600);

    Serial.println("Protocol tests are starting...");
    ResetParser();
    const char *test1 = "[--\r\n"
    "PKT.V1 HELLO\r\n"
    "LEN: 6\r\n"
    "abc123";
    TestStream((char*)test1);
    
    const char *test2 = "[--\r\n"
    "PKT.V1 HELLOREPLY\r\n"
    "LEN: 11\r\n"
    "hello reply";
    TestStream((char*)test2);

    const char *test3 = "[--\r\n"
    "PKT.V1 PING\r\n"
    "LEN: 0\r\n";
    TestStream((char*)test3);

    Serial.println("Send new packet from arduino back to console listener");
    PacketBuffer *pBuffer = CreatePacketBuffer("HELLO-REPLY", (unsigned char*)message, strlen(message));
    if (pBuffer != NULL){
      Serial.write(pBuffer->Data, pBuffer->DataLength);
    }
    DeletePacketBuffer(pBuffer);

    Serial.println(" ");
    Serial.println(" ");
    Serial.println("Protocol tests are completed. Beginning to listen");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    byte byteRead = Serial.read();
    ProcessStreamByte(byteRead, &ProcessPacket);
  }
}
