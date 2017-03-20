#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>   // git clone https://github.com/mcauser/Adafruit_SSD1306/tree/esp8266-64x48 lib
#include <Adafruit_GFX.h>       // pio lib install 13
#include <WiFiManager.h>        // pio lib install 567
#include <tinyxml2.h>           // git clone https://github.com/leethomason/tinyxml2 lib
#include "config.h"


#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


WiFiManager wifiManager;
WiFiClient client;


void setup() {
    Serial.begin(9600);

    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)

    // configure wifi on first connect otherwise just connect to the last known
    wifiManager.autoConnect();

    // download the predictions once we're connected
    String prediction_xml = download_prediction_xml();
    const char* xml = prediction_xml.c_str();

    // let's parse out a couple predictions!
    if(strcmp(xml, "ERROR") != 0) {
        int first_prediction, second_prediction;
        get_prediction(xml, first_prediction, second_prediction);

        display_predictions(first_prediction, second_prediction);
    }
    else {
        display_error();
    }

    display.display();

    // this requires GPIO16 (D0) to be shorted to RST
    Serial.println("Going to sleep.");
    ESP.deepSleep(POLL_SECONDS * 1000000, WAKE_RF_DEFAULT);
}

void loop() {
}

// clears the display and sets basic settings
void clear_display(){
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0,0);
}

// displays predictions on the display
void display_predictions(int first_prediction, int second_prediction) {
        clear_display();

        display.setTextSize(1);
        display.println("Next bus:");
        display.println();

        display.setTextSize(3);
        display.print(first_prediction);
        display.setTextSize(2);
        display.print(second_prediction);
        display.setTextSize(3);
        display.println();

        display.setTextSize(1);
        display.println("minutes");
}

// display error in case of problems getting predictions
void display_error(){
    clear_display();

    display.setTextSize(2);
    display.println("Error");

    display.setTextSize(1);
    display.println("No pred!");
}

// gets the bus prediction XML from NextBus
String download_prediction_xml() {
    Serial.print("Connecting to ");
    Serial.println(host);

    if (!client.connect(host, 80)) {
        Serial.println("connection failed");
        return String("ERROR");
    }

    String url = String("/service/publicXMLFeed?command=predictions&a=") + nb_agency +
        "&r=" + nb_route + "&s=" + nb_stop;

    Serial.print("Requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return String("ERROR");
      }
    }

    String response;
    while(client.available()){
        response += client.readStringUntil('\r');
    }
    //Serial.println("Response:"+response);
    //Serial.println();

    String xml_response = response.substring(response.indexOf("<?xml"));
    //Serial.println("XML:" + xml_response);
    //Serial.println();

    Serial.println("closing connection");

    return xml_response;
}

// extracts the first minutes from the prediction XML
// check out https://gist.github.com/grantland/7cf4097dd9cdf0dfed14 for more info on the API
void get_prediction(const char* xml, int &first_prediction, int &second_prediction) {
    //Serial.println("Parsing XML: ");
    //Serial.println(xml);

    tinyxml2::XMLDocument doc;
    doc.Parse(xml);

    tinyxml2::XMLElement* predictionsElement = doc.FirstChildElement("body")->FirstChildElement("predictions")->FirstChildElement("direction")->FirstChildElement("prediction");
    first_prediction = atoi(predictionsElement->Attribute("minutes"));

    // don't even bother with the first prediction if you can't walk to the stop fast enough
    if (first_prediction <= MIN_WALKING_TIME) {
        predictionsElement = predictionsElement->NextSiblingElement();
        first_prediction = atoi(predictionsElement->Attribute("minutes"));
    }
    second_prediction = atoi(predictionsElement->NextSiblingElement()->Attribute("minutes"));

    Serial.println("Predictions:");
    Serial.println(first_prediction);
    Serial.println(second_prediction);
}
