#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct DecodeInfo
{
    char *stego_image_fname;     //store output.bmp
    FILE *fptr_stego_image;       //store file add of outpt.bmp file
    char output_fname[20];
    FILE *fptr_output;

    int magic_string_size;    //store magic string size in a variable
    char magic_string[20];     //store the magic string
    int secret_file_extn_size;
    char secret_file_extn[20];
    int secret_file_size;

} DecodeInfo;



/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);
//open file
Status open_files_decoding(DecodeInfo *decInfo);
//skip header
Status skip_header(DecodeInfo *decInfo);
//bytes to lsb
char decode_lsb_to_byte(char *buffer);
//size to lsb
int decode_lsb_to_size(char *buffer);
//Decode magic string size
Status decode_magic_string_size(DecodeInfo *decInfo);
//Decode magic string
Status decode_magic_string(DecodeInfo *decInfo);
//Decode secret file extn size
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
//Decode secret file extn
Status decode_secret_file_extn(DecodeInfo *decInfo);
//Open output file
Status open_output_file(DecodeInfo *decInfo);
//Decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo);
//Decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo);






