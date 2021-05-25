// GooglePolylineCodec.h

#pragma once

namespace GooglePolylineCodec
{
	// Input should be the decimal value which is already multiplied by 1e5.
	// 120.1234567 => 12012345
	bool Encode(int aInput, char* aOutStr, int* aOutSize);
	bool Decode(char* aInStr, int* aOutput, int* aOutProcCount);
	bool EncodeArray(int* aInputArray, int aArraySize, char* aOutStr, int* aOutSize);
	bool DecodeArray(char* aInStr, int* aOutputArray, int* aOutArraySize);
	bool EncodePolyline(double* aInputPolyline, int aPolylineSize, char* aOutStr, int* aOutSize);
	bool DecodePolyline(char* aInStr, double* aOutputPolyline, int* aOutPolylineSize);
}