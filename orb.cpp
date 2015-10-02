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
#include "../alphautils/imtools.h"

#include "orb.h"

using namespace std;
using namespace cv;
using namespace alphautils;
using namespace alphautils::imtools;

orb::orb(int Colorspace, bool isNormalizePt, bool isCheckFile)
{
    init(Colorspace, isNormalizePt, isCheckFile);
}

orb::~orb(void)
{
	reset();
}

void orb::init(int Colorspace, bool isNormalizePt, bool isCheckFile)
{
	normalize_pt = isNormalizePt;
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
                                 num_kp * (ORB_HEADSIZE * sizeof(float) + ORB_D * sizeof(float));	// Data size
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
            *((int*)buffer_ptr) = ORB_D;
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
				for (int desc_idx = 0; desc_idx < ORB_D; desc_idx++)
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
            OutFile << ORB_D << endl;
            OutFile << num_kp << endl;

            for(int kp_idx = 0; kp_idx < num_kp; kp_idx++)
            {
				// x y a b c
                OutFile << kp[kp_idx][0] << " " << kp[kp_idx][1] << " " << kp[kp_idx][2] << " " << kp[kp_idx][3] << " " << kp[kp_idx][4] << " ";
                for(size_t desc_pos = 0; desc_pos < ORB_D; desc_pos++)
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

            size_t actual_filesize = sizeof(width) + sizeof(height) + sizeof(dim) + sizeof(num_kp) + (num_kp * (ORB_HEADSIZE * sizeof(float) + dim * sizeof(float)));
            if (actual_filesize != buffer_size)
            {
                cout << "Feature file [" << in << "] is corrupted" << endl;
				exit(EXIT_FAILURE);
            }

			// Read header "x y a b c" and "all dimensions"
			for (int kp_idx = 0; kp_idx < num_kp; kp_idx++)
			{
				// Read header "x y a b c"
                float* read_kp = new float[ORB_HEADSIZE];
				for(int head_pos = 0; head_pos < ORB_HEADSIZE; head_pos++)
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

            size_t actual_filesize = sizeof(width) + sizeof(height) + sizeof(dim) + sizeof(ret_num_kp) + (ret_num_kp * (ORB_HEADSIZE * sizeof(float) + dim * sizeof(float)));
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
	Mat tmp = imread(imgPath);
	num_kp = extract(tmp);
	tmp.release();

	return num_kp;
}

int orb::extract(const Mat& imgMat)
{	
	reset();

	//Mat imgMat = imread(image_filename);
	Mat image(imgMat.rows, imgMat.cols, CV_32FC1, Scalar(0)); // float 1 channel

	float *out = image.ptr<float>(0);

    // Make gray scale by (b+g+r)/3
    if (colorspace == RGB_SPACE)
    {
        const uchar *in = imgMat.ptr<uchar>(0);
        for (size_t i = imgMat.rows * imgMat.cols; i > 0; i--)
        {
            *out = (float(in[0]) + in[1] + in[2]) / 3.0f;
            out++;
            in += 3;
        }
    }
    // Make gray scale by (0.2989 * R) + (0.5870 * G) + (0.1140 * B)
    // http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html
    else if (colorspace == IRGB_SPACE)
    {
        //Mat tmp_gray(imgMat.rows, imgMat.cols, CV_8UC1, Scalar(0));
        //cvtColor(imgMat, tmp_gray, CV_BGR2GRAY);
        const uchar *in = imgMat.ptr<uchar>(0);
        //const uchar *in = tmp_gray.ptr<uchar>(0);
        for (size_t i = imgMat.rows * imgMat.cols; i > 0; i--)
        {
            *out = 0.2989f * in[2] + 0.5870f * in[1] + 0.1140f * in[0];
            //*out = float(in[0]);
            out++;
            in += 3;
            //in++;
        }
    }
    // Make gray from L channel from OpenCV Lab
    else
    {
        //Convert BGR to LAB
        Mat tmp_lab(imgMat.rows, imgMat.cols, CV_8UC3, Scalar(0, 0, 0));
        cvtColor(imgMat, tmp_lab, CV_BGR2Lab);
        const uchar *in = tmp_lab.ptr<uchar>(0);
        for (size_t i = imgMat.rows * imgMat.cols; i > 0; i--)
        {
            *out = float(in[0]);
            //uchar Lv, av, bv;
            //rgb2lab(in[0], in[1], in[2], Lv, av, bv);
            //*out = float(Lv);
            out++;
            in += 3;
        }
    }
	
	// Main ORB stage
	vector<KeyPoint> cv_keypoints;
	Mat cv_descriptors;
	
	ORB* orb_obj = new ORB(
    nfeatures,
    scaleFactor,
    nlevels,
    edgeThreshold,
    firstLevel,
    WTA_K,
    scoreType,
    patchSize );
	
	// ORB keypoint extraction
	orb_obj->detect(image, cv_keypoints);
	// ORB feature description
	orb_obj->compute(image, cv_keypoints, cv_descriptors);

    // Get width and height
    width = imgMat.cols;
    height = imgMat.rows;

	/*
	 *	public KeyPoint(float x,
		float y,
		float _size,
		float _angle)
	 */
	// Keep all data for INS
	num_kp = cv_keypoints.size();
	
	// Get descriptor ptr only once
	//uchar* cv_descriptors_ptr = cv_descriptors.ptr<uchar>(0);
	uchar* cv_descriptors_ptr = (uchar*)cv_descriptors.data;
	for (int kp_idx = 0; kp_idx < num_kp; kp_idx++)
	{
		kp.push_back(new float[ORB_HEADSIZE]);
		desc.push_back(new float[ORB_D]);
		float* curr_kp = kp.back();
		float* curr_desc = desc.back();
		
		// Keypoints
		// Point normalization to 0-1 scale
		if (normalize_pt)
		{
			curr_kp[0] = float(cv_keypoints[kp_idx].pt.x / width);	// x
			curr_kp[1] = float(cv_keypoints[kp_idx].pt.y / height);// y
		}
		else
		{
			curr_kp[0] = float(cv_keypoints[kp_idx].pt.x);			// x
            curr_kp[1] = float(cv_keypoints[kp_idx].pt.y);			// y
		}
		curr_kp[2] = float(cv_keypoints[kp_idx].size);			// a = size
		curr_kp[3] = float(cv_keypoints[kp_idx].angle);			// b = angle
		curr_kp[4] = 0.0f;										// c (ignore)
		
		// Descriptors
		for (int byte_idx = 0; byte_idx < ORB_BYTE; byte_idx++)		// 0-31
		{
			int full_bit_idx;
			for (int sub_bit_idx = 0; sub_bit_idx < BIT_PER_BYTE; sub_bit_idx++)	// 0-7
			{
				full_bit_idx = (byte_idx * BIT_PER_BYTE) + sub_bit_idx;
				// Copying each of D-bit to each of a descriptor dimension.
				curr_desc[full_bit_idx] = cv_descriptors_ptr[kp_idx * ORB_D + full_bit_idx];
			}
		}
	}
	

    // Flag memory allocated
    has_kp = true;
    has_desc = true;
	
	// Release memory
	delete orb_obj;
	cv_descriptors.release();

    return num_kp;
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

// Feature drawing generic
void orb::draw_feats(const string& in_img_path, const string& out_img_path, const string& feat_path, int draw_mode, int colorspace, bool normpoint, bool binary)
{
	Mat in_img = imread(in_img_path.c_str());

	draw_feats(in_img, feat_path, draw_mode, colorspace, normpoint, binary);

	imwrite(out_img_path.c_str(), in_img);
}

void orb::draw_feats(const string& in_img_path, const string& out_img_path, const vector<INS_KP>& in_keypoints, int draw_mode, int colorspace, bool normpoint)
{
	Mat in_img = imread(in_img_path.c_str());

	for (size_t kp_idx = 0; kp_idx < in_keypoints.size(); kp_idx++)
		draw_a_feat(in_img, in_keypoints[kp_idx], draw_mode, normpoint);

	imwrite(out_img_path.c_str(), in_img);
}

void orb::draw_feats(Mat& in_img, const string& feat_path, int draw_mode, int colorspace, bool normpoint, bool binary)
{
	if (str_contains(feat_path, "orb"))
	{
		orb orb_reader(colorspace, normpoint);
		orb_reader.importKeypoints(feat_path, binary);
		int num_kp = orb_reader.num_kp;
	
		for (int kp_idx = 0; kp_idx < num_kp; kp_idx++)
		{
			INS_KP curr_kp = {orb_reader.kp[kp_idx][0], orb_reader.kp[kp_idx][1], orb_reader.kp[kp_idx][2], orb_reader.kp[kp_idx][3], orb_reader.kp[kp_idx][4]};

			draw_a_feat(in_img, curr_kp, draw_mode, normpoint);
		}
	}
}

float orb::draw_a_feat(Mat& in_img, INS_KP in_keypoint, int draw_mode, bool normpoint)
{
	float ret_raw_degree = 0.0f;

	Point2f center(in_keypoint.x, in_keypoint.y);             // x, y
	if (normpoint)
	{
		center.x *= in_img.cols;
		center.y *= in_img.rows;
	}

	if (draw_mode == DRAW_POINT)
	{
		// void circle(Mat& in_img, Point center, int radius, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
		circle(in_img, center, 0, Scalar(0, 255, 0), 2, CV_AA);	// Point
	}
	else if (draw_mode == DRAW_CIRCLE)
	{
		// void circle(Mat& in_img, Point center, int radius, const Scalar& color, int thickness=1, int lineType=8, int shift=0)				
		if (in_keypoint.c == 0.0f)	// ORB has only a(size), b(angle)
			circle(in_img, center, in_keypoint.a, Scalar(0, 255, 0), 2, CV_AA);	// Circle
		circle(in_img, center, 0, Scalar(0, 255, 0), 2, CV_AA);	// Point
	}

	return ret_raw_degree;
}
//;)
