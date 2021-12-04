#include "APICanvas.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

void APICanvas::update() {
  Canvas::update();

  for (auto i = APIElements.begin(); i != APIElements.end(); i++) {
    delete *i;
  }
  APIElements.clear();
  
  for (auto const &i : apiobj.parseRules) {
    // i.first contains the key (String)
    // i.second contains an APIParseRule object
    Elements::Generic * elem;
    switch (i.second.type) {
      case APIValueType::ICON:
        elem = new Elements::Icon(
          i.second.x,
          i.second.y,
          false,
          0,
          apiobj.data[i.first]
        );
        elem->xOffset = -subCanvasOffset;
        break;
      default:
        /* Default to Elements::Text */
        elem = new Elements::Text(
          apiobj.data[i.first], 
          i.second.color,
          i.second.x,
          i.second.y,
          false,
          0
        );
        elem->xOffset = -subCanvasOffset;
        break;
    }

    APIElements.push_back(elem);
  }
}

void APICanvas::draw(LEDController * controller) {
  Canvas::draw(controller);

  for (auto i = APIElements.begin(); i != APIElements.end(); i++) {
    (*i)->draw(controller);
  }
}

void APICanvas::resetSubCanvasOffset() {
  Canvas::resetSubCanvasOffset();

  for (auto i = APIElements.begin(); i != APIElements.end(); i++) {
    (*i)->xOffset = subCanvasOffset;
  }
}

void APICanvas::setAPI(String name, String url, long refresh, bool active, char * parseRules) {
  resetAPI();

  apiobj.isActive = active;
  apiobj.name = name;
  apiobj.url = url;
  apiobj.refreshTime = refresh;
  apiobj.parseRulesString(parseRules);
}

void APICanvas::resetAPI() {
  apiobj.isActive = false;
  apiobj.name = "";
  apiobj.url = "";
  apiobj.refreshTime = 0;
  apiobj.parseRules = std::map<String, APIParseRule>();

  for (auto i = APIElements.begin(); i != APIElements.end(); i++) {
    delete *i;
  }
  APIElements.clear();
}

APIData APICanvas::getAPIData() {
  return apiobj;
}

void APICanvas::toggleAPI() {
  apiobj.isActive = !apiobj.isActive;
}

void APICanvas::updateAPI(unsigned long curtime, bool force) {
  if ((!apiobj.isActive || curtime - apiobj.lastRefreshed < apiobj.refreshTime) && !force) {
    return;
  }

  BearSSL::WiFiClientSecure bsslClient;
  bsslClient.setInsecure();

  HTTPClient https;

  Serial.printf("Updating %s\n", apiobj.name.c_str());
  https.begin(bsslClient, apiobj.url);

  int httpcode = https.GET();

  if (httpcode > 0) {
    Serial.printf("[HTTPSRequest] code: %d\n", httpcode);
    String payload = https.getString();
    Serial.printf("[HTTPSRequest] payload: \n%s\n", payload.c_str());
    // Have to do some ghetto stuff
    std::map<String, APIParseRule> rules = apiobj.parseRules;
    for (auto it = rules.begin(); it != rules.end(); it++) {
      int pos = payload.indexOf(it->first);
      int end = payload.indexOf(",", pos);
      if (it->second.type == APIValueType::NUMBER) {
        float temp = payload.substring(pos + it->first.length() + 2, end).toFloat();
        apiobj.data[it->first] = (int) round(temp);
      } else if (it->second.type == APIValueType::TIME) {
        char temp[17];
        struct tm tm;
        // Extract to tm struct 
        payload.substring(pos + it->first.length() + 3, end - 1).toCharArray(temp, 17);
        strptime(temp, "%Y-%m-%dT%H:%M", &tm);
        // Reformat to desired output
        char time[7];
        strftime(time, 7, "%I:%M%p", &tm);
        // Convert A,P to a,p
        if (time[5] == 65) time[5] = 97;
        if (time[5] == 80) time[5] = 112;
        apiobj.data[it->first] = time;
      } else {
        apiobj.data[it->first] = payload.substring(pos + it->first.length() + 3, end - 1);
      }
      Serial.printf("[INFO] %s: %s\n", it->first.c_str(), apiobj.data[it->first].c_str());
    }
  } else {
    Serial.printf("An error occurred while updating %s\n", apiobj.name.c_str());
  }
  apiobj.lastRefreshed = curtime;
  https.end();
}