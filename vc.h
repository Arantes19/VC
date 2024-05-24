//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#define VC_DEBUG
#define MAX(a, b)(a > b ? a : b)
#define MIN(a, b)(a < b ? a : b)


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


typedef struct {
	unsigned char *data;
	int width, height;
	int channels;			// Binario/Cinzentos=1; RGB=3
	int levels;				// Binario=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline;		// width * channels
} IVC;

typedef struct {
	int x, y, width, height;	// Caixa Delimitadora (Bounding Box)
	int area;					// аrea
	int xc, yc;					// Centro-de-massa
	int perimeter;				// PerМmetro
	int label;					// Etiqueta
} OVC;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROT�TIPOS DE FUN��ES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);

//Função que calcule o negativo da iamgem cinzenta
int vc_gray_negative(IVC *srcdst);

//Função que calcule o negativo de uma imagem RGB
int vc_rgb_negative(IVC *srcdst);

// funções que extraiam as componentes R, G e B
int vc_rgb_get_red_gray(IVC *srcdst);

int vc_rgb_get_green_gray(IVC *srcdst);

int vc_rgb_get_blue_gray(IVC *srcdst);

int vc_rgb_to_gray(IVC *src, IVC *dst);

int vc_rgb_to_hsv(IVC *src, IVC *dst);

int vc_scale_gray_to_rgb(IVC *src, IVC *dst);

int vc_gray_to_binary(IVC *src, IVC *dst, int threshold);

int vc_gray_to_binary_global_mean(IVC *src, IVC *dst);

int vc_midpoint(IVC *src, IVC *dst);

int vc_binary_dilate(IVC *src, IVC *dst, int size);

int vc_binary_erode(IVC *src, IVC *dst, int size);

int vc_binary_open(IVC *src, IVC *dst, int kernelerode, int kerneldilate);

int vc_binary_close(IVC *src, IVC *dst, int kernelerode, int kerneldilate);

OVC* vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels);

int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs);

int vc_gray_histogram_show(IVC *src, IVC *dst);

int vc_gray_histogram_equalization(IVC *src, IVC *dst);

int vc_gray_edge_prewitt(IVC *src, IVC *dst, float th);

int vc_gray_edge_sobel(IVC *src, IVC *dst, float th);

int vc_gray_lowpass_mean_filter(IVC *src, IVC *dst, int kernelsize);

int vc_gray_lowpass_median_filter(IVC *src, IVC *dst, int kernelsize);

IVC* vc_convert_bgr_to_rgb(IVC* src);

int vc_hsv_segmentation(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);

int vc_gray_3channels(IVC* src, IVC* dst);

int lateraisBinary(IVC *src, IVC * dst, int * hmax, int* hmin, int* wmax, int* wmin);

int lateraisHsv(IVC *src, IVC * dst, int * hmax, int* hmin, int* wmax, int* wmin);

