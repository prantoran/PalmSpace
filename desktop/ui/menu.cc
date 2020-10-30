#include "menu.h"
// #define CVUI_DISABLE_COMPILATION_NOTICES // to turn off debug msgs from cvui

// if cvui.h is included in menu.h then redefinition error occurs
#define CVUI_IMPLEMENTATION
#include "cvui.h"

Menu::Menu(
		int FLAGS_frame_width, int FLAGS_frame_height,
		int choice_divisions, int choice_screensize,
		bool FLAGS_debug, std::string window_name
	  ) {
    
    width = FLAGS_frame_width;
    height = FLAGS_frame_height;
    _window_name = window_name;
	cvui::init(_window_name);
	// Create a frame
	frame = cv::Mat(cv::Size(width, height), CV_8UC3);
	// bool use_canny = false;
	low_threshold = 50, high_threshold = 150;
	cellcnt = choice_divisions;

	onehand = true, twohand = false;
	ancdyn = true, ancstat  = false, ancmid  = false; 
	trigpalmbase = false, trigpalmfree = false, trigpinch = false, trigwait = false, trigtap = false, trigdwell = true;

	scalex = -10 + width/6, scaley = -10 + height/4;
	std::cerr << "scalex:" << scalex << " scaley:" << scaley << "\n";

	errormsg = "";
	valid = true;

	bool _debug = FLAGS_debug;

	
}


void Menu::run() {
	while(true) {
		// std::cerr << "rendering ui\n";
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
		cvui::checkbox(frame, scalex + 125, scaley + 50, "Thumb of free palm", &trigpalmfree);
		cvui::checkbox(frame, scalex + 125, scaley + 70, "Pinch with free palm", &trigpinch);
		cvui::checkbox(frame, scalex + 125, scaley + 90, "Wait to select", &trigwait);
		cvui::checkbox(frame, scalex + 125, scaley + 110, "Tap", &trigtap);
		cvui::checkbox(frame, scalex + 125, scaley + 130, "Dwell", &trigdwell);

		if (cellcnt < 3) cellcnt = 3;
		if (cellcnt > 10) cellcnt = 10;
		cvui::window(frame, scalex + 120, scaley + 170, 200, 50, "Number of cells per row/col");
		cvui::counter(frame, scalex + 175, scaley + 195, &cellcnt);



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

  divisions = cellcnt;

  debug = _debug;
}
