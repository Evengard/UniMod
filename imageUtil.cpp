#include "stdafx.h"
#include <list>
#include "math.h"
extern "C"
{
#include "png.h"
#include "pngstruct.h"
}

extern void *(__cdecl *noxAlloc)(int Size);
extern void (__cdecl *noxFree)(void *Ptr);

struct imgArrayItemAnimRec
{
  int field_0;
  void *vbagIdArrPtr;
  char vbagIdArrSize;
  char field_9;
  char isLooped;
  char gap_b[1];
  int field_C;
};

struct imgArrayItem
{
  char imageName[32];
  char animDirNameMB[32];
  char gap_40[32];
  int imageVbagId;// для собственных картинок - тут указатель
  char auxImageType;
  char imgFlagsMB;
  short field_66;
};
struct imageH /// указатель на эту структуру возвращает gLoadItem
{
  void* dataPtr;
  int field_4;
  unsigned short vbagId;
  // 0x80 значит загружено 
  // 0x40 хз, остальные - тип изображения 
  // 2 - обычный 16битный растр цветов
  // 3 - RLE популярный с прозрачностью вроде (menuBg,BorderCornerUL)
  // 7 - 16битный растр цветов с предобработкой, отключен (не может быть нарисован)
  //
  char typeFlags; 
  char field_B;
};
struct Sprite2
{
	int W,H,dX,dY;
	byte Some;
};

imgArrayItem **imgArray; //указатель на массив vbag
int *imgArraySize;
int *imgArrayNextId; // фактический размер массива vbag (создается нами)

namespace
{

	struct ImgVectorRec
	{
		imgArrayItem *Items;
		int Size;
		int Used;
		ImgVectorRec(int Size_,void *Data):
			Items((imgArrayItem *)Data), 
			Size(Size_),
			Used(0)
		{

		}
	};
	typedef std::list<ImgVectorRec> ImgVectorList;
	ImgVectorList imgVector;

	int __cdecl gLoadImgSearchImpl(const char *SearchFor)
	{
		imgArrayItem *P = *imgArray;
		imgArrayItem *E = P + *imgArrayNextId;
		for (int i=0;P!=E;P++,i++)
		{
			if (0==strncmp(P->imageName,SearchFor,sizeof(P->imageName)))
				return i;
		}
		for (ImgVectorList::iterator I=imgVector.begin();I!=imgVector.end();I++)
		{
			P = I->Items;
			E = P + I->Used;
			for (int i=0;P!=E;P++,i++)
			{
				if (0==strncmp(P->imageName,SearchFor,sizeof(P->imageName)))
				{
					//нашли
					return P->imageVbagId;
				}
			}
		}
		return 0;
	}
	void __declspec(naked) gLoadImgSearch()
	{
		__asm
		{
			push edx
			push eax
			call gLoadImgSearchImpl
			add esp,4
			pop edx // количество спрайтов в вбаге
			test eax,eax
			jz l1
			cmp eax,edx
			jge l2
			mov edi,eax
			push 0x0042F9D0
			ret
		l2:
			// случай когда кастом
			pop edi
			pop esi
			pop ebp
			pop ebx
			ret
		l1:
			push 0x0042F9C9 // случай когда не найдено
			ret
		}
	}
	void imgAddImage(const char *Name,void *Data)
	{
		const int Growth=32;
		imgArrayItem *P=0;
		for (ImgVectorList::iterator I=imgVector.begin();I!=imgVector.end();I++)
		{
			if (I->Size>I->Used)
			{
				P=I->Items + I->Used;
				I->Used++; // сейчас добавим
				break;
			}
		}
		if (P==NULL) ///  надо добавить новых
		{
			imgVector.push_back( 
				ImgVectorRec(Growth,malloc(Growth*sizeof(imgArrayItem) ))
				);
			P=imgVector.back().Items;
			imgVector.back().Used=1;/// сейчас добавим
		}
		memset(P,0,sizeof(*P));
		strncpy(P->imageName,Name,sizeof(P->imageName));
		P->imageVbagId=(int)Data;
	}
	struct PngData
	{
		png_structp png_ptr;
		png_infop info_ptr;

		png_bytep *row_pointers;
		int width;
		int height;
		int color;
	};
	struct PngReadData
	{
		void *Buf;
		void *Current;
		void *End;
		PngReadData(void *Ptr,int Size):Buf(Ptr),Current(Ptr),End((char*)Ptr+Size){}
	};
	bool imgPngStart(PngData *png,PngReadData *readData);
	void imgPngFinish(PngData *png);
	/// args:   srcPath[,imgName=srcPath /*[,imgType=2]*/ ]
	int imgLoadImage(lua_State*L)
	{
		lua_settop(L,3);
		if (!lua_isstring(L,1))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}

		imageH *H= (imageH *)noxAlloc(sizeof(imageH));
		const char *imgName=lua_tostring(L,2);
		if (imgName==NULL)
			imgName=lua_tostring(L,1);
		imgAddImage(imgName,H);

		Sprite2 *SpritePtr=NULL;

		png_structp png_ptr;
		png_infop info_ptr;

		
		PngData png;
		size_t Size;
		void *Data;
		if (!fsRead(lua_tostring(L,1),Data,Size))
			return 0;
		PngReadData readData(Data,Size);

		if (!imgPngStart(&png,&readData))
		{
			return 0;
		}
		int Width=png.width;
		int Height=png.height;

		bool useAlpha=(png.color==PNG_COLOR_TYPE_RGBA);
		int imgType=(useAlpha?3:2);
		int BlockSize=0;
		if (useAlpha)
			BlockSize=sizeof(Sprite2) + 3 * Width * Height; 
			// предполагаю что не будет через один A/C (в худшем случае размер будет 6xWxH)
		else
			BlockSize=sizeof(Sprite2) + 2 * Width * Height ;
		H->dataPtr=noxAlloc(BlockSize);
		H->typeFlags=0x80 | imgType;// загружено в указатель формат, 2
		H->vbagId=1;// для примера
		SpritePtr=(Sprite2*)H->dataPtr;
		SpritePtr->W=Width;
		SpritePtr->H=Height;
		SpritePtr->dX=0;
		SpritePtr->dY=0;
		SpritePtr->Some=0;
		short *P=(short*)(((byte*)H->dataPtr)+sizeof(Sprite2));
//#define RGB2(r,g,b) ((((r)&0x1F)<<11)|(((g)&0x3F)<<5)|((b)&0x1F))
#define RGB4(r,g,b,a) (((a)<<15)|(((r)>>3)<<10)|(((g)>>3)<<5)|((b)>>3))
#define RGB3(r,g,b) ((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3))
		if (useAlpha)
		{
			for (int j=0;j<Height;j++)
			{
				png_bytep Src=png.row_pointers[j];
				int Counter=0; //счетчик повторов
				Byte *PP=NULL;
				enum EncodeMode
				{
					emUnk=0,
					emSkip=1,
					emPlain=2,
					emTrans=5,
				}State=emUnk;
				for (int i=0;i<Width;i++)
				{
					if (State==emUnk)
					{
						if (Counter>0 && PP!=NULL)
						{
							*PP=Counter;
							Counter=0;
						}
						PP=(Byte*)P;
						if (Src[3]<0x40)
							State=emSkip;
						else if ( Src[3]> 0xC0)
							State=emPlain;
						else 
							State=emTrans;

						*(PP++)=State;
						*PP=0;
						P++;
						i--;
						continue;
					}
					else if (State==emSkip)
					{
						if (Src[3]<0x40)
						{
							if ( ++Counter==255)
								State=emUnk;
							Src+=4;
							continue;
						}
						State=emUnk;
						i--;
					}
					else if (State==emPlain)
					{
						if (Src[3]>0xC0)
						{
							if ( ++Counter==255)
								State=emUnk;
							*(P++)=RGB3(Src[0],Src[1],Src[2]);
							Src+=4;
							continue;
						}
						State=emUnk;
						i--;
					}
					else if (State==emTrans)
					{
						if ( (Src[3]>0x40) && Src[3]<0xC0)
						{
							if ( ++Counter==255)
								State=emUnk;
							*(P++)=RGB4(Src[0],Src[1],Src[2],Src[3]);
							Src+=4;
							continue;
						}
						State=emUnk;
						i--;
					}
				}
				if (Counter>0 && PP!=NULL)
					*PP=Counter;
			}
		}
		else
		{
			for (int j=0;j<Height;j++)
			{
				png_bytep Src=png.row_pointers[j];
				for (int i=0;i<Width;i++)
				{
					*(P++)=RGB3(Src[0],Src[1],Src[2]);
					Src+=3;
				}
			}
		}
		imgPngFinish(&png);
		lua_pushlightuserdata(L,H);
		return 1;
	}

	void PNGCAPI error_fn(png_structp, png_const_charp S)
	{
		throw S;
	}
	void PNGCAPI warn_fn(png_structp, png_const_charp S)
	{
	}
	void PNGCAPI  read_fn(png_structp png_ptr, png_bytep Data, png_size_t Size)
	{
		PngReadData *readData=(PngReadData *)png_ptr->io_ptr;
		if (readData==NULL)
			return;
		if (Size > (char*)readData->End - (char*)readData->Current) // слишком много, столько нет
			return;
		memcpy(Data,readData->Current,Size);
		readData->Current=((char*)readData->Current)+Size;
	}
	void* PNGCAPI pngMyMalloc(png_structp png_ptr, png_size_t Size)
	{
		return malloc(Size);
	}
	void pngMyFree(png_structp png_ptr, void *Ptr)
	{
		return free(Ptr);
	}
	bool imgPngStart(PngData *png,PngReadData *readData)
	{
		png_structp &png_ptr=png->png_ptr;
		png_infop &info_ptr=png->info_ptr;
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type;
		memset(png,0,sizeof(*png));

	   /* Create and initialize the png_struct with the desired error handler
		* functions.  If you want to use the default stderr and longjump method,
		* you can supply NULL for the last three parameters.  We also supply the
		* the compiler header file version, so that we know if the application
		* was compiled with a compatible version of the library.  REQUIRED
		*/
	   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,&error_fn, &warn_fn);
		if (png_ptr == NULL)
		{
		  return (ERROR);
		}
		try
		{
	
			png_set_mem_fn(png_ptr,NULL,pngMyMalloc,pngMyFree);
			/* Allocate/initialize the memory for image information.  REQUIRED. */
			info_ptr = png_create_info_struct(png_ptr);

			if (info_ptr == NULL)
			{
			  png_destroy_read_struct(&png_ptr, NULL, NULL);
			  return (ERROR);
			}

			/* Set error handling if you are using the setjmp/longjmp method (this is
			* the normal method of doing things with libpng).  REQUIRED unless you
			* set up your own error handlers in the png_create_read_struct() earlier.
			*/

			/*	   if (setjmp(png_jmpbuf(png_ptr)))
			{
			  // Free all of the memory associated with the png_ptr and info_ptr 
			  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			  // If we get here, we had a problem reading the file 
			  return (ERROR);
			}
			_CrtCheckMemory();*/


			/* If you are using replacement read functions, instead of calling
			* png_init_io() here you would call:
			*/
			png_set_read_fn(png_ptr, readData, &read_fn);

			/* The call to png_read_info() gives us all of the information from the
			* PNG file before the first IDAT (image data chunk).  REQUIRED
			*/
			png_read_info(png_ptr, info_ptr);

			png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			   &interlace_type, NULL, NULL);

			png->width=width;
			png->height=height;
			png->color=color_type;
			/* Set up the data transformations you want.  Note that these are all
			* optional.  Only call them if you want/need them.  Many of the
			* transformations only work on specific types of images, and many
			* are mutually exclusive.
			*/

			/* Tell libpng to strip 16 bit/color files down to 8 bits/color */
			png_set_strip_16(png_ptr);

			/* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
			* byte into separate bytes (useful for paletted and grayscale images).
			*/
			png_set_packing(png_ptr);

			/* Expand paletted colors into true RGB triplets */
			if (color_type == PNG_COLOR_TYPE_PALETTE)
			  png_set_palette_to_rgb(png_ptr);

			/* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
			if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			  png_set_expand_gray_1_2_4_to_8(png_ptr);

			/* Expand paletted or RGB images with transparency to full alpha channels
			* so the data will be available as RGBA quartets.
			*/
			//   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			//      png_set_tRNS_to_alpha(png_ptr);


			/* Add filler (or alpha) byte (before/after each RGB triplet) */
			//png_set_filler(png_ptr, 0xff, PNG_FILLER_BEFORE);


			/* Turn on interlace handling.  REQUIRED if you are not using
			* png_read_image().  To see how to handle interlacing passes,
			* see the png_read_row() method below:
			*/
			int number_passes = png_set_interlace_handling(png_ptr); 

			/* Allocate the memory to hold the image using the fields of info_ptr. */

			/* The easiest way to read the image: */
			png_bytep *row_pointers= (png_bytep *)malloc(sizeof(png_bytep)*height);

			/* Clear the pointer array */
			for (int row = 0; row < height; row++)
			{
			  row_pointers[row] = (png_bytep)malloc( png_get_rowbytes(png_ptr,info_ptr)); //(png_bytep) png_malloc(png_ptr, png_get_rowbytes(png_ptr,info_ptr));
			}
			int dataSize=32 + png_get_rowbytes(png_ptr,info_ptr);
			png_bytep Data=(png_bytep )malloc( dataSize);
			memset(Data,0,dataSize);

//			png_read_image(png_ptr, row_pointers);
			for (int pass = 0; pass <	number_passes; pass++)
			{
				for (int y = 0; y < height; y++)
				{
					png_read_row(png_ptr,  row_pointers[y] , NULL);//row_pointers[y]
				}
			}

			png->row_pointers=row_pointers;
		}
		catch(const char *S)
		{
			_CrtCheckMemory();
			conPrintI(S);
		}
		return 1;
	}
	void imgPngFinish(PngData *png)
	{
		png_structp &png_ptr(png->png_ptr);
		png_infop &info_ptr(png->info_ptr);

		try
		{
			png_bytep *row_pointers=png->row_pointers;
			int height=png->height;

			for (int row = 0; row < height; row++)
			{
			   free(row_pointers[row]);
			}
			free(row_pointers);

			/* Read rest of file, and get additional chunks in info_ptr - REQUIRED */
			png_read_end(png_ptr, info_ptr);

			/* At this point you have read the entire image */

			/* Clean up after the read, and free any memory allocated - REQUIRED */
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		}
		catch (const char *S)
		{
			conPrintI(S);
		}
		memset(png,0,sizeof(*png));
	}

}

extern void InjectJumpTo(DWORD Addr,void *Fn);

void ImageUtilInit()
{
	ASSIGN(imgArray,0x00694868);
	ASSIGN(imgArraySize,0x0069486C );
	ASSIGN(imgArrayNextId,0x00694870);

	InjectJumpTo(0x0042F982,gLoadImgSearch);
	registerclient("imgLoad",imgLoadImage); // грузить динамически изображение

}