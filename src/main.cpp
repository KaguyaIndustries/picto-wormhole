#include <Arduino.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
extern "C" {
    #include "user_interface.h"
}

String command;
bool stfu = true;
uint8 channel = 1;

void setup() {
    Serial.begin(115200);

    delay(1000);
    Serial.println("Checkpoint 1");

    // WLAN im Station-Modus starten
    wifi_set_opmode(STATION_MODE);

    // Wi-Fi-Kanal einstellen
    wifi_set_channel(channel);

}

std::vector<String> splitString(const String& str) {
    std::vector<String> result;
    int start = 0;
    int end = str.indexOf(' ');

    while (end != -1) {
        result.push_back(str.substring(start, end));
        start = end + 1;
        end = str.indexOf(' ', start);
    }
    result.push_back(str.substring(start));

    return result;
}

    uint8_t nintendopacket[128] = {
        0x80, // Frame Control Field Subtype 8 0x8000
        0x00, // Flags of Subtype: 0x00
        0x00, // Duration
        0x00,
        0xff, // Destination Address Broadcast
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0x00, // Source Address Nintendo MAC Signatur (00 22 aa), Pseudo Device (be ee ef)
        0x22,
        0xaa,
        0xbe,
        0xee,
        0xef,
        0x00, // BSS Id
        0x22,
        0xaa,
        0xbe,
        0xee,
        0xef,
        0x10, // Sequence Number 2817
        0xb0,
        0xcc, // Timestamp
        0x30,
        0x3d,
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x69, // Beacon Intervall
        0x00,
        0x21, // Capabilities Information 0x0021
        0x00,
        0x01, // Tag: Supported Rates (1)
        0x02, // Tag Length 2
        0x82, // Supported Rates 1(B) (0x82)
        0x84, // Supported Rates 2(B) (0x84)
        0x03, // Tag: DS Parameter Set (3)
        0x01, // Tag Length 1
        0x01, // Current Channel
        0x05, // Tag: Traffic Indication Map (TIM) (5)
        0x05, // Tag Length 5
        0x01, // DTIM Count 1
        0x02, // DTIM Period 2
        0x00, // Bitmap Offset
        0x00, // Partitial Virtual Bitmap: 0000
        0x00,
        0xdd, // Tag Number Vendor Specific (221)
        0x20, // Tag Length: 32
        0x00, // OUI Nintendo Co Ltd
        0x09,
        0xbf,
        0x00, // Unknown [ Start of Vendor Specific Data ]
        0x0a, // Unknown
        0x00, // Unknown
        0xa0, // Unknown
        0xc6, // Unknown
        0x01, // Unknown
        0x00, // Unknown
        0x00, // Unknown
        0x00, // Unknown
        0x00, // Unknown - breaks Connection
        0x00, // Unknown - breaks Connection
        0x00, // Unknown - breaks Connection
        0x00, // Unknown - breaks Connection
        0x15, // Unknown
        0x00, // Unknown
        0x01, // Unknown
        0x01, // Unknown
        0xc0, // Unknown
        0x00, // Unknown
        0xc0, // Unknown
        0x00, // Unknown
        0x96, // Unknown
        0x23, // Unknown
        0x64, // Unknown
        0x1d, // Unknown
        0x00, // Room - A = 0x00, B = 0x01 , 0x02 = C, 0x03 = D --- Set WIFI Channel to: A = 1, B = 7, C = 13, D = 7
        0x0a, // Occupied Slots
        0x04, // Unknown - breaks Connection
        0x00  // Unknown - breaks Connection [ End of Vendor Specific Data ]



        // 0x64, 0x5a, 0xdb, 0x9f // Frame Check Sequence [Might be Obsolete]

        //  A, B, C and D chose wireless channels 1, 7, 13 and 7, respectively
        //  http://caia.swin.edu.au/netgames2007/papers/1569050137.pdf

    };

void loop() {

    wifi_set_channel(channel);
    if(!stfu) {
        Serial.print("switch channel: ");
        Serial.println(wifi_get_channel());;
    }



    if(Serial.available()) {
        command = Serial.readStringUntil('\n');
        std::vector<String> words = splitString(command);

        delay(100);

        for (const auto& word : words) {
            Serial.println(word);
        }

        if(command.startsWith("fillslots")) {
            uint8_t hexValue = strtol(words[1].c_str(), nullptr, 16);
            nintendopacket[81] = hexValue;
        }

        if(command.startsWith("rooma")) {
            channel = 1;
            nintendopacket[80] = 0x00;
            nintendopacket[42] = 0x01;
            Serial.println("Wifi Channel set to 1, Nintendopacket[80] Set to 0x00 (Room A)");
        }

        if(command.startsWith("roomb")) {
            channel = 7;
            nintendopacket[80] = 0x01;
            nintendopacket[42] = 0x07;
            Serial.println("Wifi Channel set to 7, Nintendopacket[80] Set to 0x01 (Room B)");
        }

        if(command.startsWith("roomc")) {
            channel = 13;
            nintendopacket[80] = 0x02;
            nintendopacket[42] = 0x0d;
            Serial.println("Wifi Channel set to 13, Nintendopacket[80] Set to 0x02 (Room C)");
        }

        if(command.startsWith("roomd")) {
            channel = 7;
            nintendopacket[80] = 0x03;
            nintendopacket[42] = 0x07;
            Serial.println("Wifi Channel set to 7, Nintendopacket[80] Set to 0x03 (Room D)");
        }

        if(command.startsWith("stfu")) {
            if(stfu) {
                stfu = false;
                Serial.println("[STFU] disabled");
            } else {
                stfu = true;
                Serial.println("[STFU] enabled");
            }
        }
    }




    // Senden des Pakets
    bool result = wifi_send_pkt_freedom(nintendopacket, sizeof(nintendopacket), 0);
    if(!stfu) {
        Serial.print("Wifi Emmited: ");
        Serial.println(result);
    }

    delay(107);

}

