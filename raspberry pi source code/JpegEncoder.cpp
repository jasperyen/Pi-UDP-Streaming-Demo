#include "header.h"
#include "JpegEncoder.h"

JpegEncoder::JpegEncoder(const int w, const int h, const int qua) {
	width = w;
	height = h;
	quality = qua;

	handle = tjInitCompress();
	if (handle == NULL) {
		cout << "Create TurboJPEG compressor instance failed" << endl;
		exit(-1);
	}
	printf("Create TurboJPEG compressor instance sucess : %p\n", handle);
}

bool JpegEncoder::encodeYUV420(const unsigned char *raw_data,
													vector<unsigned char> &jpeg_data) {
	unsigned char *tjdata = NULL;
	unsigned long tjsize = 0;

	int ret = tjCompressFromYUV(handle, raw_data, width, padding, height, subsamp,
									&tjdata, &tjsize, quality, tjflag);
	if (ret < 0) {
		printf("compressor instance : %p\n", handle);
		printf("compress to jpeg failed: %s\n", tjGetErrorStr());
		return false;
	}
	jpeg_data.assign(tjdata, tjdata + tjsize);
	tjFree(tjdata);
	return true;
}


bool JpegEncoder::encodeRGB(const unsigned char *raw_data,
													vector<unsigned char> &jpeg_data) {
	unsigned char *tjdata = NULL;
	unsigned long tjsize = 0;

	int ret = tjCompress2(handle, raw_data, width, 0, height, rgbFormat,
									&tjdata, &tjsize, TJSAMP_420, quality, tjflag);
	if (ret < 0) {
		printf("compressor instance : %p\n", handle);
		printf("compress to jpeg failed: %s\n", tjGetErrorStr());
		return false;
	}
	jpeg_data.assign(tjdata, tjdata + tjsize);
	tjFree(tjdata);
	return true;
}


bool JpegEncoder::encodeBGR(const unsigned char *raw_data,
													vector<unsigned char> &jpeg_data) {
	unsigned char *tjdata = NULL;
	unsigned long tjsize = 0;

	int ret = tjCompress2(handle, raw_data, width, 0, height, bgrFormat,
									&tjdata, &tjsize, TJSAMP_420, quality, tjflag);
	if (ret < 0) {
		printf("compressor instance : %p\n", handle);
		printf("compress to jpeg failed: %s\n", tjGetErrorStr());
		return false;
	}
	jpeg_data.assign(tjdata, tjdata + tjsize);
	tjFree(tjdata);
	return true;
}
