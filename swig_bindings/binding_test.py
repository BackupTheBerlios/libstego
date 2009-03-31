import libstego
import libstegofile

palette_data = libstegofile.palette_data_t()
stego_data = libstegofile.palette_data_t()
gif_struct = libstegofile.gif_internal_data_t()
gif_struct.SrcFilename = ""
bla = libstegofile.io_gif_read("test.gif",palette_data, gif_struct)
# TODO: uint8_t is not a good return type for functions!
# Python with SWIG interpretes them as char so "return 0" is weird for it.
if bla == 0:
    print "Gif file read successful."
else:
    print "Gif file could not be read."
para = libstegofile.sortunsort_parameter()
message = "hi"
para.password = "test"
sort_unsort_capacity = 0
cap = libstego.sortunsort_check_capacity(palette_data,para)[0]
print "sort unsort capacity: "+str(cap)
if cap == 28:
    print "Capacity is correct."
else:
    print "Capacity is wrong!"
libstego.sortunsort_embed(palette_data, stego_data, message, len(message), para)
#libstegofile.io_gif_cleanup_data(palette_data)
print "Test done."
