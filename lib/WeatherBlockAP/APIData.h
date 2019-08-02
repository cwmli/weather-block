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

  void parseRulesString(String s) {
    
  }
};

#endif
