#!/usr/bin/env python
# coding=utf-8

import sys
import os
try:
	import pygtk
	pygtk.require("2.0")
except:
	pass
try:
	import gtk
	import gtk.glade
except:
	sys.exit(1)

class MainWnd:

    def __init__(self):
        #Set the Glade file
        self.gladefile = "lemonjuice.glade"
        self.wTree = gtk.glade.XML(self.gladefile)
        self.window = self.wTree.get_widget('wnd_lemonjuice')
        self.body = self.wTree.get_widget('body')
        self.btn_next = self.wTree.get_widget('btn_next')
        self.btn_prev = self.wTree.get_widget('btn_prev')
        self.lbl_info = self.wTree.get_widget('lbl_info')
        self.cmb_algo = self.wTree.get_widget('cmb_algo')
        self.img_prog_1 = self.wTree.get_widget('img_prog_1')
        self.img_prog_2 = self.wTree.get_widget('img_prog_2')
        self.img_prog_3 = self.wTree.get_widget('img_prog_3')
        self.img_prog_4 = self.wTree.get_widget('img_prog_4')
        self.img_cover = self.wTree.get_widget('img_cover')
        self.img_cover_overview = self.wTree.get_widget('img_cover_overview')
        self.algos = self.cmb_algo.get_model()
        self.message_buf = self.wTree.get_widget('txt_message').get_buffer()
        self.lbl_algo_info = self.wTree.get_widget('lbl_algo_info')
        self.lbl_chosen_algo = self.wTree.get_widget('lbl_chosen_algo')
        self.lbl_stego_name = self.wTree.get_widget('lbl_stego_name')
        self.lbl_cover_name = self.wTree.get_widget('lbl_cover_name')
        self.file_cover = self.wTree.get_widget('file_cover')
        self.txt_pass = self.wTree.get_widget('txt_pass')

        dic = {"gtk_main_quit" : gtk.main_quit}
        for key in dir(self.__class__):
            dic[key] = getattr(self,key)
        self.wTree.signal_autoconnect(dic)

        self.states = ["message", "cover", "algo", "finish"]

        self.algo_info = [
                            "BattleSteg",
                            "CPT",
                            "FriRui",
                            "GifShuffle",
                            "Der LSB-Algorithmus nutzt die niederwertigsten Bits (Least Significant Bits) des Covers um eine Nachricht einzubetten. Eine zufällige Verteilung der Nachrichtenbits ist möglich.",
                            "PQ",
                            "Sort/Unsort",
                            "SVGStego bettet ähnlich dem LSB-Algorithmus die Nachricht in den Nachkommastellen der Komponenten von Transformationsmatrizen in Scalable Vector Graphics (SVG) ein."
                        ]

        self.reset()
        self.set_state(0)

        self.window.show()

    def reset(self):
        self.state = 0
        self.message = ""
        self.algo = 0
        self.cover = "#f"
        self.stego = ""
        self.cover_pixbuf = None
        self.password = ""

    def set_state(self, state):
        self.btn_next.set_label("gtk-go-forward")
        if self.states[state] == "message":
            self.btn_prev.set_sensitive(False)
            self.btn_next.set_sensitive(True)
            self.img_prog_1.set_from_file("progress-current-1.png");
            self.img_prog_2.set_from_file("progress-grey-2.png");
            self.img_prog_3.set_from_file("progress-grey-3.png");
            self.img_prog_4.set_from_file("progress-grey-4.png");

            self.lbl_info.set_text("Bitte geben Sie die einzubettende Nachricht ein.")
            self.message_buf.set_text(self.message)
            self.body.set_current_page(0)

        elif self.states[state] == "cover":
            self.btn_prev.set_sensitive(True)
            self.btn_next.set_sensitive(True)
            self.img_prog_1.set_from_file("progress-green-1.png");
            self.img_prog_2.set_from_file("progress-current-2.png");
            self.img_prog_3.set_from_file("progress-grey-3.png");
            self.img_prog_4.set_from_file("progress-grey-4.png");

            self.file_cover.set_filename(self.cover)
            self.img_cover.set_from_pixbuf(self.cover_pixbuf)
            self.lbl_info.set_text("Bitte wählen Sie die Datei aus, in die eingebettet werden soll.")
            self.body.set_current_page(1)

        elif self.states[state] == "algo":
            self.btn_prev.set_sensitive(True)
            self.btn_next.set_sensitive(True)
            self.img_prog_1.set_from_file("progress-green-1.png");
            self.img_prog_2.set_from_file("progress-green-2.png");
            self.img_prog_3.set_from_file("progress-current-3.png");
            self.img_prog_4.set_from_file("progress-grey-4.png");

            self.lbl_info.set_text("Bitte wählen Sie den Algorithmus aus, mit dem eingebettet werden soll.")
            self.cmb_algo.set_active(self.algo)
            self.body.set_current_page(2)

        elif self.states[state] == "finish":
            self.btn_prev.set_sensitive(True)
            self.btn_next.set_label("gtk-revert-to-saved")
            self.img_prog_1.set_from_file("progress-green-1.png");
            self.img_prog_2.set_from_file("progress-green-2.png");
            self.img_prog_3.set_from_file("progress-green-3.png");
            self.img_prog_4.set_from_file("progress-current-4.png");

            self.lbl_info.set_text("Bitte vergewissern Sie sich dass alle Einstellungen korrekt sind, und klicken Sie auf \"Einbetten\" um den Vorgang zu starten.")
            if self.cover_pixbuf == None:
                self.img_cover_overview.set_from_pixbuf(None)
            else:
                self.img_cover_overview.set_from_pixbuf(self.cover_pixbuf.scale_simple(150, 90, gtk.gdk.INTERP_BILINEAR))
            self.lbl_chosen_algo.set_text(self.algos[self.algo][0])
            self.lbl_cover_name.set_text(self.cover)
            self.lbl_stego_name.set_text(self.stego)
            self.body.set_current_page(3)

        else:
            print "Invalid state " + `state`
            return

        self.state = state


    def on_btn_next_clicked(self, button):
        if self.states[self.state] == "finish":
            self.reset()
            self.set_state(self.state)
        else:
            self.set_state(self.state + 1)

    def on_btn_prev_clicked(self, button):
        self.set_state(self.state - 1)

    def on_cmb_algo_changed(self, widget):
        self.algo = self.cmb_algo.get_active()
        self.lbl_algo_info.set_text(self.algo_info[self.algo])

    def on_file_cover_file_set(self, widget):
        coverfile = self.file_cover.get_filename()
        if coverfile != None:
            self.cover = coverfile
            self.cover_pixbuf = gtk.gdk.pixbuf_new_from_file(self.cover)
            self.img_cover.set_from_pixbuf(self.cover_pixbuf)

    def on_evt_prog1_button_release_event(self, widget, event):
        self.set_state(0)
        return True

    def on_evt_prog2_button_release_event(self, widget, event):
        self.set_state(1)
        return True

    def on_evt_prog3_button_release_event(self, widget, event):
        self.set_state(2)
        return True

    def on_evt_prog4_button_release_event(self, widget, event):
        self.set_state(3)
        return True

    def on_txt_pass_changed(self, widget):
        self.password = self.txt_pass.get_text()

    def on_wnd_lemonjuice_destroy(self, widget):
        gtk.main_quit()

if __name__ == "__main__":
	hwg = MainWnd()
	gtk.main()
