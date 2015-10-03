/*
 * orb_extractor.cpp
 *
 *  Created on: October 1, 2015
 *      Author: Siriwat Kasamwattanarote
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <bitset>
#include <unistd.h> // usleep
#include <sys/types.h> // for checking exist file and dir
#include <sys/stat.h>

// Siriwat's header
#include "../alphautils/alphautils.h"
#include "../alphautils/imtools.h"
#include "orb.h"

using namespace std;
using namespace alphautils;
using namespace alphautils::imtools;

void readme(const string& exe_name)
{
	cout << "Params must be \"" << exe_name << " -i input_image [-o output_feature] [-d input_feature(display)] [-m output_mode:b|t] [-c colorspace:r(rgb)|i(irgb)|l(lab)] [-n normpoint:1|0] [-v:a|p, draw overlay in only binary mode]\"" << endl;
}

int main(int argc,char *argv[])
{
	if(argc < 2)
	{
		readme(string(argv[0]));
		exit(1);
	}

	string input_image = "";
	string output_feature = "";
	string input_feature = "";
	string output_overlay_image = "";
	bool isBinary = false;
	int colorspace = RGB_SPACE;
	bool normpoint = true;
	bool draw_mode = DRAW_POINT;

	if(argc > 2)
	{
		for(int count = 1; count < argc; count+=2)
		{
			char opt;
			if(argv[count][0] == '-')
				opt = argv[count][1];
			else
			{
				cout << "Incorrect parameter " << argv[count] << endl;
				exit(1);
			}

			switch(opt)
			{
			case 'i':
				input_image = string(argv[count + 1]);
				break;
			case 'o':
				output_feature = string(argv[count + 1]);
				break;
            case 'd':
                input_feature = string(argv[count + 1]);
                break;
			case 'm':
				if(argv[count + 1][0] == 'b')
					isBinary = true;
				break;
            case 'v':
                if (isBinary)
                {
                    output_overlay_image = input_image + "_overlay.png";
                    if (argv[count + 1][0] == 'p')
                        draw_mode = DRAW_POINT;
					else if (argv[count + 1][0] == 'c')
						draw_mode = DRAW_CIRCLE;
                }
                break;
            case 'c':
                if (argv[count + 1][0] == 'i')
                    colorspace = IRGB_SPACE;
				if (argv[count + 1][0] == 'l')
					colorspace = LAB_SPACE;
                break;
            case 'n':
				if(argv[count + 1][0] == '0')
					normpoint = false;
                break;
			}
		}
	}

    //input_image = "/dev/shm/test.jpg";

    if (input_feature != "")
    {
        orb feature_display;
        feature_display.importKeypoints(input_feature, isBinary);

        cout << feature_display.width << endl;
        cout << feature_display.height << endl;
        cout << orb::GetORBD() << endl;
        cout << feature_display.num_kp << endl;

        for(int kp_idx = 0; kp_idx < feature_display.num_kp; kp_idx++)
        {
            cout << feature_display.kp[kp_idx][0] << " " << feature_display.kp[kp_idx][1] << " " << feature_display.kp[kp_idx][2] << " " << feature_display.kp[kp_idx][3] << " " << feature_display.kp[kp_idx][4] << " ";
            for(size_t desc_pos = 0; desc_pos < ORB_D; desc_pos++)
                cout << feature_display.desc[kp_idx][desc_pos] << " ";
            cout << endl;
        }


        return EXIT_SUCCESS;
    }

	if(input_image == "")
	{
		readme(string(argv[0]));
		exit(1);
	}

	if(output_feature == "")
    {
        if (contain_path(input_image))
            output_feature = input_image + ".orb";
        else
            output_feature = "./" + input_image + ".orb";
    }

	orb orb_obj(colorspace, normpoint);

	cout << "Extract ORB..."; cout.flush();
	timespec startTime = CurrentPreciseTime();
	orb_obj.extract(input_image);
	cout << "done! (in " << setprecision(4) << fixed << TimeElapse(startTime) << " s)" << endl;

	cout << "Exporting result ";
	if(isBinary) cout << "[binary mode]...";
	else cout << "[text mode]...";
	cout.flush();
    orb_obj.exportKeypoints(output_feature, isBinary);
	cout << "OK!" << endl;

	if (output_overlay_image != "")
	{
        cout << "Drawing ORB overlay...";
        cout.flush();
		orb_obj.draw_feats(input_image, output_overlay_image, output_feature, draw_mode);
        cout << "done" << endl;
    }

    cout << "== ORB feature information ==" << endl;
    cout << "Total dimension: " << orb_obj.GetORBD() << endl;
    cout << "Total keypoint: " << orb_obj.num_kp << endl;


	return 0;
}
//;)
