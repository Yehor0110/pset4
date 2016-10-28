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
    float n = atof(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER
    int padding =  (4 - (int)(bi.biWidth * sizeof(RGBTRIPLE) * n) % 4) % 4;
    int newPadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
     int width = bi.biWidth;
    
     bi.biWidth *= n;
     bi.biHeight *= n;
     
    bf.bfSize = (14 + 40) + ((bi.biWidth * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight);
    
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    int size = bi.biWidth / n * 3;
    float mod[size];
    
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight / n; i++)
    {
        // iterate over pixels in scanline
        for (int m = 0; m < n; m++)
        {
            for (int j = 0; j < bi.biWidth / n; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
                // read RGB triple from infile
                if (m == 0)
                {
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    mod[j * 3] = triple.rgbtBlue;
                    mod[j * 3 + 1] = triple.rgbtGreen;
                    mod[j * 3 + 2] = triple.rgbtRed;
                }
                
                // write RGB triple to outfile
                for (int z = 0; z < n; z++)
                {
                    triple.rgbtBlue = mod[j * 3];
                    triple.rgbtGreen = mod[j * 3 + 1];
                    triple.rgbtRed = mod[j * 3 + 2];
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    
                    if (n < 1)
                    {
                        ++z;
                        fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    }   
                }
                if (n < 1)
                    ++j;
            }
    
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
