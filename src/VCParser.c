#include "VCParser.h"
#include "VCHelper.h"

//Name: Aaron Amelink
//Student ID: 1266687

#define STRING_SIZE 256

void deleteProperty(void* toBeDeleted) {

    
    if (toBeDeleted == NULL) {
        return;
    }

    Property* prop = (Property*)toBeDeleted;

    // Free the name and group strings

    free(prop->name);
    free(prop->group);


    freeList(prop->parameters);
    freeList(prop->values);


    free(prop);
}

int compareProperties(const void* first,const void* second){
    if (first == NULL || second == NULL) {
        return 0;
    }
    Property* prop1 = (Property*)first;
    Property* prop2 = (Property*)second;

    if (strcmp(prop1->name, prop2->name)) {
        return 0;
    }
    if (strcmp(prop1->group, prop2->group)) {
        return 0;
    }


    //compare values
    ListIterator iter1 = createIterator(prop1->values);
    ListIterator iter2 = createIterator(prop2->values);

    void* value1 = nextElement(&iter1);
    void* value2 = nextElement(&iter2);

    while (value1 != NULL && value2 != NULL) {
        if (!compareValues(value1, value2)) {
            return 0;
        } else {
            value1 = nextElement(&iter1);
            value2 = nextElement(&iter2);
        }
    }

    if (value1 != value2) {
        //if either is not null, one is longer
        return 0;
    }


    iter1 = createIterator(prop1->parameters);
    iter2 = createIterator(prop2->parameters);

    void* param1 = nextElement(&iter1);
    void* param2 = nextElement(&iter2);

    while (param1 != NULL && param2 != NULL) {
        if (!compareParameters(param1, param2)) {
            return 0;
        } else {
            param1 = nextElement(&iter1);
            param2 = nextElement(&iter2);
        }
    }

    if (param1 != param2) {
        //if either is not null, one is longer
        return 0;
    }


    return 1;
}



char* errorToString(VCardErrorCode err) {
    char* errorString;

    switch (err) {
        case OK:
            errorString = "OK";
            break;
        case INV_FILE:
            errorString = "Invalid File";
            break;
        case INV_CARD:
            errorString = "Invalid Card";
            break;
        case INV_PROP:
            errorString = "Invalid Property";
            break;
        case INV_DT:
            errorString = "Invalid Date-Time";
            break;
        case WRITE_ERROR:
            errorString = "Write Error";
            break;
        case OTHER_ERROR:
            errorString = "Other Error";
            break;
    }

    char* res = malloc(strlen(errorString) + 1);
    strcpy(res, errorString);
    return res;
}


char* propertyToString(void* prop){
    if (prop == NULL) {
        char* error = "Property NULL";
        char* res = malloc(strlen(error) +1);
        strcpy(res, error);
        *(error+strlen(error)) = '\0';
        return res;
    }
    Property p = *((Property*)prop);
    char* groupText = "\nGroup: ";
    char* parametersText = "\nParameters:\n";
    char* valuesText = "\nValues:\n";
    int size = (strlen(p.name) + strlen(groupText) + strlen(p.group) + strlen(parametersText) + strlen(valuesText) +1);
    char* string = malloc(sizeof(char) * size);
    strcpy(string, p.name);
    strcat(string, groupText);
    strcat(string, p.group);
    strcat(string,parametersText);


    ListIterator paramIter = createIterator(p.parameters);
    void* param = nextElement(&paramIter);
    while(param != NULL) {
        char* paramString = parameterToString(param);
        size += strlen(paramString);
        string = realloc(string, size);
        strcat(string, paramString);
        free(paramString);
        param = nextElement(&paramIter);
    }

    strcat(string, valuesText);

    ListIterator valueIter = createIterator(p.values);
    void* value = nextElement(&valueIter);

    while (value != NULL) {
        char* valueString = valueToString(value);
        size += strlen(valueString)+1;
        string = realloc(string, size);
        strcat(string ,valueString);
        strcat(string,"\n");
        free(valueString);
        value = nextElement(&valueIter);
    }

    return string;
}

void deleteParameter(void* toBeDeleted){
    if (toBeDeleted == NULL) {
        return;
    }
    Parameter* param = (Parameter*)(toBeDeleted);
    free(param->name);
    free(param->value);
    free(param);
    return;
}
int compareParameters(const void* first,const void* second){
    if (first == NULL || second == NULL) {
        return 0;
    }
    Parameter* param1 = (Parameter*)(first);
    Parameter* param2 = (Parameter*)(second);

    if (!stringMatch(param1->name, param2->name)) {
        return 0;
    }

    if (!stringMatch(param1->value, param2->value)) {
        return 0;
    }

    return 1;
}
char* parameterToString(void* param){
    if (param == NULL) {
        char* error = "Parameter NULL";
        char* res = malloc(strlen(error) +1);
        strcpy(res, error);
        *(error+strlen(error)) = '\0';
        return res;
    }
    Parameter* parameter = (Parameter*)(param);
    int stringLength = strlen(parameter->name) + strlen(parameter->value) + 4; //:_\n\0
    char* string = malloc(stringLength * sizeof(char));
    int count = 0;
    strcpy(string, parameter->name);

    strcat(string, ": ");
    count += 2;

    strcat(string, parameter->value);
    strcat(string, "\n");
    nullTerminate(string);

    return string;
}

void deleteValue(void* toBeDeleted){
    if (toBeDeleted == NULL) {
        return;
    }
    char* value = (char*)(toBeDeleted);
    free(value);
    return;
}
int compareValues(const void* first,const void* second){
    if (first == NULL || second == NULL) {
        return 0;
    }
    char* string1 = (char*)first;
    char* string2 = (char*)second;
    return stringMatch(string1, string2);
}
char* valueToString(void* val){
    if (val == NULL) {
        char* error = "Value NULL";
        char* res = malloc(strlen(error) +1);
        strcpy(res, error);
        *(error+strlen(error)) = '\0';
        return res;
    }
    char* value = (char*)val;
    char* valuecpy = malloc(sizeof(char) * strlen(value) +1);
    strcpy(valuecpy, value);
    return valuecpy;
}

void deleteDate(void* toBeDeleted){
    if (toBeDeleted == NULL) {
        return;
    }
    DateTime* dt = (DateTime*)(toBeDeleted);
    if (dt->date != NULL) {
        free(dt->date);
        dt->date = NULL;
    }
    if (dt->text != NULL) {
        free(dt->text);
        dt->text = NULL;
    }
    if (dt->time != NULL) {
        free(dt->time);
        dt->time = NULL;
    }
    free(dt); 
    return;
}
int compareDates(const void* first,const void* second){

    if (first == NULL || second == NULL) {
        return 0;
    } 

    DateTime* dt1 = (DateTime*)(first);
    DateTime* dt2 = (DateTime*)(second);

    if (dt1->isText == dt2->isText) {
        if (dt1->isText) {
            if (!stringMatch(dt1->text, dt2->text)) {
                return 0;
            } else {
                return 1;
            }
        } else {

            if (dt1->UTC != dt2->UTC) {
                return 0;
            }

            if (!stringMatch(dt1->date, dt2->date)) {
                return 0;
            } else {
                if (!stringMatch(dt1->time, dt2->time)) {
                    return 0;
                } else {
                    return 1;
                }
            }
        }
    } else {
        return 0;
    }


    return 1;
}

char* dateToString(void* date){
    if (date == NULL) {
        char* error = "Date NULL";
        char* res = malloc(strlen(error) +1);
        strcpy(res, error);
        *(error+strlen(error)) = '\0';
        return res;
    }
    char* string;

    DateTime dt = *((DateTime*)date);
    if (dt.isText) {
        string = malloc(sizeof(char) * strlen(dt.text));
        strcpy(string, dt.text);
    } else {
        string = malloc(sizeof(char) * (strlen(dt.date) + strlen(dt.time) + strlen(": ")));
        strcpy(string, dt.date);
        strcat(string, dt.time[0] == '\0' ? "" : ": ");
        strcat(string, dt.time);
    }

    return string;
}


char* cardToString(const Card* obj) {
    if (obj == NULL) {
        char* error = "Card NULL";
        char* res = malloc(strlen(error)+1);
        strcpy(res, error);
        nullTerminate(res);
        return res;
    }

    int size = 0;
    char* fn = propertyToString(obj->fn);
    size += strlen(fn);
    char* string = malloc(size+1);
    string[0] = '\0';
    strcat(string, fn);
    nullTerminate(string);
    free(fn);


    //date, birthday, etc

    ListIterator iterator = createIterator(obj->optionalProperties);
    void* prop = nextElement(&iterator);

    while (prop != NULL) {
        char* propString = propertyToString(prop);
        size += strlen(propString);
        string = realloc(string, size+1);
        strcat(string, propString);
        free(propString);
        nullTerminate(string);
        prop = nextElement(&iterator);
    }  

    return string;
}

void deleteCard(Card* obj) {
    if (obj == NULL) {
        return;
    }

    deleteProperty(obj->fn);
    freeList(obj->optionalProperties);

    if (obj->birthday != NULL) {
        deleteDate(obj->birthday);
    }
    if (obj->anniversary != NULL) {
        deleteDate(obj->anniversary);
    }
    free(obj);
}

VCardErrorCode createCard(char* fileName, Card** obj){
    //responsible for parsing only mandatory things and constructing card. 
    //any other parse work (parameters, etc) will be outsourced
    if (fileName == NULL) {
        return INV_FILE;
    }
    char* period = strpbrk(fileName, ".");
    if (period == NULL || (!stringMatch(period, ".vcard") && !stringMatch(period, ".vcf")) || obj == NULL) {
        VCardErrorCode error = INV_FILE;
        return error;
    }

    //get file
    FILE* fptr = fopen(fileName, "r");
    if (fptr == NULL) {
        VCardErrorCode error = INV_FILE;
        return error;
    }


    char *line1 = malloc(sizeof(char)*STRING_SIZE);
    char *line2 = malloc(sizeof(char)*STRING_SIZE);

    if (fgets(line1, STRING_SIZE, fptr) == NULL || fgets(line2, STRING_SIZE, fptr) == NULL){
        free(line1);
        free(line2);
        fclose(fptr);
        VCardErrorCode error = INV_CARD;
        return error;
    }

    stringToLower(line1);
    stringToLower(line2);

    Card* newCard = malloc(sizeof(Card));
    newCard->anniversary = NULL;
    newCard->birthday = NULL;
    newCard->fn = NULL;
    newCard->optionalProperties = NULL;
    //these will NOT stay null. to remove any uninitiliazed conditional errors.

    int hasFN = 0;
    int hasVersion = 0;
    int endLocation = -1;
    int hasBegin = 0;
    int endOfFile = 0;
    int hasAnniversery = 0;
    int hasBirthday = 0;
    int invalidRequiredProps = 0; //i.e. if we had begin:woootwoota1
    int lineReadCount = 0;

    newCard->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);

    while(endOfFile <= 1) {
        //NOTE::: could be a tab, check for full CRLF
        if (line2[0] == ' ' || checkTab(line2)) {

            int str1len = strlen(line1);

            //remove CRLF
            line1[str1len-2] = '\0';

            //remove space
            char* ptr = line2 + (checkTab(line2) ? 3 : 1);
            strcat(line1, ptr);

        } else {


            char* propName = parsePropertyName(line1);
            if (propName == NULL || endLocation != -1) {
                VCardErrorCode error = INV_CARD;
                free(line1);
                free(line2);
                free(propName);
                fclose(fptr);
                deleteCard(newCard);
                return error;
            }


            if (!stringMatch("begin", propName) && !stringMatch("version", propName) && !stringMatch("anniversary", propName) && !stringMatch("bday", propName) && !(stringMatch("end", propName)) && !(stringMatch("fn", propName))){
                //this is a regular property
                //process line 1
                int newProps = parsePropertys(line1, newCard);
                if (!newProps || lineReadCount < 2){
                    VCardErrorCode error = OTHER_ERROR;
                    if (!newProps) {
                        error = INV_PROP;
                    } else {
                        error = INV_CARD;
                    }
                    free(line1);
                    free(line2);
                    free(propName);
                    fclose(fptr);
                    deleteCard(newCard);
                    return error;
                } 


            } else {

                //count occurances of each prop with cardinality of either 1 or *1
                char* value = parsePropertyRightSide(line1);
                stringToLower(value);
                if (stringMatch("begin", propName) && stringMatch("vcard", value) && lineReadCount == 0) {
                    hasBegin++;
                } else if (stringMatch("version", propName) && stringMatch("4.0", value) && lineReadCount == 1) {
                    hasVersion++;
                } else if (stringMatch("bday", propName)) {
                    DateTime* dt = parseDateTime(value);
                    newCard->birthday = dt;
                    hasBirthday++;
                } else if (stringMatch("anniversary", propName)) {
                    DateTime* dt = parseDateTime(value);
                    newCard->anniversary = dt;
                    hasAnniversery++;
                } else if (stringMatch("end", propName) && stringMatch("vcard", value)) {
                    //endLocation = lineReadCount;
                    endOfFile = 2;
                } else if (stringMatch("fn", propName)){
                    int newProps = parsePropertys(line1, newCard);
                    if (newProps) {
                        hasFN++;
                    }
                } else {
                    invalidRequiredProps++;
                }
                
                free(value);
            }
            free(line1);
            free(propName);
            line1 = malloc(sizeof(char) * STRING_SIZE);
            strcpy(line1, line2);
            lineReadCount++;
        }

        fgets(line2, STRING_SIZE, fptr);
        if (feof(fptr)) {
            endOfFile++;
        }
    }


    stringToLower(line1);
    //line 1 at end should be END:VCARD
    if ((hasFN < 1) || (hasBegin != 1) || (hasVersion != 1) || (hasAnniversery > 1) || (hasBirthday > 1) || (invalidRequiredProps > 0) || (!stringMatch("end:vcard", line1) && !stringMatch("end:vcard\r\n", line1)) || endLocation != -1) {
        //check occurances of each prop
        VCardErrorCode error;
        if ((!stringMatch("end:vcard", line1) && !stringMatch("end:vcard\r\n", line1)) || hasBegin != 1) {
            error = INV_CARD;
        } else {
            error = INV_PROP;

        }

        free(line1);
        free(line2);
        fclose(fptr);
        deleteCard(newCard);
        return error;
    }

    free(line1);
    free(line2);
    fclose(fptr);

    *obj = newCard;
    VCardErrorCode res = OK;
    return res;

}


VCardErrorCode writeCard(const char* fileName, const Card* obj) {
    if (fileName == NULL || obj == NULL) {
        return WRITE_ERROR;
    }

    FILE* fptr = fopen(fileName, "w");
    if (fptr == NULL) {
        return WRITE_ERROR;
    }

    fprintf(fptr, "BEGIN:VCARD\r\n");
    fprintf(fptr, "VERSION:4.0\r\n");


    if (propertyToFile(fptr, obj->fn) == 0) {
        return WRITE_ERROR;
    }

    ListIterator iterator = createIterator(obj->optionalProperties);
    void* prop = nextElement(&iterator);

    while (prop != NULL) {
        if (propertyToFile(fptr, prop) == 0) {
            return WRITE_ERROR;
        }
        prop = nextElement(&iterator);
    }

    if (obj->birthday != NULL) {
        fprintf(fptr, "bday");
        if (dateToFile(fptr, obj->birthday) == 0){
            return WRITE_ERROR;
        }
    }

    if (obj->anniversary != NULL) {
        fprintf(fptr, "anniversary");
        if (dateToFile(fptr, obj->anniversary) == 0){
            return WRITE_ERROR;
        }
    }

    fprintf(fptr, "END:VCARD\r\n");

    fclose(fptr);

    return OK;
}

VCardErrorCode validateCard(const Card* obj) {

    if (obj == NULL) {
        return INV_CARD;
    }
    if (obj->fn == NULL || obj->optionalProperties == NULL) {
        return INV_CARD;
    }
    if (validateProperty(obj->fn, false) != OK) {
        return INV_PROP;
    }

    ListIterator iterator = createIterator(obj->optionalProperties);
    void* prop = nextElement(&iterator);

    while (prop != NULL) {
        VCardErrorCode res = validateProperty(prop, true);
        if (res != OK) {
            return res;
        }
        prop = nextElement(&iterator);
    }

    if (obj->birthday != NULL) {
        if (validateDateTime(obj->birthday) != OK) {
            return INV_DT;
        }
    }

    if (obj->anniversary != NULL) {
        if (validateDateTime(obj->anniversary) != OK) {
            return INV_DT;
        }
    }


    return OK;
}