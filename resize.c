/*
* Yehor Valentiukevych
*
* Copies a BMP piece by piece, just because.
* 
* check50 =>> https://sandbox.cs50.net/checks/441cad012d16401aae4ae9c3cd13dc30
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // обеспечить правильное использование
    if (argc != 4)
    {
        printf("Usage: ./copy infile outfile\n");
        return 1;
    }

    // разобрать float arg, как GetFloat в cs50.h
    float factor;
    char c;
    if(sscanf(argv[1], "%f %c", &factor, &c) != 1)
    {
        printf("n the resize factor, must be a float\n");
        return 1;
    }
    
    //запомнить filenemes
    char* infile = argv[2];
    char* outfile = argv[3];
    
    // проверить диапазон: N должно быть ( от 0.0 до 100.0)
    if(factor <= 0 || factor > 100)
    {
        printf("The resize factor, must satisfy 0 <  n <= 100.\n");
        return 1;
    }

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 1;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 1;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    // обеспечение потока (likely) 24-bit несжатый BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        printf("Unsupported file format.\n");
        return 1;
    }
    
    // Ширина оригинала на будущее
    int oldWidth = bi.biWidth;
    // определить отступы для scanlines original
    int oldPadding = (4 - (oldWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // обновить BITMAPINFIHEADER с размерами после изменения размера
     bi.biWidth *= factor;
     bi.biHeight *= factor;
     
     // определить заполнение строки для изменения размера изображения
     int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
     
     // update BITMAPFILEHEADER with new image size
     bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding) * abs(bi.biHeight);
     
    // update BITMAPFILEHEADER with new bitmap file size
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // writwe outfile's 
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // currRow stores one of the rows in the input picture
    //магазины одной из строк во входном изображении
    // rowNum в каком ряду хранится.
    //
    RGBTRIPLE currRow[oldWidth];
    
    int rowNum = -1;
    
    // перебрать каждый пиксель в выходной_файл, найти и написать новый цвет
    for(int y = 0; y < abs(bi.biHeight); y++)
    {
        // Ближайших соседей. Все пиксели одного
        //outfile строке будут извлечены из пикселей одной строки
        //в infile. Узнайть, какaя строка, которой должна быть.
        
        //Nearest neighbors. All of the pixels by one
        // outfile row will draw from the pixels of one row
        // in the infile. Find out what row that should be.
        int oldY = y / factor;
        
        // if we don't already have the correct row loaded, than seek to the 
        // desired row and read if in to our buffer
        //если уже есть правильные строки загруp, чем искать нужные строки и читать,
        // в наш буфер
        if(rowNum != oldY)
        {
            //we want to skip over the headers and oldY rows.
            //has oldWidth pixels and oldPadding bytes of padding.
            //мы хотим, чтобы пропустить заголовки и старовато строк
            //имеет oldWidth пикселей и oldPadding байт обивка.
            size_t pos = (sizeof(RGBTRIPLE) * oldWidth + oldPadding) * oldY + 
                        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
            fseek(inptr, pos, SEEK_SET);
            rowNum = oldY;
            fread(&currRow, sizeof(RGBTRIPLE), oldWidth, inptr);
        }
        
        // Рисовать пикселями в этой строке в output.      
        for (int x = 0; x < bi.biWidth; x++)
        {
            RGBTRIPLE triple = currRow[(int) (x / factor)];
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }
        
        // писать отступы для scanline to outfile        
        for (int l = 0; l < padding; l++)
        {
            fputc(0x00, outptr);
        }
                    
    }
            
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
                
                
                
