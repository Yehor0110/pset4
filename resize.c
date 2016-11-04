/*
* Yehor Valentiukevych
*
* Copies a BMP piece by piece, just because.
* 
* check50 =>> https://sandbox.cs50.net/checks/441cad012d16401aae4ae9c3cd13dc30
*/

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy infile outfile\n");
        return 1;
    }

    // remember filenames
    
            // skip over padding, if any
            if (m == 0)
                fseek(inptr, newPadding, SEEK_CUR);
    
            // then add it back (to demonstrate how)
            for (int k = 0; k < padding; k++)
                fputc(0x00, outptr);

            if (n < 1)
            {
                ++m;
                fseek(inptr, sizeof(RGBTRIPLE) * width + newPadding, SEEK_CUR);
            }
        }
        if (n < 1)
            ++i;
    }
    fclose(inptr);
   fclose(outptr);

  return 0;
}
