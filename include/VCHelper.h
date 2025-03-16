#ifndef _CARDHELPER_H
#define _CARDHELPER_H


//Name: Aaron Amelink
//Student ID: 1266687


#include "VCParser.h"

void stringToLower(char* p);

int stringMatch(char* string1, char* string2);

char* parsePropertyName(char* line);

char* parsePropertyRightSide(char* line);

void print(char* string);

int checkTab(char* string);

int parsePropertys(char* line, Card* card);

void removeCharacter(char* string);

void removeDelimCommas(char* string);

int parsePropCSV(char* value, List* propertyValues);

int parsePropertyParameters(char* line, List* parameters);

char* parsePropertyLeftSide(char* line);

Parameter* createParameter(char* parsedParam);

void nullTerminate(char* string) ;

char* createEmptyString();

DateTime* parseDateTime(char* text);

int propertyToFile(FILE *fptr, void* prop);

int dateToFile(FILE* fptr, void* date);

VCardErrorCode validateProperty(const Property* prop, bool isOptional);

VCardErrorCode validateDateTime(DateTime* dt);

#endif	