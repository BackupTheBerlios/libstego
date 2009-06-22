#!/usr/bin/env python
# coding=utf-8

import sys
import os
sys.path.append("../swig_bindings")
import lime_control
try:
    import pygtk
    pygtk.require("2.0")
except:
    pass
try:
    import gtk
    import gtk.glade
    import gobject
except:
    sys.exit(1)

class MainWnd:

    def __init__(self):
        #Set the Glade file
        self.gladefile = "limejuice.glade"
        self.wTree = gtk.glade.XML(self.gladefile)
        self.window = self.wTree.get_widget('wnd_lemonjuice')
        self.body = self.wTree.get_widget('body')
        self.btn_next = self.wTree.get_widget('btn_next')
        self.btn_prev = self.wTree.get_widget('btn_prev')
        self.img_cover = self.wTree.get_widget('img_cover')
        self.message_buf = self.wTree.get_widget('txt_message').get_buffer()
        self.info_buf = self.wTree.get_widget('txt_info').get_buffer()
        self.lbl_stego_file = self.wTree.get_widget('lbl_stego_file')
        self.lbl_method = self.wTree.get_widget('lbl_method_used')
        self.file_cover = self.wTree.get_widget('file_cover')
        self.reset_dlg = self.wTree.get_widget('msgdlg_reset')
        self.pass_dlg = self.wTree.get_widget('dlg_password')
        self.method_dlg = self.wTree.get_widget('dlg_method')
        self.txt_stego = self.wTree.get_widget('txt_stego')
        self.file_cover = self.wTree.get_widget('file_cover')
        self.txt_pass1 = self.wTree.get_widget('txt_pass')
        self.txt_pass2 = self.wTree.get_widget('txt_pass_wdh')
        self.txt_pass_m1 = self.wTree.get_widget('txt_pass1')
        self.txt_pass_m2 = self.wTree.get_widget('txt_pass_wdh1')
        self.cmb_method = self.wTree.get_widget('cmb_method')

        dic = {"gtk_main_quit" : gtk.main_quit}
        for key in dir(self.__class__):
            dic[key] = getattr(self,key)
        self.wTree.signal_autoconnect(dic)

        self.states = ["cover", "message", "finish"]


        self.extensions = { "png":"PNG Grafik",
                            "gif":"GIF Grafik",
                            "svg":"SVG Grafik",
                            "wav":"WAVE Audiodatei" } # JPEG ist extra

        for ext in self.extensions:
            filter = gtk.FileFilter()
            filter.set_name(self.extensions[ext])
            filter.add_pattern("*." + ext)
            self.file_cover.add_filter(filter)

        # JPEG muss gesondert behandelt werden, da es zwei extensions gibt (*.jpg und *.jpeg)
        filter = gtk.FileFilter()
        filter.set_name("JPEG Grafik")
        filter.add_pattern("*.jpeg")
        filter.add_pattern("*.jpg")
        self.file_cover.add_filter(filter)

        # auswahl für alle unterstützte dateien
        filter = gtk.FileFilter()
        filter.set_name("Alle unterstützten Dateien")
        for ext in self.extensions:
            filter.add_pattern("*." + ext)
        filter.add_pattern("*.jpg")
        filter.add_pattern("*.jpeg")
        self.file_cover.add_filter(filter)
        self.file_cover.set_filter(filter)

        self.error_file = gtk.MessageDialog(self.window,
                        gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
                        gtk.MESSAGE_ERROR,
                        gtk.BUTTONS_OK,
                        "Sie müssen eine Datei wählen um fortfahren zu können.")
        self.error_file.set_title("Fehler")

        self.error_pass = gtk.MessageDialog(self.window,
                        gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
                        gtk.MESSAGE_ERROR,
                        gtk.BUTTONS_OK,
                        "Die eingegebenen Passwörter stimmen nicht überein!")
        self.error_pass.set_title("Fehler")


        self.cmb_method.set_model(gtk.ListStore(gobject.TYPE_STRING))
        cell = gtk.CellRendererText()
        self.cmb_method.pack_start(cell, True)
        self.cmb_method.add_attribute(cell, 'text', 0)

        self.reset()
        self.set_state(0)

        self.window.show()

    def reset(self):
        self.action = ""
        self.state = 0
        self.message = ""
        self.msglen = 0
        self.algo = 0
        self.cover = ""
        self.cover_set = False
        self.stego = ""
        self.stego_filename = ""
        self.stego_path = ""
        self.cover_pixbuf = None
        self.extension = ""

    def set_state(self, state):
        if self.states[state] == "cover":
            self.btn_prev.set_label("Ausbetten")
            self.btn_next.set_label("Einbetten")

            self.file_cover.set_filename(self.cover)
            self.img_cover.set_from_pixbuf(self.cover_pixbuf)
            self.info_buf.set_text("Bitte wählen Sie die Datei aus, in die eingebettet werden soll, bzw. die die Nachricht enthält.")
            self.body.set_current_page(1)

        elif self.states[state] == "message":
            self.message_buf.set_text(self.message)

            if self.action == "embed":
                self.btn_prev.set_label("Zurück")
                self.btn_next.set_label("Fertig stellen")
                self.info_buf.set_text("Bitte geben Sie die Nachricht ein, die eingebettet werden soll.")
            elif self.action == "extract":
                self.btn_prev.set_label("Neu beginnen")
                self.btn_next.set_label("Schliessen")
                self.info_buf.set_text("Dies ist die eingebettete Nachricht.")

            self.body.set_current_page(0);

        elif self.states[state] == "finish":
            self.btn_prev.set_label("Neu beginnen")
            self.btn_next.set_label("Schließen")

            self.info_buf.set_text("Bitte merken Sie sich die verwendete Methode. Sie muss beim Ausbetten der Nachricht bekannt sein.")

            self.lbl_method.set_text("Methode " + `self.method + 1`)
            self.lbl_stego_file.set_text(self.stego)

            self.body.set_current_page(2)

        else:
            print "Invalid state " + `state`
            return

        self.state = state

    def on_btn_next_clicked(self, button):
        if self.states[self.state] == "finish":
            gtk.main_quit()

        elif self.states[self.state] == "message":
            if self.action == "embed":
                # prepare stego file path
                self.stego_path = os.path.join(os.path.dirname(self.cover), "stego")
                self.stego_filename = os.path.basename(self.cover)
                # check if the "stego" subdir already exists, and create it if not
                if not os.path.exists(self.stego_path):
                    os.mkdir(self.stego_path)
                self.stego = os.path.join(self.stego_path, self.stego_filename)

                self.message = self.message_buf.get_text(
                        self.message_buf.get_start_iter(),
                        self.message_buf.get_end_iter())
                self.msglen = len(self.message)+1
                self.txt_pass1.set_text("")
                self.txt_pass2.set_text("")
                result = self.pass_dlg.run()
                self.pass_dlg.hide()
                if result == 1:
                    self.password = self.txt_pass1.get_text()
                    if self.password != self.txt_pass2.get_text():
                        self.error_pass.run()
                        self.error_pass.hide()
                    else:
                        lime_control.setup_methods(self.password, self.extension)
                        self.method = lime_control.get_best_method(self.cover,
                                self.msglen)
                        if self.method >= 0:
                            lime_control.embed_or_extract_with_method(True,
                                    self.method, self.cover, self.stego,
                                    self.message, self.msglen)
                            self.set_state(self.state + 1)
                        else:
                            print "Kann die Nachricht nicht einbetten."
            elif self.action == "extract":
                gtk.main_quit()

        elif self.states[self.state] == "cover":
            if self.cover_set:
                self.action = "embed"
                self.set_state(self.state + 1)
            else:
                self.error_file.run()
                self.error_file.hide()

    def on_btn_prev_clicked(self, button):
        if self.states[self.state] == "finish":
            result = self.reset_dlg.run()
            self.reset_dlg.hide()
            if result == gtk.RESPONSE_YES:
                self.reset()
                self.set_state(self.state)

        elif self.states[self.state] == "cover":
            if self.cover_set:
                self.action = "extract"
                num_methods = lime_control.get_num_methods(self.extension)
                self.cmb_method.get_model().clear()
                for i in range(0, num_methods):
                    self.cmb_method.append_text("Methode " + `i+1`)
                self.cmb_method.set_active(0)

                self.txt_pass_m1.set_text("")
                self.txt_pass_m2.set_text("")
                result = self.method_dlg.run()
                self.method_dlg.hide()
                if result == 1:
                    self.password = self.txt_pass_m1.get_text()
                    self.method = self.cmb_method.get_active()
                    if self.password != self.txt_pass_m2.get_text():
                        self.error_pass.run()
                        self.error_pass.hide()
                    else:
                        if self.method >= 0:
                            lime_control.setup_methods(self.password, self.extension)
                            self.message = lime_control.embed_or_extract_with_method(
                                    False,
                                    self.method, self.cover, None, None, None)
                            self.set_state(self.state + 1)
                        else:
                            print "Kann die Nachricht nicht extrahieren."
                    self.set_state(1)
            else:
                self.error_file.run()
                self.error_file.hide()

        elif self.states[self.state] == "message":
            if self.action == "embed":
                self.set_state(self.state - 1)

            elif self.action == "extract":
                result = self.reset_dlg.run()
                self.reset_dlg.hide()
                if result == gtk.RESPONSE_YES:
                    self.reset()
                    self.set_state(self.state)

    def on_file_cover_file_set(self, widget):
        coverfile = self.file_cover.get_filename()
        if coverfile != None:
            self.cover = coverfile
            self.extension = self.cover.rsplit('.', 1)[1]
            self.extension = self.extension.upper()
            if self.extension != "WAV":
                self.cover_pixbuf = gtk.gdk.pixbuf_new_from_file(self.cover)
                if self.cover_pixbuf.get_width() > 400 or self.cover_pixbuf.get_height() > 250:
                    self.cover_pixbuf = self.cover_pixbuf.scale_simple(400, 250, gtk.gdk.INTERP_BILINEAR)
                self.img_cover.set_from_pixbuf(self.cover_pixbuf)
            else:
                self.img_cover.set_from_stock(gtk.STOCK_CDROM, gtk.ICON_SIZE_DIALOG)

            self.cover_set = True

    def on_wnd_lemonjuice_destroy(self, widget):
        gtk.main_quit()

if __name__ == "__main__":
    hwg = MainWnd()
    gtk.main()
