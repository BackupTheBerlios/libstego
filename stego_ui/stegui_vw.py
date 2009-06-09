import sys
sys.path.append("../python_bindings/")
import stegui_ctrl
import pygtk
import gtk
import gtk.glade
import gobject
import os


class Window:
    def __init__(self):
        self.xml = gtk.glade.XML(os.path.join(os.path.dirname(sys.argv[0]), 'steg_io1.glade'))
	self.window1 = self.xml.get_widget('window1')
        self.input_file_window = self.xml.get_widget('input_file')
        dic = {}
        for key in dir(self.__class__):
	    dic[key] = getattr(self,key)
        self.xml.signal_autoconnect(dic)
	self.window1.show()

    def input_file_chosen_clicked(self, widget):
        self.window1.hide()
        self.window2 = self.xml.get_widget('window2')
        self.window2.show()

    def input_file_set(self, widget):
        self.filename = widget.get_filename()
        file_ending = self.filename[-3:]
        print file_ending
        if not file_ending in ["gif","jpg","jpeg","png","wav","svg"]:
            self.file_input_info_text = self.xml.get_widget('file_input_info_text')
            self.file_input_info_text.set_label("Es werden nur gif, jpeg, jpeg,\n png, wav und svg als Formate unterstuetzt.\n Bitte waehlen sie eine andere Datei aus.")
            self.input_file_window.set_from_file("error.png")
        else:
            self.input_file_window.set_from_file(widget.get_filename())

    def on_message_typed_in_clicked(self, widget):
        msg_field = self.xml.get_widget('message_field')
        end = msg_field.get_buffer().get_end_iter()
        start = msg_field.get_buffer().get_start_iter()
        self.message = msg_field.get_buffer().get_text(start,end,True)
        self.window2.hide()
        self.window3 = self.xml.get_widget('window3')
        self.algo_chooser = self.xml.get_widget('algorithm_chooser')
        self.model = self.algo_chooser.get_model()
        algos = stegui_ctrl.get_algorithms(self.filename,self.message)
        for algo in algos:
            self.model.append([algo])
        self.window3.show()

    def on_back_to_input_file_clicked(self, widget):
        self.window2.hide()
        self.window1.show()



        
if __name__ == "__main__":
    Window()
    gtk.main()

