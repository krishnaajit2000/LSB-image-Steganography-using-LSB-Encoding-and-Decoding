#include <stdio.h>
#include <string.h>
#include "types.h"
#include "common.h"
#include "decode.h"
#include "encode.h"

/* Function Definitions */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /*check argv[2] is .bmp file or not
     */
    if (strstr(argv[2], ".") != NULL)
    {
	if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{

	    decInfo -> stego_image_fname = argv[2];
	    //argv[3] is given or not
	    if (argv[3] != NULL)
	    {
		strcpy(decInfo -> output_fname,argv[3]);
	    }
	    else
	    {
		printf("INFO: Output File not mentioned. Creating default.txt as default\n");
		strcpy(decInfo -> output_fname, "default");
	    }
	}
	else
	{
	    printf("ERROR: not .bmp file");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR: Please pass %s file with extension\n", argv[2]);
	return e_failure;
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");
    //function calls
    if (open_files_decoding(decInfo) == e_success)
    {
	printf("INFO: Done\n");
	if (skip_header(decInfo) == e_success)
	{
	    if (decode_magic_string_size(decInfo) == e_success)
	    {
		printf("INFO: Decoding Magic String Signature\n");
		if (decode_magic_string(decInfo) == e_success)
		{
		    printf("INFO: Done\n");
		    if (decode_secret_file_extn_size(decInfo) == e_success)
		    {
			printf("INFO: Decoding Output File Extension\n");
			if (decode_secret_file_extn(decInfo) == e_success)
			{
			    printf("INFO: Done\n");
			    if (open_output_file(decInfo) == e_success)
			    {
				printf("INFO: Done. Opened all required files\n");
				printf("INFO: Decoding %s File Size\n", decInfo -> output_fname);
				if (decode_secret_file_size(decInfo) == e_success)
				{
				    printf("INFO: Done\n");
				    printf("INFO: Decoding %s File Data\n", decInfo -> output_fname);
				    if (decode_secret_file_data(decInfo) == e_success)
				    {
					printf("INFO: Done\n");
				    }
				    else
				    {
					printf("ERROR: Failed to decode secret file data\n");
					return e_failure;
				    }
				}
				else
				{
				    printf("ERROR: Failed to decode secret file size\n");
				    return e_failure;
				}
			    }
			    else
			    {
				printf("ERROR: Failed to open outputfile\n");
				return e_failure;
			    }
			}
			else
			{
			    printf("ERROR: Failed to decode secret fiile extn\n");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("ERROR: Failed to decode secret file extn size\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("ERROR: Failed to decode magic string\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("ERROR: Failed to decode magic string size\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("ERROR: Failed to skip header\n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR: Failed to open file\n");
	return e_failure;
    }
    return e_success;
}


Status open_files_decoding(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files\n");
    decInfo->fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");
    printf("INFO: Opened %s file\n", decInfo -> stego_image_fname);
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
	return e_failure;
    }

    return e_success;
}

Status skip_header(DecodeInfo *decInfo)
{
    fseek(decInfo -> fptr_stego_image,54,SEEK_SET);
    return e_success;
}


char decode_lsb_to_byte(char *buffer)
{
    char ch =0;
    for (int i=0; i<8; i++)
    {
	ch = ((buffer[i] & 1) << (7-i)) | ch;
    }
    return ch;
}

int decode_lsb_to_size(char *buffer)
{
    int size =0;
    for (int i=0; i<32; i++)
    {
	size = ((buffer[i] & 1) << (31-i)) | size;
    }
    return size;
}


Status decode_magic_string_size(DecodeInfo *decInfo)
{
    char str[32];
    fread(str,32,1,decInfo -> fptr_stego_image);
    decInfo -> magic_string_size = decode_lsb_to_size(str);
    //printf("size of magic string : %d\n", decInfo -> magic_string_size);
    if (decInfo -> magic_string_size == strlen(MAGIC_STRING))
    {
	return e_success;
    }
    else
    {
	printf("ERROR: MAGIC_STRING size is different\n");
	return e_failure;
    }

}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char str[8];
    char magicstring[decInfo -> magic_string_size +1];
    int i;
    for (i=0; i<(decInfo -> magic_string_size); i++)
    {
	fread(str,8,1,decInfo -> fptr_stego_image);
	magicstring[i] = decode_lsb_to_byte(str);
    }
    magicstring[i] = '\0';
    //printf("magic string : %s\n", magicstring);
    strcpy(decInfo -> magic_string,magicstring);
    if (strcmp(decInfo -> magic_string,MAGIC_STRING) == 0)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char str[32];
    fread(str,32,1,decInfo -> fptr_stego_image);
    decInfo -> secret_file_extn_size = decode_lsb_to_size(str);
    //printf("size of secret file extn : %d\n", decInfo -> secret_file_extn_size);
    return e_success;

}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char str[8];
    char secretextn[(decInfo -> secret_file_extn_size) + 1];
    int i;
    for (i=0; i< (decInfo -> secret_file_extn_size); i++)
    {
	fread(str,8,1,decInfo -> fptr_stego_image);
	secretextn[i] = decode_lsb_to_byte(str);
    }
    secretextn[i] = '\0';
    //printf("secret file extn : %s\n",secretextn);
    strcpy(decInfo -> secret_file_extn, secretextn);
    strcat(decInfo -> output_fname, decInfo -> secret_file_extn);
    return e_success;
}

Status open_output_file(DecodeInfo *decInfo)
{
    decInfo -> fptr_output = fopen(decInfo -> output_fname,"w");
    printf("INFO: Opened %s\n", decInfo -> output_fname);
    // Do Error handling
    if (decInfo->fptr_output == NULL)
    { 
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo-> output_fname);
	return e_failure;
    }
    return e_success;

}





Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char str[32];
    fread(str,32,1,decInfo -> fptr_stego_image);
    decInfo -> secret_file_size = decode_lsb_to_size(str);
    //printf("size of secret file : %d\n", decInfo -> secret_file_size);
    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char str[8];
    char secret_file[(decInfo -> secret_file_size) +1];
    int i;
    for (i=0; i < (decInfo -> secret_file_size); i++)
    {
	fread(str,8,1,decInfo -> fptr_stego_image);
	secret_file[i] = decode_lsb_to_byte(str);
    }
    secret_file[i] ='\0';
    //printf("secret file data : %s\n", secret_file);
    fwrite(secret_file,decInfo -> secret_file_size,1, decInfo -> fptr_output);
    return e_success;
}








