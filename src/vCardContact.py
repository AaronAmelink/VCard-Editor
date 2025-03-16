
from ctypes import *

def create_c_string(string):
    return c_char_p(string.encode('utf-8'))

class vCardContact:
    def __init__(self, file_name=""):

        self.vCardLib = CDLL("libvcparser.so")
        if not self.vCardLib:
            print("Failed to load libvcparser.so")
            exit(1)

        #setup return values

        self.vCardLib.createVCardPointer.restype = c_void_p
        self.vCardLib.validateCard.restype = c_int
        self.vCardLib.getCardFN.restype = c_char_p
        self.vCardLib.getCardAnniversary.restype = c_char_p
        self.vCardLib.getCardBirthday.restype = c_char_p
        self.vCardLib.getCardOtherPropertyNumbers.restype = c_int
        self.vCardLib.writeCard.restype = c_int
        self.vCardLib.writeCard.argtypes = [c_char_p, c_void_p]


        # run c function to parse file_name / validate it
        self._file_name = file_name
        self._vCard_ptr = self.vCardLib.createVCardPointer(create_c_string(file_name))
        if (self.validate() != 0):
            #0 is code for OK
            print("Failed to validate vCard")
            return None
        else:
            self._other_properties = self.vCardLib.getCardOtherPropertyNumbers(self._vCard_ptr)
            self._contact_fn = self.vCardLib.getCardFN(self._vCard_ptr).decode('utf-8')
            self._anniversary = self._get_anniverstary_from_file()
            self._bday = self._get_birthday_from_file()

    def _get_anniverstary_from_file(self):
        res = self.vCardLib.getCardAnniversary(self._vCard_ptr)
        if (res):
            return res.decode('utf-8')
        else:
            return None
        
    def _get_birthday_from_file(self):
        res = self.vCardLib.getCardBirthday(self._vCard_ptr)
        if (res):
            return res.decode('utf-8')
        else:
            return None
    
    def __str__(self):
        return (f"{self.contact_fn} {self.anniversary} {self.bday} {self.file_name} {self.other_properties}")
    
    def validate(self):
        return self.vCardLib.validateCard(self._vCard_ptr)
    
    def write_to_file(self, file_name=""):
        if (file_name == ""):
            file_name = self._file_name
        res = self.vCardLib.writeCard(create_c_string(file_name), self._vCard_ptr)
        if (res != 0):
            raise ValueError("Failed to write to file")
    
    #in all setter functions, we should update the struct, call validate, then update the database.

    @property
    def contact_fn(self):
        return self._contact_fn

    @contact_fn.setter
    def contact_fn(self, v):
        res = self.vCardLib.setCardFN(self._vCard_ptr, create_c_string(v))
        if (res == 0):
            raise ValueError("Failed to set contact_fn")
        else:
            if (self.validate() != 0):
                self.vCardLib.setCardFN(self._vCard_ptr, create_c_string(self._contact_fn))
                raise ValueError("Invalid Contact")
        self._contact_fn = v

    @property
    def anniversary(self):
        return self._anniversary
    
    @anniversary.setter
    def anniversary(self, v):
        res = self.vCardLib.setCardAnniversary(self._vCard_ptr, create_c_string(v))
        if (res == 0):
            raise ValueError("Failed to set anniversary")
        else:
            if (self.validate() != 0):
                self.vCardLib.setCardAnniversary(self._vCard_ptr, create_c_string(self._anniversary))
                raise ValueError("Invalid Anniversary")
        self._anniversary = v

    @property
    def bday(self):
        return self._bday

    @bday.setter
    def bday(self, v):
        res = self.vCardLib.setCardBirthday(self._vCard_ptr, create_c_string(v))
        if (res == 0):
            raise ValueError("Failed to set bday")
        else:
            if (self.validate() != 0):
                self.vCardLib.setCardBirthday(self._vCard_ptr, create_c_string(self._bday))
                raise ValueError("Invalid Birthday")
        self._bday = v

    @property
    def file_name(self):
        return self._file_name
    
    @file_name.setter
    def file_name(self, v):
        self._file_name = v

    @property
    def other_properties(self):
        return self._other_properties
    
    @other_properties.setter
    def other_properties(self, v):
        #this should be constant.
        pass


test = vCardContact("testCardMin.vcf")
print(test)
test.contact_fn = "New Name"
test.anniversary = "New Anniversary"
test.bday = "New Birthday"
print(test)
test.write_to_file("testCardMin3.vcf")