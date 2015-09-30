/*
 *  orb.cpp
 *
 * 	ORB INS container and I/O handler
 * 
 *  Created on: September 29, 2015
 *      Author: Siriwat Kasamwattanarote
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>   // file-directory existing
#include <sys/types.h>  // file-directory
#include <dirent.h>     // file-directory

#include "opencv2/core/core.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// Siriwat's header
#include "../alphautils/alphautils.h"

#include "orb.h"

using namespace std;
using namespace cv;
using namespace alphautils;

orb::orb(bool isNormalize, bool isCheckFile)
{
    init(isNormalize, isCheckFile);
}

orb::~orb(void)
{
	reset();
}

void orb::init(bool isNormalize, bool isCheckFile)
{
	normalize_pt = isNormalize;
	check_file_exist = isCheckFile;
	num_kp = 0;
	has_kp = false;
	has_desc = false;
}

void orb::exportKeypoints(const string& out, bool isBinary)
{
	// Check directory
    string dir_name = get_directory(out);
    make_dir_available(dir_name);

    // Oxford format
	if(isBinary)
	{
		ofstream OutFile (out.c_str(), ios::binary);
		if (OutFile.is_open())
		{
		    /// Prepare buffer
		    size_t buffer_size = sizeof(int) * 4 + 											// Header size
                                 num_kp * (HEADSIZE * sizeof(float) + D * sizeof(float));	// Data size
                                 // sizeof(width) + sizeof(height) + sizeof(dim) + sizeof(num_kp) +
                                 // num_kp * (HEADSIZE * sizeof(kp) + D * sizeof(desc))
            char* buffer = new char[buffer_size];
            char* buffer_ptr = buffer;

		    /// Put buffer
            // Put width
            *((int*)buffer_ptr) = width;
            buffer_ptr += sizeof(int);

            // Put height
            *((int*)buffer_ptr) = height;
            buffer_ptr += sizeof(int);

			// Put dim
            *((int*)buffer_ptr) = D;
            buffer_ptr += sizeof(int);

			// Put num_kp
            *((int*)buffer_ptr) = num_kp;
            buffer_ptr += sizeof(int);

			// Put header "x y a b c" and "all dimensions"
			for(int kp_idx = 0; kp_idx < num_kp; kp_idx++)
			{
				// Put header x y a b c
                *((float*)buffer_ptr) = kp[kp_idx][0];
                buffer_ptr += sizeof(float);
                *((float*)buffer_ptr) = kp[kp_idx][1];
                buffer_ptr += sizeof(float);
                *((float*)buffer_ptr) = kp[kp_idx][2];
                buffer_ptr += sizeof(float);
                *((float*)buffer_ptr) = kp[kp_idx][3];
                buffer_ptr += sizeof(float);
                *((float*)buffer_ptr) = kp[kp_idx][4];
                buffer_ptr += sizeof(float);

				// Put data (e.g. 128D)
				for (int desc_idx = 0; desc_idx < D; desc_idx++)
                {
                    *((float*)buffer_ptr) = desc[kp_idx][desc_idx];
                    buffer_ptr += sizeof(float);
                }
			}

			// Write file from buffer
			OutFile.write(buffer, buffer_size);

			// Close file
			OutFile.close();

			// Release memory
			delete[] buffer;
		}
	}
	else
	{
        ofstream OutFile (out.c_str());
        if (OutFile.is_open())
        {
            OutFile << width << endl;
            OutFile << height << endl;
            OutFile << D << endl;
            OutFile << num_kp << endl;

            for(int kp_idx = 0; kp_idx < num_kp; kp_idx++)
            {
                OutFile << kp[kp_idx][0] << " " << kp[kp_idx][1] << " " << kp[kp_idx][2] << " " << kp[kp_idx][3] << " " << kp[kp_idx][4] << " ";
                for(size_t desc_pos = 0; desc_pos < D; desc_pos++)
                    OutFile << desc[kp_idx][desc_pos] << " ";
                OutFile << endl;
            }

            OutFile.close();
        }
	}
}

bool orb::importKeypoints(const string& in, bool isBinary)
{
	reset();

    if(check_file_exist && !is_path_exist(in)) // no exist
    {
        cout << "File \"" << in << "\" not found" << endl;
		return false;   // interpret as no keypoint
    }

    // Oxford format
	if(isBinary)
	{
	    // Prepare buffer
		ifstream InFile (in.c_str(), ios::binary);
		if (InFile)
		{
		    /// Create buffer
		    InFile.seekg(0, InFile.end);
            size_t buffer_size = InFile.tellg();
		    InFile.seekg(0, InFile.beg);
            char* buffer = new char[buffer_size];
            char* buffer_ptr = buffer;

		    // Read whole file into buffer
		    InFile.read(buffer, buffer_size);

			// Close file
			InFile.close();

			/// Interpret from buffer
			// Read width
            width = *((int*)buffer_ptr);
            buffer_ptr += sizeof(width);

            // Read height
            height = *((int*)buffer_ptr);
            buffer_ptr += sizeof(height);

			// Read dim
			// Same as D
            int dim;
            dim = *((int*)buffer_ptr);
            buffer_ptr += sizeof(dim);

			// Read num_kp
            num_kp = *((int*)buffer_ptr);
            buffer_ptr += sizeof(num_kp);

            size_t actual_filesize = sizeof(width) + sizeof(height) + sizeof(dim) + sizeof(num_kp) + (num_kp * (HEADSIZE * sizeof(float) + dim * sizeof(float)));
            if (actual_filesize != buffer_size)
            {
                cout << "Feature file [" << in << "] is corrupted" << endl;
				exit(EXIT_FAILURE);
            }

			// Read header "x y a b c" and "all dimensions"
			for (int kp_idx = 0; kp_idx < num_kp; kp_idx++)
			{
				// Read header "x y a b c"
                float* read_kp = new float[HEADSIZE];
				for(int head_pos = 0; head_pos < HEADSIZE; head_pos++)
                {
                    read_kp[head_pos] = *((float*)buffer_ptr);
                    buffer_ptr += sizeof(read_kp[head_pos]);
                }
                kp.push_back(read_kp);

				// Read data "eg. 128D"
                float* read_desc = new float[dim];
				for(int desc_pos = 0; desc_pos < dim; desc_pos++)
                {
                    read_desc[desc_pos] = *((float*)buffer_ptr);
                    buffer_ptr += sizeof(read_desc[desc_pos]);
                }
                desc.push_back(read_desc);
			}

			// Release buffer
			delete[] buffer;
		}
	}
	else
	{
        cout << "No implementation for reading feature text file" << endl;
        exit(EXIT_FAILURE);
        /*
        ofstream InFile (in.c_str());
        if (InFile)
        {
            InFile << D << endl;
            InFile << num_kp << endl;

            for(size_t kpIdx = 0; kpIdx < kp.size(); kpIdx++)
            {
                InFile << kp[kpIdx][0] << " " << kp[kpIdx][1] << " " << kp[kpIdx][2] << " " << kp[kpIdx][3] << " " << kp[kpIdx][4] << " ";
                for(size_t descIdx = 0; descIdx < desc[kpIdx].size(); descIdx++)
                    InFile << desc[kpIdx][descIdx] << " ";
                InFile << endl;
            }

            InFile.close();
        }
        */
	}

	// Flag memory allocated
    has_kp = true;
    has_desc = true;

    return true;
}

int orb::checkNumKp(const string& in, bool isBinary)
{
	int ret_num_kp = 0;

    if(check_file_exist && !is_path_exist(in)) // no exist
    {
        cout << "File \"" << in << "\" not found" << endl;
		return 0;   // interpret as no keypoint
    }

    // Oxford format
	if(isBinary)
	{
		ifstream InFile (in.c_str(), ios::binary);
		if (InFile)
		{
            int dim;
		    /// Create buffer
            size_t buffer_size = sizeof(width) + sizeof(height) + sizeof(dim) + sizeof(ret_num_kp);
            char* buffer = new char[buffer_size];
            char* buffer_ptr = buffer;

		    // Read whole file into buffer
		    InFile.read(buffer, buffer_size);

            InFile.seekg(0, InFile.end);
            size_t file_size = InFile.tellg();

			// Close file
			InFile.close();

			/// Interpret from buffer
			// Read width
            width = *((int*)buffer_ptr);
            buffer_ptr += sizeof(width);

            // Read height
            height = *((int*)buffer_ptr);
            buffer_ptr += sizeof(height);

			// Read dim
            dim = *((int*)buffer_ptr);
            buffer_ptr += sizeof(dim);

			// Read ret_num_kp
            ret_num_kp = *((int*)buffer_ptr);
            buffer_ptr += sizeof(ret_num_kp);

            size_t actual_filesize = sizeof(width) + sizeof(height) + sizeof(dim) + sizeof(ret_num_kp) + (ret_num_kp * (HEADSIZE * sizeof(float) + dim * sizeof(float)));
            if (actual_filesize != file_size)
            {
				cout << "Feature file [" << in << "] is corrupted: expect(" << actual_filesize << ") != actual(" << file_size << ") " << endl;
				exit(EXIT_FAILURE);
            }
            else
            // Stop reading the rest

			// Release buffer
			delete[] buffer;
		}
	}
	else
	{
        cout << "No implementation for reading feature text file" << endl;
        exit(EXIT_FAILURE);
        /*
        ofstream InFile (in.c_str());
        if (InFile)
        {
            InFile << D << endl;
            InFile << num_kp << endl;

            for(size_t kpIdx = 0; kpIdx < kp.size(); kpIdx++)
            {
                InFile << kp[kpIdx][0] << " " << kp[kpIdx][1] << " " << kp[kpIdx][2] << " " << kp[kpIdx][3] << " " << kp[kpIdx][4] << " ";
                for(size_t descIdx = 0; descIdx < desc[kpIdx].size(); descIdx++)
                    InFile << desc[kpIdx][descIdx] << " ";
                InFile << endl;
            }

            InFile.close();
        }
        */
	}

    // Keep num_kp
    num_kp = ret_num_kp;
	return ret_num_kp; // num_kp
}

int orb::extract(const string& imgPath)
{
}

int orb::extract(const Mat& img)
{
}

// Unlink function is used for giving the right (a flag)
// to an external caller for using the allocated pointer for feature kp/desc.
void orb::unlink_kp()
{
    has_kp = false;
}

void orb::unlink_desc()
{
    has_desc = false;
}

void orb::reset(void)
{
	// Release Mem
	if(num_kp)
	{
	    size_t actual_kp = kp.size();
		for(size_t kp_idx = 0; kp_idx < actual_kp; kp_idx++)
		{
			if (has_kp) delete[] kp[kp_idx];
			if (has_desc) delete[] desc[kp_idx];
		}
		vector<float*>().swap(kp);
		vector<float*>().swap(desc);
		has_kp = has_desc = false;

		num_kp = 0;
	}
}
//;)
