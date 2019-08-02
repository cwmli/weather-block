#include "Canvas.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

#include "Icons.h"

void Canvas::update() {
  for (auto i = APIElements[activeSubCanvas].begin(); i != APIElements[activeSubCanvas].end(); i++) {
    delete *i;
  }
  APIElements[activeSubCanvas].clear();
  
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
        break;
    }

    APIElements[activeSubCanvas].push_back(elem);
  }
}

void Canvas::draw(LEDController * controller) {
  for (auto i = APIElements[activeSubCanvas].begin(); i != APIElements[activeSubCanvas].end(); i++) {
    (*i)->draw(controller);
  }
  
  for (auto i = elements[activeSubCanvas].begin(); i != elements[activeSubCanvas].end(); i++) {
    (*i)->draw(controller);
  }
}

void Canvas::incrementActiveSubCanvas() {
  activeSubCanvas += 1;
  if (activeSubCanvas == SUBCANVAS_LIMIT) {
    activeSubCanvas = 0;
  }
}

void Canvas::addElement(Elements::Text * e) {
  elements[activeSubCanvas].push_back(e);
}

void Canvas::resetElements() {
  for (auto i = elements[activeSubCanvas].begin(); i != elements[activeSubCanvas].end(); i++) {
    delete *i;
  }
  elements[activeSubCanvas].clear();
}

std::vector<Elements::Text *> Canvas::getElements() {
  return elements[activeSubCanvas];
}

String Canvas::getElementsString() {
  String res;
  for (auto it = elements[activeSubCanvas].begin(); it != elements[activeSubCanvas].end(); it++) {
    unsigned long hex = ((*it)->color.r << 16) | ((*it)->color.g << 8) | (*it)->color.b;
    res += (*it)->string + " " + String(hex, HEX) + " " + (*it)->x + " " + (*it)->y + " " + (*it)->scroll + " " + (*it)->scrollSpeed;
    if (next(it) != elements[activeSubCanvas].end()) {
      res += ",";
    }
  }

  return res;
}

void Canvas::setElements(char * content) {
  resetElements();

  char * endelementset;
  char * elementset = strtok_r(content, ",", &endelementset);
  while(elementset != NULL) {
    char * values[6];
    byte index = 0; 
    char * endelemopt;
    char * elemopt = strtok_r(elementset, " ", &endelemopt);
    while (elemopt != NULL) {
      values[index++] = elemopt;
      elemopt = strtok_r(NULL, " ", &endelemopt);
    }
    
    addElement(
      new Elements::Text(
        values[0],
        CRGB(strtoul(values[1], NULL, HEX)),
        atoi(values[2]),
        atoi(values[3]),
        (atoi(values[4]) > 0),
        atoi(values[5])
      )
    );

    elementset = strtok_r(NULL, ",", &endelementset);
  }
}

void Canvas::setAPI(String name, String url, long refresh, bool active, std::map<String, APIParseRule> parseRules) {
  resetAPI();

  apiobj.isActive = active;
  apiobj.name = name;
  apiobj.url = url;
  apiobj.refreshTime = refresh;
  apiobj.parseRules = parseRules;
}

void Canvas::resetAPI() {
  apiobj.isActive = false;
  apiobj.name = "";
  apiobj.url = "";
  apiobj.refreshTime = 0;
  apiobj.parseRules = std::map<String, APIParseRule>();

  for (auto i = APIElements[activeSubCanvas].begin(); i != APIElements[activeSubCanvas].end(); i++) {
    delete *i;
  }
  APIElements[activeSubCanvas].clear();
}

APIData Canvas::getAPIData() {
  return apiobj;
}

void Canvas::toggleAPI() {
  apiobj.isActive = !apiobj.isActive;
}

void Canvas::updateAPI(unsigned long curtime) {
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
