#include "opencv2/opencv.hpp"
#include <iostream>
#include <cstdint>
#include <sndfile.h>

#include "pcm.h"


using namespace std;
using namespace cv;

SNDFILE *outfile;
SF_INFO sfinfo;

void showHelp(void){
	std::cout << "Usage: -i input_file -o output_file [-s/b/f/16] " << endl;
	std::cout << "-i\tavi, mp4..." << endl;
	std::cout << "-o\t*.wav or - for use pipe" << endl;
	std::cout << "-v\tfor show input video" << endl;
	std::cout << "-c\tfor show \"Copy out\" video" << endl;
	std::cout << "-s\tfor show input video step-by-step" << endl;
	std::cout << "-b\tfor show video after binarization" << endl;
	std::cout << "-16\tuse 16 bit pcm. Default - 14 bit" << endl;
	std::cout << "-f\tfor use Full PCM Frame" << endl;
}

#define NEXT_FRAME_KEY 32
#define EXIT_KEY 27

#define PCM_PIXEL_0 30
#define PCM_PIXEL_1 127

Mat translateImg(Mat &img, int offsetx, int offsety){
    Mat trans_mat = (Mat_<double>(2,3) << 1, 0, offsetx, 0, 1, offsety);
    warpAffine(img, img, trans_mat, img.size());
    return img;
}


#define FRAME_HEIGHT 492

int main(int argc, char *argv[]){

	char  *inFileName = NULL;
	char *outFileName = NULL;
	bool show = false;
	bool showStep = false;
	bool showBin = false;
	bool copyOut = false;
	bool b16 = false;
	bool fullPCM = false;

	bool useDevice = false;

	int deviceID;

	if (argc == 1){
		showHelp();
		return 1;
	}

	// Перебираем каждый аргумент и выводим его порядковый номер и значение
	for (int i=0; i < argc; i++){
		if (strcmp(argv[i], "-i") == 0)  inFileName = argv[i + 1];
		if (strcmp(argv[i], "-o") == 0) outFileName = argv[i + 1];
		if (strcmp(argv[i], "-d") == 0) {
			useDevice = true;
			deviceID = std::stoi(argv[i + 1]);
		}
		if (strcmp(argv[i], "-16") == 0) b16 = true;
		if (strcmp(argv[i], "-v") == 0) show = true;
		if (strcmp(argv[i], "-f") == 0) fullPCM= true;
		if (strcmp(argv[i], "-b") == 0) {
			show = true;
			showBin = true;
		}
		if (strcmp(argv[i], "-s") == 0) {
			show = true;
			showStep = true;
		}
		if (strcmp(argv[i], "-c") == 0) {
			show = true;
			showBin = true;
			copyOut = true;
		}
		if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
			showHelp();
			return 0;
		}
		//std::cout << count << " " << argv[i] << '\n';
	}

	if (inFileName == NULL && !useDevice){
		showHelp();
		return 1;
	}

	if (outFileName == NULL && !show){
		showHelp();
		return 1;
	}

  // Create a VideoCapture object and open the input file
  // If the input is the web camera, pass 0 instead of the video file name
	VideoCapture cap;
	if (useDevice) {
		cap.open(deviceID);
	} else {
		cap.open(inFileName);
	}

  // Check if camera opened successfully
  if(!cap.isOpened()){
    cerr << "Error opening video stream or file" << endl;
    return -1;
  }

	if (outFileName != NULL){
		memset (&sfinfo, 0, sizeof (sfinfo)) ;
		sfinfo.samplerate	= 44100;
		//sfinfo.frames   = 100;
		sfinfo.channels = 2 ;
		if (strcmp(outFileName, "-") == 0){
			sfinfo.format   = (SF_FORMAT_AU | SF_FORMAT_PCM_16);
		} else {
			sfinfo.format   = (SF_FORMAT_WAV | SF_FORMAT_PCM_16);
		}


		if ((outfile = sf_open (outFileName, SFM_WRITE, &sfinfo)) == NULL){
			exit (1) ;
		}
	}


  while(1){

    Mat srcFrame;
		Mat greyFrame;
		Mat binFrame;
		Mat fullFrame(FRAME_HEIGHT, 720, CV_8UC1, Scalar(PCM_PIXEL_0));

    // Capture frame-by-frame
    cap >> srcFrame;
		//bool bSuccess = cap.read(frame);

		// If the frame is empty, break immediately
		//if (!bSuccess)
		if (srcFrame.empty())
      break;

		cvtColor(srcFrame, greyFrame, CV_BGR2GRAY);

    //threshold(srcFrame, binFrame, 79, 255, THRESH_BINARY);
    //threshold(greyFrame, binFrame, 79, 255, THRESH_BINARY);
		GaussianBlur(greyFrame, greyFrame, Size(3, 1), 0, 0, BORDER_DEFAULT);
		threshold(greyFrame, binFrame, 0, 255, THRESH_BINARY + THRESH_OTSU);
		//adaptiveThreshold(greyFrame, binFrame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 2);
		//adaptiveThreshold(greyFrame, binFrame, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);

		// Подсчет количества пустых строк внизу
		for (int i = binFrame.size().height - 1; i >= 0; i--){
			if (searchStart(binFrame, i, NULL, NULL)){
				// PCM данные найдены. Перенесем их.
				//fprintf(stderr, "end: %d\n", i);
				memcpy(fullFrame.data + fullFrame.elemSize() * fullFrame.size().width * (2 + FRAME_HEIGHT - i - 1), binFrame.data, binFrame.elemSize() * binFrame.size().width * (i - 1));
				break;
			}
		}


    //frame(Rect(0, 10, frame.cols, frame.rows - 10)).copyTo(out(cv::Rect(0,0,frame.cols,frame.rows-10)));



		if (outFileName != NULL){
			if (copyOut){
				Mat pcmFrame(FRAME_HEIGHT, 144, CV_8UC1, Scalar(PCM_PIXEL_0)); // 720 / 5 = 144
				readPCMFrame(fullFrame, 0);
				copyOutBuffer();
				writePCMFrame(pcmFrame, 0, true);
				PCMFrame2wav(outfile, b16);

				readPCMFrame(fullFrame, 1);
				copyOutBuffer();
				writePCMFrame(pcmFrame, 1, true);
				PCMFrame2wav(outfile, b16);
				resize(pcmFrame, binFrame, binFrame.size(), 5, 0, INTER_NEAREST);
			} else {
				readPCMFrame(fullFrame, 0);
				PCMFrame2wav(outfile, b16);

				readPCMFrame(fullFrame, 1);
				PCMFrame2wav(outfile, b16);
			}
		}


		if (show){
			char c;
			if (showBin){
				imshow("Frame", binFrame);
			} else {
				imshow("Frame", srcFrame);
			}
			if (showStep){
				while(1){
					// Press  ESC on keyboard to exit
					c = (char)waitKey(5);
					if(c == NEXT_FRAME_KEY || c == EXIT_KEY)
						break;
				}
				if(c == EXIT_KEY)
					break;
			} else {
				c = (char)waitKey(5);
				if(c == EXIT_KEY)
					break;
			}
		}

  }

  // When everything done, release the video capture object
  cap.release();

  // Closes all the frames
  destroyAllWindows();

	if (outFileName != NULL){
		sfinfo.frames = samplesCount();
		sf_close(outfile);

		showStatistics();
	}

	return 0;
}
