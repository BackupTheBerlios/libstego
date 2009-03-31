/*
 *  This file is part of libstego.
 *
 *  libstego is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libstego is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libstego.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  Copyright 2008 2009 by Jan C. Busch, René Frerichs, Lars Herrmann,
 *  Matthias Kloppenborg, Marko Krause, Christian Kuka, Sebastian Schnell,
 *  Ralf Treu.
 *  Authors: Jan C. Busch, René Frerichs
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#include "libstego/wav.h"

/**
* Function to read a wav-file from HDD and return a wav_data_t
*
* @param *filename the path and filename of the wav-file
* @param *wav_data a struct to return the audio samples
* @param *int_dat a struct provided by the wav-library to hold the original audio-file
* @return LSTG_OK, or LSTG_ERROR when something failed.
*/
uint8_t io_wav_read(const uint8_t *filename, wav_data_t *wav_data, wav_internal_data_t *int_dat)
{
    SNDFILE *file = NULL;
    int i = 0, k = 0;
    sf_count_t frames_read = 0;

    if (int_dat == 0) {
        lstg_errno = LSTG_E_MALLOC;
        return LSTG_ERROR;
    }
    // setting format field to zero, as detailed in libsndfile's documentation
    int_dat->info.format = 0;

    // open wav-file
    file = sf_open(filename, SFM_READ, &int_dat->info);
    if (file == NULL) {
        lstg_errno = LSTG_E_OPENFAILED;
        return LSTG_ERROR;
    }

    int_dat->data = (int*)malloc(sizeof(int) * int_dat->info.channels * int_dat->info.frames);
    if (int_dat->data == NULL) {
        lstg_errno = LSTG_E_MALLOC;
        return LSTG_ERROR;
    }

    // force floating point values within the file to be scaled correctly
    sf_command(file, SFC_SET_SCALE_FLOAT_INT_READ, NULL, SF_TRUE);

    // read content of wav-file
    frames_read = sf_readf_int(file, int_dat->data, int_dat->info.frames);
    if (frames_read != int_dat->info.frames) {
        lstg_errno = LSTG_E_READFAILED;
        return LSTG_ERROR;
    }

    // close the file
    sf_close(file);

    // re-order data into a two-dimensional array. first dimension will be
    // the channel, second dimension the samples of that channel.
    wav_data->data = (int16_t**)malloc(sizeof(int16_t*) * int_dat->info.channels);
    if (wav_data->data == NULL) {
        lstg_errno = LSTG_E_MALLOC;
        return 1;
    }
    for (i = 0; i < int_dat->info.channels; i++) {
        wav_data->data[i] = (int16_t*)malloc(sizeof(int16_t)*int_dat->info.frames);
        if (wav_data->data[i] == NULL) {
            lstg_errno = LSTG_E_MALLOC;
            return 1;
        }
        for (k = 0; k < int_dat->info.frames; k++) {
            wav_data->data[i][k] = (int_dat->data[k*int_dat->info.channels+i]) >> 16;
        }
    }

    // number of channels
    wav_data->num_channels = int_dat->info.channels;
    // number of samples per channel
    wav_data->size = int_dat->info.frames;

    return LSTG_OK;
}

/**
* Function to integrate a changed wav_data_t into a wav_struct
*
* @param *wav_struct a struct provided by the wav-library to hold the original audio data
* @param *wav_data a struct to give the changed wavdata (steganogram)
* @return LSTG_OK, or LSTG_ERROR when something failed.
*/
uint8_t io_wav_integrate(wav_internal_data_t *wav_struct, const wav_data_t *wav_data)
{
    int channel = 0, frame = 0;

    // check for size mismatches
    if (wav_struct->info.channels != wav_data->num_channels ||
            wav_struct->info.frames != wav_data->size) {
        lstg_errno = LSTG_E_SIZEMISMATCH;
        return LSTG_ERROR;
    }

    for (channel = 0; channel < wav_struct->info.channels; ++channel) {
        for (frame = 0; frame < wav_struct->info.frames; ++frame) {
            wav_struct->data[frame*wav_struct->info.channels+channel] =
                    wav_data->data[channel][frame] << 16;
        }
    }

    return LSTG_OK;
}

/**
* Function to write a wav_struct to HDD by using the wav-library
*
* @param *filename the path and filename of the wav-file
* @param *wav_struct a struct provided by the wav-library to hold the original audio data
* @return LSTG_OK, or LSTG_ERROR when something failed.
*/
uint8_t io_wav_write(const uint8_t *filename, wav_internal_data_t *wav_struct)
{
    SNDFILE *file = NULL;
    wav_internal_data_t *int_dat = (wav_internal_data_t*)wav_struct;

    // make a copy of the SF_INFO struct, since sf_open seems to change some
    // values (especially, sets 'frames' to zero (bug in libsndfile?)
    SF_INFO sf_info = int_dat->info;

    if (wav_struct == NULL) {
        lstg_errno = LSTG_E_INVALIDPARAM;
        return LSTG_ERROR;
    }

    file = sf_open(filename, SFM_WRITE, &sf_info);

    if (file == NULL) {
        LSTG_E_OPENFAILED;
        return LSTG_ERROR;
    }

    sf_writef_int(file, int_dat->data, int_dat->info.frames);

    sf_close(file);

    return LSTG_OK;
}


/**
* Makes a copy from a wav_struct provided by the wav-library
*
* @param *src_struct original structure
* @param *copy target structure
* @return LSTG_OK, or LSTG_ERROR when something failed.
*/
uint8_t io_wav_copy_internal(const wav_internal_data_t *src, wav_internal_data_t *copy)
{
    copy->data = (int32_t*)malloc(sizeof(int32_t) * src->info.channels * src->info.frames);
    if (copy->data == NULL) {
        lstg_errno = LSTG_E_MALLOC;
        return LSTG_ERROR;
    }

    // copy all values of the SF_INFO structure
    memcpy(&copy->info, &src->info, sizeof(SF_INFO));

    // allocate enought memory to hold a copy of the samples
    copy->data = (int32_t*)malloc(sizeof(int32_t) * src->info.channels * src->info.frames);
    // copy all samples from src to copy
    memcpy(copy->data, src->data, sizeof(int32_t) * src->info.channels * src->info.frames);

    return LSTG_OK;
}

/**
 * Performs the necessary cleanup operations (if any) for wav_data_t's that
 * are no longer used. This function _has_ to be called for every wav_data_t
 * instance.
 *
 * @param wav_data The data structure that is to be cleaned up.
 * @return LSTG_OK, or LSTG_ERROR when something failed.
 */
uint8_t io_wav_cleanup_data(wav_data_t *wav_data) {
    int i = 0;

    if (wav_data == NULL) {
        return LSTG_OK;
    }

    for (i = 0; i < wav_data->num_channels; ++i) {
        SAFE_DELETE(wav_data->data[i]);
    }
    SAFE_DELETE(wav_data->data);
    wav_data->num_channels = 0;
    wav_data->size = 0;

    return LSTG_OK;
}

/**
 * Performs the necessary cleanup operations (if any) for the wave-library's
 * internal data structure. This function _has_ to be called for every instance
 * of such a structure.
 *
 * @param int_dat The internal data structure of the wave-library.
 * @return LSTG_OK, or LSTG_ERROR when something failed.
 */
uint8_t io_wav_cleanup_internal(wav_internal_data_t *int_dat) {
    if (int_dat == NULL) {
        return LSTG_OK;
    }

    SAFE_DELETE(int_dat->data);

    return LSTG_OK;
}
