#include "Canvas.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

void Canvas::update() {
  for (auto i = APIElements.begin(); i != APIElements.end(); i++) {
    delete *i;
  }
  APIElements.clear();
  for (auto const &i : apiobj.parseRules) {
    // i.first contains the key (String)
    // i.second contains an APIParseRule object
    APIElements.push_back(
      new Elements::Text(
        apiobj.data[i.first], 
        i.second.x,
        i.second.y,
        false,
        0
      )
    );
  }
}

void Canvas::addElement(Elements::Generic * e) {
  elements.push_back(e);
}

void Canvas::removeElement(int index) {
  auto it = elements.begin();
  for (int i = 0; i < index; i++) {
    it = it++;
  }
  delete *it;
  elements.erase(it);
}

std::vector<Elements::Generic *> Canvas::getElements() {
  return elements;
}

void Canvas::draw(LEDController * controller) {
  for (auto i = APIElements.begin(); i != APIElements.end(); i++) {
    (*i)->draw(controller);
  }
  
  for (auto i = elements.begin(); i != elements.end(); i++) {
    (*i)->draw(controller);
  }
}

void Canvas::setAPI(String name, String url, long refresh, std::map<String, APIParseRule> parseRules) {
  apiobj.isActive = true;
  apiobj.name = name;
  apiobj.url = url;
  apiobj.refreshTime = refresh;
  apiobj.parseRules = parseRules;
}

void Canvas::resetAPI() {
  apiobj.isActive = false;
}

APIData Canvas::getAPIData() {
  return apiobj;
}

void Canvas::updateAPI(long curtime) {
    if (!apiobj.isActive || curtime - apiobj.lastRefreshed < apiobj.refreshTime) {
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
        String temp = payload.substring(pos + it->first.length() + 2, end);
        Serial.printf("[INFO] %s: %s\n", it->first.c_str(), temp.c_str());
        if (it->second.type == APIValueType::NUMBER) {
          int itemp = temp.toInt();
          apiobj.data[it->first] = itemp;
        } else {
          apiobj.data[it->first] = temp;
        }
      }
    } else {
      Serial.printf("An error occurred while updating %s\n", apiobj.name.c_str());
    }
    apiobj.lastRefreshed = curtime;
    https.end();
}
