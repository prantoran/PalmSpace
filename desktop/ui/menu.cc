#include "menu.h"
// #define CVUI_DISABLE_COMPILATION_NOTICES // to turn off debug msgs from cvui

// if cvui.h is included in menu.h then redefinition error occurs
#define CVUI_IMPLEMENTATION
#include "cvui.h"

namespace PalmSpaceUI {

	Menu::Menu(
			int frame_width, 
			int frame_height,
			int anchor, 
			int trigger, 
			int initiator,
			int divisions, 
			int screensize,
			int visibility,
			bool debug, 
			std::string window_name
		) {
		
		width = frame_width;
		height = frame_height;
		_window_name = window_name;
		cvui::init(_window_name);
		// Create a frame
		frame = cv::Mat(cv::Size(width, height), CV_8UC3);
		// bool use_canny = false;
		low_threshold = 50, high_threshold = 150;
		cellcnt = divisions;

		onehand = true, twohand = false;
		if (initiator == 2) {
			twohand = true;
			onehand = false;
		}

		ancdyn = true, ancstat  = false, ancmid  = false; 
		if (anchor == 2) {
			ancdyn = false;
			ancstat = true;
		}

		if (anchor == 3) {
			ancdyn = false;
			ancmid = true;
		}

		trigpalmbase = false, trigpalmfree = false, trigpinch = false, trigwait = false, trigtap = false, trigdwell = true;
		switch (trigger) {
			case 1:
				trigdwell = false;
				trigpalmbase = true;
				break;
			case 2:
				trigdwell = false;
				trigpalmfree = true;
				break;
			case 3:
				trigdwell = false;
				trigpinch = true;
				break;
			case 4:
				trigdwell = false;
				trigwait = true;
				break;
			case 5:
				trigdwell = false;
				trigtap = true;
				break;
			default:
				break;
		}



		scalex = -10 + width/7, scaley = -10 + height/4;

		errormsg = "";
		valid = true;

		_debug = debug;



		screen_small = 1, screen_large = 0, screen_full = 0;
		if (screensize == 2) {
			screen_large = 1;
			screen_small = 0;
		} else if (screensize == 3) {
			screen_small = 0;
			screen_full = 1;
		}


		visibility_fixed = true, visibility_conditional = false;
		if (visibility == 2) {
			visibility_fixed = false;
			visibility_conditional = true;
		}
	}


	void Menu::run() {
		while(true) {
			// clear the frame
			frame = cv::Scalar(79,79,79);
			// render a message in the frame at position (10, 15)

			cvui::checkbox(frame, width - 120, height - 100, "Debug", &_debug);

			cvui::window(frame, scalex + 10, scaley + 10, 100, 80, "Initiator");
			cvui::checkbox(frame, scalex + 15, scaley + 30, "One Hand", &onehand);
			cvui::checkbox(frame, scalex + 15, scaley + 50, "Two Hand", &twohand);


			cvui::window(frame, scalex + 10, scaley + 100, 100, 120, "Anchor");
			cvui::checkbox(frame, scalex + 15, scaley + 130, "Dynamic", &ancdyn);
			cvui::checkbox(frame, scalex + 15, scaley + 150, "Static", &ancstat);
			cvui::checkbox(frame, scalex + 15, scaley + 170, "MidAir", &ancmid);


			cvui::window(frame, scalex + 120, scaley + 10, 200, 150, "Trigger");
			cvui::checkbox(frame, scalex + 125, scaley + 30, "Thumb of base palm", &trigpalmbase);
			cvui::checkbox(frame, scalex + 125, scaley + 50, "Shoot", &trigpalmfree); // "Thumb of free palm"
			cvui::checkbox(frame, scalex + 125, scaley + 70, "Pinch with free palm", &trigpinch);
			cvui::checkbox(frame, scalex + 125, scaley + 90, "Wait to select", &trigwait);
			cvui::checkbox(frame, scalex + 125, scaley + 110, "Tap", &trigtap);
			cvui::checkbox(frame, scalex + 125, scaley + 130, "Dwell", &trigdwell);

			if (cellcnt < 3) cellcnt = 3;
			if (cellcnt > 10) cellcnt = 10;
			cvui::window(frame, scalex + 120, scaley + 170, 200, 50, "Number of cells per row/col");
			cvui::counter(frame, scalex + 175, scaley + 195, &cellcnt);
			
			cvui::window(frame, scalex + 330, scaley + 10, 100, 80, "Screen Size");
			cvui::checkbox(frame, scalex + 330, scaley + 30, "Small", &screen_small);
			cvui::checkbox(frame, scalex + 330, scaley + 50, "Large", &screen_large);
			cvui::checkbox(frame, scalex + 330, scaley + 70, "Fullscreen", &screen_full);


			cvui::window(frame, scalex + 330, scaley + 100, 100, 80, "Visibility");
			cvui::checkbox(frame, scalex + 330, scaley + 120, "Fixed", &visibility_fixed);
			cvui::checkbox(frame, scalex + 330, scaley + 140, "Conditional", &visibility_conditional);

			if (cvui::button(frame, width - 120, height - 50, 100, 30, "Next")) {
				int cnt = 0;
				valid = true;
				if (onehand) cnt ++;
				if (twohand) cnt ++;
				if (cnt != 1) {
					valid = 0;
					errormsg = "Select exactly 1 initiator.";
				}

				if (valid) {
					cnt = 0;
					if (ancdyn) cnt ++;
					if (ancstat) cnt ++;
					if (ancmid) cnt ++;
					if (cnt != 1) {
						valid = 0;
						errormsg = "Select exactly 1 anchor.";
					}
				}

				if (valid) {
					cnt = 0;
					if (trigpalmbase) cnt ++;
					if (trigpalmfree) cnt ++;
					if (trigpinch) cnt ++;
					if (trigwait) cnt ++;
					if (trigtap) cnt ++;
					if (trigdwell) cnt ++;
					if (cnt != 1) {
						valid = 0;
						errormsg = "Select exactly 1 trigger.";
					}
				}

				if (valid) {
					cnt = 0;
					if (screen_large) cnt ++;
					if (screen_small) cnt ++;
					if (screen_full) cnt ++;
					if (cnt != 1) {
						valid = 0;
						errormsg = "Select exactly 1 screen size.";
					}
				}

				if (valid) {
					cnt = 0;
					if (visibility_conditional) cnt ++;
					if (visibility_fixed) cnt ++;
					if (cnt != 1) {
						valid = 0;
						errormsg = "Select exactly 1 visibility.";
					}
				}

				if (valid) {
					cv::destroyWindow(_window_name);
					break;
				}
			}


			if (!valid) {
				cvui::text(frame, 10, height-50, errormsg, 0.5, 0xFEFEFE);
			}
			// Show window content
			cvui::imshow(_window_name, frame);

			// your app logic here
			if (cv::waitKey(20) == 27) {
				break;
			}
		}
	}


	void Menu::get_choices(
		int & initiator, 
		int & anchor, 
		int & trigger, 
		int & divisions,
		int & screensize,
		int & visibility,
		int & debug) {

		if (onehand) initiator = 1;
		if (twohand) initiator = 2;

		if (ancdyn) anchor = 1;
		if (ancstat) anchor = 2;
		if (ancmid) anchor = 3;

		if (trigpalmbase) trigger = 1;
		if (trigpalmfree) trigger = 2;
		if (trigpinch) trigger = 3;
		if (trigwait) trigger = 4;
		if (trigtap) trigger = 5;
		if (trigdwell) trigger = 6;


		if (screen_small) screensize = 1;
		else if (screen_large) screensize = 2; 
		else if (screen_full) screensize = 3;


		if (visibility_fixed) visibility = 1;
		else if (visibility_conditional) visibility = 2;

		divisions = cellcnt;

		debug = _debug;
	}

}
