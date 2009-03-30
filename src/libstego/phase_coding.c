#include "libstego/phase_coding.h"
#include "libstego/random.h"

#define PI (3.14159265358979323846)
#define THR 0

uint8_t pc_embed_block(uint32_t block_size,const uint8_t *message, uint32_t msg_len, double *in, fftw_complex *out);
uint8_t pc_extract(const wav_data_t *stego_data, uint8_t *message, int32_t msg_len, const pc_parameter_t *para);



double abs_complex(fftw_complex *c)
{
  return sqrt(*c[0]**c[0] + *c[1]**c[1]);
}

void polar_2_complex(fftw_complex *c)
{
  double r = *c[0] * cos(*c[1]);
  *c[1] = *c[0] * sin(*c[1]);
  *c[0] = r;
}

void complex_2_polar(fftw_complex *c)
{
  double r = sqrt(*c[0]**c[0] + *c[1]**c[1]);
  *c[1] = atan2(*c[1],*c[0]);
  *c[0] = r;
}

/**
* Maps message bits to phases
*
*/
int32_t* pc_map_phases(uint32_t block_size, uint32_t msg_len, fftw_complex *out, uint32_t threshold)
{
  prng_state *rand;
  random_init(&rand,"546", 3);

  int32_t *map = (int32_t*) malloc(sizeof(int32_t)*msg_len*8);

  for(int i = 0;i < msg_len*8;i++)
  { 
     map[i] = -1;
     while(map[i] == -1)
     {
        map[i] = random_next(rand) % (block_size/2);
//        if(abs_complex(&out[map[i]]) < threshold)
  //      {
   //       map[i] = -1;
     //     continue;
       // }
        for(int j = 0;j < i;j++)
        {
          if(map[j] == map[i])
          {
            map[i] = -1;
            break;
          }
        }
     }
  }
  return map;
}

/**
* Embeds a message in a WAV-Soundfile using PhaseCoding
* Divides the cover into segments of the same size.
* A discrete Fourier-Transformation of all Segments is (performed.
* The phase-difference between the segments is calculated.
* The values of the phase in the first segment are set to 
* - pi/2, if the message bit, which should be embedded is a one 
* - -pi/2, if the message bit, which should be embedded is a zero 
* The phase-difference of the following segments is adjusted
* An inverse discrete Fourier-Transformation with the new
* calculated phase-matrix, returns the steganogram-signal.
* @param *src_data a struct for audiodata to provide the original audiosignal
* @param *stego_data a struct for audiodata to return the steganogram
* @param *message the message to embed
* @param msglen the length of the message
* @param *para additional parameters for PhaseCoding, including passphrase, segmentsize
* @return an errorcode or 0 if success
* @todo implement
*/
uint8_t pc_embed(const wav_data_t *src_data, wav_data_t *stego_data, const uint8_t *message, int32_t msg_len, const pc_parameter_t *para)
{
  stego_data->data = (int16_t**) malloc(sizeof(int16_t*)*src_data->num_channels);
  for(int i = 0;i < src_data->num_channels;i++)
	stego_data->data[i] = (int16_t*) malloc(sizeof(int16_t)*src_data->size);
  stego_data->num_channels = src_data->num_channels;
  stego_data->size = src_data->size;

  msg_len;
  uint32_t block_size = src_data->size;
  fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * block_size);
  double *in = (double*) malloc(sizeof(double) * block_size);

  for(int k = 0;k < src_data->num_channels;k++)
  {
	uint32_t failure = 0;
    int16_t *src_ptr = src_data->data[k];
    int16_t *dst_ptr = stego_data->data[k];
    for(int i = 0;i < block_size;i++)
    {
      in[i] = src_ptr[i]*0.99;
    }
    
    pc_embed_block(block_size, message, msg_len, in , out);

    for(int i = 0;i < block_size;i++)
    {
      dst_ptr[i] = round(in[i] / (double) block_size);
      if(abs(dst_ptr[i] - src_ptr[i]) > 2)
	failure++;
  	
    }

	printf("error rate:%f \n",(double) failure / (double) block_size);
  }

  fftw_free(in); fftw_free(out);

//  for(int i = 0;i < src_data->num_channels;i++)
 //   memcpy(stego_data->data[i],src_data->data[i], sizeof(int16_t)*src_data->size);

  uint8_t* mex = (uint8_t*) malloc(msg_len);
 pc_extract(stego_data,mex,msg_len,NULL);
  int error = 0;
  for(int i = 0;i < msg_len;i++)
	  for(int j = 0;j < 8;j++)
    //if(message[i] != mex[i])
 	if((message[i] & (1 << (j))) != (mex[i] & (1 << (j))))
  		error++;         
//  printf("bit errors: %i \n", error);
  printf("error rate: %f \n", (double) error / (double)(msg_len*8));
  return LSTG_OK;
}


uint8_t pc_embed_block(uint32_t block_size,const uint8_t *message, uint32_t msg_len, double *in, fftw_complex *out)
{
  fftw_plan p;

  //forward differencing of wave data
  p = fftw_plan_dft_r2c_1d(block_size, in, out,FFTW_ESTIMATE);
  fftw_execute(p);

  int32_t* map = pc_map_phases(block_size, msg_len, out, THR);

  for(int i = 0;i < msg_len*8;i++)
  {
    fftw_complex *c = &out[map[i]];
    complex_2_polar(c);
    *c[1] = PI*0.5 * ((message[i/8] & (1 << (i%8)) == 0) ? 1.0 : -1.0);
    polar_2_complex(c);
  }

  //backward differencing of wave data
  p = fftw_plan_dft_c2r_1d(block_size, out, in, FFTW_ESTIMATE);
  fftw_execute(p);

  fftw_destroy_plan(p);
}





/**
* Extracts a PhaseCoding-embedded-message from a stegonogram
* Divides the steganogram into segments of the same size.
* A discrete Fourier-Transformation of all segments is performed.
* The values of the phase in the first segment represents the secret message
* - if the phase is pi/2, the message bit is a one 
* - if the phase is -pi/2, the message bit is a tero 
* @param *stego_data a struct for audiodata containing the stegonagram
* @param *message returnpointer for the extracted message
* @param msglen the length of the message
* @param *para additional parameters for PhaseCoding including passphrase
* @return an errorcode or 0 if success
* @todo implement
*/
//uint8_t pc_extract() {}

uint8_t pc_extract(const wav_data_t *stego_data, uint8_t *message, int32_t msg_len, const pc_parameter_t *para)
{

  uint32_t block_size = stego_data->size;
  fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * block_size);
  double *in = (double*) malloc(sizeof(double) * block_size);

  for(int k = 0;k < stego_data->num_channels;k++)
  {
    int16_t *src_ptr = stego_data->data[k];
    for(int i = 0;i < block_size;i++)
    {
      in[i] = src_ptr[i];
    }
  }

  fftw_plan p;

  //forward differencing of wave data
  p = fftw_plan_dft_r2c_1d(block_size, in, out,FFTW_ESTIMATE);
  fftw_execute(p);

  int32_t* map = pc_map_phases(block_size, msg_len, out, THR);
  
  
  for(int i = 0;i < msg_len;i++)
  {
    uint8_t m = 0;
    for(int j = 0;j < 8;j++)
    {
      fftw_complex *c = &out[map[i*8 + j]];
      complex_2_polar(c);
      m |= ((*c[1] < 0) ? 1 : 0) << (j);
    }
    message[i] = m;
  }

  fftw_destroy_plan(p);
}
/**
* Returns the length of an embedded message
* ...
* @param *stego_data a struct for audiodata containing the stegonagram
* @param *para additional parameters for PhaseCoding including passphrase
* @return length of embedded message
* @todo implement
*/
/* int32_t pc_get_message_length(const wav_data_t *stego_data, const pc_parameter *para) */
uint8_t pc_get_message_length()
{

}

/**
* Returns the capacity of a cover image
* Depends on the size of the segments.
* @param *src_data a struct for audiodata to provide the original audiosignal
* @param *para additional parameters for PhaseCoding including passphrase
* @return capacity of cover audiosignal
* @todo implement
*/
/* int32_t pc_check_capacity(const wav_data_t *src_data, const pc_parameter *para) */
int32_t pc_check_capacity()
{

}
