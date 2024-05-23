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
    IVC * image[10];
	
	/*  Segmentação cor vermelha */
	image[0] = vc_read_image("img_tp/resis1.ppm");
	image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels); // imagens hsv devem ter todas as propriedades iguais á imagem original
	vc_rgb_to_hsv(image[0], image[1]);
	image[2] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels); //imagens das cores segmentadas devem ser pgm -> 1 channel
	vc_hsv_segmentation(image[1], image[2], 6, 16, 50, 72, 60, 80); // valores com uma margem (entre 2/5)
	
	/* Melhoramento de imagem

	Podes ir rever a matéria relativa a isto e experimentar aplicar alguns dos filtros de melhoramento da imagem.
	Ex: binary_close, binary_dilate, etc etc 

	*/
	// codigo aqui e quando conseguires melhorar podes descomentar o que está em cima e deixar apenas titulo 

	/* Criar blob e labelling 
	Isto tenho de ver melhor mas se tiveres tempo podes tentar fazer isto
	No projeto que te mandei ("TP/main.c") tens lá como o gajo fez, podes tentar recriar que tb ajuda a perceber  

	*/
	// codigo aqui e quando conseguires melhorar podes descomentar o que está em cima e deixar apenas titulo 

	/* Segmentação cor azul

	image[3] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	vc_hsv_segmentation(image[1], image[3], 115, 200, 10, 43, 35, 48);

	terminando as cenas para a cor vermelha, replicar para cor azul 
	*/
	
	/* Segmentação cor verde 

	image[4] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	vc_hsv_segmentation(image[1], image[4], 65, 115, 25, 50, 35, 60);

	terminando as cenas para a cor vermelha, replicar para cor verde 
	*/

	/* Segmentação cor castanho
	image[5] = vc_read_image("img_tp/resis3.ppm");
	image[6] = vc_image_new(image[5]->width, image[5]->height, image[5]->channels, image[5]->levels);
	vc_rgb_to_hsv(image[5], image[6]);
	image[7] = vc_image_new(image[5]->width, image[5]->height, 1, image[5]->levels);
	vc_hsv_segmentation(image[6], image[7], 20, 38, 23, 51, 31, 50);
	*/

	/* Segmentação cor verde escuro
	image[8] = vc_read_image("img_tp/resis4.ppm");
	image[9] = vc_image_new(image[8]->width, image[8]->height, image[8]->channels, image[8]->levels);
	vc_rgb_to_hsv(image[8], image[9]);
	image[10] = vc_image_new(image[8]->width, image[8]->height, 1, image[8]->levels);
	vc_hsv_segmentation(image[9], image[10], 30, 100, 3, 35, 22, 27);
	*/

	//Criar as imagens depois de realizadas as devidas alterações;
	vc_write_image("test_tp/hsv.pgm", image[1]);
    vc_write_image("test_tp/red.pgm", image[2]);
	vc_write_image("test_tp/blue.pgm", image[3]);
	vc_write_image("test_tp/green.pgm", image[4]);

	/* Escrita da hsv da resistencia 2 e segmentação do castanho
	vc_write_image("test_tp/hsv2.pgm", image[6]);
	vc_write_image("test_tp/brown.pgm", image[7]);
	*/

	/* Escrita da hsv da resistencia 3 e segmentação do verde escuro
	vc_write_image("test_tp/hsv3.pgm", image[9]);
	vc_write_image("test_tp/darkgreen.pgm", image[10]);
	*/

	//Libertar a memória alocada às imagens;
    vc_image_free(image[0]);
    vc_image_free(image[1]);
    vc_image_free(image[2]);
	vc_image_free(image[3]);
	vc_image_free(image[4]);

	/* Descomentar para testar segmentação de castanho e verde escuro
	vc_image_free(image[5]);
	vc_image_free(image[6]);
	vc_image_free(image[7]);
	vc_image_free(image[8]);
	vc_image_free(image[9]);
	vc_image_free(image[10]);
	*/
	


	//Abrir as imagens produzidas de maneira a procurar por erros;
    printf("Press any key to exit...\n");

    system("cmd /c start FilterGear img_tp/resis1.ppm");
	system("FilterGear test_tp/hsv.pgm"); // é preciso verificar se realmente é pgm as imagens hsv
    system("FilterGear test_tp/red.pgm");
	system("FilterGear test_tp/blue.pgm");
	system("FilterGear test_tp/green.pgm");

	/* Testes de segmentação do castanho
	system("cmd /c start FilterGear img_tp/resis3.ppm");
	system("FilterGear test_tp/hsv2.pgm");
	system("FilterGear test_tp/brown.pgm");
	*/
	
	/* Testes de segmentação do verde escuro
	system("cmd /c start FilterGear img_tp/resis4.ppm");
	system("FilterGear test_tp/hsv3.pgm");
	system("FilterGear test_tp/darkgreen.pgm");
	*/


	// Comandos que estou a executar para correr o código:
	// gcc vc.c main.c -o a.exe
	// ./a.exe

    getchar();
    return 0;
}
