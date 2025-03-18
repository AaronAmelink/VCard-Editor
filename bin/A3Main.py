#!/usr/bin/env python3
from curses import flash
from asciimatics.widgets import Frame, ListBox, Layout, Divider, Text, \
    Button, TextBox, Widget
from asciimatics.scene import Scene
from asciimatics.screen import Screen
from asciimatics.exceptions import ResizeScreenError, NextScene, StopApplication
import sys
from vCardContact import *


class ListView(Frame):
    def __init__(self, screen, contact_list=[]):
        super(ListView, self).__init__(screen,
                                       screen.height * 2 // 3,
                                       screen.width * 2 // 3,
                                       on_load=self._reload_list,
                                       hover_focus=True,
                                       can_scroll=True,
                                       title="Contact List")

        # Create the form for displaying the list of contacts.
        self._list_view = ListBox(
            Widget.FILL_FRAME,
            contact_list,
            name="contacts",
            add_scroll_bar=True,
            on_select=self._edit)
        




        main_layout = Layout([100], fill_frame=True)
        self.add_layout(main_layout)


        main_layout.add_widget(self._list_view)
        main_layout.add_widget(Divider())

        layout = Layout([1, 1, 1, 1])
        self.add_layout(layout)


        self._edit_button = Button("Edit", self._edit)
        self._create_button = Button("Create", self._create)
        self._db_query = Button("Query", self._query)

        layout.add_widget(self._edit_button, 0)
        layout.add_widget(self._create_button, 1)
        layout.add_widget(self._db_query, 2)
        layout.add_widget(Button("Quit", self._quit), 3)

        self.fix()

    def _reload_list(self, new_value=None):
        self._list_view.options = contact_manager.enum
        self._list_view.value = new_value

    def _create(self):
        raise NextScene("Create vCard")
    
    def _query(self):
        raise NextScene("Query vCard")

    def _edit(self):
        contact_manager.currently_selected = self._list_view.value
        raise NextScene("Edit vCard")
    
    @staticmethod
    def _quit():
        raise StopApplication("User pressed quit")


class vCardView(Frame):
    def __init__(self, screen):
        super(vCardView, self).__init__(screen,
                                          screen.height * 2 // 3,
                                          screen.width * 2 // 3,
                                          hover_focus=True,
                                          can_scroll=False,
                                          title="Edit Vcard",
                                          reduce_cpu=True)
        # Save off the model that accesses the contacts database.
        self.data = contact_manager.get_contact()
        # Create the form for displaying the list of contacts.
        layout = Layout([100], fill_frame=True)
        self.add_layout(layout)
        layout.add_widget(Text("File Name:", "file_name"))
        layout.add_widget(Text("Contact:", "contact_fn"))
        layout.add_widget(Text("Birthday:", "bday", readonly=True))
        layout.add_widget(Text("Anniversary:", "anniversary", readonly=True))
        layout.add_widget(Text("Other Properties:", "other_properties", readonly=True))

        layout2 = Layout([1, 1, 1])
        self.add_layout(layout2)
        layout2.add_widget(Button("OK", self._ok), 0)
        layout2.add_widget(Button("Cancel", self._cancel), 2)
        self.fix()

    def reset(self):
        # Do standard reset to clear out form, then populate with new data.
        super(vCardView, self).reset()
        self.data = contact_manager.get_contact()

    def _ok(self):
        self.save()
        if (not contact_manager.update_contact(self.data)):
            super(vCardView, self).reset()
        else:
            contact_manager.save_current_contact_to_file()
            raise NextScene("Main")

    @staticmethod
    def _cancel():
        raise NextScene("Main")

class vCardCreateView(Frame):
    def __init__(self, screen):
        super(vCardCreateView, self).__init__(screen,
                                          screen.height * 2 // 3,
                                          screen.width * 2 // 3,
                                          hover_focus=True,
                                          can_scroll=False,
                                          title="Create VCard",
                                          reduce_cpu=True)
        # Save off the model that accesses the contacts database.
        self.data = {"contact_fn": "", "anniversary": "", "bday": "", "file_name": "", "other_properties": ""}
        # Create the form for displaying the list of contacts.
        layout = Layout([100], fill_frame=True)
        self.add_layout(layout)
        layout.add_widget(Text("File Name:", "file_name"))
        layout.add_widget(Text("Contact:", "contact_fn"))
        layout.add_widget(Text("Birthday:", "bday", readonly=True))
        layout.add_widget(Text("Anniversary:", "anniversary", readonly=True))
        layout.add_widget(Text("Other Properties:", "other_properties", readonly=True))

        layout2 = Layout([1, 1, 1])
        self.add_layout(layout2)
        layout2.add_widget(Button("OK", self._ok), 0)
        layout2.add_widget(Button("Cancel", self._cancel), 2)
        self.fix()

    def reset(self):
        # Do standard reset to clear out form, then populate with new data.
        super(vCardCreateView, self).reset()

    def _ok(self):
        self.save()
        if (not contact_manager.create_contact(self.data)):
            super(vCardCreateView, self).reset()
        else:
            contact_manager.save_current_contact_to_file()
            raise NextScene("Main")

    @staticmethod
    def _cancel():
        raise NextScene("Main")

class QueryView(Frame):
    def __init__(self, screen):
        super(QueryView, self).__init__(screen,
                                            screen.height * 2 // 3,
                                            screen.width * 2 // 3,
                                            hover_focus=True,
                                            can_scroll=False,
                                            title="Query vCard",
                                            reduce_cpu=True)
        
            # Create the form for displaying the list of contacts.

        self._list_view = ListBox(
            Widget.FILL_FRAME,
            [],
            name="contacts",
            add_scroll_bar=True)


        layout = Layout([100], fill_frame=True)
        self.add_layout(layout)
        layout.add_widget(self._list_view)

        layout2 = Layout([1, 1, 1])
        self.add_layout(layout2)

        layout2.add_widget(Button("Display All", self._display), 0)
        layout2.add_widget(Button("Display contacts born in june", self._display), 1)

        layout2.add_widget(Button("Back", self._cancel), 2)
        self.fix()

    def _display(self):
        query = "SELECT * FROM CONTACT"
        if (contact_manager.sql_manager):
            try: 
                options = []
                res = contact_manager.sql_manager.run_query(query)
                if (res):
                    for i, j in enumerate(res):
                        options.append((j[1], i))
                self._list_view.options = options
            except mysql.connector.Error as err:
                pass
        
        self.fix()

    @staticmethod
    def _cancel():
        raise NextScene("Main")


class LoginView(Frame):
    def __init__(self, screen):
        super(LoginView, self).__init__(screen,
                                          screen.height * 2 // 3,
                                          screen.width * 2 // 3,
                                          hover_focus=True,
                                          can_scroll=False,
                                          title="Login",
                                          reduce_cpu=True)
        # Save off the model that accesses the contacts database.
        # Create the form for displaying the list of contacts.
        layout = Layout([100], fill_frame=True)
        self.add_layout(layout)
        layout.add_widget(Text("Database Name:", "db_name"))
        layout.add_widget(Text("User name:", "db_user"))
        layout.add_widget(Text("Password:", "db_pass"))
        layout.add_widget(Text("", "error", readonly=True))


        layout2 = Layout([1, 1, 1])
        self.add_layout(layout2)
        layout2.add_widget(Button("OK", self._ok), 0)
        layout2.add_widget(Button("Cancel", self._cancel), 2)
        self.fix()

    def _ok(self):
        self.save()
        try:
            contact_manager.init_sql(self.data["db_name"], self.data["db_user"], self.data["db_pass"])
            contact_manager.load_contacts() 
            raise NextScene("Main")

        except mysql.connector.Error as err:
            self.data = {"error": "Invalid Login"}
            self.fix()

        contact_manager.load_contacts()  

    @staticmethod
    def _cancel():
        raise NextScene("Main")

def demo(screen, scene):
    scenes = [
        Scene([LoginView(screen)], -1, name="Login"),
        Scene([ListView(screen, contact_manager.enum)], -1, name="Main"),
        Scene([vCardView(screen)], -1, name="Edit vCard"),
        Scene([vCardCreateView(screen)], -1, name="Create vCard"),
        Scene([QueryView(screen)], -1, name="Query vCard")
    ]

    screen.play(scenes, stop_on_resize=True, start_scene=scene, allow_int=True)

last_scene = None

contact_manager = ContactManager()

while True:
    try:
        Screen.wrapper(demo, catch_interrupt=True, arguments=[last_scene])
        sys.exit(0)
    except ResizeScreenError as e:
        last_scene = e.scene