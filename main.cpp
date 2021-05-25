// main.cpp
//
#include "stdafx.h"

#include "GooglePolylineCodec.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

void TestSingleValue()
{
	std::cout << "============================" << std::endl;
	std::cout << "Start TestSingleValue()" << std::endl;

	int inputData[] = {
		-17998321,
		3850000,
		-12020000,
		220000,
		-75000,
		255200,
		-550300,
		0
	};

	const int inputDataSize = sizeof(inputData) / sizeof(inputData[0]);

	char* expectedCode[] = {
		"`~oia@",
		"_p~iF",
		"~ps|U",
		"_ulL",
		"nnqC",
		"_mqN",
		"vxq`@",
		"?"
	};

	int totalCount = inputDataSize;
	int encodeFailureCount = 0;
	int decodeFailureCount = 0;

	for (int i = 0; i < totalCount; ++i)
	{
		char encodedStr[7] = { 0 };
		int outSize = 0;
		GooglePolylineCodec::Encode(inputData[i], encodedStr, &outSize);
		if (strcmp(encodedStr, expectedCode[i]) != 0)
		{
			encodeFailureCount++;
			std::cout << "Encode failure! Expected: " << expectedCode[i] << " Actual: " << encodedStr << std::endl;
		}

		int decodedResult = 0;
		int procCount = 0;
		GooglePolylineCodec::Decode(encodedStr, &decodedResult, &procCount);
		if (inputData[i] != decodedResult)
		{
			decodeFailureCount++;
			std::cout << "Decode failure! Expected: " << inputData[i] << " Actual: " << decodedResult << std::endl;
		}

	}

	std::cout << "Total Test Count:" << totalCount << std::endl;
	std::cout << "Encode Failure Count:" << encodeFailureCount << std::endl;
	std::cout << "Decode Failure Count:" << decodeFailureCount << std::endl;

	if (encodeFailureCount>0 || decodeFailureCount>0)
	{
		std::cout << "Test Failed!" << std::endl;
	}
	else
	{
		std::cout << "Test Pass!" << std::endl;
	}

	std::cout << "End TestSingleValue()" << std::endl;
	std::cout << "============================" << std::endl << std::endl;
}

void TestArrayValue()
{
	std::cout << "============================" << std::endl;
	std::cout << "Start TestArrayValue()" << std::endl;

	int inputData[] = {
		-17998321,
		3850000,
		-12020000,
		220000,
		-75000,
		255200,
		-550300,
		0
	};

	const int inputDataSize = sizeof(inputData) / sizeof(inputData[0]);

	char* expectedPolylineCode = "`~oia@_p~iF~ps|U_ulLnnqC_mqNvxq`@?";

	bool testEncodeFailed = false;
	bool testDecodeFailed = false;

	char encodedStr[128] = { 0 };
	int encodedStrSize = 0;
	GooglePolylineCodec::EncodeArray(inputData, 8, encodedStr, &encodedStrSize);
	if (strcmp(encodedStr, expectedPolylineCode) != 0)
	{
		std::cout << "Encode failure! Expected: " << expectedPolylineCode << " Actual: " << encodedStr << std::endl;
		testEncodeFailed = true;
	}

	int result[inputDataSize] = { 0 };
	int resultSize = 0;
	GooglePolylineCodec::DecodeArray(encodedStr, result, &resultSize);

	if (resultSize != inputDataSize)
	{
		std::cout << "Decode Result Count Not Matched! Expected: " << inputDataSize << " Actual: " << resultSize << std::endl;
		testDecodeFailed = true;
	}
	else
	{
		for (int i = 0; i < resultSize; ++i)
		{
			if (result[i] != inputData[i])
			{
				std::cout << "Decode Result Not Matched! Expected: " << inputData[i] << " Actual: " << result[i] << std::endl;
				testDecodeFailed = true;
			}
		}
	}

	if (testEncodeFailed || testDecodeFailed)
	{
		std::cout << "Test Failed!" << std::endl;
	}
	else
	{
		std::cout << "Test Pass!" << std::endl;
	}

	std::cout << "End TestArrayValue()" << std::endl;
	std::cout << "============================" << std::endl << std::endl;
}

void TestPolyline()
{
	std::cout << "============================" << std::endl;
	std::cout << "Start TestPolyline()" << std::endl;

	double inputPolylineData[] = {
		25.06427, 121.65832,
		25.06222, 121.65338,
		25.06165, 121.65173,
		25.06133, 121.64797,
		25.06097, 121.64644,
		25.06044, 121.64527,
		25.05991, 121.64446,
		25.05853, 121.64267,
		25.05731, 121.64127,
		25.05625, 121.63958,
		25.05601, 121.63857,
		25.05591, 121.63755,
		25.05626, 121.63561
	};

	const int inputDataSize = sizeof(inputPolylineData) / sizeof(inputPolylineData[0]);

	char expectedPolylineStr[] = "uj~wCokpeVxKz]pBhI~@nVfApHhBhFhB`DrGdJrFvGrEpIn@hERjEeAbK";

	bool ok = true;
	bool testEncodeFailed = false;
	bool testDecodeFailed = false;

	char encodedStr[1024] = { 0 };
	int encodedStrSize = 0;
	ok = GooglePolylineCodec::EncodePolyline(inputPolylineData, inputDataSize, encodedStr, &encodedStrSize);
	if (!ok || strcmp(encodedStr, expectedPolylineStr) != 0)
	{
		std::cout << "Encode failure! Expected: " << expectedPolylineStr << " Actual: " << encodedStr << std::endl;
		testEncodeFailed = true;
	}

	double decodedResult[1024] = { 0.0 };
	int resultSize = 0;
	ok = GooglePolylineCodec::DecodePolyline(encodedStr, decodedResult, &resultSize);
	if (ok)
	{
		for (int i = 0; i < resultSize; ++i)
		{
			if (decodedResult[i] != inputPolylineData[i])
			{
				std::cout << "Decode Result Not Matched! Expected: " << inputPolylineData[i] << " Actual: " << decodedResult[i] << std::endl;
				testDecodeFailed = true;
			}
		}
	}
	else
	{
		std::cout << "Decode failure!" << std::endl;
		testDecodeFailed = true;
	}

	if (testEncodeFailed || testDecodeFailed)
	{
		std::cout << "Test Failed!" << std::endl;
	}
	else
	{
		std::cout << "Test Pass!" << std::endl;
	}

	std::cout << "End TestPolyline()" << std::endl;
	std::cout << "============================" << std::endl << std::endl;
}

int main()
{
	TestSingleValue();

	TestArrayValue();

	TestPolyline();

	return 0;
}
