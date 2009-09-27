import libstego
import libstegofile

algos = ["Battlesteg", "CPT", "F5", "Frirui",
        "Adv. Gifshuffle", "Gifshuffle", "LSB",
        "PQ","Sort/Unsort", "SVG Steg",
        "Echo Hiding", "Phase Coding"]

supported_filetypes = ["gif","jpg","jpeg","png","wav","svg"]


method_to_algo = {}
methods = {}
num_methods = 0

def setup_methods(password, filetype):
    # Add all parameter sets for your algorithms here
    # Order them from the most secure to the least secure

    global num_methods
    global method_to_algo
    global methods

    num_methods = 0

    """JPEG params"""
    if filetype in ["JPEG", "JPG"]:
        # PQ
        #~ para = libstegofile.pq_parameter()
        #~ para.header_size = 0
        #~ para.quality = 0
        #~ para.password = password
        #~ para.pwlen = len(password)
        #~ methods[num_methods] = para
        #~ method_to_algo[num_methods] = "PQ"
        #~ num_methods += 1

        # F5
        para = libstegofile.f5_parameter()
        methods[num_methods] = para
        method_to_algo[num_methods] = "F5"
        num_methods += 1

        """GIF params"""
    elif filetype == "GIF":
        # Adv. Gifshuffle
        para = libstegofile.gifshuffle_parameter()
        para.method = 1
        para.password = password
        para.pwlen = len(password)
        methods[num_methods] = para
        method_to_algo[num_methods] = "Adv. Gifshuffle"
        num_methods += 1

        # Gifshuffle
        # TODO add Gifshuffle parameter set(s) here

        # Frirui
        para = libstegofile.frirui_parameter()
        para.method = 0
        para.size = 100
        para.password = password
        para.pwlen = len(password)
        methods[num_methods] = para
        method_to_algo[num_methods] = "Frirui"
        num_methods += 1

        # Sort/Unsort
        para = libstegofile.sortunsort_parameter()
        para.password = password
        para.pwlen = len(password)
        methods[num_methods] = para
        method_to_algo[num_methods] = "Sort/Unsort"
        num_methods += 1


        """PNG params"""
    elif filetype == "PNG":
        # CPT
        for i in [8,6,4,2]:
            para = libstegofile.cpt_parameter()
            para.block_width = i;
            para.block_height = i;
            para.password = password
            para.pwlen = len(password)
            methods[num_methods] = para
            method_to_algo[num_methods] = "CPT"
            num_methods += 1

        # Battlesteg
        para = libstegofile.battlesteg_parameter()
        para.startbit = 7
        para.move_away = 3
        para.range = 5
        para.password = password
        para.pwlen = len(password)
        methods[num_methods] = para
        method_to_algo[num_methods] = "Battlesteg"
        num_methods += 1


        """SVG Params"""
    elif filetype == "SVG":
        # SVG Steg
        for i in [6,4,2,0]:
            para = libstegofile.svg_parameter_t()
            para.password = password
            para.pwlen = len(password)
            para.first_embed_digit = i
            methods[num_methods] = para
            method_to_algo[num_methods] = "SVG Steg"
            num_methods += 1


        """WAV Params"""
    elif filetype == "WAV":
        # Echo Hiding
        para = libstegofile.eh_parameter()
        para.amplitude = 0.4;
        para.zero_offset = 900;
        para.one_offset = 2700;
        para.block_size = 80000;
        methods[num_methods] = para
        method_to_algo[num_methods] = "Echo Hiding"
        num_methods += 1

        # Phase Coding
        # TODO: add Phase Coding parameter set(s) here


    """LSB"""
    # LSB isn't dependant on a certain filetype
    para = libstegofile.lsb_parameter_t()
    para.password = password
    para.pwlen = len(password)
    para.select_mode = 2
    para.use_msb = 0
    methods[num_methods] = para
    method_to_algo[num_methods] = "LSB"
    num_methods += 1

def get_num_methods(filetype):
    global num_methods
    # has to be called to make sure the parameter sets are built
    setup_methods("---dummy---", filetype)
    return num_methods

def get_best_method(filename, msglen):
    global num_methods
    global method_to_algo
    global methods

    for method in range(0, num_methods):
        fits = False
        para = methods[method]
        print "Testing method " + `method+1`
        print "Algorithm: " + method_to_algo[method]

        if method_to_algo[method] == "PQ":
            # prepare data strucures
            jpeg_data = libstegofile.jpeg_data_t()
            jpeg_int = libstegofile.jpeg_internal_data_t()

            # read file data
            err = libstegofile.io_jpeg_read(filename, jpeg_data, jpeg_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.pq_check_capacity(jpeg_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_jpeg_cleanup_data(jpeg_data)
            libstegofile.io_jpeg_cleanup_internal_data(jpeg_int)


        elif method_to_algo[method] == "F5":
            # prepare data strucures
            jpeg_data = libstegofile.jpeg_data_t()
            jpeg_int = libstegofile.jpeg_internal_data_t()

            # read file data
            err = libstegofile.io_jpeg_read(filename, jpeg_data, jpeg_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.f5_check_capacity(jpeg_data) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_jpeg_cleanup_data(jpeg_data)
            libstegofile.io_jpeg_cleanup_internal_data(jpeg_int)


        elif method_to_algo[method] in ["Adv. Gifshuffle", "Gifshuffle"]:
            # prepare data strucures
            palette_data = libstegofile.palette_data_t()
            gif_int = libstegofile.gif_internal_data_t()

            # read file data
            err = libstegofile.io_gif_read(filename, palette_data, gif_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.gifshuffle_check_capacity(palette_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_gif_cleanup_data(palette_data)
            libstegofile.io_gif_cleanup_internal(gif_int)


        elif method_to_algo[method] == "Frirui":
            # prepare data strucures
            palette_data = libstegofile.palette_data_t()
            gif_int = libstegofile.gif_internal_data_t()

            # read file data
            err = libstegofile.io_gif_read(filename, palette_data, gif_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.frirui_check_capacity(palette_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_gif_cleanup_data(palette_data)
            libstegofile.io_gif_cleanup_internal(gif_int)


        elif method_to_algo[method] == "Sort/Unsort":
            # prepare data strucures
            palette_data = libstegofile.palette_data_t()
            gif_int = libstegofile.gif_internal_data_t()

            # read file data
            err = libstegofile.io_gif_read(filename, palette_data, gif_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.sortunsort_check_capacity(palette_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_gif_cleanup_data(palette_data)
            libstegofile.io_gif_cleanup_internal(gif_int)

        elif method_to_algo[method] == "CPT":
            # prepare data strucures
            rgb_data = libstegofile.rgb_data_t()
            png_int = libstegofile.png_internal_data_t()

            # read file data
            err = libstegofile.io_png_read(filename, rgb_data, png_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.cpt_check_capacity(rgb_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_png_cleanup_data(rgb_data)
            libstegofile.io_png_cleanup_internal(png_int)


        elif method_to_algo[method] == "Battlesteg":
            # prepare data strucures
            rgb_data = libstegofile.rgb_data_t()
            png_int = libstegofile.png_internal_data_t()

            # read file data
            err = libstegofile.io_png_read(filename, rgb_data, png_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.battlesteg_check_capacity(rgb_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_png_cleanup_data(rgb_data)
            libstegofile.io_png_cleanup_internal(png_int)


        elif method_to_algo[method] == "SVG Steg":
            # prepare data strucures
            svg_data = libstegofile.svg_data_t()
            svg_int = libstegofile.svg_internal_data_t()

            # read file data
            err = libstegofile.io_svg_read(filename, svg_data, svg_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.svg_check_capacity(svg_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_svg_cleanup_data(svg_data)
            libstegofile.io_svg_cleanup_internal(svg_int)

        elif method_to_algo[method] == "Echo Hiding":
            # prepare data strucures
            wav_data = libstegofile.wav_data_t()
            wav_int = libstegofile.wav_internal_data_t()

            # read file data
            err = libstegofile.io_wav_read(filename, wav_data, wav_int)
            if err != 0:
                # TODO better error handling -> detailed message what went wrong
                # by using the 'lstg_errno' code
                return None

            # check if message fits with the current params
            if libstego.eh_check_capacity(wav_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_wav_cleanup_data(wav_data)
            libstegofile.io_wav_cleanup_internal(wav_int)


        elif method_to_algo[method] == "Phase Coding":
            # TODO implement
            pass


        elif method_to_algo[method] == "LSB":
            # TODO implement
            pass

        if fits:
            print "best method: " + `method+1`
            return method

    return None


def embed_or_extract_with_method(embed, method, filename, destfilename, message, msglen):
    if embed:
        print "embedding with method " + `method+1` + ", algo: " + method_to_algo[method]
    else:
        p_message = libstego.new_charp()
        p_msglen = libstego.new_intp()
        print "extracting with method " + `method+1` + ", algo: " + method_to_algo[method]

    if method_to_algo[method] == "PQ":
        # prepare data strucures
        jpeg_data = libstegofile.jpeg_data_t()
        stego_data = libstegofile.jpeg_data_t()
        jpeg_int = libstegofile.jpeg_internal_data_t()

        # read file data
        err = libstegofile.io_jpeg_read(filename, jpeg_data, jpeg_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading JPG"
            return

        para = methods[method]
        if embed:
            err = libstego.pq_embed(jpeg_data, stego_data, message, len(message) + 1, para)
            if err == 0:
                err = libstegofile.io_jpeg_integrate(jpeg_int, stego_data)
                if err != 0:
                    print "Error while integrating JPEG"
                err = libstegofile.io_jpeg_write(destfilename, jpeg_int)
                if err != 0:
                    print "Error while writing JPEG"
            else:
                print "An error occured while embedding with PQ."
        else:
            err = libstego.pq_extract(jpeg_data, p_message, p_msglen, para)
            if err != 0:
                print "An error occured while extracting with PQ."

        # cleanup
        libstegofile.io_jpeg_cleanup_data(jpeg_data)
        libstegofile.io_jpeg_cleanup_data(stego_data)
        libstegofile.io_jpeg_cleanup_internal_data(jpeg_int)


    elif method_to_algo[method] == "F5":
        # prepare data strucures
        jpeg_data = libstegofile.jpeg_data_t()
        stego_data = libstegofile.jpeg_data_t()
        jpeg_int = libstegofile.jpeg_internal_data_t()

        # read file data
        err = libstegofile.io_jpeg_read(filename, jpeg_data, jpeg_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading JPG"
            return


        para = methods[method]
        if embed:
            err = libstego.f5_embed(jpeg_data, stego_data, message, len(message) + 1, para)
            if err == 0:
                err = libstegofile.io_jpeg_integrate(jpg_int, stego_data)
                err = libstegofile.io_jpeg_write(destfilename, jpeg_int)
            else:
                print "Error while embedding with F5."
        else:
            p_msglen = libstego.new_intp0()
            err = libstego.f5_extract(jpeg_data, p_message, p_msglen, para)
            if err != 0:
                print "Error while extracting with F5."

        # cleanup
        libstegofile.jpeg_cleanup_data(jpeg_data)
        libstegofile.io_jpeg_cleanup_data(stego_data)
        libstegofile.io_jpeg_cleanup_internal_data(jpeg_int)


    elif method_to_algo[method] == "Adv. Gifshuffle":
        # prepare data strucures
        palette_data = libstegofile.palette_data_t()
        stego_data = libstegofile.palette_data_t()
        gif_int = libstegofile.gif_internal_data_t()

        # read file data
        err = libstegofile.io_gif_read(filename, palette_data, gif_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading GIF"
            return

        para = methods[method]
        if embed:
            pw_message = para.password
            pw_message_len = para.pwlen
            para.password = ""
            para.pwlen = 0
            pwmsg_data = libstegofile.palette_data_t()
            para.method = 0
            err = libstego.gifshuffle_embed(palette_data, pwmsg_data, pw_message,
                    pw_message_len, para)
            para.method = 1
            err = libstego.gifshuffle_embed(pwmsg_data, stego_data,
                    message, msglen, para)

            err = libstegofile.io_gif_integrate(gif_int, stego_data)
            err = libstegofile.io_gif_write(destfilename, gif_int)
        else:
            pw_message = para.password
            pw_message_len = para.pwlen
            para.password = ""
            para.pwlen = 0
            pwmsg_data = libstegofile.palette_data_t()
            para.method = 0
            err = libstego.gifshuffle_embed(palette_data, pwmsg_data, pw_message,
                    pw_message_len, para)
            para.adv_pal = pwmsg_data
            para.method = 1
            err = libstego.gifshuffle_extract(palette_data, p_message, p_msglen, para)

        # cleanup
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(palette_data)
        libstegofile.io_gif_cleanup_internal(gif_int)


    elif method_to_algo[method] == "Gifshuffle":
        # prepare data strucures
        palette_data = libstegofile.palette_data_t()
        stego_data = libstegofile.palette_data_t()
        gif_int = libstegofile.gif_internal_data_t()

        # read file data
        err = libstegofile.io_gif_read(filename, palette_data, gif_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading GIF"
            return

        para = methods[method]
        if embed:
            err = libstego.gifshuffle_embed(palette_data, stego_data, message, msglen, para)
            err = libstegofile.io_gif_integrate(gif_int, stego_data)
            err = libstegofile.io_gif_write(destfilename, gif_int)
        else:
            err = libstego.gifshuffle_extract(palette_data, p_message, p_msglen, para)

        # cleanup
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(palette_data)
        libstegofile.io_gif_cleanup_internal(gif_int)


    elif method_to_algo[method] == "Frirui":
        # prepare data strucures
        palette_data = libstegofile.palette_data_t()
        stego_data = libstegofile.palette_data_t()
        gif_int = libstegofile.gif_internal_data_t()

        # read file data
        err = libstegofile.io_gif_read(filename, palette_data, gif_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading GIF"
            return

        para = methods[method]
        if embed:
            err = libstego.frirui_embed(palette_data, stego_data, message, msglen, para)

            err = libstegofile.io_gif_integrate(gif_int, stego_data)
            err = libstegofile.io_gif_write(destfilename, gif_int)
        else:
            err = libstego.frirui_extract(palette_data, p_message, p_msglen, para)

        # cleanup
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(palette_data)
        libstegofile.io_gif_cleanup_internal(gif_int)


    elif method_to_algo[method] == "Sort/Unsort":
        # prepare data strucures
        palette_data = libstegofile.palette_data_t()
        stego_data = libstegofile.palette_data_t()
        gif_int = libstegofile.gif_internal_data_t()

        # read file data
        err = libstegofile.io_gif_read(filename, palette_data, gif_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading GIF"
            return

        para = methods[method]
        if embed:
            libstego.sortunsort_embed(palette_data, stego_data, message, msglen, para)

            err = libstegofile.io_gif_integrate(gif_int, stego_data)
            err = libstegofile.io_gif_write(destfilename, gif_int)
        else:
            libstego.sortunsort_extract(palette_data, p_message, p_msglen, para)

        # cleanup
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(palette_data)
        libstegofile.io_gif_cleanup_internal(gif_int)

    elif method_to_algo[method] == "CPT":
        # prepare data strucures
        rgb_data = libstegofile.rgb_data_t()
        stego_data = libstegofile.rgb_data_t()
        png_int = libstegofile.png_internal_data_t()

        # read file data
        err = libstegofile.io_png_read(filename, rgb_data, png_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading PNG"
            return

        para = methods[method]
        if embed:
            print(para.password)
            print(para.pwlen)
            print(para.block_width)
            print(para.block_height)
            err = libstego.cpt_embed(rgb_data, stego_data, message, msglen, para)
            if err == 0:
                err = libstegofile.io_png_integrate(png_int, stego_data)
                err = libstegofile.io_png_write(destfilename, png_int)
        else:
            err = libstego.cpt_extract(rgb_data, p_message, p_msglen, para)

        # cleanup
        libstegofile.io_png_cleanup_data(rgb_data)
        libstegofile.io_png_cleanup_data(stego_data)
        libstegofile.io_png_cleanup_internal(png_int)

    elif method_to_algo[method] == "Battlesteg":
        # prepare data strucures
        rgb_data = libstegofile.rgb_data_t()
        stego_data = libstegofile.rgb_data_t()
        png_int = libstegofile.png_internal_data_t()

        # read file data
        err = libstegofile.io_png_read(filename, rgb_data, png_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading PNG"
            return

        para = methods[method]
        if embed:
            err = libstego.battlesteg_embed(rgb_data, stego_data, message, msglen, para)
            if err == 0:
                err = libstegofile.io_png_integrate(png_int, stego_data)
                err = libstegofile.io_png_write(destfilename, png_int)
        else:
            err = libstego.battlesteg_extract(rgb_data, p_message, p_msglen, para)

        # cleanup
        libstegofile.io_png_cleanup_data(rgb_data)
        libstegofile.io_png_cleanup_data(stego_data)
        libstegofile.io_png_cleanup_internal(png_int)


    elif method_to_algo[method] == "SVG Steg":
        # prepare data strucures
        svg_data = libstegofile.svg_data_t()
        svg_stego = libstegofile.svg_data_t()
        svg_int = libstegofile.svg_internal_data_t()

        # read file data
        err = libstegofile.io_svg_read(filename, svg_data, svg_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading SVG"
            return

        para = methods[method]
        if embed:
            err = libstego.svg_embed(svg_data, stego_data, message, msglen, para)
            if err == 0:
                err = libstegofile.io_svg_integrate(svg_int, stego_data)
                if err != 0:
                    print "Error while integrating SVG"
                err = libstegofile.io_svg_write(destfilename, svg_int)
                if err != 0:
                    print "Error while writing SVG"
            else:
                print "Error while embedding with SVG Stego"
        else:
            err = libstego.svg_extract(svg_data, p_message, p_msglen, para)
            if err != 0:
                print "Error while extracting with SVG Stego"

        # cleanup
        libstegofile.svg_cleanup_data(svg_data)
        libstegofile.svg_cleanup_data(stego_data)
        libstegofile.svg_cleanup_internal(svg_int)

    elif method_to_algo[method] == "Echo Hiding":
        # prepare data strucures
        wav_data = libstegofile.wav_data_t()
        stego_data = libstegofile.wav_data_t()
        wav_int = libstegofile.wav_internal_data_t()

        # read file data
        err = libstegofile.io_wav_read(filename, wav_data, wav_int)
        if err != 0:
            # TODO better error handling
            print "Error while reading WAV"
            return

        para = methods[method]
        if embed:
            err = libstego.eh_embed(wav_data, stego_data, message, msglen, para)
            if err == 0:
                err = libstegofile.io_wav_integrate(wav_int, stego_data)
                if err != 0:
                    print "Error while integrating WAV"
                err = libstegofile.io_wav_write(destfilename, wav_int)
                if err != 0:
                    print "Error while writing WAV"
            else:
                print "Error while embedding with Echo Hiding"
        else:
            err = libstego.eh_extract(wav_data, p_message, p_msglen, para)
            if err != 0:
                print "Error while extracting with Echo Hiding"

        # cleanup
        libstegofile.wav_cleanup_data(wav_data)
        libstegofile.wav_cleanup_data(stego_data)
        libstegofile.wav_cleanup_internal(wav_int)


    elif method_to_algo[method] == "Phase Coding":
        # TODO implement
        pass


    elif method_to_algo[method] == "LSB":
        # TODO implement
        pass

    if embed:
        return None
    else:
        message = str(libstego.charp_value(p_message))
        print message
        msglen = libstego.intp_value(p_msglen)
        return message
