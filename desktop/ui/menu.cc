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
			bool depth,
			int trial_start_btn_location,
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

		trigpalmbase = false, trigpalmfree = false, trigpinch = false, trigtap = false, trigdwell = true, trigtapdepth = false, trigtapdepthsingle = false, trigdepthdistance = false;
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
			case 5:
				trigdwell = false;
				trigtap = true;
				break;
			case 8:
				trigdwell = false;
				trigtapdepth = true;
				break;
			case 9:
				trigdwell = false;
				trigtapdepthsingle = true;
				break;
			case 10:
				trigdwell = false;
				trigdepthdistance = true;
				break;
			default:
				break;
		}


		trial_start_btn_location_left = true;
		trial_start_btn_location_center = false;
		switch (trial_start_btn_location) {
			case 2:
				trial_start_btn_location_left = false;
				trial_start_btn_location_center = true;
				break;
			default:
				break;
		}

		scalex = width/8, scaley = height/8;

		errormsg = "";

		_debug = debug;

		_depth = depth;

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
		// TODO map enter keypress to next button
		while(true) {
			// clear the frame
			frame = cv::Scalar(79,79,79);
			// render a message in the frame at position (10, 15)
			
			// TODO make UI relative to width and height
			
			cvui::checkbox(frame, width - 120, height - 100, "Debug", &_debug);
			cvui::checkbox(frame, width - 120, height - 130, "Depth", &_depth);

			cvui::window(frame, scalex + 10, scaley + 10, 100, 80, "Initiator");
			cvui::checkbox(frame, scalex + 15, scaley + 30, "One Hand", &onehand);
			cvui::checkbox(frame, scalex + 15, scaley + 50, "Two Hand", &twohand);


			cvui::window(frame, scalex + 10, scaley + 100, 100, 120, "Anchor");
			cvui::checkbox(frame, scalex + 15, scaley + 130, "Dynamic", &ancdyn);
			cvui::checkbox(frame, scalex + 15, scaley + 150, "Static", &ancstat);
			cvui::checkbox(frame, scalex + 15, scaley + 170, "MidAir", &ancmid);


			cvui::window(frame, scalex + 120, scaley + 10, 200, 180, "Trigger");
			cvui::checkbox(frame, scalex + 125, scaley + 30, "Thumb of base palm", &trigpalmbase);
			cvui::checkbox(frame, scalex + 125, scaley + 50, "Shoot", &trigpalmfree); // "Thumb of free palm"
			cvui::checkbox(frame, scalex + 125, scaley + 70, "Pinch with free palm", &trigpinch);
			cvui::checkbox(frame, scalex + 125, scaley + 90, "Tap Depth Distance", &trigdepthdistance);
			cvui::checkbox(frame, scalex + 125, scaley + 110, "Tap", &trigtap);
			cvui::checkbox(frame, scalex + 125, scaley + 130, "Dwell", &trigdwell);
			cvui::checkbox(frame, scalex + 125, scaley + 150, "Tap Depth", &trigtapdepth);
			cvui::checkbox(frame, scalex + 125, scaley + 170, "Tap Depth Single", &trigtapdepthsingle);

			if (cellcnt < 3) cellcnt = 3;
			if (cellcnt > 10) cellcnt = 10;
			cvui::window(frame, scalex + 120, scaley + 200, 200, 50, "Number of cells per row/col");
			cvui::counter(frame, scalex + 175, scaley + 225, &cellcnt);
			
			cvui::window(frame, scalex + 330, scaley + 10, 100, 80, "Screen Size");
			cvui::checkbox(frame, scalex + 330, scaley + 30, "Small", &screen_small);
			cvui::checkbox(frame, scalex + 330, scaley + 50, "Large", &screen_large);
			cvui::checkbox(frame, scalex + 330, scaley + 70, "Fullscreen", &screen_full);

			cvui::window(frame, scalex + 330, scaley + 100, 100, 70, "Visibility");
			cvui::checkbox(frame, scalex + 330, scaley + 120, "Fixed", &visibility_fixed);
			cvui::checkbox(frame, scalex + 330, scaley + 140, "Conditional", &visibility_conditional);

			cvui::window(frame, scalex + 330, scaley + 180, 100, 70, "Trial Start Button");
			cvui::checkbox(frame, scalex + 330, scaley + 200, " Left", &trial_start_btn_location_left);
			cvui::checkbox(frame, scalex + 330, scaley + 220, "Center", &trial_start_btn_location_center);

			if (cvui::button(frame, width - 120, height - 50, 100, 30, "Next")) {
				if (is_valid()) {
					cv::destroyWindow(_window_name);
					break;
				}
			}

			cvui::text(frame, 10, height-50, errormsg, 0.5, 0xFEFEFE);

			// Show window content
			cvui::imshow(_window_name, frame);

			int wait_return = cv::waitKey(20);
			
			if (wait_return == 13) { // enter key pressed
				if (is_valid()) {
					cv::destroyWindow(_window_name);
					break;
				}
			}

			if (wait_return == 27) {
				cv::destroyWindow(_window_name);
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
		int & debug,
		int & depth,
		int & trial_start_btn_location) {

		if (onehand) initiator = 1;
		if (twohand) initiator = 2;

		if (ancdyn) anchor = 1;
		if (ancstat) anchor = 2;
		if (ancmid) anchor = 3;

		if (trigpalmbase) trigger = 1;
		if (trigpalmfree) trigger = 2;
		if (trigpinch) trigger = 3;
		if (trigtap) trigger = 5;
		if (trigdwell) trigger = 6;
		if (trigtapdepth) trigger = 8;
		if (trigtapdepthsingle) trigger = 9;
		if (trigdepthdistance) trigger = 10;


		if (screen_small) screensize = 1;
		else if (screen_large) screensize = 2; 
		else if (screen_full) screensize = 3;


		if (visibility_fixed) visibility = 1;
		else if (visibility_conditional) visibility = 2;

		divisions = cellcnt;

		debug = _debug;

		depth = _depth;

		if (trial_start_btn_location_left) trial_start_btn_location = 1;
		else if (trial_start_btn_location_center) trial_start_btn_location = 2;
	}


	void button(cv::Mat & frame, const std::string & label, bool & state) {
		if (cvui::button(frame, frame.rows - 120, frame.cols - 50, 100, 30, label)) {
			state = !state;
		} else {
			std::cerr << "could draw button for label:" << label << "\n";
		}
	}


	bool Menu::is_valid() {
		int cnt = 0;
		bool valid = true;

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
			if (trigtap) cnt ++;
			if (trigdwell) cnt ++;
			if (trigtapdepth) cnt ++;
			if (trigtapdepthsingle) cnt ++;
			if (trigdepthdistance) cnt ++;
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
			cnt = 0;
			if (trial_start_btn_location_left) cnt ++;
			if (trial_start_btn_location_center) cnt ++;
			if (cnt != 1) {
				valid = 0;
				errormsg = "Select exactly 1 location for trial start button.";
			}
		}


		if ((trigtapdepth || trigdepthdistance) && !_depth) {
			valid = 0;
			errormsg = "depth camera should be used for depth based tap.";
		}

		return valid;
	}
}



