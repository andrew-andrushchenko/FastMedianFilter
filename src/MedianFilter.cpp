#define EMPTY_ARGS_ERROR_CODE -1
#define INVALID_ARGS_ERROR_CODE -2
#define IMAGE_PROCESSING_ERROR_CODE -3

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include "image_proc/Image.h"
#include "filter/medianf.h"
#include "utils/ArgumentParser.h"

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		const char* usage_description_formatted = 
			"Usage: %s -f <filename> -k <ksize>\n"
			"Note: <ksize> should be an odd number.\n"
			"Currently it works for PNG, JPEG and BMP image formats\n";

		printf(usage_description_formatted, argv[0]);
		return EMPTY_ARGS_ERROR_CODE;
	}

	ConsoleArgParser parser(argc, argv);

	if (parser.CmdOptionExists("-f"))
	{
		const std::string& filename = parser.GetCmdOption("-f");
		if (!filename.empty())
		{
			if (parser.CmdOptionExists("-k"))
			{
				int kernel;
				try
				{
					kernel = std::abs(std::stoi(parser.GetCmdOption("-k")));
				}
				catch (...)
				{
					std::cerr << "FATAL ERROR: Kernel size should be a number." << std::endl;
					return INVALID_ARGS_ERROR_CODE;
				}

				Image in(filename.c_str());
				if (in.IsEmpty())
				{
					std::cerr << "FATAL ERROR: Invalid image path." << std::endl;
					return INVALID_ARGS_ERROR_CODE;
				}
				Image out;

				bool result = MedianSmooth(in, out, kernel);
				if (result)
				{
					std::string o_filename(filename);
					std::ostringstream s_stream;
					s_stream << "+med_filtered_" << kernel << "x" << kernel;
					o_filename.insert(o_filename.find_last_of("."), s_stream.str());
					out.WriteToFile(o_filename.c_str());
				}
				else
				{
					std::cerr << "FATAL ERROR: Failed to process image." << std::endl;
					return IMAGE_PROCESSING_ERROR_CODE;
				}
			}
			else
			{
				std::cerr << "FATAL ERROR: Kernel size was not defined." << std::endl;
				return INVALID_ARGS_ERROR_CODE;
			}
		}
	}
	else
	{
		std::cerr << "FATAL ERROR: expected a file for processing." << std::endl;
		return INVALID_ARGS_ERROR_CODE;
	}

	return 0;
}