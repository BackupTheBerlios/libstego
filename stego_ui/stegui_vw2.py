# coding=utf-8
import sys
sys.path.append("../python_bindings/")
import stegui_ctrl
import pygtk
import gtk
import gtk.glade
import gobject
import os

class Window:
#################### EMBED OR EXTRACT? ########################
    def __init__(self):
        """Initiate with window to choose either to embed or to
        extract a message.
        """
        self.xml = gtk.glade.XML(os.path.join(os.path.dirname(sys.argv[0]), 'steg_io2.glade'))
        self.window4 = self.xml.get_widget('window4')
        self.input_file_window = self.xml.get_widget('input_file')
        dic = {"on_window_destroy" : gtk.main_quit}
        for key in dir(self.__class__):
            dic[key] = getattr(self,key)
        self.xml.signal_autoconnect(dic)
        self.embed = True
        self.extract = False
        self.window4.show()

    def on_radio_embed_toggled(self,widget):
        """Toggles the embed radio button."""
        self.embed = True
        self.extract = False

    def on_radio_extract_toggled(self, widget):
        """Toggles the extract radio button."""
        self.extract = True
        self.embed = False

    def on_embed_or_extract_chosen_clicked(self, widget):
        """When the next button on the "embed or extract" window
        is clicked, this function grabs the chose radio button
        and opens either the first embedding or the first extraction
        window.
        """
        self.window4.hide()
        self.window3 = self.xml.get_widget('window3')
        self.algo_chooser = self.xml.get_widget('algorithm_chooser')
        self.model = self.algo_chooser.get_model()
        self.model.clear()
        algos = stegui_ctrl.algos
        for algo in algos:
            self.model.append([algo])
        self.window3.show()


#################### CHOOSE ALGORITHM #########################

# TODO: Add audio algorithms!
    def on_back_to_embed_or_extract_clicked(self,ding):
        """When back button on window to choose algorithm gets clicked,
        this function hides the actual window and shows the "embed or
        extract" window.
        """
        self.window3.hide()
        self.window4.show()

    def on_algorithm_chooser_changed(self, widget):
        if not self.algo_chooser.get_active_text() == None:
            self.algo = self.algo_chooser.get_active_text()
            label = self.xml.get_widget('algorithm_info')
            if self.algo == "Battlesteg":
                label.set_text("")
            elif self.algo == "Sort / Unsort":
                label.set_text("")
            elif self.algo == "CPT":
                label.set_text("")
            elif self.algo == "LSB":
                label.set_text("Der LSB-Algorithmus nutzt die niederwertigsten Bits (Least Significant Bits) des Covers um eine Nachricht einzubetten. Eine zufällige Verteilung der Nachrichtenbits ist möglich.")
            elif self.algo == "F5":
                label.set_text("")
            elif self.algo == "Frirui":
                label.set_text("")
            elif self.algo == "GifShuffle":
                label.set_text("")
            elif self.algo == "PQ":
                label.set_text("")
            elif self.algo == "SVG Steg":
                label.set_text("SVGStego bettet ähnlich dem LSB-Algorithmus die Nachricht in den Nachkommastellen der Komponenten von Transformationsmatrizen in Scalable Vector Graphics (SVG) ein.")


    def on_algorithm_chosen_clicked(self, widget):
        """When next button on window to choose algorithm get clicked,
        this function shows the window to choose it's parameters.
        """
        if not self.algo_chooser.get_active_text() == None:
            self.window3.hide()
            if self.algo == "Battlesteg":
                self.window_battlesteg_param = self.xml.get_widget('battlesteg_param_window')
                self.window_battlesteg_param.show()
            elif self.algo == "Sort / Unsort":
                self.window_sortunsort_param = self.xml.get_widget('sortunsort_param_window')
                self.window_sortunsort_param.show()
            elif self.algo == "CPT":
                self.window_cpt_param = self.xml.get_widget('cpt_param_window')
                self.window_cpt_param.show()
            elif self.algo == "LSB":
                self.window_lsb_param = self.xml.get_widget('lsb_param_window')
                self.window_lsb_param.show()
            elif self.algo == "F5":
                self.window_f5_param = self.xml.get_widget('f5_param_window')
                self.window_f5_param.show()
            elif self.algo == "Frirui":
                self.window_frirui_param = self.xml.get_widget('frirui_param_window')
                self.window_frirui_param.show()
            elif self.algo == "GifShuffle":
                self.window_gifshuffle_param = self.xml.get_widget('gifshuffle_param_window')
                self.window_gifshuffle_param.show()
            elif self.algo == "PQ":
                self.window_pq_param = self.xml.get_widget('pq_param_window')
                self.window_pq_param.show()
            elif self.algo == "SVG Steg":
                self.window_svg_param = self.xml.get_widget('svg_param_window')
                self.window_svg_param.show()

######################### SET PARAMETERS ##############################
# TODO: Add audio algorithms!
    def params_chosen(self):
        """This function gets called by the <algorithm>_params_chosen
        functions of each algorithm.
        If we are embedding, it shows the window to choose an input
        file and sets it's text to the appropriate file format.
        If we are extracting, it opens the same window but changes
        the text to ask for the steganogram.
        """
        self.window1 = self.xml.get_widget('window1')
        self.file_input_text = self.xml.get_widget('file_input_info_text')
        self.format = stegui_ctrl.get_appropiate_format(self.algo)
        if self.embed:
            self.file_input_text.set_text(
                    "Bitte wählen Sie eine "+self.format+"-Datei aus.")
        else:
            self.file_input_text.set_text(
                    "Bitte wählen Sie die "+self.format+"-Datei aus, welche die Geheimnachricht enthält.")
        self.window1.show()

    def on_battlesteg_parameters_chosen_clicked(self, widget):
        """When next button in battlesteg parameters window gets
        clicked, this function calls stegui_ctrl.create_battlesteg_parameter
        to produce a new battlesteg parameter.
        """
        startbit = self.xml.get_widget('battlesteg_startbit').get_value_as_int()
        ranged_shots = self.xml.get_widget('battlesteg_ranged_shots').get_value_as_int()
        range = self.xml.get_widget('battlesteg_range').get_value_as_int()
        filter = 0
        if self.xml.get_widget('battlesteg_filter').get_active_text() == "Laplace":
            filter = 0
        password = self.xml.get_widget('battlesteg_password').get_text()
        if password != "":
            self.window_battlesteg_param.hide()
            self.para = stegui_ctrl.create_battlesteg_parameter(startbit,
                    ranged_shots,range,filter,password)
            self.params_chosen()

    def on_sortunsort_parameters_chosen_clicked(self, widget):
        """When next button in sortunsort parameters window gets
        clicked, this function calls stegui_ctrl.create_sortunsort_parameter
        to produce a new sortunsort parameter.
        """
        password = self.xml.get_widget('sortunsort_password').get_text()
        if password != "":
            self.window_sortunsort_param.hide()
            self.para = stegui_ctrl.create_sortunsort_parameter(password)
            self.params_chosen()

    def on_f5_parameters_chosen_clicked(self, widget):
        """When next button in f5 parameters window gets
        clicked, this function calls stegui_ctrl.create_f5_parameter
        to produce a new f5 parameter.
        """
        password = self.xml.get_widget('f5_password').get_text()
        if password != "":
            self.window_f5_param.hide()
            self.para = stegui_ctrl.create_f5_parameter(password)
            self.params_chosen()

    def on_gifshuffle_parameters_chosen_clicked(self, widget):
        """When next button in gifshuffle parameters window gets
        clicked, this function calls stegui_ctrl.create_gifshuffle_parameter
        to produce a new gifshuffle parameter.
        """
        password = self.xml.get_widget('gifshuffle_password').get_text()
        method = 2
        if (self.xml.get_widget('gifshuffle_method').get_active_text()
                =="GifShuffle (Standard)"):
            method = 0
        elif (self.xml.get_widget('gifshuffle_method').get_active_text()
                == "Advanced GifShuffle"):
            method = 1
        if method != 2:
            self.window_gifshuffle_param.hide()
            self.para = stegui_ctrl.create_gifshuffle_parameter(password, method)
            self.params_chosen()

    def on_pq_parameters_chosen_clicked(self, widget):
        """When next button in pq parameters window gets
        clicked, this function calls stegui_ctrl.create_pq_parameter
        to produce a new pq parameter.
        """
        password = self.xml.get_widget('pq_password').get_text()
        if password != "":
            self.window_pq_param.hide()
            self.para = stegui_ctrl.create_pq_parameter(password)
            self.params_chosen()

    def on_lsb_parameters_chosen_clicked(self, widget):
        """When next button in lsb parameters window gets
        clicked, this function calls stegui_ctrl.create_lsb_parameter
        to produce a new lsb parameter.
        """
        password = self.xml.get_widget('lsb_password').get_text()
        use_msb = self.xml.get_widget('lsb_use_msb').get_active()
        method = 0
        if (self.xml.get_widget('lsb_method').get_active_text()
                =="fortlaufend"):
            method = 1
        elif (self.xml.get_widget('lsb_method').get_active_text()
                == "zufällig"):
            method = 2

        print "use_msb: " + `use_msb`
        self.window_lsb_param.hide()
        self.para = stegui_ctrl.create_lsb_parameter(password, method, use_msb)
        self.params_chosen()

    def on_svg_parameters_chosen_clicked(self, widget):
        """When next button in svg parameters window gets
        clicked, this function calls stegui_ctrl.create_svg_parameter
        to produce a new svg parameter.
        """
        password = self.xml.get_widget('svg_password').get_text()
        digit = self.xml.get_widget('svg_first_embed_digit').get_value_as_int()
        if password != "":
            self.window_svg_param.hide()
            self.para = stegui_ctrl.create_svg_parameter(password, digit)
            self.params_chosen()

    def on_frirui_parameters_chosen_clicked(self, widget):
        """When next button in frirui parameters window gets
        clicked, this function calls stegui_ctrl.create_frirui_parameter
        to produce a new frirui parameter.
        """
        password = self.xml.get_widget('frirui_password').get_text()
        method = 3
        if (self.xml.get_widget('frirui_method').get_active_text()
                =="Pixelketten"):
            method = 0
        elif (self.xml.get_widget('frirui_method').get_active_text()
                == "2x2-Block"):
            method = 1
        elif (self.xml.get_widget('frirui_method').get_active_text()
                == "3x3-Block"):
            method = 2
        size = self.xml.get_widget('frirui_size').get_value_as_int()
        threshold = self.xml.get_widget('frirui_threshold').get_value_as_int()
        if password != "" and method != 3:
            self.window_frirui_param.hide()
            self.para = stegui_ctrl.create_frirui_parameter(password,
                    method, size, threshold)
            self.params_chosen()

    def on_cpt_parameters_chosen_clicked(self, widget):
        """When next button in cpt parameters window gets
        clicked, this function calls stegui_ctrl.create_cpt_parameter
        to produce a new cpt parameter.
        """
        password = self.xml.get_widget('cpt_password').get_text()
        size_x = self.xml.get_widget('cpt_size_x').get_value_as_int()
        size_y = self.xml.get_widget('cpt_size_y').get_value_as_int()
        if password != "":
            self.window_cpt_param.hide()
            self.para = stegui_ctrl.create_cpt_parameter(password,
                    size_x, size_y)
            self.params_chosen()

    def on_battlesteg_param_back_clicked(self, widget):
        """When back button in battlesteg parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_battlesteg_param.hide()
        self.window3.show()

    def on_sortunsort_param_back_clicked(self, widget):
        """When back button in sortunsort parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_sortunsort_param.hide()
        self.window3.show()

    def on_f5_param_back_clicked(self, widget):
        """When back button in f5 parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_f5_param.hide()
        self.window3.show()

    def on_gifshuffle_param_back_clicked(self, widget):
        """When back button in gifshuffle parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_gifshuffle_param.hide()
        self.window3.show()

    def on_pq_param_back_clicked(self, widget):
        """When back button in pq parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_pq_param.hide()
        self.window3.show()

    def on_lsb_param_back_clicked(self, widget):
        """When back button in lsb parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_lsb_param.hide()
        self.window3.show()

    def on_svg_param_back_clicked(self, widget):
        """When back button in svg parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_svg_param.hide()
        self.window3.show()

    def on_frirui_param_back_clicked(self, widget):
        """When back button in frirui parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_frirui_param.hide()
        self.window3.show()

    def on_cpt_param_back_clicked(self, widget):
        """When back button in cpt parameters window gets
        clicked, this function shows the window to choose an algorithm again.
        """
        self.window_cpt_param.hide()
        self.window3.show()

##################### CHOOSE INPUT FILE #######################

    def input_file_chosen_clicked(self, widget):
        """When next button in "choose input file" window gets clicked,
        this function uses stegui_ctrl.get_capacity to get the
        capacity and saves it in self.capacity.
        """
        self.filename = self.xml.get_widget('choose_input_file').get_filename()
        if self.embed:
#TODO: being a supported filetype is not enough!
            if self.filename[-3:] in stegui_ctrl.supported_filetypes:
                self.window1.hide()
                self.cap = stegui_ctrl.get_capacity(self.algo,
                        self.filename, self.para) / 8
                self.window5 = self.xml.get_widget('window5')
                self.window5.show()
        else:
            self.message = stegui_ctrl.embed_or_extract(self.algo,
                    self.filename, None,None, self.para, False)
            self.msg_field = self.xml.get_widget('hidden_message')
            self.msg_field.get_buffer().set_text(self.message)
            self.window7 = self.xml.get_widget('window7')
            self.window1.hide()
            self.window7.show()


# TODO: Add pixmaps for file formats that cannot be displayed
    def input_file_set(self, widget):
        """When an input file is set, this function check whether
        or not the given file has the appropriate format.
        If it has not, an error message gets printed.
        If if has, the window to choose an output file get called.
        """
        self.filename = widget.get_filename()
        file_ending = self.filename[-3:]
        if file_ending.lower() == "peg":
            file_ending = "jpg"
        if not file_ending.lower() == self.format.lower() :
            self.file_input_info_text = self.xml.get_widget('file_input_info_text')
            self.file_input_info_text.set_label(
                    "Fehler! Sie müssen eine "+self.format+"-Datei angeben!")
            self.input_file_window.set_from_file("error.png")
        else:
            self.input_file_window.set_from_file(widget.get_filename())

# TODO: Add Audio algorithms!
    def on_choose_input_file_back_clicked(self, widget):
        """When back button in "choose input file" window gets clicked,
        this function calls the parameter window for the chosen algorithm.
        """
        self.window1.hide()
        if self.algo == "Battlesteg":
            self.window_battlesteg_param.show()
        elif self.algo == "Sort / Unsort":
            self.window_sortunsort_param.show()
        elif self.algo == "CPT":
            self.window_cpt_param.show()
        elif self.algo == "F5":
            self.window_f5_param.show()
        elif self.algo == "Frirui":
            self.window_frirui_param.show()
        elif self.algo == "GifShuffle":
            self.window_gifshuffle_param.show()
        elif self.algo == "LSB":
            self.window_lsb_param.show()
        elif self.algo == "PW":
            self.window_pq_param.show()
        elif self.algo == "SVG Steg":
            self.window_svg_param.show()


###################### CHOOSE OUTPUT FILE #######################

    def on_dest_file_chosen_clicked(self, widget):
        """When next button on "choose output file" window gets
        clicked, this function set it's file ending to the appropriate
        format if none given.
        It then shows "enter message" window and uses self.capacity
        to show the user how many character he can type in.
        """
        folder = self.xml.get_widget('dest_folder').get_filename()
        file = self.xml.get_widget('dest_file').get_text()
        if not folder == "" or file == "":
            if file[-3:] != self.format:
                file = file + "." + self.format.lower()
            self.window5.hide()
            self.window2 = self.xml.get_widget('window2')
            self.destfile = folder + "/" + file
            self.msg_in_label = self.xml.get_widget('message_input_label')
            print self.cap
            self.msg_in_label.set_text("Bitte geben Sie nun die geheime Nachricht ein.\nMaximal können Sie "+str(self.cap)+" Zeichen eingeben.")
            self.window2.show()

    def on_back_to_input_file_clicked(self, widget):
        """When back button in "choose output file" window gets
        clicked, this function shows the file input window again.
        """
        self.window5.hide()
        self.window1.show()


########################## ENTER MESSAGE ############################

    def on_message_typed_in_clicked(self, widget):
        """When next button in "enter message" window gets clicked,
        this function checks if the amount of entered characters is
        not too many for the capacity of the chosen algorithm.
        If length is ok, it calls stegui_ctrl.embed to embed the message
        into the given file, writing it into the given output file.
        If not, it changes the windows info text to tell the user
        how too many characters he entered.
        """
        msg_field = self.xml.get_widget('message_field')
        end = msg_field.get_buffer().get_end_iter()
        start = msg_field.get_buffer().get_start_iter()
        self.message = msg_field.get_buffer().get_text(start,end,True)
        if len(self.message) > self.cap:
            self.msg_in_label.set_text("Fehler! Sie haben "+str(len(self.message)-self.cap)+" Zeichen zu viel eingegeben!")
        else:
            if stegui_ctrl.embed_or_extract(self.algo, self.filename,
                    self.destfile, self.message,self.para, True):
                self.window2.hide()
                self.window6 = self.xml.get_widget('window6')
                self.xml.get_widget('cover_img').set_from_file(self.filename)
                self.xml.get_widget('stego_img').set_from_file(self.destfile)
                self.window6.show()
            else:
                print "error embedding message."
#TODO: do something or die...

    def on_back_to_output_file_clicked(self, widget):
        """When back button in "enter message" window gets clicked,
        this function shows the "choose output file" window again.
        """
        self.window2.hide()
        self.window5.show()

######################### EVERYTHING DONE ##########################

    def on_quit_clicked(self, widget):
        """When quit button in "embedding worked" window gets clicked,
        this function quits the whole application.
        """
        gtk.main_quit()

    def on_quit2_clicked(self, widget):
        gtk.main_quit()

if __name__ == "__main__":
    Window()
    gtk.main()
