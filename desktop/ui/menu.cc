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
			bool trial_pause_before_each_target,
			bool trial_show_button_during_trial,
			int targets_cnt,
			int inputspace,
			std::string window_name
		) {

		m_exit = false;
		
		width = frame_width;
		height = frame_height;
		_window_name = window_name;
		cvui::init(_window_name);
		// Create a frame
		frame = cv::Mat(cv::Size(width, height), CV_8UC3);
		// bool use_canny = false;
		low_threshold = 50, high_threshold = 150;
		cellcnt = divisions;
		if (!(cellcnt%2)) {
			cellcnt ++;
		} 

		onehand = true, twohand = false;
		if (initiator == 2) {
			twohand = true;
			onehand = false;
		}

		ancdyn = false, ancstat  = false, anchandtoscreen = false, ancpad = false, ancpadlarge = false; 
		switch (anchor) {
			case 1:
				ancdyn = true;
				break;
			case 2:
				ancstat = true;
				break;
			case 3:
				anchandtoscreen = true;
				break;
			case 4:
				ancpad = true;
				break;
			case 5:
				ancpadlarge = true;
				break;
			default:
				break;
		}

		trigpalmbase = false, trigpalmfree = false, trigpinch = false, trigtap = false, trigdwell = false, trigtapdepth = false, trigtapdepthsingle = false, trigdepthdistance = false;
		switch (trigger) {
			case 1:
				trigpalmbase = true;
				break;
			case 2:
				trigpalmfree = true;
				break;
			case 3:
				trigpinch = true;
				break;
			case 5:
				trigtap = true;
				break;
			case 6:
				trigdwell = true;
				break;
			case 8:
				trigtapdepth = true;
				break;
			case 9:
				trigtapdepthsingle = true;
				break;
			case 10:
				trigdepthdistance = true;
				break;
			default:
				break;
		}


		trial_start_btn_location_left = false;
		trial_start_btn_location_center = false;
		trial_start_btn_location_left_center = false;
		trial_start_btn_location_right_center = false;

		switch (trial_start_btn_location) {
			case 1:
				trial_start_btn_location_left = true;
				break;
			case 2:
				trial_start_btn_location_center = true;
				break;
			case 3:
				trial_start_btn_location_left_center = true;
				break;
			case 4:
				trial_start_btn_location_right_center = true;
			default:
				break;
		}

		scalex = width/8, scaley = height/8;

		errormsg = "";

		_debug = debug;

		_depth = depth;

		screen_small = false, screen_large = false, screen_full = false, screen_400 = false;
		switch(screensize) {
			case 1:
				screen_small = true;
				break;
			case 2:
				screen_large = true;
				break;
			case 3:
				screen_full  = true;
				break;
			case 4:
				screen_400   = true;
				break;
			default:
				break;
		}

		visibility_fixed = true, visibility_conditional = false;
		if (visibility == 2) {
			visibility_fixed = false;
			visibility_conditional = true;
		}

		m_trial_pause_before_each_target = false;
		if (trial_pause_before_each_target) m_trial_pause_before_each_target = true;

		m_trial_show_button_during_trial = false;
		if (trial_show_button_during_trial) m_trial_show_button_during_trial = true;

		m_targetscnt = targets_cnt;


		m_inputspace_palmsized = false, m_inputspace_sameasscreensize = false;
		switch(inputspace) {
			case 1:
				m_inputspace_sameasscreensize = 1;
				break;
			case 2:
				m_inputspace_palmsized = 2;
				break;
			default:
				std::cout << "ERROR menu.cc Menu() invalid inputspace choice\n";
				break;
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

			#ifdef REALSENSE_CAM
				cvui::checkbox(frame, width - 120, height - 130, "Depth", &_depth);
			#endif
			// cvui::window(frame, scalex + 10, scaley + 10, 100, 80, "Initiator");
			// cvui::checkbox(frame, scalex + 15, scaley + 30, "One Hand", &onehand);
			// cvui::checkbox(frame, scalex + 15, scaley + 50, "Two Hand", &twohand);


			cvui::window(frame, scalex - 20, scaley + 10, 130, 100, "Technique");
			cvui::checkbox(frame, scalex - 15, scaley + 30, "S2H - relative", &ancdyn);
			// cvui::checkbox(frame, scalex - 15, scaley + 150, "H2S - absolute", &ancstat);
			// cvui::checkbox(frame, scalex - 15, scaley + 170, "MidAir", &ancmid);
			// cvui::checkbox(frame, scalex - 15, scaley + 190, "Pad", &ancpad);
			cvui::checkbox(frame, scalex - 15, scaley + 50, "S2H - absolute", &ancpadlarge);
			cvui::checkbox(frame, scalex - 15, scaley + 70, "H2S - absolute", &anchandtoscreen);

			

			cvui::window(frame, scalex + 120, scaley + 10, 200, 80, "Trigger");
			cvui::checkbox(frame, scalex + 125, scaley + 30, "Dwell", &trigdwell);
			
			// cvui::checkbox(frame, scalex + 125, scaley + 50, "Tap Depth", &trigtapdepth);
			cvui::checkbox(frame, scalex + 125, scaley + 50, "Tap", &trigtap);
			// cvui::checkbox(frame, scalex + 125, scaley + 30, "Thumb of base palm", &trigpalmbase);
			// cvui::checkbox(frame, scalex + 125, scaley + 50, "Shoot", &trigpalmfree); // "Thumb of free palm"
			// cvui::checkbox(frame, scalex + 125, scaley + 70, "Pinch with free palm", &trigpinch);
			// cvui::checkbox(frame, scalex + 125, scaley + 90, "Tap Depth Distance", &trigdepthdistance);
			// cvui::checkbox(frame, scalex + 125, scaley + 170, "Tap Depth Single", &trigtapdepthsingle);

			if (cellcnt < 3) cellcnt = 9;
			if (cellcnt > 9) cellcnt = 3;
			cvui::window(frame, scalex + 120, scaley + 100, 200, 50, "Number of cells per row/col");
			cvui::counter(frame, scalex + 175, scaley + 125, &cellcnt, 2);
			
			cvui::window(frame, scalex + 120, scaley + 160, 200, 60, "Number of targets");
			cvui::counter(frame, scalex + 175, scaley + 190, &m_targetscnt, 1);
			if (m_targetscnt > 20) m_targetscnt = 1;
			if (m_targetscnt < 1) m_targetscnt = 20;

			// cvui::window(frame, scalex + 120, scaley + 160, 200, 80, "Trial options");
			// cvui::checkbox(frame, scalex + 125, scaley + 190, "Pause before each target", &m_trial_pause_before_each_target);
			// cvui::checkbox(frame, scalex + 125, scaley + 220, "Show button during trial", &m_trial_show_button_during_trial);
			


			cvui::window(frame, scalex + 330, scaley + 10, 100, 100, "Screen Size");
			cvui::checkbox(frame, scalex + 330, scaley + 30, "Small", &screen_small);
			cvui::checkbox(frame, scalex + 330, scaley + 50, "Large", &screen_large);
			cvui::checkbox(frame, scalex + 330, scaley + 70, "Fullscreen", &screen_full);
			cvui::checkbox(frame, scalex + 330, scaley + 90, "128x128", &screen_400);

			// cvui::window(frame, scalex + 330, scaley + 120, 100, 70, "Visibility");
			// cvui::checkbox(frame, scalex + 330, scaley + 140, "Fixed", &visibility_fixed);
			// cvui::checkbox(frame, scalex + 330, scaley + 160, "Conditional", &visibility_conditional);

			// cvui::window(frame, scalex + 330, scaley + 180, 100, 100, "Start Button");
			// cvui::checkbox(frame, scalex + 330, scaley + 200, " Left", &trial_start_btn_location_left);
			// cvui::checkbox(frame, scalex + 330, scaley + 220, "Center", &trial_start_btn_location_center);
			// cvui::checkbox(frame, scalex + 330, scaley + 240, "Left Center", &trial_start_btn_location_left_center);
			// cvui::checkbox(frame, scalex + 330, scaley + 260, "Right Center", &trial_start_btn_location_right_center);

			cvui::window(frame, scalex + 330, scaley + 120, 160, 70, "Input Space");
			cvui::checkbox(frame, scalex + 330, scaley + 140, "Same as screensize", &m_inputspace_sameasscreensize);
			cvui::checkbox(frame, scalex + 330, scaley + 160, "Palm sized", &m_inputspace_palmsized);


			if (cvui::button(frame, width - 120, height - 50, 100, 30, "Next")) {
				if (is_valid()) {
					cv::destroyWindow(_window_name);
					break;
				}
			}
			
			// if (cvui::button(frame, 20, height - 50, 100, 30, "Exit")) {
			// 	m_exit = true;

			// 	cv::destroyWindow(_window_name);
			// 	break;
				
			// }

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
		int & trial_start_btn_location,
		bool & trial_pause_before_each_target,
		bool & trial_show_button_during_trial,
		int & targets_cnt,
		int & inputspace) {

		if (onehand) initiator = 1;
		if (twohand) initiator = 2;

		if (ancdyn) anchor = 1;
		if (ancstat) anchor = 2;
		if (anchandtoscreen) anchor = 3;
		if (ancpad) anchor = 4;
		if (ancpadlarge) anchor = 5;

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
		else if (screen_400) screensize = 4;


		if (visibility_fixed) visibility = 1;
		else if (visibility_conditional) visibility = 2;

		divisions = cellcnt;

		debug = _debug;

		depth = _depth;

		if (trial_start_btn_location_left) trial_start_btn_location = 1;
		else if (trial_start_btn_location_center) trial_start_btn_location = 2;
		else if (trial_start_btn_location_left_center) trial_start_btn_location = 3;
		else if (trial_start_btn_location_right_center) trial_start_btn_location = 4;

		targets_cnt = m_targetscnt;

		if (m_inputspace_sameasscreensize) inputspace = 1;
		else if (m_inputspace_palmsized) inputspace = 2;
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
			valid = false;
			errormsg = "Select exactly 1 initiator.";
		}

		if (valid) {
			cnt = 0;
			if (ancdyn) cnt ++;
			if (ancstat) cnt ++;
			if (anchandtoscreen) cnt ++;
			if (ancpad) cnt ++;
			if (ancpadlarge) cnt ++;
			if (cnt != 1) {
				valid = false;
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
				valid = false;
				errormsg = "Select exactly 1 trigger.";
			}
		}

		if (valid) {
			cnt = 0;
			if (screen_large) cnt ++;
			if (screen_small) cnt ++;
			if (screen_full) cnt ++;
			if (screen_400) cnt ++;
			if (cnt != 1) {
				valid = false;
				errormsg = "Select exactly 1 screen size.";
			}
		}

		if (valid) {
			cnt = 0;
			if (visibility_conditional) cnt ++;
			if (visibility_fixed) cnt ++;
			if (cnt != 1) {
				valid = false;
				errormsg = "Select exactly 1 visibility.";
			}
		}


		if (valid) {
			cnt = 0;
			if (trial_start_btn_location_left) cnt ++;
			if (trial_start_btn_location_center) cnt ++;
			if (trial_start_btn_location_left_center) cnt ++;
			if (trial_start_btn_location_right_center) cnt ++;
			if (cnt != 1) {
				valid = false;
				errormsg = "Select exactly 1 location for trial start button.";
			}
		}

		if (valid) {
			if (ancdyn && screen_large) {
				valid = false;
				errormsg = "Better to use small screan size for relative hand to screen.";
			}
		}


		if ((trigtapdepth || trigdepthdistance) && !_depth) {
			valid = false;
			errormsg = "depth camera should be used for depth based tap.";
		}

		if (valid) {
			cnt = 0;
			if (m_inputspace_sameasscreensize) cnt ++;
			if (m_inputspace_palmsized) cnt ++;

			if (cnt != 1) {
				valid = false;
				errormsg = "Select exactly 1 choice for input space.";

			}
		}

		return valid;
	}
}



