/*
 *  orb.h
 *
 * 	ORB INS container and I/O handler
 * 
 *  Created on: September 29, 2015
 *      Author: Siriwat Kasamwattanarote
 */
#pragma once
 
#define D 128
#define HEADSIZE 5

using namespace std;
using namespace cv;

class orb
{
    bool check_file_exist;
		
    bool normalize_pt;

    // Memory management flag
    bool has_kp;
    bool has_desc;

public:
	orb(bool isNormalize = false, bool isCheckFile = true);
	~orb(void);
	void init(bool isNormalize = false, bool isCheckFile = true);
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
