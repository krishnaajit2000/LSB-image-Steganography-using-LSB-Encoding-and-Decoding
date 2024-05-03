# LSB-image-Steganography-using-LSB-Encoding-and-Decoding
A highly confidential piece of data, image, or file can be hidden inside another image using a technique called steganography.
In fields where work requires a high degree of confidence, steganography is primarily utilised.
There are various methods to do steganography,from which I selected digital steganographic method.
The project aimed to transmit a confidential text document encrypted within a BMP file image and encode the data into the LSB of the picture bytes after encoding the secret text's length.
The first step in the decoding procedure is to decode the text's length before gradually deciphering each word.
After decoding, the secret text is the output in the end.
Here LSB is chose because, changes made on LSB won't affect much change the image but change in MSB could.
