#include <stdio.h>
#include <stdlib.h> 
#include "vc.h"

/// @brief Open an image, make changes, and save to a new file.
/// @return new image
int openModifyAndSaveImage()
{
    IVC * image;
    int i;

    image = vc_read_image("Images/FLIR/flir-01.pgm");
    if(image == NULL)
    {
        printf("ERROR -> vc_read_image():\n\tFile not found!\n");
        getchar();
        return 0;
    }

    for (i = 0; i < image->bytesperline*image->height; i+=image->channels)
    {
        image->data[i] = 255 - image->data[i];
    }
//teste
    vc_write_image("output/vc-0001.pgm", image);
    vc_image_free(image);

    return 0;
}

/// @brief Create a binary image and save to file.
/// @return image created
int createAndSaveBinaryImage()
{
    IVC * image;
    int x, y;
    long int pos;

    image = vc_image_new(320, 280, 1, 1);
    if(image == NULL)
    {
        printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }

    for (x = 0; x < image->width; x++)
    {
        for (y = 0; y < image->height; y++)
        {
            pos = y * image->bytesperline + x * image->channels;

            if((x <= image->width/2) && (y <= image->height/2)) image->data[pos] = 1;
            else if((x > image->width/2) && (y > image->height/2)) image->data[pos] = 1;
            else image->data[pos] = 0;
        }
    }

    vc_write_image("output/teste.pbm", image);
    vc_image_free(image);

    return 0;
}

/// @brief Read a binary image, make changes, and save to a new file.
/// @return new binary image
int readModifyAndSaveBinaryImage()
{
    IVC * image;
    int i;

    image = vc_read_image("output/teste.pbm");
    if(image == NULL)
    {
        printf("ERROR -> vc_read_image():\n\tFile not Found!\n");
        getchar();
        return 0;
    }

    for(i=0; i< image->bytesperline*image->height; i+=image->channels)
    {
        if(image->data[i] == 1) image->data[i] = 0;
        else image->data[i] = 1;
    }

    vc_write_image("output/teste2.pbm", image);
    vc_image_free(image);

    return 0;
}

/// @brief Create an image (256x256) in grayscale with a horizontal gradient, and save to file.
/// @return horizontal gradient image
int createHorizontalGradientImage()
{
    IVC *image;
	int x, y;
	long int pos;

	image = vc_image_new(256, 256, 1, 255);
	if (image == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}

	for (x = 0; x<image->width; x++)
	{
		for (y = 0; y<image->height; y++)
		{
			pos = y * image->bytesperline + x * image->channels;

			image->data[pos] = (unsigned char) x;
		}
	}

	vc_write_image("output/horizontal.pgm", image);
	vc_image_free(image);

    return 0;

}

/// @brief Create an image (256x256) in grayscale with a vertical gradient, and save to file.
/// @return vertical gradient image
int createVerticalGradientImage()
{
    IVC* image;
	int x, y;
	long int pos;

	image = vc_image_new(256, 256, 1, 255);
	if (image == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}

	for (x = 0; x<image->width; x++)
	{
		for (y = 0; y<image->height; y++)
		{
			pos = y * image->bytesperline + x * image->channels;

			image->data[pos] = (unsigned char) y;
		}
	}

	vc_write_image("output/vertical.pgm", image);
	vc_image_free(image);

    return 0;
}

/// @brief Create an image (256x256) in grayscale with a diagonal gradient, and save to file.
/// @return diagonal gradient image
int createDiagonalGradientImage()
{
    IVC *image;
	int x, y;
	long int pos;

	image = vc_image_new(256, 256, 1, 255);
	if (image == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}

	for (x = 0; x<image->width; x++)
	{
		for (y = 0; y<image->height; y++)
		{
			pos = y * image->bytesperline + x * image->channels;
			image->data[pos] = (unsigned char) ((x + y) / 2);
		}
	}

	vc_write_image("output/diagonal.pgm", image);
	vc_image_free(image);

    return 0;
}

/// @brief Read an image define blobs and labelling and then paint each blob with diff colors
/// @return labelled image with each blob paint with diferent colors
int labellingImagePainEachBlob()
{
    IVC *image[2];
	int i;

	//image[0] = vc_read_image("Images/Labelling/labelling-1.pgm");
	image[0] = vc_read_image("Images/Labelling/labelling-2.pgm");
	if (image[0] == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		getchar();
		return 0;
	}

	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, 255);
	if (image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		getchar();
		return 0;
	}

	int nblobs;
	OVC *blobs;
	blobs = vc_binary_blob_labelling(image[0], image[1], &nblobs);
    unsigned char *data = (unsigned char *)image[1]->data;
    int width = image[0]->width;
	int height = image[0]->height;
    int bytesperline = image[0]->bytesperline;
    int channels = image[0]->channels;
    int pos;

    for (int i = 0; i < height * width; i++)
    {
        pos = i * channels;
        int label = data[i]; // Get the label of the current pixel
        if (label > 0) {
            // Loop through the blobs to find the corresponding label
            for (int j = 0; j < nblobs; j++) {
                if (blobs[j].label == label) {
                // Assign different colors based on the label
                    if (label == 1) data[pos] = 155;
                    else if (label == 2) data[pos] = 200;
                    else if (label == 3) data[pos] = 255;
                    else if (label == 4) data[pos] = 100;
                    // Add more conditions as needed for additional labels
                    break; // Exit the loop once the corresponding label is found
                }
            }
        }   
    }   

	if (blobs != NULL)
	{
		printf("\nNumber of labels: %d\n", nblobs);
		for (i = 0; i<nblobs; i++)
		{
			printf("-> Label %d\n", blobs[i].label);
		}

		free(blobs);
	}

	vc_write_image("output/labelling2.pgm", image[1]);

	vc_image_free(image[0]);
	vc_image_free(image[1]);

	return 0;
}

int main(void)
{
    IVC * image;
	IVC * image2;
	IVC * image3;
	IVC * image4;
	IVC * image5;
	
	//Está comentado pq não se usa
	//int i;

	image = vc_read_image("img_tp/resis1.ppm");
	//Os canais estão como "1" pq são imagens grayscale
	image2 = vc_image_new(image->width, image->height, 3, image->levels);
	image3 = vc_image_new(image->width, image->height, 3, image->levels);
	//image4 = vc_image_new(image->width, image->height, 1, image->levels);
	//image5 = vc_image_new(image->width, image->height, 1, image->levels);

	if (image == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		getchar();
		return 0;
	}
/*
	//Serve para imagens binárias
	vc_rgb_to_gray(image, image2);

	vc_gray_to_binary(image2, image3, 165);//Quando se usa a outra função de segmentação fica mal ent mudei para esta e acho que este threshold é o melhor mas dps ve

	vc_binary_close(image3, image4, 9, 9);

	lateraisBinary(image4, image5);
*/
	//Serve para imagens HSV (AINDA NÃO FUNCIONA)
	vc_rgb_to_hsv(image,image2);

	vc_hsv_segmentation(image2, image3, 36, 44, 55, 100, 70, 90);
	


	//Criar as imagens depois de realizadas as devidas alterações;
	vc_write_image("image2.ppm", image2);
    vc_write_image("TESTE.ppm", image3);
    //vc_write_image("Erode.ppm", image4);
   	//vc_write_image("Laterais.ppm", image5);



	//Libertar a memória alocada às imagens;
    vc_image_free(image);
    vc_image_free(image2);
    vc_image_free(image3);
    //vc_image_free(image4);
    //vc_image_free(image5);


	//Abrir as imagens produzidas de maneira a procurar por erros;
    printf("Press any key to exit...\n");
    system("cmd /c start FilterGear img_tp/resis1.ppm");
	system("FilterGear image2.ppm");
    system("FilterGear TESTE.ppm");
	//system("cmd /c start FilterGear Erode.ppm");
    //system("FilterGear Laterais.ppm");
    getchar();

    return 0;
}