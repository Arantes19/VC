//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Desabilita (no MSVC++) warnings de fun��es n�o seguras (fopen, sscanf, etc...)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "vc.h"

#define MAX3(r,g,b) (r>g? (r>b ? r:b):(g>b?g:b))
#define MIN3(r,g,b) (r<g?(r<b?r:b):(g<b?g:b))

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUNÇÕES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Alocar memória para uma imagem
IVC *vc_image_new(int width, int height, int channels, int levels)
{
	IVC *image = (IVC *)malloc(sizeof(IVC));

	if (image == NULL)
		return NULL;
	if ((levels <= 0) || (levels > 255))
		return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *)malloc(image->width * image->height * image->channels * sizeof(char));

	if (image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}

// Libertar memória de uma imagem
IVC *vc_image_free(IVC *image)
{
	if (image != NULL)
	{
		if (image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUNÇÕES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

char *netpbm_get_token(FILE *file, char *tok, int len)
{
	char *t;
	int c;

	for (;;)
	{
		while (isspace(c = getc(file)))
			;
		if (c != '#')
			break;
		do
			c = getc(file);
		while ((c != '\n') && (c != EOF));
		if (c == EOF)
			break;
	}

	t = tok;

	if (c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while ((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));

		if (c == '#')
			ungetc(c, file);
	}

	*t = 0;

	return tok;
}

long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}

void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				// datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}

// Leitura da imagem
IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;

	// Abre o ficheiro
	if ((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if (strcmp(tok, "P4") == 0)
		{
			channels = 1;
			levels = 1;
		} // Se PBM (Binary [0,1])
		else if (strcmp(tok, "P5") == 0)
			channels = 1; // Se PGM (Gray [0,MAX(level,255)])
		else if (strcmp(tok, "P6") == 0)
			channels = 3; // Se PPM (RGB [0,MAX(level,255)])
		else
		{
#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
#endif

			fclose(file);
			return NULL;
		}

		if (levels == 1) // PBM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL)
				return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL)
				return 0;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			if ((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca memória para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL)
				return NULL;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			size = image->width * image->height * image->channels;

			if ((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}

		fclose(file);
	}
	else
	{
#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
#endif
	}

	return image;
}

int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;

	if (image == NULL)
		return 0;

	if ((file = fopen(filename, "wb")) != NULL)
	{
		if (image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL)
				return 0;

			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);

			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if (fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);

			if (fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				return 0;
			}
		}

		fclose(file);

		return 1;
	}

	return 0;
}

// Gerar negativo de imagem gray
int vc_gray_negative(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verifica��o de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 1)
		return 0;

	// Inverte a imagem Gray
	for (y = 0; y < width; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos] = 255 - data[pos];
		}
		return 1;
	}
}

// Gerar negativo da imagem RGB
int vc_rgb_negative(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verifica��o de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 3)
		return 0;

	// Inverte a imagem RGB
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < height; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos] = 255 - data[pos];
			data[pos + 1] = 255 - data[pos + 1];
			data[pos + 2] = 255 - data[pos + 2];
		}
	}
	return 1;
}

// Extrair componente Red da imagem RGB para Gray
int vc_rgb_get_red_gray(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verificacao de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 3)
		return 0;

	// Extrai a componente Red
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos + 1] = data[pos]; // Green
			data[pos + 2] = data[pos]; // Blue
		}
	}
	return 1;
}

// Extrair componente Green da imagem RGB para Gray
int vc_rgb_get_green_gray(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verificacao de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 3)
		return 0;

	// Extrai a componente Green
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos] = data[pos + 1];	   // Red
			data[pos + 2] = data[pos + 1]; // Blue
		}
	}
	return 1;
}

// Extrair componente Blue da imagem RGB para Gray
int vc_rgb_get_blue_gray(IVC *srcdst)
{
	unsigned char *data = (unsigned char *)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	// Verificacao de erros
	if ((srcdst->width <= 0) || (srcdst->height <= 0) || (srcdst->data == NULL))
		return 0;
	if (channels != 3)
		return 0;

	// Extrai a componente Blue
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			data[pos] = data[pos + 2];	   // Red
			data[pos + 1] = data[pos + 2]; // Green
		}
	}
	return 1;
}

// Converter de RGB para Gray
int vc_rgb_to_gray(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float rf, gf, bf;

	// Verificao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 3) || (dst->channels != 1))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src; // Calcular indíce, channels_src = 3
			pos_dst = y * bytesperline_dst + x * channels_dst; // channels_dst = 1

			rf = (float)datasrc[pos_src];
			gf = (float)datasrc[pos_src + 1];
			bf = (float)datasrc[pos_src + 2];

			datadst[pos_dst] = (unsigned char)((rf * 0.299) + (gf * 0.587) + (bf * 0.114)); // Armazenar informação
		}
	}
	return 1;
}

// Converter RGB para HSV
int vc_rgb_to_hsv(IVC *src, IVC *dst)
{
	if (src->height < 0 || src->width < 0 || (src->levels < 0 && src->levels>255))
		return 0;
	if (src->channels != 3 || dst->channels != 3)
		return 0;

	int x, y;
	float matiz, sat, valor, max, min, r,g,b;
	long int possrc, posdst;

	for (x = 0; x < src->width; x++)
	{
		for (y = 0; y < src->height; y++)
		{
			possrc = y * src->bytesperline + x * src->channels;
			posdst = y * dst->bytesperline + x * dst->channels;

			max = min = src->data[possrc];

			r = src->data[possrc];
			g = src->data[possrc+1];
			b = src->data[possrc+2];
			max = MAX3(r, g, b);
			min = MIN3(r, g, b);


			valor = max;
			if (valor > 0 && (max - min) > 0)
				sat = 255.0f * (max - min) / valor;
			else
				sat = 0;

			if (sat == 0)
				matiz = 0;
			else
			{
				if ((max == src->data[possrc]) && (src->data[possrc + 1] > src->data[possrc + 2]))
					matiz = 60 * (src->data[possrc + 1] - src->data[possrc + 2]) / (max - min);
				else if (max == src->data[possrc] && src->data[possrc + 2] > src->data[possrc + 1])
					matiz = 360 + 60 * (src->data[possrc + 1] - src->data[possrc + 2]) / (max - min);
				else if (max == src->data[possrc + 1])
					matiz = 120 + 60 * (src->data[possrc + 2] - src->data[possrc]) / (max - min);
				else if (max == src->data[possrc + 2])
					matiz = 240 + 60 * (src->data[possrc] - src->data[possrc + 1]) / (max - min);
				else
					matiz = 0;
			}
			dst->data[posdst] = (int)(matiz * 255.0f) / 360;
			dst->data[posdst + 1] = (int)sat;
			dst->data[posdst + 2] = (int)valor;
		}
	}
	return 0;
}

int vc_scale_gray_to_rgb(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float r, g, b, max, med, min, grey;

	// Verificao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 3))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src; // Calcular indíce, channels_src = 3
			pos_dst = y * bytesperline_dst + x * channels_dst; // channels_dst = 1

			grey = datasrc[pos_src];

			if (grey < 64)
			{
				r = 0;
				g = (grey * 4.0); // multiplicar por 4 porque a escala está dividida em 4
				b = 255;
			}
			else if (grey < 128)
			{
				r = 0;
				g = 255.0;
				b = 255.0 - (grey - 64.0) * 4.0; // Aos 255 que é onde começa subtrai-se o valor do gray - 64 e multiplica-se por 4
			}
			else if (grey < 192)
			{
				r = (grey - 128) * 4; // multiplicar por 4 porque a escala está dividida em 4
				g = 255.0;
				b = 0;
			}
			else
			{
				r = 255.0;
				g = 255.0 - (grey - 192.0) * 4.0; // Aos 255 que é onde começa subtrai-se o valor do gray - 192 e multiplica-se por 4
				b = 0;
			}

			datadst[pos_dst] = (unsigned char)r;
			datadst[pos_dst + 1] = (unsigned char)g;
			datadst[pos_dst + 2] = (unsigned char)b;
		}
	}
	return 1;
}

int vc_gray_to_binary(IVC *src, IVC *dst, int threshold)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos;
	float gray;

	// Verificao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != dst->channels))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src; // Calcular indíce, channels_src = 1

			gray = datasrc[pos];

			if (gray > threshold)
				datadst[pos] = 255;
			else
				datadst[pos] = 0;
		}
	}
}

int vc_gray_to_binary_global_mean(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos;
	float gray, med, threshold;

	// Verificao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != dst->channels))
		return 0;

	for (y = 0, gray = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src; // Calcular indíce, channels_src = 1
			gray += datasrc[pos];
		}
	}
	med = gray / (height * width);
	threshold = med;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline_src + x * channels_src; // Calcular indíce, channels_src = 1
			gray = datasrc[pos];

			if (gray > threshold)
				datadst[pos] = 255;
			else
				datadst[pos] = 0;
		}
	}
}

int vc_midpoint(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y, j, i;
	long int pos;
	float gray, med, threshold, vx, vy;

	// Verificao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != dst->channels))
		return 0;

	for (y = 0, gray = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			for (j = -12; j <= 12; j++) // (25-1)/2 = 12
			{
				for (i = -12; i <= 12; i++)
				{
					vx = (x + j);
					vy = (y + i);
					if (((vx >= 0) && (vx < width)) && ((vy >= 0) && (vy < height)))
						threshold = 1 / 2 * (0 + 255);
				}
			}
		}
	}
}

int vc_binary_dilate(IVC *src, IVC *dst, int size)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	int xk, yk;
	int i, j;
	long int pos, posk;
	int s1, s2;
	unsigned char pixel;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels))
		return 0;
	if (channels != 1)
		return 0;


	//Kernel pode ser par
	s2 = (size - 1) / 2;
	s1 = -(s2);

	memcpy(datadst, datasrc, bytesperline * height);

	// Cálculo da erosão
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;

			pixel = datasrc[pos];

			for (yk = s1; yk <= s2; yk++)
			{
				j = y + yk;

				if ((j < 0) || (j >= height))
					continue;

				for (xk = s1; xk <= s2; xk++)
				{
					i = x + xk;

					if ((i < 0) || (i >= width))
						continue;

					posk = j * bytesperline + i * channels;

					pixel |= datasrc[posk];
				}
			}

			// Se um qualquer pixel da vizinhança, na imagem de origem, for de plano de fundo, então o pixel central
			// na imagem de destino é também definido como plano de fundo.
			if (pixel == 255)
				datadst[pos] = 255;
		}
	}
	return 1;
}

int vc_binary_erode(IVC *src, IVC *dst, int size)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	int xk, yk;
	int i, j;
	long int pos, posk;
	int s1, s2;
	unsigned char pixel;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels))
		return 0;
	if (channels != 1)
		return 0;

	//Kernel tem que ser impar
	s2 = (size - 1) / 2;
	s1 = -(s2);

	memcpy(datadst, datasrc, bytesperline * height);

	// Cálculo da erosão
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;

			pixel = datasrc[pos];

			for (yk = s1; yk <= s2; yk++)
			{
				j = y + yk;

				if ((j < 0) || (j >= height))
					continue;

				for (xk = s1; xk <= s2; xk++)
				{
					i = x + xk;

					if ((i < 0) || (i >= width))
						continue;

					posk = j * bytesperline + i * channels;

					pixel &= datasrc[posk];
				}
			}

			// Se um qualquer pixel da vizinhança, na imagem de origem, for de plano de fundo, então o pixel central
			// na imagem de destino é também definido como plano de fundo.
			if (pixel == 0)
				datadst[pos] = 0;
		}
	}

	return 1;
}

int vc_binary_open(IVC *src, IVC *dst, int kernelerode, int kerneldilate)
{
	IVC *temp;
	temp = vc_image_new(src->width, src->height, 1, 255); // Creates an empty image with the resolution of the src image
	if (temp == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		return 0;
	}
	vc_binary_erode(src, temp, kernelerode);
	vc_binary_dilate(temp, dst, kerneldilate);

	vc_image_free(temp);

	return 1;
}

int vc_binary_close(IVC *src, IVC *dst, int kerneldilate, int kernelerode)
{
	IVC *temp;
	temp = vc_image_new(src->width, src->height, 1, 255); // Creates an empty image with the resolution of the src image
	if (temp == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tOut of memory!\n");
		return 0;
	}
	vc_binary_dilate(src, temp, kerneldilate);
	vc_binary_erode(temp, dst, kernelerode);

	vc_image_free(temp);

	return 1;
}

// Etiquetagem de blobs
// src		: Imagem bin�ria de entrada
// dst		: Imagem grayscale (ir� conter as etiquetas)
// nlabels	: Endere�o de mem�ria de uma vari�vel, onde ser� armazenado o n�mero de etiquetas encontradas.
// OVC*		: Retorna um array de estruturas de blobs (objectos), com respectivas etiquetas. � necess�rio libertar posteriormente esta mem�ria.
OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = { 0 };
	int labelarea[256] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num, tmplabel;
	OVC* blobs; // Apontador para array de blobs (objectos) que ser� retornado desta fun��o.

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return NULL;
	if (channels != 1) return NULL;

	// Copia dados da imagem bin�ria para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pix�is de plano de fundo devem obrigat�riamente ter valor 0
	// Todos os pix�is de primeiro plano devem obrigat�riamente ter valor 255
	// Ser�o atribu�das etiquetas no intervalo [1,254]
	// Este algoritmo est� assim limitado a 254 labels
	for (i = 0, size = bytesperline * height; i < size; i++)
	{
		if (datadst[i] != 0) datadst[i] = 255;
	}

	// Limpa os rebordos da imagem bin�ria
	for (y = 0; y < height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x < width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels; // B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels; // D
			posX = y * bytesperline + x * channels; // X

			// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					// Se A est� marcado
					if (datadst[posA] != 0) num = labeltable[datadst[posA]];
					// Se B est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (labeltable[datadst[posB]] < num)) num = labeltable[datadst[posB]];
					// Se C est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (labeltable[datadst[posC]] < num)) num = labeltable[datadst[posC]];
					// Se D est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (labeltable[datadst[posD]] < num)) num = labeltable[datadst[posD]];

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
					labeltable[num] = num;

					// Actualiza a tabela de etiquetas
					if (datadst[posA] != 0)
					{
						if (labeltable[datadst[posA]] != num)
						{
							for (tmplabel = labeltable[datadst[posA]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posB] != 0)
					{
						if (labeltable[datadst[posB]] != num)
						{
							for (tmplabel = labeltable[datadst[posB]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posC] != 0)
					{
						if (labeltable[datadst[posC]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posD] != 0)
					{
						if (labeltable[datadst[posD]] != num)
						{
							for (tmplabel = labeltable[datadst[posD]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
			{
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	//printf("\nMax Label = %d\n", label);

	// Contagem do n�mero de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a < label - 1; a++)
	{
		for (b = a + 1; b < label; b++)
		{
			if (labeltable[a] == labeltable[b]) labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que n�o hajam valores vazios (zero) entre etiquetas
	if(!*nlabels>0)
		*nlabels = 0;

	for (a = 1; a < label; a++)
	{
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++; // Conta etiquetas
		}
	}

	// Se n�o h� blobs
	if (*nlabels == 0) return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC*)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
	{
		for (a = 0; a < (*nlabels); a++) blobs[a].label = labeltable[a];
	}
	else return NULL;

	return blobs;
}


int vc_binary_blob_info(IVC* src, OVC* blobs, int nblobs)
{
	unsigned char* data = (unsigned char*)src->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, i;
	long int pos;
	int xmin, ymin, xmax, ymax;
	long int sumx, sumy;

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1) return 0;

	// Conta �rea de cada blob
	for (i = 0; i < nblobs; i++)
	{
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		blobs[i].area = 0;

		for (y = 1; y < height - 1; y++)
		{
			for (x = 1; x < width - 1; x++)
			{
				pos = y * bytesperline + x * channels;

				if (data[pos] == blobs[i].label)
				{
					// �rea
					blobs[i].area++;

					// Centro de Gravidade
					sumx += x;
					sumy += y;

					// Bounding Box
					if (xmin > x) xmin = x;
					if (ymin > y) ymin = y;
					if (xmax < x) xmax = x;
					if (ymax < y) ymax = y;

					// Per�metro
					// Se pelo menos um dos quatro vizinhos n�o pertence ao mesmo label, ent�o � um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label))
					{
						blobs[i].perimeter++;
					}
				}
			}
		}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		//blobs[i].xc = (xmax - xmin) / 2;
		//blobs[i].yc = (ymax - ymin) / 2;
		blobs[i].xc = sumx / MAX(blobs[i].area, 1);
		blobs[i].yc = sumy / MAX(blobs[i].area, 1);
	}

	return 1;
}

int vc_draw_bounding_box(IVC* src, IVC* dst, OVC* blobs, int firstIteration)
{
	if (src->height < 0 || src->width < 0 || (src->levels < 0 || src->levels>255))
		return 0;
	if (src->channels != 1 || dst->channels != 1)
		return 0;

	int x, y;
	long int posy, posx, posTotal;

	if(!firstIteration)
		memcpy(dst->data, src->data, src->bytesperline * src->height);

	for (x = blobs->x; x < blobs->width + blobs->x; x++)
	{
		for (y = blobs->y; y < blobs->height + blobs->y; y++)
		{
			posTotal = y * src->bytesperline + x * src->channels;
			if (x == blobs->x || x == blobs->x + blobs->width - 1 ||
				y == blobs->y || y == blobs->y + blobs->height - 1)
			{
					dst->data[posTotal] = 255;
			}
		}
	}

	return 1;
	//// Bounding Box
	//blobs[i].x = xmin;
	//blobs[i].y = ymin;
	//blobs[i].width = (xmax - xmin) + 1;
	//blobs[i].height = (ymax - ymin) + 1;

	//// Centro de Gravidade
	////blobs[i].xc = (xmax - xmin) / 2;
	////blobs[i].yc = (ymax - ymin) / 2;
	//blobs[i].xc = sumx / MAX(blobs[i].area, 1);
	//blobs[i].yc = sumy / MAX(blobs[i].area, 1);

}


int vc_draw_bounding_box_rgb(IVC* src, IVC* dst, OVC* blobs, int firstIteration)
{
	if (src->height < 0 || src->width < 0 || (src->levels < 0 || src->levels>255))
		return 0;
	if (src->channels != 1 || dst->channels != 3)
		return 0;

	int x, y;
	long int posy, posx, posTotal, posdst;

	if (!firstIteration)
		memcpy(dst->data, src->data, src->bytesperline * src->height);

	for (x = blobs->x; x < blobs->width + blobs->x; x++)
	{
		for (y = blobs->y; y < blobs->height + blobs->y; y++)
		{
			posTotal = y * src->bytesperline + x * src->channels;
			posdst = y * dst->bytesperline + x * dst->channels;
			if (x == blobs->x || x == blobs->x + blobs->width - 1 ||
				y == blobs->y || y == blobs->y + blobs->height - 1)
			{
				dst->data[posdst] = 255;
				dst->data[posdst+1] = 255;
				dst->data[posdst+2] = 255;
			}
		}
	}

	return 1;
	//// Bounding Box
	//blobs[i].x = xmin;
	//blobs[i].y = ymin;
	//blobs[i].width = (xmax - xmin) + 1;
	//blobs[i].height = (ymax - ymin) + 1;

	//// Centro de Gravidade
	////blobs[i].xc = (xmax - xmin) / 2;
	////blobs[i].yc = (ymax - ymin) / 2;
	//blobs[i].xc = sumx / MAX(blobs[i].area, 1);
	//blobs[i].yc = sumy / MAX(blobs[i].area, 1);

}



int vc_gray_histogram_show(IVC* src, IVC* dst)
{
	if (src->height < 0 || src->width < 0 || (src->levels < 0 && src->levels>255))
		return 0;
	if (src->channels != 1 || dst->channels != 1)
		return 0;

	int x, y;
	long int pos;
	int ni[256] = { 0 };

	for (int x = 0; x < src->width; x++)
	{
		for (int y = 0; y < src->height; y++)
		{
			pos = y * src->bytesperline + x * src->channels;
			unsigned char brilho = src->data[pos];
			ni[brilho] = ni[brilho] + 1;
		}
	}

	float pdf[256];
	int n = src->width * src->height;

	for (int i = 0; i < 256; i++)
	{
		pdf[i] = (float)ni[i] / (float)n;
		printf("Nivel:%d pdf:%f\n", i, pdf[i]);
	}

	float pdfMax = 0;
	for (int i = 0; i < 256; i++)
	{
		if (pdf[i] > pdfMax)
			pdfMax = pdf[i];
	}
	printf("%f", pdfMax);

	float pdfNorm[256];
	for (int i = 0; i < 256; i++)
	{
		pdfNorm[i] = pdf[i] / pdfMax;
	}

	for (int i = 0; i < dst->width * dst->height; i++)
	{
		dst->data[i] = 0;
	}
	for (int x = 0; x < 256; x++) {
		for (y = (256 - 1); y >= (256 - 1) - pdfNorm[x] * 255; y--) {
			dst->data[y * 256 + x] = 255;
		}
	}
	return 1;
}


int vc_gray_histogram_equalization(IVC* src, IVC* dst)
{

	if (src->height < 0 || src->width < 0 || (src->levels < 0 && src->levels>255))
		return 0;
	if (src->channels != 1 || dst->channels != 1)
		return 0;

	int x, y;
	long int pos;

	int ni[256] = { 0 };

	for (int x = 0; x < src->width; x++)
	{
		for (int y = 0; y < src->height; y++)
		{
			pos = y * src->bytesperline + x * src->channels;
			unsigned char brilho = src->data[pos];
			ni[brilho] = ni[brilho] + 1;
		}
	}



	float pdf[256];
	int n = src->width * src->height;

	for (int i = 0; i < 256; i++)
	{
		pdf[i] = (float)ni[i] / (float)n;
		printf("Nivel:%d pdf:%f\n", i, pdf[i]);
	}

	float cdf[256];

	for (int i = 0; i < 256; i++)
	{
		if (i == 0)
			cdf[i] = pdf[i];
		else
			cdf[i] = cdf[i - 1] + pdf[i];
	}

	float cdfMin = 1;

	for (int i = 0; i < 256; i++)
	{
		if (cdf[i] != 0)
		{
			cdfMin = cdf[i];
			break;
		}
	}

	for (int x = 0; x < src->width; x++)
	{
		for (int y = 0; y < src->height; y++)
		{
			pos = y * src->bytesperline + x * src->channels;
			unsigned char brilho = src->data[pos];
			dst->data[pos] = (unsigned char)((cdf[brilho] - cdfMin) / (1.0f - cdfMin) * (src->levels - 1));
		}
	}
	return 1;
}


int vc_hsv_histogram_equalization(IVC* src, IVC* dst)
{
	if (src->height < 0 || src->width < 0 || (src->levels < 0 && src->levels>255))
		return 0;
	if (src->channels != 3 || dst->channels != 3)
		return 0;

	int x, y;
	long int pos;

	int ni[256] = { 0 };
	

	for (int x = 0; x < src->width; x++)
	{
		for (int y = 0; y < src->height; y++)
		{
			pos = y * src->bytesperline + x * src->channels;
			unsigned char brilho = src->data[pos];
			unsigned char brilho2 = src->data[pos +1];
			unsigned char brilho3 = src->data[pos + 2];
			ni[brilho] = ni[brilho] + 1;
			ni[brilho2] = ni[brilho2] + 1;
			ni[brilho3] = ni[brilho3] + 1;
		}
	}

	return 1;
}

// Detecção de contornos pelos operadores Prewitt
int vc_gray_edge_prewitt(IVC *src, IVC *dst, float th) // th = [0.001, 1.000]
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	long int posX, posA, posB, posC, posD, posE, posF, posG, posH;
	int i, size;
	float histmax;
	int histthreshold;
	int sumx, sumy;
	float hist[256] = {0.0f};

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels))
		return 0;
	if (channels != 1)
		return 0;

	size = width * height;

	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			// PosA PosB PosC
			// PosD PosX PosE
			// PosF PosG PosH

			posA = (y - 1) * bytesperline + (x - 1) * channels;
			posB = (y - 1) * bytesperline + x * channels;
			posC = (y - 1) * bytesperline + (x + 1) * channels;
			posD = y * bytesperline + (x - 1) * channels;
			posX = y * bytesperline + x * channels;
			posE = y * bytesperline + (x + 1) * channels;
			posF = (y + 1) * bytesperline + (x - 1) * channels;
			posG = (y + 1) * bytesperline + x * channels;
			posH = (y + 1) * bytesperline + (x + 1) * channels;

			// PosA*(-1) PosB*0 PosC*(1)
			// PosD*(-1) PosX*0 PosE*(1)
			// PosF*(-1) PosG*0 PosH*(1)

			sumx = datasrc[posA] * -1;
			sumx += datasrc[posD] * -1;
			sumx += datasrc[posF] * -1;

			sumx += datasrc[posC] * +1;
			sumx += datasrc[posE] * +1;
			sumx += datasrc[posH] * +1;
			sumx = sumx / 3; // 3 = 1 + 1 + 1

			// PosA*(-1) PosB*(-1) PosC*(-1)
			// PosD*0    PosX*0    PosE*0
			// PosF*(1)  PosG*(1)  PosH*(1)

			sumy = datasrc[posA] * -1;
			sumy += datasrc[posB] * -1;
			sumy += datasrc[posC] * -1;

			sumy += datasrc[posF] * +1;
			sumy += datasrc[posG] * +1;
			sumy += datasrc[posH] * +1;
			sumy = sumy / 3; // 3 = 1 + 1 + 1

			// datadst[posX] = (unsigned char)sqrt((double)(sumx*sumx + sumy*sumy));
			datadst[posX] = (unsigned char)(sqrt((double)(sumx * sumx + sumy * sumy)) / sqrt(2.0));
			// Explicação:
			// Queremos que no caso do pior cenário, em que sumx = sumy = 255, o resultado
			// da operação se mantenha no intervalo de valores admitido, isto é, entre [0, 255].
			// Se se considerar que:
			// max = 255
			// Então,
			// sqrt(pow(max,2) + pow(max,2)) * k = max <=> sqrt(2*pow(max,2)) * k = max <=> k = max / (sqrt(2) * max) <=>
			// k = 1 / sqrt(2)
		}
	}

	// Calcular o histograma com o valor das magnitudes
	for (i = 0; i < size; i++)
	{
		hist[datadst[i]]++;
	}

	// Definir o threshold.
	// O threshold é definido pelo nível de intensidade (das magnitudes)
	// quando se atinge uma determinada percentagem de pixeis, definida pelo utilizador.
	// Por exemplo, se o parâmetro 'th' tiver valor 0.8, significa the o threshold será o
	// nível de magnitude, abaixo do qual estão pelo menos 80% dos pixeis.
	histmax = 0.0f;
	for (i = 0; i <= 255; i++)
	{
		histmax += hist[i];

		// th = Prewitt Threshold
		if (histmax >= (((float)size) * th))
			break;
	}
	histthreshold = i == 0 ? 1 : i;

	// Aplicada o threshold
	for (i = 0; i < size; i++)
	{
		if (datadst[i] >= (unsigned char)histthreshold)
			datadst[i] = 255;
		else
			datadst[i] = 0;
	}

	return 1;
}

// int vc_gray_edge_sobel(ivc *src, ivc *dst, float th)
//{
//
// }

int vc_gray_lowpass_mean_filter(IVC *src, IVC *dst, int kernelsize)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int i, j, y, x, xk, yk, s1, s2;
	long int pos, posKernel;
	unsigned char pixel;
	float temp = 0, media;

	s2 = kernelsize; // 9
	s1 = -(s2);		 // -9

	// Verificao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != dst->channels != 1))
		return 0;

	memcpy(datadst, datasrc, bytesperline * height);

	for (y = 0; y < height; y++) // 255
	{
		for (x = 0; x < width; x++) // 255
		{
			temp = 0;
			pos = y * bytesperline + x * channels;
			pixel = datasrc[pos];

			for (yk = s1; yk <= s2; yk++) //(-9;<=9;-9++)
			{
				for (xk = s1; xk <= s2; xk++) //(-9;<=9;-9++)
				{
					posKernel = (y + yk) * bytesperline + (x + xk) * channels;
					temp += datasrc[posKernel];
				}
			}
			media = temp * (1.0F / 9);
			datadst[pos] = media;
		}
	}
	return 1;
}

int vc_gray_lowpass_median_filter(IVC *src, IVC *dst, int kernelsize)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int i, j, y, x, xk, yk, s1, s2;
	long int pos, posKernel;
	unsigned char pixel;
	float temp = 0, mediana;

	s2 = (kernelsize - 1) / 2;
	s1 = -(s2);

	memcpy(datadst, datasrc, bytesperline * height);

	// Verificao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != dst->channels))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;
			pixel = datasrc[pos];

			for (yk = s1; yk <= s2; yk++)
			{
				for (xk = s1; xk <= s2; xk++)
				{
					if (((s2 / 2) % 2) == 0)
					{
					}
				}
			}
		}
	}
}

IVC *vc_convert_bgr_to_rgb(IVC *src)
{
	unsigned char *datadst = src->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;

	// Verificacao de erros
	if ((width <= 0) || (height <= 0) || (datadst == NULL))
		return 0;
	if (channels != 3)
		return 0;
	// Verifica se existe blobs

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = y * bytesperline + x * channels;
			unsigned char aux = datadst[pos];
			datadst[pos] = datadst[pos + 2];
			datadst[pos + 2] = aux;
		}
	}
	return src;
}

int vc_hsv_segmentation(IVC *src, IVC *dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_src = src->width * src->channels;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_src = src->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	float h, s, v;
	long int pos_src, pos_dst;
	int x, y;

	// Verifica��o de erros
	if ((width <= 0) || (height <= 0) || (datasrc == NULL))
		return 0;
	if (width != dst->width || height != dst->height)
		return 0;
	if (channels_src != 3 || dst->channels != 1)
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;
			h = ((float)datasrc[pos_src]) / 255.0f * 360.0f;
			s = ((float)datasrc[pos_src + 1]) / 255.0f * 100.0f;
			v = ((float)datasrc[pos_src + 2]) / 255.0f * 100.0f;

			if (h >= hmin && h <= hmax && s >= smin && s <= smax && v >= vmin && v <= vmax)
			{
				datadst[pos_dst] = (unsigned char)255;
			}
			else
			{
				datadst[pos_dst] = (unsigned char)0;
			}
		}
	}
	return 1;
}

int vc_gray_3channels(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;

	// Verificacao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height))
		return 0;
	if ((src->channels != 1) || (dst->channels != 3))
		return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src; // Calcular indíce, channels_src = 1
			pos_dst = y * bytesperline_dst + x * channels_dst; // channels_dst = 3

			datadst[pos_dst] = datasrc[pos_src];
			datadst[pos_dst + 1] = datasrc[pos_src];
			datadst[pos_dst + 2] = datasrc[pos_src];
		}
	}
	return 1;
}


int lateraisBinary(IVC *src, IVC * dst, int * hmax, int* hmin, int* wmax, int* wmin){

	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y;
	int* i, *j;
	long int pos, posdst;
	unsigned char pixel;
	int haux, waux;


	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels))
		return 0;
	if (channels != 1)
		return 0;


	memcpy(datadst, datasrc, bytesperline * height);

	// Cálculo da erosão
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;

			pixel = datasrc[pos];

			haux = y;
			waux = x;

			//Se o pixel for preto
			if (pixel == 0)
			{
				//Descobre a altura min (ou o topo do retangulo)
				if (haux < *hmin)
				{
					*hmin = haux;
				}
				
				//Descobre a altura max (ou a base do retangulo)
				if (haux > *hmax)
				{
					*hmax = haux;
				}

				//Descobre a largura min (ou o lado esquerdo do retangulo)
				if (waux < *wmin)
				{	
					*wmin = waux;
				}

				//Descobre a largura max (ou o lado direito do retangulo)
				if (waux > *wmax)
				{
					*wmax = waux;
				}
			}
			
		}
	}

	//Desenhar linha entre wmin e wmax na linha de hmax && wmin e wmax na linha de hmin && hmax a hmin na coluna de wmin && hmax a hmin na coluna de wmax
	for (i = hmin; i <= hmax; i++)
	{
		for (j = wmin; j <= wmax; j++)
		{
			if (i==hmin || i==hmax || j==wmin || j==wmax)
			{
					posdst = (*i) * bytesperline + (*j) * channels;
					datadst[posdst] = 0;		
			}
		
		}
		
	} 
	/*
	haux = ( hmax - hmin ) / 2;
	waux = ( wmax - wmin ) / 2;

	haux = hmax - haux;
	waux = wmax - waux;
	*/
	return 1;
}


//Ideia seria receber o RGB(src) e uma imagem(dst) para "desenhar" as laterais da resistencia
//Para "desenhar" seria necessário passar a RGB para HSV, passar para binario, fazer o close e passar pela função lateraisBinarios. 
//Seria necessario enviar os dados de hmax, hmin, wmax e wmin para desenhar utilizando essas coordenadas

int lateraisHsv(IVC *src, IVC * dst, int * hmax, int* hmin, int* wmax, int* wmin){

	IVC *img[5];//Imagens para operações intermedias
	unsigned char *datasrc = (unsigned char *)src->data;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_src = src->width * src->channels;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_src = src->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	long int pos_src, pos_dst;
	int x, y, pixel;
	int* i, *j;

	// Verificação de erros
	if ((width <= 0) || (height <= 0) || (datasrc == NULL))
		return 0;
	if (width != dst->width || height != dst->height)
		return 0;
	if (channels_src != 3 || dst->channels != 1)
		return 0;


	memcpy(datadst, datasrc, bytesperline_src * height);

	//RGB para GrayScale
	img[0] = vc_image_new(src->width, src->height, 1, src->levels);
	vc_rgb_to_gray(src, img[0]);
	
	if (img[0] == NULL)
	{
		printf("GANDA ERRO!!");	
		return 0;
	}

	img[1] = vc_image_new(src->width, src->height, 1, 1);
	vc_gray_to_binary(img[0], img[1], 180);

	if (img[1] == NULL)
	{
		printf("GANDA ERRO!!");	
		return 0;
	}

	img[2] = vc_image_new(src->width, src->height, 1, 1);
	lateraisBinary(img[1], img[2], hmax, hmin, wmax, wmin);
	
	if (img[2] == NULL)
	{
		printf("GANDA ERRO!!");	
		return 0;
	}

	
	for (i = hmin; i <= hmax; i++)
	{
		for (j = wmin; j <= wmax; j++)
		{
			if (i==hmin || i==hmax || j==wmin || j==wmax)
			{
					pos_dst = (*i) * bytesperline_dst + (*j) * channels_dst;
					datadst[pos_dst] = 255;		
			}
		
		}
		
	}

	

	return 1;

}

int vc_join_images(IVC* src, IVC* dst)
{
	if (src->height < 0 || src->width < 0 || (src->levels < 0 && src->levels>255))
		return 0;
	if (src->channels != 1 || dst->channels != 1)
		return 0;

	int x, y;
	long int possrc, posdst;

	for (x = 0; x < src->width; x++)
	{
		for (y = 0; y < src->height; y++)
		{
			possrc = y * src->bytesperline + x * src->channels;
			posdst = y * dst->bytesperline + x * dst->channels;

			if (dst->data[posdst] == 255)
				continue;
			dst->data[possrc] = src->data[possrc];
		}
	}
	return 1;
}

int vc_table_resistors_value(char* stringCor)
{
	if (stringCor != NULL)
	{
		if (strcmp(stringCor, "Preto") == 0)
			return 0;
		else if (strcmp(stringCor, "Castanho") == 0)
			return 1;
		else if (strcmp(stringCor, "Vermelho") == 0)
			return 2;
		else if (strcmp(stringCor, "Laranja") == 0)
			return 3;
		else if (strcmp(stringCor, "Amarelo") == 0)
			return 4;
		else if (strcmp(stringCor, "Verde") == 0)
			return 5;
		else if (strcmp(stringCor, "Azul") == 0)
			return 6;
		else if (strcmp(stringCor, "Roxo") == 0)
			return 7;
		else if (strcmp(stringCor, "Cinzento") == 0)
			return 8;
		else if (strcmp(stringCor, "Branco") == 0)
			return 9;
		else if (strcmp(stringCor, "Dourado") == 0)
			return 10;
	}
	return -1;
}

int vc_table_resistors_multiplier(char* stringCor)
{
	if (stringCor != NULL)
	{
		if (strcmp(stringCor, "Preto") == 0)
			return 1;
		else if (strcmp(stringCor, "Castanho") == 0)
			return 10;
		else if (strcmp(stringCor, "Vermelha") == 0)
			return 100;
		else if (strcmp(stringCor, "Laranja") == 0)
			return 1000;
		else if (strcmp(stringCor, "Amarelo") == 0)
			return 10000;
		else if (strcmp(stringCor, "Verde") == 0)
			return 100000;
		else if (strcmp(stringCor, "Azul") == 0)
			return 1000000;
	}
	return 0;
}
