#include <stdio.h>
#include "encode.h"
#include <string.h>
#include "types.h"
#include "common.h"

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*check argv[2] is .bmp file or not
      .bmp => encInfo -> src_imgae_fname = argv[2]
      not -> return e_failure
     */
    if (strstr(argv[2], ".") != NULL)
    {
	if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
	    encInfo -> src_image_fname = argv[2];
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR: Please pass %s file with extension\n", argv[2]);
	return e_failure;
    }

    /*check argv[3] is .txt or not
      .txt => store argv[3] into encInfo -> secret_fname = argv[3]
      not -> return e_failure
     */
    if (strstr(argv[3], ".") != NULL)
    {
	if (strcmp(strstr(argv[3], "."), ".txt") == 0)
	{
	    encInfo -> secret_fname = argv[3];
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR: Please pass %s file with extension\n", argv[3]);
	return e_failure;
    }

    /*check argv[4] is passed or not
      passed => check argv[4] is .bmp or not
      {
      .bmp => store argv[4] into encInfo -> stego_image_fname
      not -> return e_failure
      }
      not passed => encInfo -> stego_image_fname = "default.bmp"

     */
    if (argv[4])
    {
	if (strstr(argv[4],".") != NULL)
	{
	    if (strcmp(strstr(argv[4], "."), ".bmp") ==0)
	    {
		encInfo -> stego_image_fname = argv[4];

	    }
	    else
	    {
		return e_failure;
	    }
	}
	else
	{
	    printf("ERROR: Please pass %s file with extension", argv[4]);
	}
    }
    else
    {
	printf("INFO: Output File not mentioned . Creating default.bmp as default\n");
	encInfo -> stego_image_fname = "default.bmp";
    }



    return e_success;
}

//do encoding
Status do_encoding(EncodeInfo *encInfo)
{
    //funtn calls

    if (open_files(encInfo) == e_success)
    {
	printf("INFO: Done\n");
	printf("INFO: ## Encoding Procedure Started ##\n");
	printf("INFO: Checking for %s size\n", encInfo -> secret_fname);
	printf("INFO: Done. Not Empty\n");
	printf("INFO: Checking for %s capcity to handle %s\n", encInfo -> src_image_fname, encInfo -> secret_fname);
	if (check_capacity(encInfo) == e_success )
	{
	    printf("INFO: Done. Found OK\n");
	    printf("INFO: Copying Image Header\n");
	    if ( copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
	    {
		printf("INFO: Done\n");
		if (encode_magic_string_size (strlen(MAGIC_STRING),encInfo) == e_success)
		{
		    printf("INFO: Encoding Magic String Signature\n");
		    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
		    {
			printf("INFO: Done\n");
			if (encode_secret_file_extn_size(strlen(strstr(encInfo -> secret_fname, ".")),encInfo) == e_success)
			{
			    printf("INFO: Encoding %s File Extension\n", encInfo -> secret_fname);
			    if (encode_secret_file_extn(strstr(encInfo -> secret_fname, "."),encInfo) == e_success)
			    {
				printf("INFO: Done\n");
				printf("INFO: Encoding %s File Size\n", encInfo -> secret_fname);
				if (encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
				{
				    printf("INFO: Done\n");
				    printf("INFO: Encoding %s File Data\n", encInfo -> secret_fname);
				    if (encode_secret_file_data(encInfo) == e_success)
				    {
					printf("INFO: Done\n");
					printf("INFO: Copying Left Over Data\n");
					if (copy_remaining_img_data(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
					{
					    printf("INFO: Done\n");
					}
					else
					{
					    printf("ERROR: Fail to encode remaning data\n");
					    return e_failure;
					}
				    }
				    else
				    {
					printf("ERROR: Fail to encode secret file data\n");
					return e_failure;
				    }
				}
				else
				{
				    printf("ERROR: Fail to encode secret file size\n");
				    return e_failure;
				}

			    }
			    else
			    {
				printf("ERROR: Fail to encode secret file extn\n");
				return e_failure;
			    }
			}
			else
			{
			    printf("ERROR: Fail to encode secret file extn size\n");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("ERROR: Fail to encode magic string\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("ERROR: Fail to encode magic string size\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("ERROR: Fail to copy bmp header\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("ERROR: Fail to check capacity\n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR: Fail to open files\n");
	return e_failure;
    }
    return e_success;
}


//check capacity
Status check_capacity(EncodeInfo *encInfo)
{
    //find bmp file size
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);     //call and store
    int magic_string_length = strlen(MAGIC_STRING);

    //find extn size
    uint length_of_extension = strlen(strstr(encInfo -> secret_fname, "."));

    //find the secret file size
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);    //call and store

    //checking size capacity is enough
    if ((encInfo -> image_capacity) > (54 + 32 + (magic_string_length * 8) + 32 + (length_of_extension * 8) + 32 + (encInfo -> size_secret_file * 8)))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

//get file size
uint get_file_size(FILE *fptr)
{
    //move the fptr to e nd
    fseek(fptr, 0, SEEK_END);
    //ftell
    return ftell(fptr);

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char str[54];
    //move file pointer(source image) to starting position
    fseek(fptr_src_image,0,SEEK_SET);

    //fread(str, )
    fread(str,54 ,1,fptr_src_image);


    //fwrite()
    fwrite(str, 54,1,fptr_stego_image);
    return e_success;
}


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    printf("INFO: Opened %s\n", encInfo -> src_image_fname);
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");         //shows whts the error is
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    printf("INFO: Opened %s\n", encInfo -> secret_fname);
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    printf("INFO: Opened %s\n", encInfo -> stego_image_fname);
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i=7; i>=0; i--)
    {
	image_buffer[7-i] = (image_buffer[7-i] & (~1)) | ((data & 1<<i) >>i);
    }
}

Status encode_size_to_lsb(int data, char *image_buffer)
{
    for (int i=31; i>=0; i--)
    {
	image_buffer[31-i] = (image_buffer[31-i] & (~1)) | ((data & 1<<i) >>i);
    }

}

Status encode_magic_string_size(int size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str,32,1,encInfo -> fptr_src_image);
    encode_size_to_lsb(size,str);
    fwrite(str,32,1,encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char str[8];
    for (int i=0; i < strlen(magic_string); i++)
    {
	fread(str, 8,1,encInfo -> fptr_src_image);
	encode_byte_to_lsb(magic_string[i], str);
	fwrite(str,8,1, encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str,32,1,encInfo -> fptr_src_image);
    encode_size_to_lsb(size, str);
    fwrite(str,32,1,encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char str[8];
    for (int i=0; i < strlen(file_extn); i++)
    {
	fread(str,8,1,encInfo -> fptr_src_image);
	encode_byte_to_lsb(file_extn[i], str);
	fwrite(str,8,1,encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str,32,1,encInfo -> fptr_src_image);
    encode_size_to_lsb(file_size, str);
    fwrite(str,32,1,encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_data[encInfo -> size_secret_file];
    fseek(encInfo -> fptr_secret,0,SEEK_SET);
    fread(secret_data, encInfo -> size_secret_file ,1, encInfo -> fptr_secret);
    char str[8];
    for (int i=0; i < strlen(secret_data); i++)
    {	
	fread(str,8,1,encInfo -> fptr_src_image);
	encode_byte_to_lsb(secret_data[i], str);
	fwrite(str,8,1,encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char ch[1];
    while (fread(ch,1,1,fptr_src_image) >= 1)
    {
	fwrite(ch,1,1,fptr_stego_image);	
    }
    return e_success;
}
