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
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#include "libstego/echo_hiding.h"
/**
* Embeds a message in a WAV-Soundfile using Echo-Hiding
* First two copys of the cover-signal are created.
* One copy is applied with an echo which represents a one.
* The other copy is applied with an echo which represents a zero.
* The offset of these two echos are different and they are lower than 
* three milliseconds.
* After that a mix-signal is created, based on the bits of the
* secret message. 
* At last the two signals are mixed by this mix-signal
*
* @param *src_data a struct for audiodata to provide the original audiosignal
* @param *stego_data a struct for audiodata to return the steganogram
* @param *message the message to embed
* @param msglen the length of the message
* @param *para additional parameters for EchoHiding, including echoamplitude, 
* echo-one-offset, echo-zero-offset, blocksize
*
* @return LSTG_OK on success, LSTG_ERROR if there was an error.
*/

uint8_t eh_embed(const wav_data_t *src_data, wav_data_t *stego_data,
 		 uint8_t *message, uint8_t msglen, eh_parameter *para)
{
    uint8_t  channels = src_data->num_channels;
    uint32_t num_samples = src_data->size;

    //check the size of the cover, if the message will fit in
    printf("check if messagelen suits to cover\n");
    uint32_t maxsize = (uint32_t)floor(src_data->size/para->block_size)/8;
    if (maxsize < msglen+1) {
        FAIL(LSTG_E_INSUFFCAP);
    }

    printf("initiate stego_data\n");
    //initiate stego_data
    stego_data->num_channels = channels;
    stego_data->size = num_samples;
    // allocate enough memory to hold a copy of the samples
    stego_data->data = (int16_t**)malloc(sizeof(int16_t*) 
                       * src_data->num_channels * src_data->size);
    // copy all samples from src to copy
    uint32_t i;
    uint32_t k;
    for (i = 0; i < channels; i++) {
        stego_data->data[i] = (int16_t*)malloc(sizeof(int16_t)*num_samples);
        if (stego_data->data[i] == NULL) {
            FAIL(LSTG_E_MALLOC);
        }
        for (k = 0; k < num_samples; k++) {
            stego_data->data[i][k] = src_data->data[i][k];
        }
    }

    //initiate one_echo
    wav_data_t one_echo;
    one_echo.num_channels = channels;
    one_echo.size = num_samples;
    //allocate enough memory to hold a copy of the samples
    one_echo.data = (int16_t**)malloc(sizeof(int16_t*) * src_data->num_channels * src_data->size);
    // copy all samples from src to copy
    for (i = 0; i < channels; i++) {
        one_echo.data[i] = (int16_t*)malloc(sizeof(int16_t)*num_samples);
        if (one_echo.data[i] == NULL) {
            FAIL(LSTG_E_MALLOC);
        }
        for (k = 0; k < num_samples; k++) {
            one_echo.data[i][k] = src_data->data[i][k];
        }
    }
    //initiate zero_echo
    wav_data_t zero_echo;
    zero_echo.num_channels = channels;
    zero_echo.size = num_samples;
    //allocate enough memory to hold a copy of the samples
    zero_echo.data = (int16_t**)malloc(sizeof(int16_t*) * src_data->num_channels * src_data->size);
    // copy all samples from src to copy
    for (i = 0; i < channels; i++) {
        zero_echo.data[i] = (int16_t*)malloc(sizeof(int16_t)*num_samples);
        if (zero_echo.data[i] == NULL) {
            SAFE_DELETE(one_echo.data);
            FAIL(LSTG_E_MALLOC);
        }
        for (k = 0; k < num_samples; k++) {
            zero_echo.data[i][k] = src_data->data[i][k];
        }
    }

    //the two signals are applied with echos with different 
    //delay-times zero_offset and one_offset
    eh_generate_echo(&zero_echo, para->zero_offset, para->amplitude);
    eh_generate_echo(&one_echo, para->one_offset,  para->amplitude);

    //initiate mix_signal
    wav_data_t mix_signal;
    mix_signal.num_channels = channels;
    mix_signal.size = num_samples;
    //allocate enough memory to hold a copy of the samples
    mix_signal.data = (int16_t**)malloc(sizeof(int16_t*) * src_data->num_channels * src_data->size);
    // copy all samples from src to copy
    for (i = 0; i < channels; i++) {
        mix_signal.data[i] = (int16_t*)malloc(sizeof(int16_t)*num_samples);
        if (mix_signal.data[i] == NULL) {
            SAFE_DELETE(zero_echo.data);
            SAFE_DELETE(one_echo.data);
            FAIL(LSTG_E_MALLOC);
        }
        for (k = 0; k < num_samples; k++) {
            mix_signal.data[i][k] = 1;
        }
    }

    //initiate inverse_mix_signal
    wav_data_t inverse_mix_signal;
    inverse_mix_signal.num_channels = channels;
    inverse_mix_signal.size = num_samples;
    //allocate enough memory to hold a copy of the samples
    inverse_mix_signal.data = (int16_t**)malloc(sizeof(int16_t*) * src_data->num_channels * src_data->size);
    // copy all samples from src to copy
    for (i = 0; i < channels; i++) {
    inverse_mix_signal.data[i] = (int16_t*)malloc(sizeof(int16_t)*num_samples);
        if (inverse_mix_signal.data[i] == NULL) {
            SAFE_DELETE(zero_echo.data);
            SAFE_DELETE(one_echo.data);
            SAFE_DELETE(mix_signal.data);
            FAIL(LSTG_E_MALLOC);
        }
        for (k = 0; k < num_samples; k++) {
            inverse_mix_signal.data[i][k] = 0;
        }
    }

    //the mix-signals are filled with values representing the 
    //secret message.
    eh_generate_mix(&mix_signal, &inverse_mix_signal, message, msglen, para->block_size);

    //the steganogramm-signal is mixed out of the echo signals
    eh_mix_echoes(stego_data, zero_echo, 
                        one_echo, mix_signal,
                        inverse_mix_signal);

    //check if the messagelength is correct embedded. 
    uint8_t *test_length = (uint8_t*) malloc (sizeof(uint8_t));
    *test_length = eh_get_message_length(stego_data, para);
    if(msglen != *test_length){
        SAFE_DELETE(zero_echo.data);
        SAFE_DELETE(one_echo.data);
        SAFE_DELETE(mix_signal.data);
        SAFE_DELETE(inverse_mix_signal.data);
        FAIL(LSTG_E_MLEN_EM_FAIL);
        //FAIL(LSTG_E_OPENFAILED);
    }
    
    //Helpsignals for the Embedding-process are not longer needed
    SAFE_DELETE(test_length);
    SAFE_DELETE(zero_echo.data);
    SAFE_DELETE(one_echo.data);
    SAFE_DELETE(mix_signal.data);
    SAFE_DELETE(inverse_mix_signal.data);

    return LSTG_OK;
}

/**
* An audio-signal is applied with an echo. 
* The delay of the echo is the offset. 
* The echo is added with the given amplitude.
*
* @param *echo a struct for audiodata to provide the original audiosignal
* which should be applied with the echo
* @param offset is the delay of the echo
* @param amplitude for the echo
*
* @return LSTG_OK on success, LSTG_ERROR if there was an error.
*/
uint32_t eh_generate_echo(wav_data_t *echo, uint32_t offset, float amplitude)
{
    uint32_t c;
    uint32_t i;
    int16_t maxint = 32767;
    int32_t h;
    int16_t pos;
    double max_faktor;

    for (c = 0; c < echo->num_channels; c++) {
        for (i = 0; i < echo->size; i++) {
            h=i-offset;
            pos=(echo->data[c][i]>0)?(echo->data[c][i]):(echo->data[c][i]*-1);
            max_faktor = 1-(double)pos/(double)maxint;
            if(h>=0){
                 echo->data[c][i] = 1*echo->data[c][i] + max_faktor * amplitude * echo->data[c][i-offset];
            }
            else{
                echo->data[c][i] = 1*echo->data[c][i];
            }
        }
    }
    return LSTG_OK;
}


/**
* The two mix-signals are generated.
* If a message-bit is 1 an amount of samples, which
* is given by blocksize, of the mix_signal is set to 1.
* If a message-bit is 0 an amount of samples, which
* is given by blocksize, of the mix_signal is set to 0.
* The inverse_mix_signal is 0 when mix_signal is 1 and
* 1 when mix_signal is 0.
*
* @param *mix_signal 
* @param *inverse_mix_signal 
* @param *message decides bit for bit, if the values of
* mix_signal or inverse_mix_signal are set with 1.
* @param msglen is the length of the message
* @param blocksize is the amount of samples that are
* needed to represent a bit of the message.
*
* @return LSTG_OK on success, LSTG_ERROR if there was an error.
*/
uint32_t eh_generate_mix(wav_data_t *mix_signal, wav_data_t *inverse_mix_signal, 
                          uint8_t *message, uint8_t msglen, uint32_t blocksize)
{
    uint32_t c;
    uint8_t i;
    uint32_t k;
    uint32_t b;
    uint8_t  len = msglen;
    uint8_t bit = 0;
  
    //The message-length is encoded in the mix_signals
    for (i = 0; i < 8; i++) {     
        bit = 0;
	if((len & 128) == 128){
	    bit = 1;
	}
        for (c = 0; c < mix_signal->num_channels; c++) {
            for (b = 0; b < blocksize; b++) {
                mix_signal->data[c][(i*blocksize)+b] = bit;
                inverse_mix_signal->data[c][(i*blocksize)+b] = 1-bit;
            }
        }
        len = len << 1;
    }

    uint32_t n = 0;

    //The message is encoded in the mix_signals
    for (i = 0; i < msglen; i++) {     
        for (k = 0; k < 8; k++) {
            uint32_t one = 1;
            n=(message[i] & (one << (7-k))) >> (7 - k);
            for (c = 0; c < mix_signal->num_channels; c++) {
                for (b = 0; b < blocksize; b++) {
                    mix_signal->data[c][(8*blocksize) + (i*blocksize*8) 
                    + (k*blocksize) + b] = n;
                    inverse_mix_signal->data[c][(8*blocksize) 
                    + (i*blocksize*8) + (k*blocksize) + b] =(n>0)?(0):(1);
                }
            }
        }
    }

    return LSTG_OK;
}



/**
* Two echo-signals are mixed by using two mix_signals.
* The result is the steganogramm.
*
* @param *stego_signal is the resulting steganogramm 
* @param zero_echo is the echo-signal which represents a 0
* @param one_echo is the echo-signal which represents a 1
* @param mix_signal gives the amplitude of the
* zero_echo per sample.
* @param inverse_mix_signal gives the amplitude of the
* one_echo per sample.
*
* @return LSTG_OK on success, LSTG_ERROR if there was an error.
*/
uint32_t eh_mix_echoes(wav_data_t *stego_signal, wav_data_t zero_echo, 
                        wav_data_t one_echo, wav_data_t mix_signal,
                        wav_data_t inverse_mix_signal)
{
    uint32_t c;
    uint32_t i;
    for (c = 0; c < stego_signal->num_channels; c++) {
        for (i = 0; i < stego_signal->size; i++) {
            stego_signal->data[c][i] =mix_signal.data[c][i]*zero_echo.data[c][i]
            + inverse_mix_signal.data[c][i]*one_echo.data[c][i];
        }
    }
    return LSTG_OK;
}

/**
* Returns the length of an embedded message.
* The first 32 bit of the secret message, which represents the messagelength, 
* are extracted with autocorrelation.
* The autocorrelation is performed with the zero_offset
* and with the one_offset for each block in which a 
* messagebit is embedded. If the result of the correlation
* with the zero_offset is greater than the result with
* the one_offset the embedded bit in this block is a zero.
* Otherwise a One.
*
* @param *stego_data a struct for audiodata containing the stegonagram
* @param *para additional parameters for EchoHiding 
*
* @return LSTG_OK on success, LSTG_ERROR if there was an error.
*/
uint32_t eh_get_message_length(wav_data_t *stego_data, eh_parameter *para)
{
    uint8_t *msglen = (uint8_t*) malloc (sizeof(uint8_t));

    uint32_t block_size = para->block_size;

    //index variables
    uint8_t i;
    uint32_t b;
    
    //helping variables for calculate the autocorrelation
    int32_t sum_one;
    double erg_one = 0;
    int32_t sum_zero;
    double erg_zero = 0;

    //number of bits representing the messagelength
    uint32_t num_bits = 8;


    for (i = 0; i<num_bits;i++){
        for (b = 0; b < block_size; b++) {
            sum_zero = stego_data->data[0][(i*block_size)+b] * 
                stego_data->data[0][(i*block_size)+b+para->zero_offset];
            erg_zero = erg_zero + (double)sum_zero/block_size;
	    sum_one = stego_data->data[0][(i*block_size)+b] *
                stego_data->data[0][(i*block_size)+b+para->one_offset];
            erg_one = erg_one + (double)sum_one/block_size;

        }
        erg_zero=(erg_zero>0)?(erg_zero):(erg_zero*-1);
        erg_one=(erg_one>0)?(erg_one):(erg_one*-1);

        if(erg_zero<erg_one){
            *msglen <<= 1;
        }
	else{
	    *msglen <<= 1;
	    *msglen+= 1;
	}
        erg_zero = 0;
        erg_one = 0;
    }
    return *msglen;	

}


/**
* Extracts a EchoHiding-embedded-message from a stegonogram
* An autocorrelation of the steganogram is performed.
* The peaks of the autocorrelation-signal represents an echo.
* The offset of those peaks represents a one or a zero.
*
* @param *stego_data a struct for audiodata containing the stegonagram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for EchoHiding including echoamplitude, 
* echo-one-offset, echo-zero-offset, blocksize
*
* @return LSTG_OK on success, LSTG_ERROR if there was an error.
*/
uint8_t eh_extract(wav_data_t *stego_data, uint8_t 
                   **message, uint8_t *msglen, eh_parameter *para) 
{
    uint32_t capacity = 0;
    eh_check_capacity(stego_data, para, &capacity);
    *msglen = eh_get_message_length(stego_data, para);
    if(*msglen>=capacity){
        FAIL(LSTG_E_INSUFFCAP);
    }
    eh_auto_correlate(stego_data, para, msglen, message);
    return LSTG_OK;
}


/**
* An autocorrelation of the steganogram is performed.
* The autocorrelation is performed with the zero_offset
* and with the one_offset for each block in which a 
* messagebit is embedded. If the result of the correlation
* with the zero_offset is greater than the result with
* the one_offset the embedded bit in this block is a zero.
* Otherwise a One.
*
* @param *stego_data a struct for audiodata containing the stegonagram
* @param *para additional parameters for EchoHiding including echoamplitude, 
* echo-one-offset, echo-zero-offset, blocksize
* @param msglen the length of the message
* @param **message returnpointer for the extracted message
*
* @return LSTG_OK on success, LSTG_ERROR if there was an error.
*/
uint32_t eh_auto_correlate(wav_data_t *stego_data, 
                   eh_parameter *para, uint8_t *msglen, uint8_t **message)
{
    printf("begin of autocorrelation\n");
    uint32_t block_size = para->block_size;

    //index variables
    uint32_t i = 0;
    uint32_t k = 0;
    uint32_t b;
    
    //helping variables for calculate the autocorrelation
    int32_t sum_one;
    double erg_one = 0;
    int32_t sum_zero;
    double erg_zero = 0;
    
    //Initiate message
    *message = (uint8_t*)malloc(sizeof(uint8_t) * *msglen);

    uint32_t msg_samples = 8*block_size;

    for (i = 0; i<(*msglen);i++){
        (*message)[i] = 0;
        for (k = 0; k<8;k++){
            for (b = 0; b < block_size; b++) {
	        sum_one = stego_data->data[0][msg_samples+(i*block_size*8)+(k*block_size)+b] 
                * stego_data->data[0][msg_samples+(i*block_size*8)+(k*block_size)+b+para->one_offset];
                erg_one = erg_one + (double)sum_one/block_size;
		sum_zero = stego_data->data[0][msg_samples+(i*block_size*8)+(k*block_size)+b] 
                * stego_data->data[0][msg_samples+(i*block_size*8)+(k*block_size)+b+para->zero_offset];
                erg_zero = erg_zero + (double)sum_zero/block_size;
            }
            erg_one=(erg_one>0)?(erg_one):(erg_one*-1);
            erg_zero=(erg_zero>0)?(erg_zero):(erg_zero*-1);
           
            if(erg_one>erg_zero){
                if (k != 7) {            
                    (*message)[i] = ((*message)[i] + 0) << 1;
                }
                else {
                    (*message)[i] = (*message)[i] + 0;
                }
                printf("0");
	    } 
	    else{
                if (k != 7) {            
                    (*message)[i] = ((*message)[i] + 1) << 1;
                }
                else{
                    (*message)[i] = (*message)[i] + 1;
                }
                printf("1");
	    }
            erg_one = 0;
            erg_zero = 0;
        }
    }
    return LSTG_OK;
}


/**
* Get the maximum capacity of the cover with the given parameters.
* The capacity is depends on the blocksize
* This is only the theoretical maximum capacity.
*
* @param *src_data a struct for audiodata to provide the original audiosignal
* @param *para additional parameters for EchoHiding including echoamplitude, 
* echo-one-offset, echo-zero-offset, blocksize
* @param capacity Pointer to an uint32_t, will containt the maximum capacity,
* in bits, after the function returns.
*
* @return LSTG_OK on success, LSTG_ERROR if there was an error.
*/

uint32_t eh_check_capacity(const wav_data_t *src_data, 
               const eh_parameter *para, uint32_t *capacity)
{
    *capacity = (uint32_t)floor(src_data->size/para->block_size)-8;
    return LSTG_OK;

}
