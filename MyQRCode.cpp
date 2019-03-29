// Fill out your copyright notice in the Description page of Project Settings.

#include "MyQRCode.h"
#include "qrencode.h"
#include "FileManagerGeneric.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include<string>
#include<fstream>
using namespace std;

#define QRCODE_TEXT					"http://www.ultramundum.org/index.htm";		// Text to encode into QRCode
#define OUT_FILE					"E:/test.bmp"								// Output file name
#define OUT_FILE_PIXEL_PRESCALER	8											// Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension)

#define PIXEL_COLOR_R				0											// Color of bmp pixels
#define PIXEL_COLOR_G				0
#define PIXEL_COLOR_B				0

// BMP defines

typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef signed long		LONG;

#define BI_RGB			0L

#pragma pack(push, 2) //2字节对齐，不然会出问题

typedef struct
{
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)
//	-------------------------------------------------------

static IImageWrapperPtr GetImageWrapperByExtention(const FString InImagePath)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (InImagePath.EndsWith(".png"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (InImagePath.EndsWith(".jpg") || InImagePath.EndsWith(".jpeg"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else if (InImagePath.EndsWith(".bmp"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	else if (InImagePath.EndsWith(".ico"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);

	}
	else if (InImagePath.EndsWith("exr"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	}
	else if (InImagePath.EndsWith(".icns"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
	}
	return nullptr;
}

// Sets default values
AMyQRCode::AMyQRCode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyQRCode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyQRCode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FString AMyQRCode::SetQECode(FString name)
{
	FString SavePath = FPaths::GameDir() + "Content/Texture/TargetQRCode.bmp";
	std::string MyStdString(TCHAR_TO_UTF8(*name));
	std::string MyStdPath(TCHAR_TO_UTF8(*SavePath));
	const char*			szSourceSring = MyStdString.c_str();    //QRCODE_TEXT
	const char*		szSavePathSring = MyStdPath.c_str();
	unsigned int	unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
	unsigned char*	pRGBData, *pSourceData, *pDestData;
	QRcode*			pQRC;
	FILE*			f;



	// Compute QRCode
	pQRC = QRcode_encodeString(szSourceSring, 0, QR_ECLEVEL_H, QR_MODE_8, 1);
	if (pQRC)
	{
		unWidth = pQRC->width; // 矩阵的维数
		unWidthAdjusted = unWidth * OUT_FILE_PIXEL_PRESCALER * 3; //每一个维度占的像素的个数（8），每个像素3个字节
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * OUT_FILE_PIXEL_PRESCALER;

// Allocate pixels buffer
pRGBData = (unsigned char*)malloc(unDataBytes);
if (!pRGBData)
{
	printf("Out of memory");
	exit(-1);
}

// Preset to white

memset(pRGBData, 0xff, unDataBytes);


// Prepare bmp headers

BITMAPFILEHEADER kFileHeader;
kFileHeader.bfType = 0x4d42;  // "BM"
kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) +
sizeof(BITMAPINFOHEADER) +
unDataBytes;
kFileHeader.bfReserved1 = 0;
kFileHeader.bfReserved2 = 0;
kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) +
sizeof(BITMAPINFOHEADER);

BITMAPINFOHEADER kInfoHeader;
kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
kInfoHeader.biWidth = unWidth * OUT_FILE_PIXEL_PRESCALER;
kInfoHeader.biHeight = -((int)unWidth * OUT_FILE_PIXEL_PRESCALER); //负数可以控制图像上下颠倒
kInfoHeader.biPlanes = 1;
kInfoHeader.biBitCount = 24;
kInfoHeader.biCompression = BI_RGB;
kInfoHeader.biSizeImage = 0;
kInfoHeader.biXPelsPerMeter = 0;
kInfoHeader.biYPelsPerMeter = 0;
kInfoHeader.biClrUsed = 0;
kInfoHeader.biClrImportant = 0;


// Convert QrCode bits to bmp pixels

pSourceData = pQRC->data;
for (y = 0; y < unWidth; y++)
{
	pDestData = pRGBData + unWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;
	for (x = 0; x < unWidth; x++)
	{
		if (*pSourceData & 1)
		{
			for (l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)
			{
				for (n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)
				{
					*(pDestData + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_B;
					*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_G;
					*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_R;
				}
			}
		}
		pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;
		pSourceData++;
	}
}


// Output the bmp file

if (!(fopen_s(&f, szSavePathSring, "wb")))
{
	fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
	fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
	fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);

	fclose(f);
}
else
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("QR Unable to open file!"));
}

// Free data

free(pRGBData);
QRcode_free(pQRC);
return SavePath;
	}
	else
	{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("QR NULL returned!"));
	return SavePath;
	}

}

UTexture2D * AMyQRCode::GetTexture2DFromDiskFile(const FString & FilePath)
{
	TArray<uint8> RawFileData;
	UTexture2D* MyTexture = NULL;
	if (FFileHelper::LoadFileToArray(RawFileData, *FilePath /*"<path to file>"*/))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		// Note: PNG format.  Other formats are supported
		IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			const TArray<uint8>* UncompressedBGRA = NULL;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				// Create the UTexture for rendering
				MyTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

				// Fill in the source data from the file
				void* TextureData = MyTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
				MyTexture->PlatformData->Mips[0].BulkData.Unlock();

				// Update the rendering resource from data.
				MyTexture->UpdateResource();
			}
		}
	}
	return MyTexture;
}

UTexture2D * AMyQRCode::LoadTexture2D(const FString & ImagePath, bool & IsValid, int32 & OutWidth, int32 & OutHeight)
{
	UTexture2D* Texture = nullptr;
	IsValid = false;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ImagePath))
	{
		return nullptr;
	}
	TArray<uint8> CompressedData;
	if (!FFileHelper::LoadFileToArray(CompressedData, *ImagePath))
	{
		return nullptr;
	}
	IImageWrapperPtr ImageWrapper = GetImageWrapperByExtention(ImagePath);
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.Num()))
	{
		const TArray<uint8>* UncompressedRGBA = nullptr;
		if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);
			if (Texture != nullptr)
			{
				IsValid = true;
				OutWidth = ImageWrapper->GetWidth();
				OutHeight = ImageWrapper->GetHeight();
				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedRGBA->GetData(), UncompressedRGBA->Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}
	return Texture;
}




