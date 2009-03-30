#ifndef __PHASE_CODING_H__
#define __PHASE_CODING_H__

#include "libstego/types.h"
#include "libstego/errors.h"
#include <fftw3.h>
#include <math.h>




uint8_t pc_embed(const wav_data_t *src_data, wav_data_t *stego_data, const uint8_t *message, int32_t msg_len, const pc_parameter_t *para);

/* uint8_t pc_extract(const wav_data_t *stego_data, uint8_t *message, int32_t msglen, const pc_parameter *para); */

/* int32_t pc_get_message_length(const wav_data_t *stego_data, const pc_parameter *para); */

/* int32_t pc_check_capacity(const wav_data_t *src_data, const pc_parameter *para); */

#endif // __PHASE_CODING_H__
