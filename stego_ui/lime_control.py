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

def setup_methods(password):
    # Add all parameter sets for your algorithms here
    # Order them from the most secure to the least secure

    num_methods = 0

    """JPEG params"""
    # PQ
    para = libstegofile.pq_parameter()
    para.header_size = 0
    para.quality = 0
    methods[num_methods] = para
    method_to_algo[num_methods] = "PQ"
    num_methods += 1

    # F5
    para = libstegofile.f5_parameter()
    methods[num_methods] = para
    method_to_algo[num_methods] = "F5"
    num_methods += 1

    """GIF params"""
    # Adv. Gifshuffle
    para = libstegofile.gifshuffle_parameter()
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
    methods[num_methods] = para
    method_to_algo[num_methods] = "Sort/Unsort"
    num_methods += 1


    """PNG params"""
    # CPT
    for i in [2,4,6,8]:
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
    # Echo Hiding
    # TODO: add Echo Hiding parameter set(s) here

    # Phase Coding
    # TODO: add Phase Coding parameter set(s) here

    """LSB"""
    para = libstegofile.lsb_parameter_t()
    para.password = password
    para.pwlen = len(password)
    para.select_mode = 2
    para.use_msb = 0
    methods[num_methods] = para
    method_to_algo[num_methods] = "LSB"
    num_methods += 1


def get_methods_for_filetype(filetype):
    m = {}
    selected = 0


    # TODO: Add LSB wrappers to all formats, and "LSB" to all method_to_algo
    # checks. See PNG for an example. Will be done once the wrappers are added
    # to libstego
    for i in [0, num_methods-1]:
        if filetype in ["JPEG", "JPG"]:
            if  method_to_algo[i] in ["F5", "PQ"]:
                m[selected] = methods[i]
                selected += 1

        if filetype == "GIF":
            if  method_to_algo[i] in ["Adv. Gifshuffle", "Gifshuffle", "Frirui", "Sort/Unsort"]:
                m[selected] = methods[i]
                selected += 1

        if filetype == "PNG":
            if  method_to_algo[i] in ["CPT", "Battlesteg"]:
                m[selected] = methods[i]
                selected += 1

        if filetype == "SVG":
            if  method_to_algo[i] == "SVG Steg":
                m[selected] = methods[i]
                selected += 1

        if filetype == "WAV":
            if  method_to_algo[i] in ["Echo Hiding", "Phase Coding"]:
                m[selected] = methods[i]
                selected += 1

    return [selected, m]

def get_best_method(filename, filetype, msglen, password):
    # setup all methods
    setup_methods(password)

    # get methods for current file type
    num_selected, sel_method = get_methods_for_filetype(filetype)

    for i in [0, num_selected-1]:
        fits = False

        if method_to_algo[i] == "PQ":
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
            para = methods[i]
            if libstego.pq_check_capacity(jpeg_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.jpeg_cleanup_data(jpeg_data)
            libstegofile.io_jpeg_cleanup_internal(jpg_int)


        if method_to_algo[i] == "F5":
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
            para = methods[i]
            if libstego.f5_check_capacity(jpeg_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.jpeg_cleanup_data(jpeg_data)
            libstegofile.io_jpeg_cleanup_internal(jpg_int)


        if method_to_algo[i] in ["Adv. Gifshuffle", "Gifshuffle"]:
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
            para = methods[i]
            if libstego.gifshuffle_check_capacity(palette_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_gif_cleanup_data(palette_data)
            libstegofile.io_gif_cleanup_internal(gif_int)


        if method_to_algo[i] == "Frirui":
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
            para = methods[i]
            if libstego.frirui_check_capacity(palette_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_gif_cleanup_data(palette_data)
            libstegofile.io_gif_cleanup_internal(gif_int)


        if method_to_algo[i] == "Sort/Unsort":
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
            para = methods[i]
            if libstego.sortunsort_check_capacity(palette_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_gif_cleanup_data(palette_data)
            libstegofile.io_gif_cleanup_internal(gif_int)

        if method_to_algo[i] == "CPT":
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
            para = methods[i]
            if libstegofile.cpt_check_capacity(rgb_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_png_cleanup_data(rgb_data)
            libstegofile.io_png_cleanup_internal(png_int)


        if method_to_algo[i] == "Battlesteg":
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
            para = methods[i]
            if libstegofile.battlesteg_check_capacity(rgb_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.io_png_cleanup_data(rgb_data)
            libstegofile.io_png_cleanup_internal(png_int)


        if method_to_algo[i] == "SVG Steg":
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
            para = methods[i]
            if libstego.svg_check_capacity(svg_data, para) >= msglen:
                fits = True

            # cleanup
            libstegofile.svg_cleanup_data(svg_data)
            libstegofile.svg_cleanup_internal(svg_int)

        if method_to_algo[i] == "Echo Hiding":
            # TODO implement
            pass


        if method_to_algo[i] == "Phase Coding":
            # TODO implement
            pass


        if method_to_algo[i] == "LSB":
            # TODO implement
            pass

        if fits:
            return i

    return None


def embed_or_extract_with_method(embed, method, filename, destfilename, message, msglen):
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
                err = libstegofile.io_jpeg_integrate(jpg_int, stego_data)
                err = libstegofile.io_jpeg_write(destfilename, jpg_int)
            else:
                print "An error occured while embedding with PQ."
        else:
            err = libstego.pq_extract(jpeg_data, message, msglen, para)
            if err != 0:
                print "An error occured while extracting with PQ."

        # cleanup
        libstegofile.jpeg_cleanup_data(jpeg_data)
        libstegofile.io_jpeg_cleanup_data(stego_data)
        libstegofile.io_jpeg_cleanup_internal(jpg_int)


    if method_to_algo[method] == "F5":
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
                err = libstegofile.io_jpeg_write(destfilename, jpg_int)
            else:
                print "Error while embedding with F5."
        else:
            err = libstego.f5_extract(jpeg_data, message, msglen, para)
            if err != 0:
                print "Error while extracting with F5."

        # cleanup
        libstegofile.jpeg_cleanup_data(jpeg_data)
        libstegofile.io_jpeg_cleanup_data(stego_data)
        libstegofile.io_jpeg_cleanup_internal(jpg_int)


    if method_to_algo[method] == "Adv. Gifshuffle":
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
            err = libstego.gifshuffle_extract(palette_data, message, msglen, para)

        # cleanup
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(palette_data)
        libstegofile.io_gif_cleanup_internal(gif_int)


    if method_to_algo[method] == "Gifshuffle":
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
            err = libstego.gifshuffle_extract(palette_data, message, msglen, para)

        # cleanup
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(palette_data)
        libstegofile.io_gif_cleanup_internal(gif_int)


    if method_to_algo[method] == "Frirui":
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
            err = libstego.frirui_extract(palette_data, message, msglen, para)

        # cleanup
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(palette_data)
        libstegofile.io_gif_cleanup_internal(gif_int)


    if method_to_algo[method] == "Sort/Unsort":
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
            libstego.sortunsort_extract(palette_data, message, msglen, para)

        # cleanup
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(palette_data)
        libstegofile.io_gif_cleanup_internal(gif_int)

    if method_to_algo[method] == "CPT":
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
            err = libstego.cpt_embed(rgb_data, stego_data, message, msglen, para)
            if err == 0:
                err = libstegofile.io_png_integrate(png_int, stego_data)
                err = libstegofile.io_png_write(destfilename, png_int)
        else:
            err = libstego.cpt_extract(rgb_data, message, msglen, para)

        # cleanup
        libstegofile.io_png_cleanup_data(rgb_data)
        libstegofile.io_png_cleanup_data(stego_data)
        libstegofile.io_png_cleanup_internal(png_int)


    if method_to_algo[method] == "Battlesteg":
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
            err = libstego.battlesteg_extract(rgb_data, message, msglen, para)

        # cleanup
        libstegofile.io_png_cleanup_data(rgb_data)
        libstegofile.io_png_cleanup_data(stego_data)
        libstegofile.io_png_cleanup_internal(png_int)


    if method_to_algo[method] == "SVG Steg":
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
            err = libstego.svg_extract(svg_data, message, msglen, para)
            if err != 0:
                print "Error while extracting with SVG Stego"

        # cleanup
        libstegofile.svg_cleanup_data(svg_data)
        libstegofile.svg_cleanup_data(stego_data)
        libstegofile.svg_cleanup_internal(svg_int)

    if method_to_algo[method] == "Echo Hiding":
        # TODO implement
        pass


    if method_to_algo[method] == "Phase Coding":
        # TODO implement
        pass


    if method_to_algo[method] == "LSB":
        # TODO implement
        pass



# TODO: Put Variations of Algorithms here! (= different Parameter-Sets)
# TODO: Put useful Parameter values here!
# If returns None the message is too huge or the media too small
def get_algorithm(filename, msglen, password):
    if filename.endswith("jpeg") or filename.endswith("jpg"):
        # PQ, F5, LSB
        jpeg_data = libstegofile.jpeg_data_t()
        jpeg_int = libstegofile.jpeg_internal_data_t()
        err = libstegofile.io_jpeg_read(filename,jpeg_data, jpeg_int)
#TODO: ERROR (everywhere)

        para = libstegofile.pq_parameter()
        para.header_size = 0
        para.quality = 0

        if libstego.pq_check_capacity(jpeg_data, para) >= msglen:
            libstegofile.jpeg_cleanup_data(jpeg_data)
            libstegofile.io_jpeg_cleanup_internal(jpg_int)
            return ["PQ",para]

        para = libstegofile.f5_parameter()


        if libstego.f5_check_capacity(jpeg_data, para) >= msglen:
            libstegofile.jpeg_cleanup_data(jpeg_data)
            libstegofile.io_jpeg_cleanup_internal(jpg_int)
            return ["F5",para]

        para = libstegofile.lsb_parameter()


#        if libstego.lsb_check_capacity(jpeg_data, para) >= msglen:
#            libstegofile.jpeg_cleanup_data(jpeg_data)
#            libstegofile.io_jpeg_cleanup_internal(jpg_int)
#            return ["LSB",para]

        libstegofile.jpeg_cleanup_data(jpeg_data)
        libstegofile.io_jpeg_cleanup_internal(jpg_int)
        return

    elif filename.endswith("gif"):
        # AdvGifshuffle, Gifshuffle, Frirui, Sort/Unsort, LSB
        palette_data = libstegofile.palette_data_t()
        gif_int = libstegofile.gif_internal_data_t()
        err = libstegofile.io_gif_read(filename, palette_data, gif_int)

        para = libstegofile.gifshuffle_parameter()
        if libstego.gifshuffle_check_capacity(palette_data, para) >= msglen:
            return ["Adv. Gifshuffle", para]
# no elif gifshuffle since it has the same capacity that adv gusfhuffle has

        para = libstegofile.frirui_parameter()

        para.method = 0;
        para.size = 100;
        para.password = password
        para.pwlen = len(password)

        #para.method = 1;
        #para.threshold = 574;
        #para.password = password
        #para.pwlen = len(password)

        #para.method = 1;
        #para.threshold = 1148;
        #para.password = password
        #para.pwlen = len(password)

        #para.method = 2;
        #para.password = password
        #para.pwlen = len(password)

        #para.method = 1;
        #para.threshold = 2295;
        #para.password = password
        #para.pwlen = len(password)

        #para.method = 0;
        #para.size = 6;
        #para.password = password
        #para.pwlen = len(password)


        # DONE: multiple parameter sets for frirui from Matthias here!
        if libstego.frirui_check_capacity(palette_data, para) >= msglen:
            return ["Frirui", para]

        para = libstegofile.sortunsort_parameter()
        if libstego.sortunsort_check_capacity(palette_data, para) >= msglen:
            return ["Sort/Unsort", para]

        """para = libstegofile_lsb_parameter_t()
        #TODO: LSB Wrappers for structs like palette_data -> Jan ?
        if libstego.lsb_check_capacity(palette_data, para) >= msglen:
            return ["LSB", para]"""


    elif filename.endswith("png"):
        # CPT, Battlesteg, LSB
        rgb_data = libstegofile.rgb_data_t()
        png_int = libstegofile.png_internal_data_t()
        err = libstegofile.io_png_read(filename, rgb_data, png_int)

        for i in [2,4,6,8]:
            para = libstegofile.cpt_parameter()
            para.block_width = i;
            para.block_height = i;
            para.password = password
            para.pwlen = len(password)

            if libstegofile.cpt_check_capacity(rgb_data, para) >= msglen:
                return ["CPT", para]

        para = libstegofile.battlesteg_parameter()
        para.startbit = 7
        para.move_away = 3
        para.range = 5
        para.password = password
        para.pwlen = len(password)

        # parameter set "capacity"
        #para = libstegofile.battlesteg_parameter()
        #para.startbit = 6
        #para.move_away = 3
        #para.range = 5
        #para.password = "asdfasdf"
        #para.pwlen = 8

        if libstegofile.battlesteg_check_capacity(rgb_data, para) >= msglen:
            return ["BattleSteg", para]

        """para = libstegofile.lsb_parameter()
        para.password = password
        para.pwlen = len(password)
        para.select_mode = 2


        if libstegofile.lsb_check_capacity(rgb_data, para) >= msglen:
            return ["LSB", para]"""

    elif filename.endswith("wav"):
        # Phase Coding, Echo Hiding, LSB
        pass

    elif filename.endswith("svg"):
        # SVGSteg

        svg_data = libstegofile.svg_data_t()
        svg_int = libstegofile.svg_internal_data_t()
        err = libstegofile.io_svg_read(filename, svg_data, svg_int)

        if err != 0:
            return None

        para = libstegofile.svg_parameter_t()
        para.password = password
        para.pwlen = len(password)
        para.first_embed_digit = 3

        fits = False
        if libstego.svg_check_capacity(svg_dat, para) >= msglen:
            fits = True

        libstegofile.svg_cleanup_data(svg_dat)
        libstegofile.svg_cleanup_internal(svg_int)

        if fits == True:
            return ["SVG", para]

def call_embed_extract(embed, embed_func, extract_func, src_data,
                       stego_data, message, msglen, para):
    if embed:
        embed_func(src_data, stego_data, message, msglen, para)
    else:
        extract_func(src_data, message, msglen, para)

def call_file_read(data_type, int_type, readfunc, filename):
    """
    returns (src_data, stego_data, int_data)
    """
    src_data = data_type()
    stego_data = data_type()
    int_data = int_type()
    readfunc(filename, src_data, int_data)
    return (src_data, stego_data, int_data)

def embed_or_extract(algo, para, filename, destfilename):
#TODO: File read + Cleanup + implement for earch algo + parameter sets!
#TODO: Return (None) or Return Error
#TODO: After embed call function recursively to call extract (verify msg)
#TODO: Lots of "returns"

    if filename.endswith("jpeg") or filename.endswith("jpg"):
        src_data = libstegofile.jpeg_data_t()
        jpg_int = libstegofile.jpeg_internal_data_t()
        libstegofile.io_jpeg_read(filename, src_data, jpg_int)
        if algo == "PQ":
            if embed:
                stego_data = libstegofile.jpeg_data_t()
                err = libstego.pq_embed(src_data, stego_data, message, len(message) + 1, para)
                if err == 0:
                    libstegofile.io_jpeg_integrate(jpg_int, stego_data)
                    libstegofile.io_jpeg_write(destfilename, jpg_int)
                libstegofile.io_jpeg_cleanup_data(stego_data)
            else:
                err = libstego.pq_extract(src_data, message, msglen, para)
        elif algo == "F5":
            if embed:
                stego_data = libstegofile.jpeg_data_t()
                err = libstego.f5_embed(src_data, stego_data, message, len(message) + 1, para)
                if err == 0:
                    libstegofile.io_jpeg_integrate(jpg_int, stego_data)
                    libstegofile.io_jpeg_write(destfilename, jpg_int)
                libstegofile.io_jpeg_cleanup_data(stego_data)
            else:
                err = libstego.f5_extract(src_data, message, msglen, para)
        """elif algo == "LSB":
            if embed:
                stego_data = libstegofile.jpeg_data_t()
                err = libstego.lsb_embed(src_data, stego_data, message, len(message) + 1, para)
                if err == 0:
                    libstegofile.io_jpeg_integrate(jpg_int, stego_data)
                    libstegofile.io_jpeg_write(destfilename, jpg_int)
                libstegofile.io_jpeg_cleanup_data(stego_data)
            else:
                err = libstego.lsb_extract(src_data, message, msglen, para)"""

        libstegofile.io_jpeg_cleanup_data(src_data)
        libstegofile.io_jpeg_cleanup_internal(jpg_int)
        return err
    elif filename.endswith("gif"):
        src_data = libstegofile.palette_data_t()
        stego_data = libstegofile.palette_data_t()
        gif_int = libstegofile.gif_internal_data_t()
        libstegofile.io_gif_read(filename, src_data, gif_int)
        if algo == "Adv. Gifshuffle":
            if embed:
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

        elif algo == "Gifshuffle":
            if embed:
                libstego.gifshuffle_embed(src_data, stego_data, message,
                            msglen, para)
            else:
                libstego.gifshuffle_extract(src_data, message,
                            msglen, para)


        elif algo == "Sort/Unsort":
            if embed:
                libstego.sortunsort_embed(src_data, stego_data, message,
                        msglen, para)

            else:
                libstego.sortunsort_extract(src_data, message,
                        msglen, para)


        elif algo == "Frirui":
            if embed:
                libstego.frirui_embed(src_data, stego_data, message,
                        msglen, para)

            else:
                libstego.frirui_extract(src_data, message,
                        msglen, para)


        elif algo == "LSB":
            if embed:
                #embed
                pass
            else:
                #extract
                pass

        # Write back and clean up
        libstegofile.io_gif_integrate(gif_int, stego_data)
        libstegofile.io_gif_write(destfilename, gif_int)
        libstegofile.io_gif_cleanup_data(stego_data)
        libstegofile.io_gif_cleanup_data(src_data)
        libstegofile.io_gif_cleanup_internal(gif_int)

    elif filename.endswith("png"):
        src_data, stego_data, png_int = call_file_read(libstegofile.rgb_data_t,
                                                       libstegofile.png_internal_data_t,
                                                       libstegofile.io_png_read)
        if algo == "CPT":
            call_embed_extract(embed, libstego.cpt_embed, libstego.cpt_extract,
                               src_data, stego_data, message, msglen, para)
        elif algo == "Battlesteg":
            call_embed_extract(embed, libstego.battlesteg_embed, libstego.battlesteg_extract,
                               src_data, stego_data, message, msglen, para)

        elif algo == "LSB":
            call_embed_extract(embed, libstego.lsb_embed, libstego.lsb_extract,
                               src_data, stego_data, message, msglen, para)

        libstegofile.io_png_integrate(png_int, stego_data)
        libstegofile.io_png_write(destfilename, png_int)
        libstegofile.io_png_cleanup_data(stego_data)
        libstegofile.io_png_cleanup_data(src_data)
        libstegofile.io_png_cleanup_internal(png_int)


    elif filename.endswith("wav"):
        if algo == "Echo Hiding":
            if embed:
                #embed
                pass
            else:
                #extract
                pass

        elif algo == "Phase Coding":
            if embed:
                #embed
                pass
            else:
                #extract
                pass

        elif algo == "LSB":
            if embed:
                #embed
                pass
            else:
                #extract
                pass

    elif filename.endswith("svg"):
        if algo == "SVG Steg":
            if embed:
                #embed
                pass
            else:
                #extract
                pass

        elif algo == "LSB":
            if embed:
                #embed
                pass
            else:
                #extract
                pass
