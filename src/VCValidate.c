#include "VCParser.h"
#include "VCHelper.h"

//Name: Aaron Amelink
//Student ID: 1266687


//function stubs
static int validateKind(const Property* prop);

static int validateN(const Property* prop);

static int validateParameter(Parameter* param);

static int validateParameters(List* parameters);

static int validatePropertyValuesNotNull(List* values);

static int validateUID(const Property* prop);

static int validateREV(const Property* prop);

static int validatePRODID(const Property* prop);

static int validateGender(const Property* prop);

//end function stubs

static int validateParameter(Parameter* param) {
    if (param == NULL) {
        return 0;
    }
    if (param->name == NULL || param->value == NULL) {
        return 0;
    }
    if (stringMatch(param->name, "") || stringMatch(param->value, "")) {
        return 0;
    }
    return 1;
}

static int validateParameters(List* parameters) {
    if (parameters == NULL) {
        return 0;
    }
    ListIterator iterator = createIterator(parameters);
    void* param = nextElement(&iterator);

    while (param != NULL) {
        if (!validateParameter((Parameter*)param)) {
            return 0;
        }
        param = nextElement(&iterator);
    }
    return 1;
}

static int validatePropertyValuesNotNull(List* values) {
    if (values == NULL) {
        return 0;
    }
    int count = 0;
    ListIterator valIter = createIterator(values);
    void* val = nextElement(&valIter);
    while(val != NULL) {
        count++;
        val = nextElement(&valIter);
    }
    return count == values->length;
}

VCardErrorCode validateDateTime(DateTime* dt) {
    if (dt == NULL) {
        return INV_DT;
    }
    if (dt->date == NULL || dt->time == NULL || dt->text == NULL) {
        return INV_DT;
    }
    if (!dt->isText && !stringMatch(dt->text, "")) {
        return INV_DT;
    }
    if (dt->UTC && dt->isText) {
        return INV_DT;
    }
    if (dt->isText && (!stringMatch(dt->date, "") || !stringMatch(dt->time, ""))) {
        return INV_DT;
    }

    return OK;
}


VCardErrorCode validateProperty(const Property* prop, bool isOptional) {
    if (prop == NULL) {
        return INV_PROP;
    }

    if (prop->name == NULL || prop->group == NULL || stringMatch(prop->name, "") || prop->values == NULL) {
        return INV_PROP;
    }

    if (!validateParameters(prop->parameters)) {
        return INV_PROP;
    }

    if (!validatePropertyValuesNotNull(prop->values)) {
        return INV_PROP;
    }
    
    char* lowerName = malloc(strlen(prop->name)+1);
    strcpy(lowerName, prop->name);
    stringToLower(lowerName);

    if (prop->values->length == 0) { //all properties must have at least one value
        free(lowerName);
        return INV_PROP;
    }

    //ew ew ew ew, but no hashmaps so :/
    if (stringMatch(lowerName, "source")) {
        //cardinality *, no restrictions
        
    } else if (stringMatch(lowerName, "kind")) {
        free(lowerName);
        return validateKind(prop) == 0 ? INV_PROP : OK;

    } else if (stringMatch(lowerName, "xml")) {
        //cardinality *, no restrictions other then length

    } else if (stringMatch(lowerName, "n")) {
        free(lowerName);
        return validateN(prop) == 0 ? INV_PROP : OK;

    } else if (stringMatch(lowerName, "nickname")) {
        //zero restictions with nickname

    } else if (stringMatch(lowerName, "photo")) {
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "gender")) {
        free(lowerName);
        return validateGender(prop) == 0 ? INV_PROP : OK;

    } else if (stringMatch(lowerName, "adr")) {
        //no restrictions

    } else if (stringMatch(lowerName, "tel")) {
        //no restrictions
    } else if (stringMatch(lowerName, "email")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }

    } else if (stringMatch(lowerName, "impp")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "lang")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "tz")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "geo")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "title")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "role")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "logo")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "org")) {
        //no restrictions
    } else if (stringMatch(lowerName, "member")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "related")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "categories")) {
        //only length restriction
        //per spec: One or more text values separated by a COMMA character
        //as we only process via ';', these values would be appearing a 1 value.
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "note")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "prodid")) {
        free(lowerName);
        return validatePRODID(prop) == 0 ? INV_PROP : OK;
    
    } else if (stringMatch(lowerName, "rev")) {
        free(lowerName);
        return validateREV(prop) == 0 ? INV_PROP : OK;
    
    } else if (stringMatch(lowerName, "sound")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "uid")) {
        free(lowerName);
        return validateUID(prop) == 0 ? INV_PROP : OK;
    } else if (stringMatch(lowerName, "clientpidmap")) {
        //only length restriction
        if (prop->values->length != 2) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "url")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "key")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "fburl")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "caladruri")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "caluri")) {
        //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "fn") && !isOptional) {
       //only length restriction
        if (prop->values->length != 1) {
            free(lowerName);
            return INV_PROP;
        }
    } else if (stringMatch(lowerName, "bday") || stringMatch(lowerName, "anniversary")){
        free(lowerName);
        return INV_DT;
    } else {
        free(lowerName);
        return INV_CARD;
    }

    free(lowerName);
    return OK;
}

static int validateKind(const Property* prop) {
    if (prop == NULL) {
        return 0;
    }
    static int amt = 0;

    amt++;

    if (amt > 1) {
        return 0;
    }
    if (prop->values->length != 1) {
        return 0;
    }
    ListIterator iter = createIterator(prop->values);
    void* value = nextElement(&iter);
    if (value == NULL) {
        return 0;
    }
    return 1;
}

static int validateN(const Property* prop) {
    if (prop == NULL) {
        return 0;
    }
    if (prop->values->length != 5) {
        return 0;
    }
    static int amt = 0;
    amt++;
    if (amt > 1) {
        return 0;
    }
    return 1;
}

static int validateGender(const Property* prop) {
    if (prop == NULL) {
        return 0;
    }
    if (prop->values->length > 2 ) {
        return 0;
    }
    static int amt = 0;
    amt++;
    if (amt > 1) {
        return 0;
    }

     
    return 1;
}


//i would *love* to do these one only functions as a hashmap in the general validate function, but this is c
//so the cleanest way to do this is to have a function for each property
//basically just trying to keep the validateProperty function clean of all the property specific validation
static int validatePRODID(const Property* prop) {
    if (prop == NULL) {
        return 0;
    }
    if (prop->values->length != 1) {
        return 0;
    }
    static int amt = 0;
    amt++;
    if (amt > 1) {
        return 0;
    }
    return 1;
}

static int validateREV(const Property* prop) {
    if (prop == NULL) {
        return 0;
    }
    if (prop->values->length != 1) {
        return 0;
    }
    static int amt = 0;
    amt++;
    if (amt > 1) {
        return 0;
    }
    return 1;
}

static int validateUID(const Property* prop) {
    if (prop == NULL) {
        return 0;
    }
    if (prop->values->length != 1) {
        return 0;
    }
    static int amt = 0;
    amt++;
    if (amt > 1) {
        return 0;
    }
    return 1;
}