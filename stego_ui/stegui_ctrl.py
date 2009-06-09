import libstego
import libstegofile

algos = ["Battlesteg", "CPT", "F5", "Frirui",
        "GifShuffle", "LSB", "PQ","Sort / Unsort", "SVG Steg"]

supported_filetypes = ["gif","jpg","peg","png","wav","svg"]

def get_appropiate_format(algorithm):
    """This function gets the name of an algorithm as a String
    and returns a String of it's appropriate file format.
    """
    if algorithm in ["Battlesteg","CPT","LSB"]:
        format = "PNG"
    elif algorithm in ["F5","MB2", "PQ"]:
        format = "JPG"
    elif algorithm in ["Sort / Unsort","GifShuffle",
                 "Frirui"]:
        format = "GIF"
    elif algorithm in ["Phase Coding","Echo Hiding", "PW"]:
        format = "WAV"
    elif algorithm == "SVG Steg":
        format = "SVG"
    return format

def create_battlesteg_parameter(startbit, ranged_shots,
        range, filter, password):
    """This functions gets parameters
    startbit
    ranged_shots
    range
    filter
    password
    and with them produces and returns a new battlesteg parameter
    using libstegofile's types.h.
    """
    para = libstegofile.battlesteg_parameter()
    para.startbit = startbit
    para.move_away = ranged_shots
    para.range = range
    para.password = password
    para.pwlen = len(password)
    return para

def create_sortunsort_parameter(password):
    """This functions gets parameter
    password
    and with it produces and returns a new sortunsort parameter
    using libstegofile's types.h.
    """
    para = libstegofile.sortunsort_parameter()
    para.password = password
    para.pwlen = len(password)
    return para

def create_f5_parameter(password):
    """This functions gets parameter
    password
    and with it produces and returns a new f5 parameter
    using libstegofile's types.h.
    """
    para = libstegofile.f5_parameter()
    para.password = password
    para.pwlen = len(password)
    return para

def create_gifshuffle_parameter(password, method):
    """This functions gets parameter
    password
    method
    and with them produces and returns a new gifshuffle parameter
    using libstegofile's types.h.
    """
    para = libstegofile.gifshuffle_parameter()
    para.password = password
    para.pwlen = len(password)
    para.method = method
    return para

def create_pq_parameter(password):
    """This functions gets parameter
    password
    and with it produces and returns a new pq parameter
    using libstegofile's types.h.
    """
    para = libstegofile.pq_parameter()
    para.password = password
    para.pwlen = len(password)
    return para

def create_lsb_parameter(password, method, use_msb):
    """This functions gets parameter
    method
    password
    and with them produces and returns a new lsb parameter
    using libstegofile's types.h.
    """
    para = libstegofile.lsb_parameter_t()
    para.password = password
    para.pwlen = len(password)
    para.select_mode = method
    print "use_msb: " + `use_msb`
    if use_msb:
        para.use_msb = 1;
    else:
        para.use_msb = 0;
    print "para.use_msb: " + `para.use_msb`
    return para

def create_svg_parameter(password, digit):
    """This functions gets parameter
    password
    digit
    and with them produces and returns a new svg parameter
    using libstegofile's types.h.
    """
    para = libstegofile.svg_parameter_t()
    para.password = password
    para.pwlen = len(password)
    para.first_embed_digit = digit
    return para

def create_frirui_parameter(password, method, size, threshold):
    """This functions gets parameter
    password
    method
    size
    threshold
    and with them produces and returns a new frirui parameter
    using libstegofile's types.h.
    """
    para = libstegofile.frirui_parameter()
    para.password = password
    para.pwlen = len(password)
    para.method = method
    para.size = size
    para.threshold = threshold
    return para

def create_cpt_parameter(password, size_x, size_y):
    """This functions gets parameter
    password
    size_x
    size_y
    and with them produces and returns a new cpt parameter
    using libstegofile's types.h.
    """
    para = libstegofile.cpt_parameter()
    para.block_width = size_x
    para.block_height = size_y
    para.passwd = password
    para.pwlen = len(password)
    return para

# TODO io_cleanup when cap is done.
def get_capacity(algorithm,filename,para):
    """This function gets parameters
    algorithm ... the String naming the chosen algorithm
    filename ... the chosen input filename
    para ... a parameter for the chosen algorithm
    and computes the capacity of the algorithm for the
    given file and parameters.
    It returns the capacity.
    """
    if filename[-3:] == "gif":
        palette_data = libstegofile.palette_data_t()
        gif_struct = libstegofile.gif_internal_data_t()
        bla = libstegofile.io_gif_read(filename,palette_data, gif_struct)
        if algorithm == "Frirui":
            cap = libstego.frirui_check_capacity(palette_data, para)
        elif algorithm == "GifShuffle":
            cap = libstego.gifshuffle_check_capacity(palette_data, para)
        elif algorithm == "Sort / Unsort":
            cap = libstego.sortunsort_check_capacity(palette_data, para)
        libstegofile.io_gif_cleanup_data(palette_data)
        #libstegofile.io_gif_cleanup_internal(gif_struct)

    elif filename[-3:] == "png":
        rgb_data = libstegofile.rgb_data_t()
        rgb_internal = libstegofile.png_internal_data_t()
        bla = libstegofile.io_png_read(filename,rgb_data,rgb_internal)
        if algorithm == "Battlesteg":
            cap = libstego.battlesteg_check_capacity(rgb_data, para)
        elif algorithm == "CPT":
            cap = libstego.cpt_check_capacity(rgb_data, para)
        elif algorithm == "LSB":
            bytes = libstego.new_charpp()
            num_bytes = libstego.new_intp()
            libstego.lsb_convert_png(rgb_data, bytes, num_bytes)
            cap = libstego.lsb_check_capacity_indirect(libstego.charpp_value(bytes), libstego.intp_value(num_bytes), para);
            libstego.lsb_cleanup_converted_data(libstego.charpp_value(bytes));
            libstego.delete_charpp(bytes);

    elif filename[-3:] == "wav":
        wav_data = wav_data_t()
        wav_int = wav_internal_data_t()
        bla = libstegofile.io_wav_read(filename, wav_data, wav_int)
        if algorithm == "Echo Hiding":
            cap = libstego.echohiding_check_capacity(wav_data, para)
        elif algorithm == "Phase Coding":
            cap = libstego.phasecoding_check_capacity(wav_data, para)
        elif algorithm == "PW":
            cap = libstego.pw_check_capacity(wav_data, para)

    elif filename[-3:] == "svg":
        svg_data = libstegofile.svg_data_t()
        svg_int = libstegofile.svg_internal_data_t()
        bla = libstegofile.io_svg_read(filename, svg_data, svg_int)
        if algorithm == "SVG Steg":
            cap = libstego.svg_check_capacity(svg_data, para)

    elif filename [-3:] in ["peg", "jpg"]:
        jpeg_data = libstegofile.jpeg_data_t()
        jpeg_int = libstegofile.jpeg_internal_data_t()
        bla = libstegofile.io_jpeg_read(filename,jpeg_data, jpeg_int)
        if algorithm == "F5":
            cap = libstego.f5_check_capacity(jpeg_data, para)
        elif algorithm == "MB2":
            cap = libstego.mb2_check_capacity(jpeg_data, para)
        elif algorithm == "PQ":
            cap = libstego.pq_check_capacity(jpeg_data, para)

    elif algorithm == "LSB":
        cap = libstego.lsb_check_capacity(palette_data, para)
    print cap
    if cap[0] == 0:
        return cap[1]
    else:
        print "Error computing capacity!"
        return 666
        #TODO: change

def embed_or_extract(algorithm, filename, destfilename,
        message, para, embed):
    """This function gets as parameters
    filename ... the input file name
    destfilename ... the output filen ame
    message ... the message
    para ... the parameter for the chosen algorithm
    embed ... boolean: if True, we embed, else we extract.
    If embed is True it uses libstego to embed the given message
    in the given output file, calling the <algorithm>_embed()
    function in libstego.
    If embed is False, it uses libstego to extract a message
    from the given file, calling the <algorithm>_extract()
    function in libstego.
    This function can therefor be called recursively to check
    if embedding a message worked by calling it with embed=False.
    """
    ending = filename[-3:]
    print ending
    if embed:
        msglen = len(message)+1
    else:
        message = libstego.new_charp()
        msglen = libstego.new_intp()
    if ending == "gif":
        src_data = libstegofile.palette_data_t()
        stego_data = libstegofile.palette_data_t()
        gif_int = libstegofile.gif_internal_data_t()
        libstegofile.io_gif_read(filename, src_data, gif_int)
        if algorithm == "Sort / Unsort":
            if embed:
                libstego.sortunsort_embed(src_data, stego_data, message,
                        msglen, para)
            else:
                libstego.sortunsort_extract(src_data, message,
                        msglen, para)
        elif algorithm == "GifShuffle":
            if embed:
                if para.method == 1:
                    pw_message = para.password
                    pw_message_len = para.pwlen
                    para.password = ""
                    para.pwlen = 0
                    pwmsg_data = libstegofile.palette_data_t()
                    para.method = 0
                    libstego.gifshuffle_embed(src_data, pwmsg_data, pw_message,
                            pw_message_len, para)
                    para.method = 1
                    libstego.gifshuffle_embed(pwmsg_data, stego_data,
                            message, msglen, para)
                else:
                    print "normal gifshuffle:"
                    print "pw: "+para.password
                    print "pwlen: "+str(para.pwlen)
                    libstego.gifshuffle_embed(src_data, stego_data, message,
                            msglen, para)
            else:
                if para.method == 1:
                    pw_message = para.password
                    pw_message_len = para.pwlen
                    para.password = ""
                    para.pwlen = 0
                    pwmsg_data = libstegofile.palette_data_t()
                    para.method = 0
                    libstego.gifshuffle_embed(src_data, pwmsg_data, pw_message,
                            pw_message_len, para)
                    para.adv_pal = pwmsg_data
                    para.method = 1
                    libstego.gifshuffle_extract(src_data, message,
                            msglen, para)
                else:
                    libstego.gifshuffle_extract(src_data, message,
                            msglen, para)

        elif algorithm == "Frirui":
            if embed:
                libstego.frirui_embed(src_data, stego_data, message,
                        msglen, para)
            else:
                libstego.frirui_extract(src_data, message,
                        msglen, para)
        if embed:
            print "integrate and write ..."
            libstegofile.io_gif_integrate(gif_int, stego_data)
            libstegofile.io_gif_write(destfilename, gif_int)
        #libstegofile.io_gif_cleanup_data(stego_data)
        #libstegofile.io_gif_cleanup_data(src_data)
        #libstegofile.io_gif_cleanup_internal(gif_int)
    elif ending == "png":
        src_data = libstegofile.rgb_data_t()
        stego_data = libstegofile.rgb_data_t()
        png_int = libstegofile.png_internal_data_t()
        libstegofile.io_png_read(filename, src_data, png_int)
        if algorithm == "Battlesteg":
            if embed:
                libstego.battlesteg_embed(src_data, stego_data, message,
                        msglen, para)
            else:
                libstego.battlesteg_extract(src_data, message,
                        msglen, para)

        elif algorithm == "CPT":
            if embed:
                libstego.cpt_embed(src_data, stego_data, message,
                        msglen, para)
            else:
                libstego.cpt_extract(src_data, message,
                        msglen, para)

        elif algorithm == "LSB":
            bytes = libstego.new_charpp()
            num_bytes = libstego.new_intp()
            stego_data = src_data
            libstego.lsb_convert_png(stego_data, bytes, num_bytes)
            if embed:
                ret = libstego.lsb_embed_indirect(libstego.charpp_value(bytes),
                        libstego.intp_value(num_bytes), message, msglen, para)
                if ret != 0:
                    print "An error occured. Error code: " + `libstego.cvar.lstg_errno`
                    return False
            else:
                ret = libstego.lsb_extract_indirect(libstego.charpp_value(bytes),
                        libstego.intp_value(num_bytes), message, msglen, para)
                if ret != 0:
                    print "An error occured. Error code: " + `libstego.cvar.lstg_errno`
                    return False

            libstego.lsb_cleanup_converted_data(libstego.charpp_value(bytes));
            libstego.delete_charpp(bytes);

        if embed:
            libstegofile.io_png_integrate(png_int, stego_data)
            libstegofile.io_png_write(destfilename, png_int)
            libstegofile.io_png_cleanup_data(stego_data)
        # png cleanup ruft segmentation faults hervor...
        #libstegofile.io_png_cleanup_data(src_data)
        #libstegofile.io_png_cleanup_internal(png_int)

    elif ending == "svg":
        src_data = libstegofile.svg_data_t()
        stego_data = libstegofile.svg_data_t()
        svg_int = libstegofile.svg_internal_data_t()
        libstegofile.io_svg_read(filename, src_data, svg_int)
        if algorithm == "SVG Steg":
            if embed:
                ret = libstego.svg_embed(src_data, stego_data, message,
                        msglen, para)

                if ret != 0:
                    print "An error occured. Error code: " + `libstego.cvar.lstg_errno`
                    return False

            else:
                ret = libstego.svg_extract(src_data, message, msglen, para)

                if ret != 0:
                    print "An error occured. Error code: " + `libstego.cvar.lstg_errno`
                    return False

        if embed:
            libstegofile.io_svg_integrate(svg_int, stego_data)
            libstegofile.io_svg_write(destfilename, svg_int)
            libstegofile.io_svg_cleanup_data(stego_data)

        libstegofile.io_svg_cleanup_data(src_data)
        libstegofile.io_svg_cleanup_internal(svg_int)

    elif ending == "wav":
        src_data = libstegofile.wav_data_t()
        stego_data = libstegofile.wav_data_t()
        wav_int = libstegofile.wav_internal_data_t()
        libstegofile.io_wav_read(filename, src_data, svg_int)
        if algorithm == "Echo Hiding":
            if embed:
                libstego.echohiding_embed(src_data, stego_data, message,
                        msglen, para)
            else:
                libstego.echohiding_extract(src_data, message,
                        msglen, para)
        elif algorithm == "Phase Coding":
            if embed:
                libstego.phasecoding_embed(src_data, stego_data, message,
                        msglen, para)
            else:
                libstego.phasecoding_extract(src_data, message,
                        msglen, para)
        libstegofile.io_wav_integrate(wav_int, stego_data)
        libstegofile.io_wav_write(destfilename, wav_int)
        libstegofile.io_wav_cleanup_data(stego_data)
        libstegofile.io_wav_cleanup_data(src_data)
        libstegofile.io_wav_cleanup_internal(wav_int)
    elif ending == "jpg" or "peg":
        src_data = libstegofile.jpeg_data_t()
        stego_data = libstegofile.jpeg_data_t()
        jpg_int = libstegofile.jpeg_internal_data_t()
        libstegofile.io_jpeg_read(filename, src_data, jpg_int)
        if algorithm == "F5":
            if embed:
                libstego.f5_embed(src_data, stego_data, message,
                        msglen, para)
            else:
                libstego.f5_extract(src_data, message,
                        msglen, para)
        elif algorithm == "MB2":
            if embed:
                libstego.mb2_embed(src_data, stego_data, message,
                        msglen, para)
            else:
                libstego.mb2_extract(src_data, message,
                        msglen, para)
        libstegofile.io_jpeg_integrate(jpg_int, stego_data)
        libstegofile.io_jpeg_write(destfilename, jpg_int)
        #libstegofile.io_jpeg_cleanup_data(stego_data)
        libstegofile.io_jpeg_cleanup_data(src_data)
        libstegofile.io_jpeg_cleanup_internal(jpg_int)
# TODO: extract for testing purpose!
    if embed:
        return True
    else:
        new_message = str(libstego.charp_value(message))
        length = libstego.intp_value(msglen)
        print new_message
        return new_message
