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
    IVC* tp[10];
    IVC* image[30];
    IVC* dilateImages[10];
    IVC* blobsArray[10];
    OVC* blobSegmentation;
    OVC* arrayBlobs[10] = { 0 };
    int nblobs;
    OVC* blobs;
    int nBlobsSegmentation;
    int iteradorSegmentador = 0;

    tp[0] = vc_read_image("img_tp/resis1.ppm");
    tp[1] = vc_read_image("img_tp/resis2.ppm");
    tp[2] = vc_read_image("img_tp/resis3.ppm");
    tp[3] = vc_read_image("img_tp/resis4.ppm");
    tp[4] = vc_read_image("img_tp/resis5.ppm");
    tp[5] = vc_read_image("img_tp/resis6.ppm");
    tp[6] = vc_read_image("img_tp/resis7.ppm");
    tp[7] = vc_read_image("img_tp/resis8.ppm");
    tp[8] = vc_read_image("img_tp/resis9.ppm");
    tp[9] = vc_read_image("img_tp/resis10.ppm");
    
    /* Teste de valores HSV em todas as resistencias

    // Segmentação da resis1 
    image[0] = vc_image_new(tp[0]->width, tp[0]->height, tp[0]->channels, tp[0]->levels);
    vc_rgb_to_hsv(tp[0], image[0]);
    image[1] = vc_image_new(tp[0]->width, tp[0]->height, 1, tp[0]->levels);
    vc_hsv_segmentation(image[0], image[1], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis1HSV.pgm", image[1]);

    // Segmentação resis2
    image[2] = vc_image_new(tp[1]->width, tp[1]->height, tp[1]->channels, tp[1]->levels);
    vc_rgb_to_hsv(tp[1], image[2]);
    image[3] = vc_image_new(tp[1]->width, tp[1]->height, 1, tp[1]->levels);
    vc_hsv_segmentation(image[2], image[3], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis2HSV.pgm", image[3]);

    // Segmentação resis3 
    image[4] = vc_image_new(tp[2]->width, tp[2]->height, tp[2]->channels, tp[2]->levels);
    vc_rgb_to_hsv(tp[2], image[4]);
    join = vc_image_new(tp[2]->width, tp[2]->height, 1, tp[2]->levels);
    vc_hsv_segmentation(image[4], join, 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis3HSV.pgm", join);

    // Segmentação resis4 
    image[6] = vc_image_new(tp[3]->width, tp[3]->height, tp[3]->channels, tp[3]->levels);
    vc_rgb_to_hsv(tp[3], image[6]);
    image[7] = vc_image_new(tp[3]->width, tp[3]->height, 1, tp[3]->levels);
    vc_hsv_segmentation(image[6], image[7], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis4HSV.pgm", image[7]);

    // Segmentação da resis5 
    image[8] = vc_image_new(tp[4]->width, tp[4]->height, tp[4]->channels, tp[4]->levels);
    vc_rgb_to_hsv(tp[4], image[8]);
    image[9] = vc_image_new(tp[4]->width, tp[4]->height, 1, tp[4]->levels);
    vc_hsv_segmentation(image[8], image[9], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis5HSV.pgm", image[9]);

    // Segmentação resis6 
    image[10] = vc_image_new(tp[5]->width, tp[5]->height, tp[5]->channels, tp[5]->levels);
    vc_rgb_to_hsv(tp[5], image[10]);
    image[11] = vc_image_new(tp[5]->width, tp[5]->height, 1, tp[5]->levels);
    vc_hsv_segmentation(image[10], image[11], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis6HSV.pgm", image[11]);

    // Segmentação resis7
    image[12] = vc_image_new(tp[6]->width, tp[6]->height, tp[6]->channels, tp[6]->levels);
    vc_rgb_to_hsv(tp[6], image[12]);
    image[13] = vc_image_new(tp[6]->width, tp[6]->height, 1, tp[6]->levels);
    vc_hsv_segmentation(image[12], image[13], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis7HSV.pgm", image[13]);

    // Segmentação resis8 
    image[14] = vc_image_new(tp[7]->width, tp[7]->height, tp[7]->channels, tp[7]->levels);
    vc_rgb_to_hsv(tp[7], image[14]);
    image[15] = vc_image_new(tp[7]->width, tp[7]->height, 1, tp[7]->levels);
    vc_hsv_segmentation(image[14], image[15], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis8HSV.pgm", image[15]);

    // Segmentação resis9 
    image[16] = vc_image_new(tp[8]->width, tp[8]->height, tp[8]->channels, tp[8]->levels);
    vc_rgb_to_hsv(tp[8], image[16]);
    image[17] = vc_image_new(tp[8]->width, tp[8]->height, 1, tp[8]->levels);
    vc_hsv_segmentation(image[16], image[17], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis9HSV.pgm", image[17]);

    // Segmentação resis10 
    image[18] = vc_image_new(tp[9]->width, tp[9]->height, tp[9]->channels, tp[9]->levels);
    vc_rgb_to_hsv(tp[9], image[14]);
    image[19] = vc_image_new(tp[9]->width, tp[9]->height, 1, tp[9]->levels);
    vc_hsv_segmentation(image[18], image[19], 30, 45, 45, 65, 50, 90);
    vc_write_image("resis_test_tp/resis10HSV.pgm", image[19]);
    
    */

    IVC *join = vc_image_new(tp[0]->width, tp[0]->height, 1, tp[0]->levels);

    image[0] = vc_image_new(tp[0]->width, tp[0]->height, tp[0]->channels, tp[0]->levels);
    vc_rgb_to_hsv(tp[0], image[0]);
    image[1] = vc_image_new(tp[0]->width, tp[0]->height, 1, tp[0]->levels);
    vc_hsv_segmentation(image[0], image[1], 30, 45, 45, 65, 50, 90);
    dilateImages[0] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);   
    vc_binary_dilate(image[6], dilateImages[0], 9);
    vc_join_images(dilateImages[0], join);
    
    //nBlobsSegmentation = 0;
    blobsArray[2] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    blobSegmentation = vc_binary_blob_labelling(dilateImages[0], blobsArray[2], &nBlobsSegmentation);
    if (blobSegmentation != NULL) 
    {
        vc_binary_blob_info(blobsArray[2], blobSegmentation, nBlobsSegmentation);
        //printf("\nNumber of labels(Madeira): %d\n", nBlobsSegmentation);
        if (blobSegmentation[0].area >= 550 && blobSegmentation[0].area <= 1250) {
            strcpy(blobSegmentation[0].cor, "Madeira");
        } else return 0;
        arrayBlobs[iteradorSegmentador] = malloc(sizeof(OVC));
        memcpy(arrayBlobs[iteradorSegmentador], blobSegmentation, sizeof(OVC));
        iteradorSegmentador++;
    }else return 0;
    //printf("Area 1:%d \nArea 2:%d \nArea 3: %d", blobSegmentation[0].area, blobSegmentation[1].area, blobSegmentation[2].area);
    vc_write_image("test_tp/hsvResis.pgm", dilateImages[0]);


    blobsArray[3] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    nblobs = 0;
    blobs = vc_binary_blob_labelling(dilateImages[0], blobsArray[3], &nblobs);

    if (blobs != NULL) {
        vc_binary_blob_info(blobsArray[3], blobs, nblobs);
        blobsArray[4] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);

        for (int i = 0; i < nblobs; i++) {
            OVC blobAtual = blobs[i];
            vc_draw_bounding_box(dilateImages[0], blobsArray[4], &blobAtual, i);    
        }

        vc_write_image("test_tp/BOX_RESIS.pgm", blobsArray[4]);
    }


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
    vc_join_images(dilateImages[0], join);

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
    vc_join_images(dilateImages[0], join);

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
    vc_join_images(dilateImages[0], join);
    vc_write_image("test_tp/coresJuntas.pgm", join);


    /* Junção das segmentações */
    
    blobsArray[0] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
    nblobs = 0;
    blobs = vc_binary_blob_labelling(join, blobsArray[0], &nblobs);

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

    /* Segmentação cor castanho

	join = vc_read_image("img_tp/resis3.ppm");
	image[6] = vc_image_new(join->width, join->height, join->channels, join->levels);
	vc_rgb_to_hsv(join, image[6]);
	image[7] = vc_image_new(join->width, join->height, 1, join->levels);
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

    /* Libertar a memória alocada às imagens */
    vc_image_free(image[0]);
    vc_image_free(image[1]);
    vc_image_free(image[2]);
    vc_image_free(image[3]);
    vc_image_free(image[4]);
    vc_image_free(join);
    vc_image_free(image[6]);
    vc_image_free(image[7]);
    vc_image_free(image[8]);
    vc_image_free(image[9]);
    vc_image_free(image[10]);
    vc_image_free(image[11]);
    vc_image_free(image[12]);
    vc_image_free(image[13]);
    vc_image_free(image[14]);
    vc_image_free(image[15]);
    vc_image_free(image[16]);
    vc_image_free(image[17]);
    vc_image_free(image[18]);
    vc_image_free(image[19]);
    vc_image_free(tp[0]);
    vc_image_free(tp[1]);
    vc_image_free(tp[2]);
    vc_image_free(tp[3]);
    vc_image_free(tp[4]);
    vc_image_free(tp[5]);
    vc_image_free(tp[6]);
    vc_image_free(tp[7]);
    vc_image_free(tp[8]);
    vc_image_free(tp[9]);

    //system("cmd /c start FilterGear resis_test_tp/resisHSV.pgm");
    //system("FilterGear test_tp/BOX_RESIS.pgm");


    printf("Press any key to exit...\n");
    return 0;
}


int main(void)
{
    TP();
}

