// GooglePolylineCodec.cpp
//
// https://developers.google.com/maps/documentation/utilities/polylinealgorithm
// https://developers.google.com/maps/documentation/utilities/polylineutility
//
#include "stdafx.h"
#include "GooglePolylineCodec.h"

namespace GooglePolylineCodec
{

	bool Encode(int aInput, char* aOutStr, int* aOutSize)
	{
		if (aOutStr == NULL || aOutSize == NULL)
		{
			return false;
		}

		// Step 1: Get sign bit
		int sign = (aInput >= 0) ? 1 : -1;
		int step1Result = aInput;

		// Step 2: Left-shift the binary value one bit
		int step2Result = step1Result << 1;

		// Step 3: If the original decimal value is negative, invert this encoding
		int step3Result = (sign == -1) ? ~step2Result : step2Result;

		// Step 4: Break the binary value out into 5-bit chunks (starting from the right hand side)
		//         And place the 5-bit chunks in reverse order.
		int step4Chunk[6] = { 0 };
		step4Chunk[0] = (step3Result & 0x1F);
		step4Chunk[1] = (step3Result & 0x3E0) >> 5;
		step4Chunk[2] = (step3Result & 0x7C00) >> 10;
		step4Chunk[3] = (step3Result & 0xF8000) >> 15;
		step4Chunk[4] = (step3Result & 0x1F00000) >> 20;
		step4Chunk[5] = (step3Result & 0x3E000000) >> 25;

		// Step 5: OR each value with 0x20 if another bit chunk follows
		int index = -1;
		for (int i = 5; i >= 1; --i)
		{
			if (step4Chunk[i] != 0)
			{
				index = i - 1;
				break;
			}
		}
		int step5Chunk[6] = { 0 };
		for (int i = 0; i <= index; ++i)
		{
			step5Chunk[i] = (step4Chunk[i] | 0x20);
		}
		for (int i = index + 1; i < 6; ++i)
		{
			step5Chunk[i] = step4Chunk[i];
		}

		int validCount = index + 2;

		// Step 6: Add 63 to each value
		char step6Chunk[6] = { 0 };
		for (int i = 0; i < validCount; ++i)
		{
			step6Chunk[i] = step5Chunk[i] + 63;
		}

		// Step 7: Convert each value to its ASCII equivalent
		aOutStr[0] = step6Chunk[0];
		aOutStr[1] = step6Chunk[1];
		aOutStr[2] = step6Chunk[2];
		aOutStr[3] = step6Chunk[3];
		aOutStr[4] = step6Chunk[4];
		aOutStr[5] = step6Chunk[5];
		aOutStr[6] = '\0';

		*aOutSize = validCount;

		return true;

	}

	bool Decode(char* aInStr, int* aOutput, int* aOutProcCount)
	{
		if (aInStr == NULL || aOutput == NULL || aOutProcCount == NULL)
		{
			return false;
		}

		if (aInStr[0] == '\0')
		{
			return false;
		}

		// Step 1: Substract 63 and check the 0x20 for the last bit chunk
		int validCount = -1;
		int step1Chunk[6] = { 0 };
		for (int i = 0; i < 7; ++i)
		{
			if (aInStr[i] != '\0')
			{
				step1Chunk[i] = aInStr[i] - 63;
			}
			else
			{
				// Error: we should see the last chunk first before meet the zero value chunk.
				return false;
			}

			if (!(step1Chunk[i] & 0x20))
			{
				validCount = i + 1;
				break;
			}

			// Remove the 0x20 bit
			step1Chunk[i] = step1Chunk[i] & 0x1F;
		}

		// Step 2: Merge the 5-bit chunks into single binary value in reverse order.
		int step2Result = 0;
		for (int i = 0; i < 6; ++i)
		{
			step2Result = step2Result | (step1Chunk[i] << i * 5);
		}

		// Step 3: If the first bit is 1, it is a negative number that should be inverted.
		int sign = (step2Result & 0x1) ? -1 : 1;
		int step3Result = (sign == -1) ? ~step2Result : step2Result;

		// Step 4: Right-shift the binary value one bit.
		// We don't have to add 1 at leftmost bit for negative number
		// since on a two's complement system, for signed numbers,
		// the arithmetic right-shift will take care of it.
		int step4Result = step3Result >> 1;

		*aOutput = step4Result;
		*aOutProcCount = validCount;

		return true;
	}

	bool EncodeArray(int* aInputArray, int aArraySize, char* aOutStr, int* aOutSize)
	{
		if (aInputArray == NULL || aOutStr == NULL || aOutSize == NULL)
		{
			return false;
		}

		int totalSize = 0;
		int outSize = 0;
		for (int i = 0; i < aArraySize; ++i)
		{
			if (!Encode(aInputArray[i], aOutStr + totalSize, &outSize))
			{
				return false;
			}
			totalSize += outSize;
		}

		*aOutSize = totalSize;

		return true;
	}

	bool DecodeArray(char* aInStr, int* aOutputArray, int* aOutArraySize)
	{
		if (aInStr == NULL || aOutputArray == NULL || aOutArraySize == NULL)
		{
			return false;
		}

		int index = 0;
		int offset = 0;

		while (*(aInStr + offset) != '\0')
		{
			int decodedResult = 0;
			int procCount = 0;
			if (Decode(aInStr + offset, &decodedResult, &procCount))
			{
				aOutputArray[index] = decodedResult;
				offset += procCount;
				index++;
			}
			else
			{
				*aOutArraySize = 0;
				return false;
			}
		}

		*aOutArraySize = index;

		return true;
	}

	bool EncodePolyline(double* aInputPolyline, int aPolylineSize, char* aOutStr, int* aOutSize)
	{
		if (aInputPolyline == NULL || aOutStr == NULL || aOutSize == NULL)
		{
			return false;
		}

		// Convert to decimal value and multiply by 1e5
		int* integerInputData = new int[aPolylineSize];
		for (int i = 0; i < aPolylineSize; ++i) 
		{
			integerInputData[i] = static_cast<int>(aInputPolyline[i] * 100000.0);
		}

		// Prepare for delta polyline
		int* polylineData = new int[aPolylineSize];
		if (polylineData == NULL)
		{
			return false;
		}

		int prevLat = integerInputData[0];
		int prevLon = integerInputData[1];
		polylineData[0] = prevLat;
		polylineData[1] = prevLon;
		int rowCount = aPolylineSize / 2;
		for (int i = 1; i < rowCount; ++i)
		{
			polylineData[i * 2 + 0] = integerInputData[i * 2 + 0] - prevLat;
			polylineData[i * 2 + 1] = integerInputData[i * 2 + 1] - prevLon;
			prevLat = integerInputData[i * 2 + 0];
			prevLon = integerInputData[i * 2 + 1];
		}

		bool ok = GooglePolylineCodec::EncodeArray(polylineData, aPolylineSize, aOutStr, aOutSize);

		if (!ok)
		{
			*aOutSize = 0;
		}

		delete[] polylineData;

		return ok;
	}

	bool DecodePolyline(char* aInStr, double* aOutputPolyline, int* aOutPolylineSize)
	{
		if (aInStr == NULL || aOutputPolyline == NULL || aOutPolylineSize == NULL)
		{
			return false;
		}

		int decodedResult[1024] = { 0 };
		int resultSize = 0;
		bool ok = GooglePolylineCodec::DecodeArray(aInStr, decodedResult, &resultSize);
		if (!ok)
		{
			*aOutPolylineSize = 0;
			return false;
		}

		int* integerData = new int[resultSize];
		if (integerData == NULL)
		{
			*aOutPolylineSize = 0;
			return false;
		}

		int prevLat = decodedResult[0];
		int prevLon = decodedResult[1];
		integerData[0] = prevLat;
		integerData[1] = prevLon;
		int roawCount = resultSize / 2;
		for (int i = 1; i < roawCount; ++i)
		{
			integerData[i * 2 + 0] = decodedResult[i * 2 + 0] + prevLat;
			integerData[i * 2 + 1] = decodedResult[i * 2 + 1] + prevLon;
			prevLat = integerData[i * 2 + 0];
			prevLon = integerData[i * 2 + 1];
		}

		for (int i = 0; i < resultSize; ++i)
		{
			aOutputPolyline[i] = static_cast<double>(integerData[i]) / 100000.0;
		}

		*aOutPolylineSize = resultSize;

		return true;
	}

}