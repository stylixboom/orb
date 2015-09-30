/*
 *  orb.h
 *
 * 	ORB INS container and I/O handler
 * 
 *  Created on: September 29, 2015
 *      Author: Siriwat Kasamwattanarote
 */
#pragma once

#define ORB_BYTE 32
#define BIT_PER_BYTE 8
#define D ORB_BYTE*BIT_PER_BYTE	// 256 bits
#define HEADSIZE 5

using namespace std;
using namespace cv;

class orb
{
	// Default parameters of ORB
	int nfeatures=500;			// 900
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
	
	// Color space
	const static int RGB_SPACE = 10;
	const static int IRGB_SPACE = 11;
	const static int LAB_SPACE = 12;

	// Memory management flag
	bool has_kp;
	bool has_desc;

public:
	orb(int Colorspace = RGB_SPACE, bool isNormalizePt = false, bool isCheckFile = true);
	~orb(void);
	void init(int Colorspace = RGB_SPACE, bool isNormalizePt = false, bool isCheckFile = true);
	static int GetORBD() { return D; };
	static int GetORBHeadSize() { return HEADSIZE; };

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
};
//;)
