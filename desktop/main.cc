
// An example of sending OpenCV webcam frames into a MediaPipe graph.
// This example requires a linux computer and a GPU with EGL support drivers.
#include <cstdlib>

#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/status.h"

#include "desktop/anchors/anchors.h"
#include "desktop/triggers/triggers.h"
#include "desktop/initiators/initiators.h"


#include "desktop/handlers/handlers.h";


// #include <boost/lambda/lambda.hpp>
// #include <boost/regex.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>

// #include "lib/crow_all.h"
// #include "lib/base64.h"

#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"

// #include <pthread.h>
// #include <mutex>
// #include <chrono>

// #include <websocketpp/config/asio_no_tls.hpp>
// #include <websocketpp/server.hpp>

// #include <boost/beast/core.hpp>
// #include <boost/beast/websocket.hpp>
// #include <boost/asio/dispatch.hpp>
// #include <boost/asio/strand.hpp>

#include <cstdlib>
#include <functional>
#include <memory>
#include <thread>
#include <vector>


// #include <rapidjson/document.h>
// #include "rapidjson/document.h"

// #include "webrtcserve/observers.h"

// #include <webrtc/api/peer_connection_interface.h>


// namespace beast = boost::beast;         // from <boost/beast.hpp>
// namespace http = beast::http;           // from <boost/beast/http.hpp>
// namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
// namespace net = boost::asio;            // from <boost/asio.hpp>
// using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

DEFINE_string(
    calculator_graph_config_file, 
    "mediapipe/graphs/hand_tracking/multi_hand_tracking_mobile.pbtxt",
    "Name of file containing text format CalculatorGraphConfig proto.");
DEFINE_string(input_video_path, "",
              "Full path of video to load. "
              "If not provided, attempt to use a webcam.");
DEFINE_string(output_video_path, "",
              "Full path of where to save result (.mp4 only). "
              "If not provided, show result in a window.");

DEFINE_int32(frame_width, 640, "frame/screen width in pixels.");
DEFINE_int32(frame_height, 480, "frame/screen height in pixels.");
DEFINE_int32(fps, 30, "frames per second.");
DEFINE_int32(debug, 0, "debug mode");

// DEFINE_string(image_ext, "webp", "File extension of the images to read.");
// DEFINE_int32(read_delay, 300, "delay of reading images in microseconds");
// DEFINE_int32(prefidx, 20, "the end of prefix to exclude"
            // " or the start of substring of base64 data received from server");
// DEFINE_string(wshost, "127.0.0.1", "boost:beast websocket ip");
// DEFINE_string(wsport, "8080", "boost:beast websocket port");


// const std::string dataURL_prefix = "data:image/png;base64,";

std::shared_ptr<MediaPipeMultiHandGPU> mp_graph = NULL;

// std::vector<std::queue<std::shared_ptr<cv::Mat>>> inq(1), outq(1);

// int reqCnt = 0;

// struct business {
//   bool is_busy;
//   std::mutex is_busy_mutex;
// } busy[1];

// void clearq(std::queue<cv::Mat> &q) {
//   std::queue<cv::Mat> empty;
//   std::swap(q, empty);
// }

// void server() {
//   crow::SimpleApp app;

//   // CROW_ROUTE(app, "/")([](){
//   // return "Hello world";
//   // });

//   CROW_ROUTE(app, "/worker")
//   .methods("POST"_method)
//   ([](const crow::request& req){
//     // std::cout << "entered workers/true\n";
//     std::string body = req.body;
//     // std::cout << "body:" << body << "\n";
//     if (body.length() <= FLAGS_prefidx) {
//       // std::cerr << "invalid request body:" << body << "\n";
//       crow::json::wvalue jsonResp;
//       jsonResp["img_id"] = ++reqCnt;
//       jsonResp["data_url"] = dataURL_prefix;

//       auto res = crow::response{std::move(jsonResp)};
//       res.add_header("Access-Control-Allow-Origin", "*");
//       res.add_header("Access-Control-Allow-Headers", "Content-Type");
//       res.add_header("Access-Control-Allow-Methods", "POST");
//       res.add_header("Access-Control-Max-Age", "86400");

//       // std::cout << "sending resp for reqcnt:" << reqCnt << "\n";
//       return res;

//     }
//     body = body.substr(FLAGS_prefidx);
//     // std::cout << "FLAGS_prefidx:" << FLAGS_prefidx <<" updated body:" << body << "\n";

//     std::vector<unsigned char> decoded = base64_decode(body);

//     std::string encoded = "";
//     cv::Mat matImg;
//     // matImg = cv::imdecode(cv::Mat(1, bufferLength, cv::CV_8UC1, buffer), cv::CV_LOAD_IMAGE_UNCHANGED);
//     matImg = cv::imdecode(decoded, cv::IMREAD_UNCHANGED);
    
//     if (!(matImg.empty())) {
//       int qID = -1;
      
//       cv::Mat result;

//       while (qID == -1) {
//         for (int i = 0 ; i < 1; i ++) {
          
//           if (!busy[i].is_busy) {
//             // std::lock_guard<std::mutex> guard(busy[i].is_busy_mutex);
//             qID = i;
//             busy[i].is_busy = true;
//             // clearq(outq[i]);
//             // clearq(inq[i]);
//             while (!inq[i].empty()) inq[i].empty();
//             while (!outq[i].empty()) outq[i].empty();

//             inq[i].push(std::make_shared<cv::Mat>(matImg));
//             inq[i].push(std::make_shared<cv::Mat>(result));
//           }
//         }

//         if (qID != -1) {
//           while (true) {
            
//             if (outq[qID].empty()) {
//               // std::cout << "qID:" << qID << " outq empty\n";
//               std::this_thread::sleep_for(std::chrono::microseconds(50));
//             } else {
//               break;
//             }
//           }
//           // std::cout << "trying to get lock\n";
//           // std::lock_guard<std::mutex> guard(busy[qID].is_busy_mutex);
//           // std::cout << "received lock\n";
//           std::shared_ptr<cv::Mat> fromq = outq[qID].front();


//           // if (fromq->empty()) {
//           //   std::cerr << "server fromq mat empty\n";
//           // }


//           // if (fromq->clone().empty()) {
//           //   std::cerr << "server fromq clone mat empty\n";
//           // } else {
//           //   std::cout << "fromq->clone() has data\n";
//           // }


//           // cv::imshow("fromq->clone()", fromq->clone());
//           // cv::waitKey();    

//           cv::Mat ret = fromq->clone();
//           outq[qID].pop();
//           busy[qID].is_busy = false;
//           // std::cout << "reset is_busy\n";

//           if (!(ret.empty())) {
//             // cv::imshow("ret", ret);
//             // cv::waitKey();
//             std::vector<unsigned char> encbuf;
//             cv::imencode(".png", ret, encbuf);

//             // for (auto & u: encbuf) {
//             //   std::cout << u;
//             // } std::cout << "\n";
//             // std::cout << "creating enc_msg char pointer\n";
//             unsigned char  *enc_msg = reinterpret_cast<unsigned char*> (&encbuf[0]);
//             // std::cout << "created enc_msg char pointer, encoding to base64\n";
            
//             encoded = base64_encode(enc_msg, encbuf.size());
//             // std::cout << "encoded to base64, encoded:" << encoded << "\n";
//           } else {
//             // std::cerr << "processed matrix empty for request body:" << body << "\n";
//             // std::cerr << "ret empty\n";
//           }
//         } else {
//           std::this_thread::sleep_for(std::chrono::microseconds(50));
//         }
//       }

//       // std::cout << "encoded outside while(qID==-1):" << encoded << "\n";

//     } else {
//       std::cerr << "empty matrix from request data url:" << body << "\n";
//       // encoded = dataURL_prefix;
//     }

//     // if (!(matImg.empty())) {
//     // cv::imshow("Image from Char Array", matImg);
//     // }
//     // if (!matImg.empty()) {
//     // std::cout << "matImg empty\n";
//     // }
//     // cv::imwrite( FLAGS_output_video_path + "/test.png", matImg);
//     // cv::imshow("client", matImg);
//     // cv::waitKey();

//     // std::cout << "encoded base64:" << encoded << "\n";
//     crow::json::wvalue jsonResp;
//     jsonResp["img_id"] = ++reqCnt;
//     jsonResp["data_url"] = dataURL_prefix+encoded;

//     auto res = crow::response{std::move(jsonResp)};
//     res.add_header("Access-Control-Allow-Origin", "*");
//     res.add_header("Access-Control-Allow-Headers", "Content-Type");
//     res.add_header("Access-Control-Allow-Methods", "POST");
//     res.add_header("Access-Control-Max-Age", "86400");

//     // std::cout << "sending resp for reqcnt:" << reqCnt << "\n";
//     return res;
//   });

//   CROW_ROUTE(app, "/worker")
//   .methods("OPTIONS"_method)
//   ([](const crow::request& req){
//   // std::cout << "OPTIONS /worker entered\n";

//   auto res = crow::response{};
//   res.add_header("Access-Control-Allow-Origin", "*");
//   res.add_header("Access-Control-Allow-Headers", "Content-Type");
//   res.add_header("Access-Control-Allow-Methods", "POST");
//   res.add_header("Access-Control-Max-Age", "86400");

//   return res;
//   });


//   // app.loglevel(crow::LogLevel::DEBUG);

//   app.port(18080).multithreaded().run();
// }

// void *server_thread_handler(void *threadid) {
//   long tid;
//   tid = (long)threadid;
//   std::cout << "insider server_thread_handler:" << tid << "\n";
//   server();
//   pthread_exit(NULL);
// }

//------------------------------------------------------------------------------

// Report a failure
// void
// fail(beast::error_code ec, char const* what) {
//     std::cerr << what << ": " << ec.message() << "\n";
// }

// Echoes back all received WebSocket messages
// class session : public std::enable_shared_from_this<session> {
//     websocket::stream<beast::tcp_stream> ws_;
//     beast::flat_buffer buffer_;

// public:
//     // Take ownership of the socket
//     explicit
//     session(tcp::socket&& socket)
//         : ws_(std::move(socket))
//     {
//     }

//     // Get on the correct executor
//     void
//     run()
//     {
//         // We need to be executing within a strand to perform async operations
//         // on the I/O objects in this session. Although not strictly necessary
//         // for single-threaded contexts, this example code is written to be
//         // thread-safe by default.
//         net::dispatch(ws_.get_executor(),
//             beast::bind_front_handler(
//                 &session::on_run,
//                 shared_from_this()));
//     }

//     // Start the asynchronous operation
//     void
//     on_run()
//     {
//         // Set suggested timeout settings for the websocket
//         ws_.set_option(
//             websocket::stream_base::timeout::suggested(
//                 beast::role_type::server));

//         // Set a decorator to change the Server of the handshake
//         ws_.set_option(websocket::stream_base::decorator(
//             [](websocket::response_type& res)
//             {
//                 res.set(http::field::server,
//                     std::string(BOOST_BEAST_VERSION_STRING) +
//                         " websocket-server-async");
//             }));
//         // Accept the websocket handshake
//         ws_.async_accept(
//             beast::bind_front_handler(
//                 &session::on_accept,
//                 shared_from_this()));
//     }

//     void
//     on_accept(beast::error_code ec)
//     {
//         if(ec)
//             return fail(ec, "accept");

//         // Read a message
//         do_read();
//     }

//     void
//     do_read()
//     {
//         // Read a message into our buffer
//         ws_.async_read(
//             buffer_,
//             beast::bind_front_handler(
//                 &session::on_read,
//                 shared_from_this()));
//     }

//     void
//     on_read(
//         beast::error_code ec,
//         std::size_t bytes_transferred)
//     {
//         boost::ignore_unused(bytes_transferred);

//         // This indicates that the session was closed
//         if(ec == websocket::error::closed)
//             return;

//         if(ec)
//             fail(ec, "read");

        
//         auto resp = ws_.got_text();
//         std::cerr << "resp:" <<resp << "\n";
//         auto buffer_data = buffer_.data();
//         // auto buffer_data_str = boost::beast::buffers(buffer_data);
//         auto buffer_data_str = beast::buffers_to_string(buffer_data);

//         // std::cerr << "buffer_data:" << beast::make_printable(buffer_data) << " buffer_data_str:" << buffer_data_str << "\n";
//         std::cerr << "buffer_data_str:" << buffer_data_str << "\n";
//         rapidjson::Document message_object;
//         // message_object.Parse(msg->get_payload().c_str());
//         message_object.Parse(buffer_data_str.c_str());
        
//         if (message_object.HasMember("type")) {
//           std::string type = message_object["type"].GetString();

//           std::cerr << "type:" << type << "\n";
//           if (type == "offer") {
//             std::string sdp = message_object["payload"]["sdp"].GetString();
//             std::cerr << "sdp:" << sdp << "\n";
//             // Do some some stuff with the offer.
//             // create an RTCPeerConnection and an RTCDataChannel on the server so we can process the client’s offer and generate an answer. 
//             // the WebRTC library uses the observer pattern to handle WebRTC events like onmessage and onOfferCreated instead of convenient JS callbacks
//             // To even get a peer connection running, we have to implement all 19 possible events by overriding the webrtc::*Observer family of abstract classes.

//             // webrtc::PeerConnectionObserver for peer connection events such as receiving ICE candidates.
//             // webrtc::CreateSessionDescriptionObserver for creating an offer or answer.
//             // webrtc::SetSessionDescriptionObserver for acknowledging and storing an offer or answer.
//             // webrtc::DataChannelObserver for data channel events like receiving SCTP messages.
            

//           } else {
//             std::cout << "Unrecognized WebSocket message type." << std::endl;
//           }
//         }
        
//         ws_.text(resp);
//         ws_.async_write(
//             buffer_data,
//             beast::bind_front_handler(
//                 &session::on_write,
//                 shared_from_this()));
//     }

//     void
//     on_write(
//         beast::error_code ec,
//         std::size_t bytes_transferred)
//     {
//         boost::ignore_unused(bytes_transferred);

//         if(ec)
//             return fail(ec, "write");

//         // Clear the buffer
//         buffer_.consume(buffer_.size());

//         // Do another read
//         do_read();
//     }
// };

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
// class listener : public std::enable_shared_from_this<listener>
// {
//     net::io_context& ioc_;
//     tcp::acceptor acceptor_;

// public:
//     listener(
//         net::io_context& ioc,
//         tcp::endpoint endpoint)
//         : ioc_(ioc)
//         , acceptor_(ioc)
//     {
//         beast::error_code ec;

//         // Open the acceptor
//         acceptor_.open(endpoint.protocol(), ec);
//         if(ec)
//         {
//             fail(ec, "open");
//             return;
//         }

//         // Allow address reuse
//         acceptor_.set_option(net::socket_base::reuse_address(true), ec);
//         if(ec)
//         {
//             fail(ec, "set_option");
//             return;
//         }

//         // Bind to the server address
//         acceptor_.bind(endpoint, ec);
//         if(ec)
//         {
//             fail(ec, "bind");
//             return;
//         }

//         // Start listening for connections
//         acceptor_.listen(
//             net::socket_base::max_listen_connections, ec);
//         if(ec)
//         {
//             fail(ec, "listen");
//             return;
//         }
//     }

//     // Start accepting incoming connections
//     void
//     run()
//     {
//         do_accept();
//     }

// private:
//     void
//     do_accept()
//     {
//         // The new connection gets its own strand
//         acceptor_.async_accept(
//             net::make_strand(ioc_),
//             beast::bind_front_handler(
//                 &listener::on_accept,
//                 shared_from_this()));
//     }

//     void
//     on_accept(beast::error_code ec, tcp::socket socket)
//     {
//         if(ec)
//         {
//             fail(ec, "accept");
//         }
//         else
//         {
//             // Create the session and run it
//             std::make_shared<session>(std::move(socket))->run();
//         }

//         // Accept another connection
//         do_accept();
//     }
// };

//------------------------------------------------------------------------------


int main(int argc, char** argv) {

  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::cerr << "frame_width:" << FLAGS_frame_width << " frame_height:" << FLAGS_frame_height << "\n";
  

//   try {
    // Check command line arguments.
    // if(argc != 4)
    // {
    //     std::cerr <<
    //         "Usage: websocket-client-sync <host> <port> <text>\n" <<
    //         "Example:\n" <<
    //         "    websocket-client-sync echo.websocket.org 80 \"Hello, world!\"\n";
    //     return EXIT_FAILURE;
    // }
    // std::string host = net::ip::make_address(FLAGS_wshost);//argv[1];
    // auto const address = net::ip::make_address(FLAGS_wshost);//argv[1];
    // auto const port = static_cast<unsigned short>(std::stoi(FLAGS_wsport));
    // auto const  port = FLAGS_wsport;//argv[2];
    // auto const  text = "websocket from boost";//argv[3];
    // auto const threads = 1;
    // std::cerr << "ws host:" << host << " port:" << port << "\n";

    // std::cerr << "ws address:" << address << " port:" << port << "\n";


    // The io_context is required for all I/O
    // net::io_context ioc{threads};

    // Create and launch a listening port
    // std::make_shared<listener>(ioc, tcp::endpoint{address, port})->run();

    // Run the I/O service on the requested number of threads
    // std::vector<std::thread> v;
    // v.reserve(threads - 1);
    // for(auto i = threads - 1; i > 0; --i)
        // v.emplace_back(
        // [&ioc]
        // {
            // ioc.run();
        // });
    // ioc.run();
//   } catch (const std::exception& e) {
    // std::cerr << "in catch block, err:" << e.what()  << "\n";
//   }
  
  
  
//   return EXIT_SUCCESS;


  // pthread_t server_thread;

  // int rc = pthread_create(&server_thread, NULL, server_thread_handler, (void *)1);
  // if (rc) {
  //   std::cerr << "unable to start server thread\n";
  //   exit(-1);
  // } else {
  //   // std::cout << "server pthread_t:" << (int)pthread_t << "\n";
  // }


  int choice_anchor;
  int choice_trigger;
  int choice_initiator;
  int choice_divisions;
  
  if (mp_graph == NULL) {
    mp_graph = std::make_shared<MediaPipeMultiHandGPU>();
    AnchorHandler handler_anchor;
    handler_anchor._dynamic = AnchorDynamic(cv::Scalar(25, 25, 255), cv::Scalar(255, 25, 25));
    handler_anchor._static = AnchorStatic(cv::Scalar(25, 25, 255), cv::Scalar(255, 25, 25));

    TriggerHandler handler_trigger;
    handler_trigger._thumb = TriggerThumb(FLAGS_frame_width, FLAGS_frame_height);
    handler_trigger._thumb_other = TriggerThumbOther(FLAGS_frame_width, FLAGS_frame_height);
    handler_trigger._pinch = TriggerPinch(FLAGS_frame_width, FLAGS_frame_height);
    handler_trigger._wait = TriggerWait(FLAGS_frame_width, FLAGS_frame_height, -1);
    // handler_trigger._tap = TriggerTap(FLAGS_frame_width, FLAGS_frame_height);
    handler_trigger._dwell = TriggerDwell();

    InitiatorHandler handler_initiator;
    handler_initiator._default = InitiatorDefault();
    handler_initiator._twohand = InitiatorTwoHand();

    mp_graph->anchor = handler_anchor;
    mp_graph->initiator = handler_initiator;
    mp_graph->trigger = handler_trigger;
  }

  // choice_initiator = 1;
  // choice_anchor = 1;
  // choice_trigger = 1;

  // while(true) {

    std::cout << "Pick initiator (1 : Default, 2: Two-Hand): ";
    std::cin >> choice_initiator;
    mp_graph->initiator._choice = choice_initiator;

    std::cout << "Pick anchor (1 : Dynamic, 2: static, 3: midair): ";
    std::cin >> choice_anchor;
    mp_graph->anchor._choice = choice_anchor;

    std::cout << "Pick the number of cells per row/col (3 - 10): ";
    std::cin>> choice_divisions;
    mp_graph->anchor.setDivisions(choice_divisions);
    
    std::cout << "Pick trigger (1: Thumb of base palm, 2: Thumb of free palm, 3: Pinch with free palm, 4: Wait to select, 5: Tap, 6: Dwell): ";
    std::cin >> choice_trigger;
    mp_graph->trigger._choice = choice_trigger;
    mp_graph->trigger._wait.choice = choice_anchor;

    if (choice_anchor == 1 && choice_initiator == 2) {
      std::cerr << "two-hand initiator cannot be used with dynamic anchor\n";
      exit(-1);
    }

    std::cout << "setting up strict: whether or not to look for index pointer from 2nd hand only\n";
    if (choice_trigger == 1 || choice_trigger == 5) {
      std::cerr << "setting initiator strict true\n";
      mp_graph->initiator.setStrict(true); // only look for pointer in second hand
    } else if (choice_trigger == 6) {
      std::cout << "given dwell trigger, checking choice_anchor\n";
      if (choice_anchor != 3) {
          std::cerr << "setting initiator strict true for choice_trigger=6(dwell) and choice_anchor!=3(not midair)\n";
        mp_graph->initiator.setStrict(true); // only look for pointer in second hand
      }
    }

    ::mediapipe::Status run_status = mp_graph->run(
        FLAGS_calculator_graph_config_file, 
        FLAGS_input_video_path, 
        FLAGS_output_video_path, 
        FLAGS_frame_width, 
        FLAGS_frame_height, 
        FLAGS_fps,
        FLAGS_debug); //,
        // FLAGS_image_ext,
        // FLAGS_read_delay,
        // inq, outq);
    
    if (!run_status.ok()) {
      LOG(ERROR) << "Failed to run the graph: " << run_status.message();
      // return EXIT_FAILURE;
    } else {
      LOG(INFO) << "Success!";
    }
  // }

  // std::this_thread::sleep_for(std::chrono::microseconds(20sssss0000));


  return EXIT_SUCCESS;
}

