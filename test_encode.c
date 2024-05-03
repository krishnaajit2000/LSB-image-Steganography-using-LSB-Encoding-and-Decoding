#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

OperationType check_operation_type(char *argv[])
{
    /*check argv[] is -e or not
      -e --> return e_encode
      check argv
     */
    if (argv[1] != NULL)
    {
	if (strcmp(argv[1],"-e") == 0)
	{
	    return e_encode;
	}
	else if (strcmp(argv[1],"-d") == 0)
	{
	    return e_decode;
	}
	else
	{
	    return e_unsupported;
	}
    }
    else
    {
	return e_unsupported;
    }
}

int main( int argc , char *argv[])
{
    EncodeInfo encInfo;    //structure variable
    DecodeInfo decInfo;
    int ret = check_operation_type(argv);
    if (ret == e_encode)
    {
	if (argc >= 4)
	{
	    if (read_and_validate_encode_args(argv, &encInfo) == e_success)
	    {
		if ( do_encoding(&encInfo) == e_success)
		{
		    printf("INFO: ## Encoding Done Successfully\n");
		}
		else
		{
		    printf("ERROR: Fail to Done Encoding\n");
		}
	    }
	    else
	    {
		printf("ERROR: Failed to read and validate encoding\n");
	    }
	}
	else
	{
	    printf("ERROR: Please pass atleast 4 5 command line arguments\n");
	}
    }
    else if (ret == e_decode)
    {
	if (argc >= 3)
	{
	    if ((read_and_validate_decode_args(argv,&decInfo)) == e_success)
	    {
		if ( do_decoding(&decInfo) == e_success )
		{
		    printf("INFO: ## Decoding Done Successfully ##\n");
		}
		else
		{
		    printf("ERROR:Failed to Done Decoding\n");
		}
	    }
	    else
	    {
		printf("ERROR: Failed to read and validate decoding\n");
	    }
	}
	else
	{
	    printf("ERROR: Please pass atleast 3 4 command line argument\n");

	}
    }
    else
    {
	printf("ERROR: Please pass arg like\nencoding : ./a.out -e beautiful.bmp secret.txt output.bmp ,\ndecoding : ./a.out -d output.bmp secret\n");
    }
}


