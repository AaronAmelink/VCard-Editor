#include "VCParser.h"
#include "VCHelper.h"

//Name: Aaron Amelink
//Student ID: 1266687

void stringToLower(char* p) {
    while (*p != '\0') {
        if (*p <= 90 && *p >= 65) {
            *p += 0x20;
        }
        p++;
    }
}

void removeCharacter(char* string) {
    char* temp = string;
    while (*temp != '\0') {
        *temp = *(temp+1);
        temp++;
    }
    *temp = '\0';
}

void nullTerminate(char* string) {
    //assuming that string has been malloced accordingly;
    int size = strlen(string);
    *(string+size) = '\0';
}

int containsAlphabet(char* string) {
    int strLen = strlen(string);
    for (int i = 0; i < strLen; i++) {
        if ((string[i] >= 65 && string[i] <= 90) || (string[i] >= 97 && string[i] <= 122)) {
            return 1;
        }
    }
    return 0;
}

char* createEmptyString() {
    char* empty = malloc(1);
    *empty = '\0';
    return empty;
}

DateTime* parseDateTime(char* text) {
    DateTime* dt = malloc(sizeof(DateTime));
    
    dt->date = createEmptyString();
    dt->text = createEmptyString();
    dt->time = createEmptyString();
    dt->UTC = false;
    dt->isText = false;

    char* time = strpbrk(text, "t");
    int dateLength = 0;
    if (time == NULL) {
        dateLength = strlen(text);
    } else {
        dateLength = time-text;
        time++;
    }

    char* date = NULL;
    if (dateLength != 0) {
        date = malloc(dateLength +1);
        strncpy(date, text, dateLength);
        date[dateLength] = '\0';
        if (containsAlphabet(date)) {
            char* newText = malloc(strlen(text) +1);
            strcpy(newText, text);
            newText[strlen(newText)] = '\0';
            dt->isText = true;
            free(dt->text);
            free(date);
            dt->text = newText;
            dt->UTC = false;
            return dt;
            //this is text;
        } else {
            free(dt->date);
            dt->date = date;
        }
    }

    dt->isText = false;

    if (time != NULL) {
        int timeLength = strlen(text) - (time - text); //-2 for CRLF;
        char* timeText = malloc(timeLength+1);
        strncpy(timeText, time, timeLength);
        timeText[timeLength] = '\0';
        char* zed = strpbrk(timeText, "z");
        if (zed != NULL) {
            removeCharacter(zed);
            dt->UTC = true;
        }
        if (containsAlphabet(timeText)) {
            char* newText = malloc(strlen(text) +1);
            strcpy(newText, text);
            newText[strlen(newText)] = '\0';
            dt->isText = true;
            free(dt->text);
            free(dt->date);
            dt->date = createEmptyString();
            free(timeText);
            dt->text = newText;
            dt->UTC = false;
            return dt;
        }
        timeText[timeLength] = '\0';
        free(dt->time);

        dt->time = timeText;
    }

    return dt;
}

void print(char * string) {
    printf(string);
    free(string);
}

int checkTab(char* string) {
    char* tab = "   ";
    for (int i = 0; i < strlen(tab); i++){
        if (*(string+i) != *(tab+i)) {
            return 0;
        }
    }
    return 1;
}



//wrapper becasue ew why does strcmp return 0
int stringMatch(char* string1, char* string2) {
    return strcmp(string1, string2) == 0;
}

Parameter* createParameter(char* parsedParam) {
    char* eq = strpbrk(parsedParam, "=");
    if (eq == NULL) {
        free(parsedParam);
        return NULL;
    }
    *eq = '\0';
    eq++;

    Parameter* param = malloc(sizeof(Parameter));


    param->name = malloc(strlen(parsedParam)+1);
    param->value = malloc(strlen(eq)+1);

    strcpy(param->name, parsedParam);
    strcpy(param->value, eq);


    free(parsedParam);
    return param;
}


char* parsePropertyLeftSide(char* line) {
    char* seperator = strpbrk(line, ":")+1;
    if (seperator == NULL) {
        return NULL;
    }

    int strLen = (seperator - line-1);
    char* leftSide = malloc(sizeof(char) * (strLen+1));
    strncpy(leftSide, line, strLen);
    leftSide[strLen] = '\0';
    return leftSide;
}

char* parsePropertyName(char* line) {
    //creates a **copy** of the prop name.
    char* leftSide = parsePropertyLeftSide(line);
    char* anchor = leftSide;
    char* period = strpbrk(leftSide, ".");

    if (period != NULL) {
        leftSide = ++period;
    }

    char* firstParam = strpbrk(leftSide, ";");

    if (firstParam == NULL) {
        char* propName = malloc(strlen(leftSide)+1);
        strcpy(propName, leftSide);
        propName[strlen(leftSide)] = '\0';
        stringToLower(propName);
        free(anchor);
        return propName;
    }

    int strLen2 = firstParam - leftSide;
    char* propName = malloc(strLen2+1);
    strncpy(propName, leftSide, strLen2);
    *(propName+strLen2) = '\0';

    free(anchor);
    stringToLower(propName);
    return propName;
}



char* parseGroup(char* leftSide) {
    char* period = strpbrk(leftSide, ".");
    if (period == NULL) {
        return NULL;
    }

    if (period == leftSide) {
        //length == 0
        return NULL;
    }

    char* string = malloc(period - leftSide +1);
    strncpy(string, leftSide, period - leftSide);
    string[period - leftSide] = '\0';
    return string;
}


char* parsePropertyRightSide(char* line) {
    char* colon = strpbrk(line, ":");
    if (colon == NULL) {
        return NULL;
    }
    colon++;
    int stringLength = strlen(colon) -2;
    char* newString = malloc(stringLength+1); //remove CRLF
    strncpy(newString, colon, stringLength);
    newString[stringLength] = '\0';
    return newString;
}

int parsePropertyParameters(char* line, List* parameters) {

    char* leftSide = parsePropertyLeftSide(line);

    if (leftSide == NULL) {
        return 0;
    }

    char* semiColon = strpbrk(leftSide, ";");
    if (semiColon == NULL) {
        free(leftSide);
        return 1;
    }
    semiColon++;
    char* iterParam = malloc( strlen(leftSide) - (semiColon - leftSide)+1);
    char* iterAnchor = iterParam;


    strcpy(iterParam, semiColon);

    while(semiColon != NULL) {
        semiColon = strpbrk(iterParam, ";");
        int strLength = semiColon != NULL ? (semiColon-iterParam) : strlen(iterParam);
        //*(semiColon) = '\0';
        char* newParam = malloc(strLength+1);
        strncpy(newParam, iterParam, strLength);
        newParam[strLength] = '\0';
        Parameter* param = createParameter(newParam);
        if (param == NULL) {
            free(iterAnchor);
            free(leftSide);
            return 0;
        }

        insertBack(parameters, param);

        iterParam = semiColon != NULL ? semiColon + 1 : NULL;
    }

    free(iterAnchor);
    free(leftSide);
    return 1;
}



void removeDelimCommas(char* string) {
    char* comma = strpbrk(string, "\\,");
    while (comma != NULL) {
        removeCharacter(string);
        comma = strpbrk(string, "\\,");
    }
}

int parsePropCSV(char* value, List* propertyValues) {
    char* iterValue = value;
    char* comma = strpbrk(value, ";");
    while (comma != NULL) {

        *comma = '\0';
        int strSize = sizeof(char) * (comma - iterValue +1);
        char* newValue = malloc(strSize);
        strcpy(newValue, iterValue);
        newValue[strlen(newValue)] = '\0';
        iterValue = comma+1;
        comma = strpbrk(iterValue, ";");
        insertBack(propertyValues, newValue);
    }
    char* newValue = malloc(sizeof(char) * (strlen(iterValue) +1));
    strcpy(newValue, iterValue);
    newValue[strlen(newValue)] = '\0';
    insertBack(propertyValues, newValue);
    return 1;
}

int parsePropertys(char* line, Card* card){
    Property* newProp = malloc(sizeof(Property));
    char* seperator = strpbrk(line, ":")+1;
    if (seperator == NULL) {
        free(newProp);
        return 0;
    }

    char* rightSide = parsePropertyRightSide(line);
    char* leftSide = parsePropertyLeftSide(line);
    char* group = parseGroup(leftSide);
    char* key = parsePropertyName(line);

    if (stringMatch(key, "")){
        free(group);
        free(newProp);
        free(key);
        free(rightSide);
        free(leftSide);
        return 0;
    }

    newProp->name = key;
    newProp->group = group == NULL ? createEmptyString() : group;
    newProp->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
    newProp->values = initializeList(&valueToString, &deleteValue, &compareValues);

    int success = parsePropertyParameters(line, newProp->parameters);

    if (success == 0) {
        deleteProperty(newProp);
        free(leftSide);
        free(rightSide);
        return 0;
    }

    parsePropCSV(rightSide, newProp->values);


    if (stringMatch(newProp->name, "fn") && card->fn == NULL) {
        card->fn = newProp;
    } else {
        insertBack(card->optionalProperties, newProp);
    }
    free(leftSide);
    free(rightSide);
    return 1;
}


int propertyToFile(FILE *fptr, void* prop) {
    if (prop == NULL) {
        return 0;
    }
    Property p = *((Property*)prop);

    if (!stringMatch(p.group, "")) {
        fprintf(fptr, "%s.", p.group);
    }
    fprintf(fptr, "%s", p.name);



    ListIterator paramIter = createIterator(p.parameters);
    void* param = nextElement(&paramIter);
    while(param != NULL) {
        Parameter* parm = (Parameter*)param;
        fprintf(fptr, ";%s=%s", parm->name, parm->value);
        param = nextElement(&paramIter);
    }

    fprintf(fptr, ":");


    ListIterator valueIter = createIterator(p.values);
    void* value = nextElement(&valueIter);

    while (value != NULL) {
        char* str = (char*)value;
        fprintf(fptr, "%s", str);
        value = nextElement(&valueIter);

        //if next is not null seperate
        if (value != NULL) {
            fprintf(fptr, ";");
        }
    }

    fprintf(fptr,"\r\n");

    return 1;
}

int dateToFile(FILE* fptr, void* date) {
    if (date == NULL) {
        return 0;
    }
    DateTime* dt = (DateTime*)date;
    if (dt->isText) {
        fprintf(fptr, ";VALUE=text:%s\r\n", dt->text);
        return 1;
    }

    fprintf(fptr, ":");

    if (!stringMatch(dt->date, "")) {
        fprintf(fptr, "%s", dt->date);
    }

    if (!stringMatch(dt->time, "")) {
        fprintf(fptr, "T%s", dt->time);
    }

    if (dt->UTC) {
        fprintf(fptr, "Z");
    }

    fprintf(fptr, "\r\n");
    return 1;
}

Card* createVCardPointer(char* fileName) {
    Card *card = NULL;
    VCardErrorCode response = createCard(fileName, &card);
    if (response != OK) {
        return NULL;
    }
    return card;
}

int getCardOtherPropertyNumbers(Card* card) {
    if (card == NULL) {
        return 0;
    }
    else {
        return card->optionalProperties->length;
    }
}

char* getCardFN(Card* card) {
    if (card == NULL) {
        return NULL;
    }
    if (card->fn == NULL) {
        return NULL;
    }
    Property* fn = card->fn;
    ListIterator iter = createIterator(fn->values);
    char* value = nextElement(&iter);
    if (value == NULL) {
        return NULL;
    }
    char* copy = malloc(strlen(value)+1);
    strcpy(copy, value);
    return copy;
}

int setCardFN(Card* card, char* fn) {
    //should always have an FN, so theres no case to check if it doesnt exist
    if (card == NULL || fn == NULL) {
        return 0;
    }
    if (card->fn != NULL) {
        deleteProperty(card->fn);
    }
    card->fn = malloc(sizeof(Property));
    Property* prop = card->fn;
    prop->name = malloc(strlen("FN")+1);
    strcpy(prop->name, "FN");

    //might need to parse group later on??
    card->fn->group = createEmptyString();
    prop->values = initializeList(&valueToString, &deleteValue, &compareValues);
    prop->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);

    insertBack(prop->values, fn);
    return 1;
}

char* getCardAnniversary(Card* card) {
    if (card == NULL) {
        return NULL;
    }
    if (card->anniversary == NULL) {
        return NULL;
    }
    char* text = dateToString(card->anniversary);
    return text;
}

int setCardAnniversary(Card* card, char* anniversary) {
    if (card == NULL || anniversary == NULL) {
        return 0;
    }

    DateTime* dt = parseDateTime(anniversary);
    if (dt == NULL) {
        return 0;
    }
    if (card->anniversary != NULL) {
        free(card->anniversary);

    }
    card->anniversary = dt;
    return 1;
}

char* getCardBirthday(Card* card) {
    if (card == NULL) {
        return NULL;
    }
    if (card->birthday == NULL) {
        return NULL;
    }
    char* text = dateToString(card->birthday);
    return text;
}

int setCardBirthday(Card* card, char* birthday) {
    if (card == NULL || birthday == NULL) {
        return 0;
    }

    DateTime* dt = parseDateTime(birthday);
    if (dt == NULL) {
        return 0;
    }
    if (card->birthday != NULL) {
        free(card->birthday);
    }
    card->birthday = dt;
    return 1;
}
