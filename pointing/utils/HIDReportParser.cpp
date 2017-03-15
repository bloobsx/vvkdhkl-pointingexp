/* -*- mode: c++ -*-
 *
 * pointing/utils/HIDReportParser.cpp --
 *
 * Initial software
 * Authors: Izzatbek Mukhanov
 * Copyright © Inria
 *
 * http://libpointing.org/
 *
 * This software may be used and distributed according to the terms of
 * the GNU General Public License version 2 or any later version.
 *
 */

#include <stdio.h>
#include <memory.h>
#include <sstream>
#include <pointing/utils/HIDReportParser.h>
#include <pointing/utils/HIDTags.h>

using namespace std;

namespace pointing
{
  // FIXME: Wheel, Z

  HIDReportParser::HIDReportParser()
    :lastRepCount(0),lastRepSize(0),curRepInfo(0),report(0),debugLevel(0) { }

  HIDReportParser::HIDReportParser(unsigned char *desc, int size, int debugLevel):
    lastRepCount(0),lastRepSize(0),curRepInfo(0),report(0),debugLevel(debugLevel)
  {
    if (size)
      setDescriptor(desc, size);
  }

  void HIDReportParser::parseItem(const HIDItem &item)
  {
    int value = item.dataAsUnsignedLong();
    unsigned int typeAndTag = item.typeAndTag();
    switch(typeAndTag)
    {
    case INPUT:
    {
      int size = lastRepCount * lastRepSize;
      // The first bit defines if the input is
      // constant or variable (spec. HID 1.11, p.30)
      if (value & INPUT_CONSTANT)
      {
        curRepInfo->size += size;
      }
      else if (lastUsagePage == USAGE_PAGE_BUTTONS && curRepInfo->buttonsPos == -1)
      {
        curRepInfo->buttonsPos = curRepInfo->size;
        curRepInfo->size += size;
      }
      else if(lastUsagePage == USAGE_PAGE_GEN_DESKTOP)
      {
        int usageXY = 0;
        for (list<int>::iterator it = usageList.begin(); it != usageList.end(); it++)
        {
          if (*it == USAGE_X)
          {
            curRepInfo->dxPos = curRepInfo->size;
            usageXY++;
          }
          if (*it == USAGE_Y)
          {
            curRepInfo->dyPos = curRepInfo->size;
            usageXY++;
          }
          curRepInfo->size += lastRepSize;
        }
        if (usageXY == 2)
        {
          curRepInfo->min = dataMap[LOGICAL_MIN];
          curRepInfo->max = dataMap[LOGICAL_MAX];
          int mask = 0xFFFFFFFF;
          curRepInfo->dMask = ~(mask << lastRepSize);
        }
        usageList.clear();
        dataMap.clear();
      }
      else {
        curRepInfo->size += size;
      }
      break;
    }
    case REPORT_ID:
    {
      map<int, MouseReport>::iterator it = reportMap.find(value);
      if (it == reportMap.end())
      {
        MouseReport report = MouseReport();
        report.reportId = value;
        reportMap[value] = report;
      }
      curRepInfo = &reportMap[value];
      break;
    }
    case USAGE_PAGE:
    {
      lastUsagePage = value;
      break;
    }
    case USAGE:
    {
      if (lastUsagePage == USAGE_PAGE_GEN_DESKTOP && parentUsage == USAGE_POINTER)
        usageList.push_back(value);
      lastUsage = item.dataAsSignedLong();
      break;
    }
    case COLLECTION:
    {
      if (lastUsage == USAGE_MOUSE)
        parentUsage = USAGE_MOUSE;
      else if(lastUsage == USAGE_POINTER && parentUsage == USAGE_MOUSE) // Not sure about the second condition
        parentUsage = USAGE_POINTER;
      break;
    }
    case REPORT_SIZE:
    {
      lastRepSize = value;
      break;
    }
    case REPORT_COUNT:
    {
      lastRepCount = value;
      break;
    }
    case LOGICAL_MIN:
    case LOGICAL_MAX:
    case USAGE_MIN:
    case USAGE_MAX:
      dataMap[typeAndTag] = value;
      break;
    default:
      break;
    }
  }

  void HIDReportParser::clearAll()
  {
    lastUsage = 0;
    parentUsage = 0;
    lastUsagePage = 0;
    lastRepCount = 0;
    lastRepSize = 0;
    reportMap.clear();
    reportMap[0] = MouseReport();
    curRepInfo = &reportMap[0];
    dataMap.clear();
    usageList.clear();
  }

  bool HIDReportParser::findCorrectReport()
  {
    for (map<int, MouseReport>::iterator it = reportMap.begin(); it != reportMap.end(); it++)
    {
      //cout << it->first << endl;
      MouseReport rep = it->second;
      if (rep.dxPos && rep.dyPos)
      {
        curRepInfo = &(it->second);
        // 1 byte for tagId by SPEC HID;
        if (curRepInfo->reportId)
        {
          curRepInfo->size += 8;
          curRepInfo->dxPos += 8;
          curRepInfo->dyPos += 8;
          curRepInfo->buttonsPos += 8;
        }
        if (debugLevel)
        {
          cerr << "    HIDReportParser: report ID #" << curRepInfo->reportId << " - "
               << "buttons: " << curRepInfo->buttonsPos
               << ", dx: " << curRepInfo->dxPos
               << ", dy: " << curRepInfo->dyPos
               << ", total size: " << curRepInfo->size << endl;
        }
        return true;
      }
    }
    return false;
  }

  HIDReportParser::~HIDReportParser()
  {
    delete report;
  }

  bool HIDReportParser::setDescriptor(const unsigned char *desc, int size)
  {
    clearAll();

    int currentPosition = 0;
    while(currentPosition < size) {
      HIDItem currentItem(desc + currentPosition);
      parseItem(currentItem);
      currentPosition += currentItem.totalSize();
    }
    bool result = findCorrectReport();

    delete report;
    report = new unsigned char[curRepInfo->size];
    memset(report, 0, curRepInfo->size / 8);
    return result;
  }

  bool HIDReportParser::setReport(const unsigned char *newReport)
  {
    if (curRepInfo->reportId && curRepInfo->reportId != newReport[0])
      return false;
    memcpy(report, newReport, curRepInfo->size / 8);
    return true;
  }

  int HIDReportParser::getReportLength()
  {
    // Should be divisible by 8, thus not returning ceil(curRepInfo->size / 8)
    return curRepInfo->size / 8;
  }

  bool HIDReportParser::getReportData(int *dx, int *dy, int *buttons) const
  {
    if (!curRepInfo->size)
      return false;

    int *dxLoc = (int *)(report + (curRepInfo->dxPos / 8));
    int *dyLoc = (int *)(report + (curRepInfo->dyPos / 8));
    *dx = *dxLoc >> (curRepInfo->dxPos % 8) & curRepInfo->dMask;
    *dy = *dyLoc >> (curRepInfo->dyPos % 8) & curRepInfo->dMask;
    if (*dx > curRepInfo->max)
      *dx = *dx - curRepInfo->dMask - 1;
    if (*dy > curRepInfo->max)
      *dy = *dy - curRepInfo->dMask - 1;
    *buttons = *(report + (curRepInfo->buttonsPos / 8)) >> (curRepInfo->buttonsPos % 8) & 7;
    return true;
  }
}
