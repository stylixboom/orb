/*
 *  orb.h
 *
 * 	ORB INS container and I/O handler
 * 
 *  Created on: September 29, 2015
 *      Author: Siriwat Kasamwattanarote
 */
#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"

// Siriwat's header
#include "../alphautils/imtools.h"

#define ORB_BYTE 32
#define BIT_PER_BYTE 8
#define ORB_D ORB_BYTE*BIT_PER_BYTE	// 256 bits
#define ORB_HEADSIZE 5

using namespace std;
using namespace cv;
using namespace alphautils::imtools;

class orb
{
	// Default parameters of ORB
	int nfeatures=900;			// 900
	float scaleFactor=1.2f;		// 4
	int nlevels=8;				//
	int edgeThreshold=15; // Changed default (31);
	int firstLevel=0;
	int WTA_K=2;
	int scoreType=ORB::HARRIS_SCORE;
	int patchSize=31;
	//int fastThreshold=20;
	
	bool check_file_exist;
	
	int colorspace;
	bool normalize_pt;

	// Memory management flag
	bool has_kp;
	bool has_desc;

public:	
	orb(int Colorspace = RGB_SPACE, bool isNormalizePt = false, bool isCheckFile = true);
	~orb(void);
	void init(int Colorspace = RGB_SPACE, bool isNormalizePt = false, bool isCheckFile = true);
	static int GetORBD() { return ORB_D; };
	static int GetORBHeadSize() { return ORB_HEADSIZE; };

	vector<float*> kp; // x y a b c
	vector<float*> desc; // x-descriptors
	int num_kp;
	int width, height;
	void exportKeypoints(const string& out, bool isBinary = true);
	bool importKeypoints(const string& in, bool isBinary = true);
	int checkNumKp(const string& in, bool isBinary = true);
	int extract(const string& imgPath);
	int extract(const Mat& img);
	void unlink_kp();
	void unlink_desc();
	void reset(void);
	
	// Feature drawing generic
	void draw_feats(const string& in_img_path, const string& out_img_path, const string& feat_path, int draw_mode, int colorspace = RGB_SPACE, bool normpoint = true, bool binary = true);
	void draw_feats(const string& in_img_path, const string& out_img_path, const vector<INS_KP>& in_keypoints, int draw_mode, int colorspace = RGB_SPACE, bool normpoint = true);
	void draw_feats(Mat& in_img, const string& feat_path, int draw_mode, int colorspace = RGB_SPACE, bool normpoint = true, bool binary = true);
	float draw_a_feat(Mat& in_img, INS_KP in_keypoint, int draw_mode, bool normpoint = true);
};
//;)
