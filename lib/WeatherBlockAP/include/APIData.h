#ifndef APIDATA_H
#define APIDATA_H

#include <map>
#include <time.h>

#include "APIParseRule.h"

struct APIData {
  String name;
  String url;
  bool isActive;
  unsigned long refreshTime;
  unsigned long lastRefreshed = 0;

  std::map<String, String> data;
  std::map<String, APIParseRule> parseRules;


  APIData() {}

  APIData(String name, String url, long time) : name(name), url(url), refreshTime(time) {
    lastRefreshed = 0;
  }

  String parseRulesString() {
    String res; 
    for (auto it = parseRules.begin(); it != parseRules.end(); it++) {
      if (it->second.type == APIValueType::ICON) {
        res += it->first + " " + it->second.x + " " + it->second.y + " ffffff " + it->second.type;
      } else {
        unsigned long hex = (it->second.color.r << 16) | (it->second.color.g << 8) | (it->second.color.b);
        res += it->first + " "  + it->second.x + " " + it->second.y + " " + String(hex, HEX) + " " + it->second.type;
      }
      if (next(it) != parseRules.end()) {
        res += ",";
      }
    }

    return res;
  }

  void parseRulesString(char * content) {
    parseRules.clear();

    char * endparserule;
    char * parserule = strtok_r(content, ",", &endparserule);
    while(parserule != NULL) {
      char * values[5];
      byte index = 0; 
      char * endrule;
      char * rule = strtok_r(parserule, " ", &endrule);
      while (rule != NULL) {
        values[index++] = rule;
        rule = strtok_r(NULL, " ", &endrule);
      }

      parseRules.insert(std::make_pair(
        values[0],
        APIParseRule{
          atoi(values[1]),
          atoi(values[2]),
          CRGB(strtoul(values[3], NULL, HEX)),
          static_cast<APIValueType>(atoi(values[4]))
        }
      ));

      // Serial.printf("Parserule: %s | Tokens: %s, %s, %s, %s\n", parserule, values[0], values[1], values[2], values[3]);
      parserule = strtok_r(NULL, ",", &endparserule);
    }
  }
};

#endif
