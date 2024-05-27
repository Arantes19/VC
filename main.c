#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
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

int TP()
{
    IVC* image[30];
    IVC* dilateImages[10];
    IVC* blobsArray[10];
    OVC* blobSegmentation;
    OVC* arrayBlobs[10] = { 0 };
    int nblobs;
    OVC* blobs;
    int nBlobsSegmentation;
    int iteradorSegmentador = 0;

    image[0] = vc_read_image("img_tp/resis1.ppm");

    image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
    vc_rgb_to_hsv(image[0], image[1]);

    image[5] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);

    /* Segmentação da resis1 */
    nBlobsSegmentation = 0;
    image[6] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    vc_hsv_segmentation(image[1], image[6], 30, 45, 45, 65, 50, 90);

    dilateImages[0] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);   
    vc_binary_close(image[6], dilateImages[0], 9, 9);

    vc_join_images(dilateImages[0], image[5]);

    blobsArray[2] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    blobSegmentation = vc_binary_blob_labelling(dilateImages[0], blobsArray[2], &nBlobsSegmentation);
    if (blobSegmentation != NULL) 
    {
        vc_binary_blob_info(blobsArray[2], blobSegmentation, nBlobsSegmentation);
        printf("\nNumber of labels(Madeira): %d\n", nBlobsSegmentation);
        if (blobSegmentation[0].area >= 550 && blobSegmentation[0].area <= 1250){
            strcpy(blobSegmentation[0].cor, "Madeira");
        }else return 0;
        arrayBlobs[iteradorSegmentador] = malloc(sizeof(OVC));
        memcpy(arrayBlobs[iteradorSegmentador], blobSegmentation, sizeof(OVC));
        iteradorSegmentador++;
    }else return 0;
    printf("Area 1:%d \nArea 2:%d \nArea 3:%dx  ", blobSegmentation[0].area, blobSegmentation[1].area, blobSegmentation[2].area);
    vc_write_image("test/hsvRESIS.pgm", dilateImages[0]);


    blobsArray[3] = vc_image_new(image[0]->width, image[0]->height, 1, 255);
    nblobs = 0;
    blobs = vc_binary_blob_labelling(dilateImages[0], blobsArray[3], &nblobs);

    if (blobs != NULL) {
        vc_binary_blob_info(blobsArray[3], blobs, nblobs);
        blobsArray[4] = vc_image_new(image[0]->width, image[0]->height, 1, 255);

        for (int i = 0; i < nblobs; i++) {
            OVC blobAtual = blobs[i];
            vc_draw_bounding_box(dilateImages[0], blobsArray[4], &blobAtual, i);    
        }

        vc_write_image("test_tp/BOX_RESIS.pgm", blobsArray[4]);
    }



    /*
    blobsArray[1] = vc_image_new(image[0]->width, image[0]->height, 1, 255);
        for (int i = 0; i < nblobs; i++) {
            OVC blobAtual = blobs[i];
            vc_draw_bounding_box(blobsArray[0], blobsArray[1], &blobAtual, i);    
        }
    */
    /* Segmentação cor verde */

    image[4] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    vc_hsv_segmentation(image[1], image[4], 67, 110, 25, 50, 37, 50);
    vc_binary_close(image[4], dilateImages[0], 9, 9);
    blobsArray[1] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    nBlobsSegmentation = 0;
    blobSegmentation = vc_binary_blob_labelling(dilateImages[0], blobsArray[1], &nBlobsSegmentation);
    if (blobSegmentation != NULL) 
    {
        vc_binary_blob_info(blobsArray[1], blobSegmentation, nBlobsSegmentation);
        printf("\nNumber of labels(Verde): %d\n", nBlobsSegmentation);
        if (blobSegmentation[0].area >= 550 && blobSegmentation[0].area <= 1250)
            strcpy(blobSegmentation[0].cor, "Verde");
        arrayBlobs[iteradorSegmentador] = malloc(sizeof(OVC));
        memcpy(arrayBlobs[iteradorSegmentador], blobSegmentation, sizeof(OVC));
        iteradorSegmentador++;
    }
    printf("Area 1:%d \n", blobSegmentation[0].area);
    vc_write_image("test_tp/hsvVerde0.pgm", dilateImages[0]);
    vc_join_images(dilateImages[0], image[5]);

    /* Segmentação cor azul */

    nBlobsSegmentation = 0;
    image[3] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    vc_hsv_segmentation(image[1], image[3], 115, 200, 10, 43, 35, 48);
    vc_binary_close(image[3], dilateImages[0], 9, 9);
    blobSegmentation = vc_binary_blob_labelling(dilateImages[0], blobsArray[1], &nBlobsSegmentation);
    if (blobSegmentation != NULL) 
    {
        vc_binary_blob_info(blobsArray[1], blobSegmentation, nBlobsSegmentation);
        printf("\nNumber of labels(Azul): %d\n", nBlobsSegmentation);
        if (blobSegmentation[0].area >= 500 && blobSegmentation[0].area <= 1250)
            strcpy(blobSegmentation[0].cor, "Azul");
        arrayBlobs[iteradorSegmentador] = malloc(sizeof(OVC));
        memcpy(arrayBlobs[iteradorSegmentador], blobSegmentation, sizeof(OVC));
        iteradorSegmentador++;
    }
    printf("Area 1:%d\n", blobSegmentation[0].area);
    vc_write_image("test_tp/hsvAzul.pgm", dilateImages[0]);
    vc_join_images(dilateImages[0], image[5]);


    /* Segmentação do vermelho */

    nBlobsSegmentation = 0;
    image[2] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    vc_hsv_segmentation(image[1], image[2], 6, 16, 50, 72, 60, 80);
    vc_binary_close(image[2], dilateImages[0], 9, 9);
    blobSegmentation = vc_binary_blob_labelling(dilateImages[0], blobsArray[1], &nBlobsSegmentation);
    if (blobSegmentation != NULL) {
        vc_binary_blob_info(blobsArray[1], blobSegmentation, nBlobsSegmentation);
        printf("\nNumber of labels(Vermelho): %d\n", nBlobsSegmentation);
        if (blobSegmentation[0].area >= 550 && blobSegmentation[0].area <= 1250)
            strcpy(blobSegmentation[0].cor, "Vermelho");
        arrayBlobs[iteradorSegmentador] = malloc(sizeof(OVC));
        memcpy(arrayBlobs[iteradorSegmentador], blobSegmentation, sizeof(OVC));
        iteradorSegmentador++;
    } 
    printf("Area 1:%d\n", blobSegmentation[0].area);
    vc_write_image("test_tp/hsvred.pgm", dilateImages[0]);
    vc_join_images(dilateImages[0], image[5]);
    vc_write_image("test_tp/coresJuntas.pgm", image[5]);


    /* Segmentação cor castanho

	image[5] = vc_read_image("img_tp/resis3.ppm");
	image[6] = vc_image_new(image[5]->width, image[5]->height, image[5]->channels, image[5]->levels);
	vc_rgb_to_hsv(image[5], image[6]);
	image[7] = vc_image_new(image[5]->width, image[5]->height, 1, image[5]->levels);
	vc_hsv_segmentation(image[6], image[7], 20, 38, 23, 51, 31, 50);
	vc_write_image("test_tp/hsvcastanho.pgm", image[7]);
    */

	/* Segmentação cor preta

	image[8] = vc_read_image("img_tp/resis4.ppm");
	image[9] = vc_image_new(image[8]->width, image[8]->height, image[8]->channels, image[8]->levels);
	vc_rgb_to_hsv(image[8], image[9]);
	image[10] = vc_image_new(image[8]->width, image[8]->height, 1, image[8]->levels);
	vc_hsv_segmentation(image[9], image[10], 30, 100, 3, 35, 22, 27);
	vc_write_image("test_tp/hsvpreto.pgm", image[10]);
    */

    /* Segmentação cor laranja

    image[11] = vc_read_image("img_tp/resis7.ppm");
	image[12] = vc_image_new(image[11]->width, image[11]->height, image[11]->channels, image[11]->levels);
	vc_rgb_to_hsv(image[11], image[12]);
	image[13] = vc_image_new(image[11]->width, image[11]->height, 1, image[11]->levels);
	vc_hsv_segmentation(image[12], image[13], 10, 24, 70, 83, 80, 100);
    vc_write_image("test_tp/hsvlaranja.pgm", image[13]);
    */

    /* Junção das segmentações */
    
    blobsArray[0] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    nblobs = 0;
    blobs = vc_binary_blob_labelling(image[5], blobsArray[0], &nblobs);

    if (blobs != NULL) {
        int isLastDigit = 0, indexTotalOhmRes = 0, totalResist = 0, multiplicador = 0;
        int totalOhmRes[10];
        vc_binary_blob_info(blobsArray[0], blobs, nblobs);
        printf("\nNumber of labels: %d\n", nblobs);
        for (int i = 0; i < nblobs; i++) {
            if (i + 1 >= nblobs)
                isLastDigit = 1;
            strncpy(blobs[i].cor, arrayBlobs[i]->cor, sizeof(char[10]));
            printf("-> Label %d\n", blobs[i].label);
            printf("-> Number pixels area: %d\n", blobs[i].area);
            printf("-> Number pixels perimeter: %d\n", blobs[i].perimeter);
            printf("-> Label color: %s\n", blobs[i].cor);
            int novoValorFaixa = vc_table_resistors_value(blobs[i].cor);
            totalOhmRes[indexTotalOhmRes++] = novoValorFaixa;
            
            if (isLastDigit)
                multiplicador = vc_table_resistors_multiplier(blobs[i].cor);
        }
        for (int i = 0; i < indexTotalOhmRes-1; i++)
            totalResist = totalResist * 10 + totalOhmRes[i];
        totalResist = totalResist * multiplicador;
        printf("Valor resistencia: %d\n", totalResist);

        blobsArray[1] = vc_image_new(image[0]->width, image[0]->height, 1, 255);
        for (int i = 0; i < nblobs; i++) {
            OVC blobAtual = blobs[i];
            vc_draw_bounding_box(blobsArray[0], blobsArray[1], &blobAtual, i);    
        }
        vc_write_image("test_tp/teste.ppm", image[0]);
        vc_write_image("test_tp/drawingBox.pgm", blobsArray[1]);
    }
    

    /* Libertar a memória alocada às imagens */
    vc_image_free(image[0]);
    vc_image_free(image[1]);
    vc_image_free(image[2]);
    vc_image_free(image[3]);
    vc_image_free(image[4]);
    vc_image_free(image[5]);
    vc_image_free(image[6]);
    vc_image_free(image[7]);
    vc_image_free(image[8]);
    vc_image_free(image[9]);
    vc_image_free(image[10]);
    vc_image_free(dilateImages[0]);
    vc_image_free(blobsArray[1]);
    vc_image_free(blobsArray[0]);
    vc_image_free(blobsArray[2]);
    vc_image_free(blobsArray[3]);
    vc_image_free(blobsArray[4]);
    vc_image_free(blobsArray[5]);


    //vc_image_free(image[7]);
    //vc_image_free(image[8]);
    //vc_image_free(image[9]);
    //vc_image_free(image[10]);
    //vc_image_free(image[11]);
    //vc_image_free(image[12]);
    //vc_image_free(image[13]);

    //system("cmd /c start FilterGear test_tp/hsvVerde.pgm");
    //system("cmd /c start FilterGear test_tp/hsvAzul.pgm");
    system("cmd /c start FilterGear test_tp/hsvRESIS.pgm");
    //system("cmd /c start FilterGear test_tp/coresJuntas.pgm");
    //system("cmd /c start FilterGear test_tp/drawingBox.pgm");
    system("cmd /c start FilterGear test_tp/BOX_RESIS.pgm");
    //system("cmd /c start FilterGear test_tp/hsvlaranja.pgm");

    printf("Press any key to exit...\n");

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

    return 0;
}


int main(void)
{
    TP();
}

