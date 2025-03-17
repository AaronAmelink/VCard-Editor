
from ctypes import *
import os


def create_c_string(string):
    return c_char_p(string.encode('utf-8'))

class vCardContact:
    def __init__(self, file_name=""):

        print(file_name)

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

        self._contact_fn = None
        self._anniversary = None
        self._bday = None
        self._file_name = None
        self._other_properties = None

        if (file_name == ""):
            self._vCard_ptr = self.vCardLib.createVCardPointer(c_char_p(0))

        # run c function to parse file_name / validate it
        else :
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
            if (self._file_name == None):
                raise ValueError("No file name provided")
            else:
                file_name = self._file_name
        res = self.vCardLib.writeCard(create_c_string(file_name), self._vCard_ptr)
        if (res != 0):
            raise ValueError("Failed to write to file")
    
    #in all setter functions, we should update the struct, call validate, then update the database.

    @property
    def contact_fn(self):
        if (self._contact_fn == None):
            return ""
        return self._contact_fn

    @contact_fn.setter
    def contact_fn(self, v):
        if (v == None or v == ""):
            raise ValueError("Contact Name cannot be empty")

        res = self.vCardLib.setCardFN(self._vCard_ptr, create_c_string(v))
        if (res == 0):
            raise ValueError("Failed to set contact_fn")
        else:
            if (self.validate() != 0):
                if (self._contact_fn != None):
                    self.vCardLib.setCardFN(self._vCard_ptr, create_c_string(self._contact_fn))
                raise ValueError("Invalid Contact")
        self._contact_fn = v

    @property
    def anniversary(self):
        return self._anniversary
    
    @anniversary.setter
    def anniversary(self, v):
        if (v == None):
            self.vCardLib.removeCardAnniversary(self._vCard_ptr)
            self._anniversary = None
            return

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
        if (v == None):
            self.vCardLib.removeCardBirthday(self._vCard_ptr)
            self._bday = None
            return
        
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
        if (self._file_name == None):
            return "No File Name"
        return self._file_name
    
    @file_name.setter
    def file_name(self, v):
        self._file_name = v

    @property
    def other_properties(self):
        return str(self._other_properties)
    
    @other_properties.setter
    def other_properties(self, v):
        #this should be constant.
        pass


class ContactManager:
    def __init__(self):
        self.contacts = []
        self.enum = []
        self.currently_selected = 0
    
    def add_contact(self, contact):
        self.contacts.append(contact)
        self.enum.append((contact.contact_fn, len(self.contacts)-1))

    def get_enum(self):
        self.enum = []
        for i, j in enumerate(self.contacts):
            self.enum.append((j.contact_fn, i))
        return self.enum

    def remove_contact(self, index):
        self.contacts.pop(index)
        self.enum.pop(index)


    def load_contacts(self):
        files = os.listdir( os.path.abspath(os.getcwd()) + "/bin/cards/")
        for file in files:
            if file.endswith(".vcf") or file.endswith(".vcard"):
                self.add_contact(vCardContact(os.path.abspath(os.getcwd()) +"/bin/cards/"+file))

    def save_contacts_to_file(self):
        for contact in self.contacts:
            contact.write_to_file()

    def get_contact(self, index=-1):
        contact = self.contacts[self.currently_selected if index == -1 else index]
        return {"contact_fn": contact.contact_fn, "anniversary": contact.anniversary, "bday": contact.bday, "file_name": contact.file_name, "other_properties": contact.other_properties}

    def create_contact(self, data):
        try:
            self.add_contact(vCardContact())
            self.contacts[-1].contact_fn = data["contact_fn"]
            self.contacts[-1].anniversary = data["anniversary"] if data["anniversary"] != "" else None
            self.contacts[-1].bday = data["bday"] if data["bday"] != "" else None
            self.contacts[-1].file_name = data["file_name"]
            self.contacts[-1].other_properties = 0
            self.get_enum()
            return True
        except ValueError as e:
            self.remove_contact(-1)
            return False

    def update_contact(self, data):

        try:
            self.contacts[self.currently_selected].anniversary = data["anniversary"] if data["anniversary"] != "" else None
            self.contacts[self.currently_selected].bday = data["bday"] if data["bday"] != "" else None
            self.contacts[self.currently_selected].file_name = data["file_name"]
            self.contacts[self.currently_selected].other_properties = data["other_properties"]
            self.contacts[self.currently_selected].contact_fn = data["contact_fn"]
            self.get_enum()
            return True
        except ValueError as e:
            return False

