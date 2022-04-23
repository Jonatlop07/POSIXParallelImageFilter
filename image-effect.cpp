#include <opencv2/opencv.hpp>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int THREAD_NUM;

struct threadData {
  int threadNumber;
  const cv::Mat *imageToProcess;
  cv::Mat *processedImage;
};

void *applyFilter(void *data) {
  struct threadData *currentThreadData = (threadData *) data;
  const int threadNumber = currentThreadData->threadNumber;
  const cv::Mat *imageToProcess = currentThreadData->imageToProcess;
  cv::Mat *processedImage = currentThreadData->processedImage;

  int cols, rows, channels; 

  rows = imageToProcess->rows;
  cols = imageToProcess->cols;
  channels = imageToProcess->channels();

  const int blockSize = rows / THREAD_NUM;  

  const int blockStart = threadNumber * blockSize; 
  const int blockEnd = (threadNumber + 1) * (blockSize);

  int iterationLimit = blockEnd;
  const int rest = rows - blockEnd;
  
  if (threadNumber == THREAD_NUM - 1) {
    iterationLimit += rest;
  }

  for (int r = blockStart; r < iterationLimit; ++r) {
    for (int c = 0; c < cols; ++c) {
      for (int ch = 0; ch < channels; ++ch) {
	const int left = c - 1;
	const int right = c + 1;
	const int up = r - 1;
	const int down = r + 1;

	int accumulation = 0;
	int neighbors = 0;

	if (up >= 0 && left >= 0) {
	  accumulation += (unsigned short int) imageToProcess->at<cv::Vec3b>(up, left)[ch];
	  ++neighbors;
	}
	if (up >= 0) {
	  accumulation += (unsigned short int) imageToProcess->at<cv::Vec3b>(up, c)[ch];
	  ++neighbors;
	}
	if (up >= 0 && right < cols) {
	  accumulation += (unsigned short int) imageToProcess->at<cv::Vec3b>(up, right)[ch];
	  ++neighbors;
	}
	if (right < cols) {
	  accumulation += (unsigned short int) imageToProcess->at<cv::Vec3b>(r, right)[ch];
	  ++neighbors;
	}
	if (down < rows && right < cols) {
	  accumulation += (unsigned short int) imageToProcess->at<cv::Vec3b>(down, right)[ch];
	  ++neighbors;
	}
	if (down < rows) {
	  accumulation += (unsigned short int) imageToProcess->at<cv::Vec3b>(down, c)[ch];
	  ++neighbors;
	}
	if (down < rows && left >= 0) {
	  accumulation += (unsigned short int) imageToProcess->at<cv::Vec3b>(down, left)[ch];
	  ++neighbors;
	}
	if (left >= 0) {
	  accumulation += (unsigned short int) imageToProcess->at<cv::Vec3b>(r, left)[ch];
	  ++neighbors;
	}
	processedImage->at<cv::Vec3b>(r, c)[ch] = (unsigned short int) (accumulation / neighbors);
      }
    }
  }
}


int main(int argc, char *argv[]) {
  const char *inputImageName = argv[1];
  const char *outputImageName = argv[2];
  THREAD_NUM = *argv[3];

  pthread_t threads[THREAD_NUM];
  struct threadData threadsData[THREAD_NUM];
  int currentThread = 0, r, *rh0;

  const cv::Mat image = cv::imread(inputImageName);
  
  cv::Mat *outputImage = new cv::Mat(image.rows, image.cols, CV_8UC(image.channels()));

  if (image.empty()) {
    perror("Error loading image\n");
    exit(1);
  }
  
  for (currentThread = 0; currentThread < THREAD_NUM; ++currentThread) {
    threadsData[currentThread].threadNumber = currentThread;
    threadsData[currentThread].imageToProcess = &image;
    threadsData[currentThread].processedImage = outputImage;
  }

  for (currentThread = 0; currentThread < THREAD_NUM; ++currentThread) {
    r = pthread_create(
      &threads[currentThread],
      NULL,
      applyFilter,
      (void *) &threadsData[currentThread]
    );

    if (r != 0) {
      perror("\n-->pthread_join error: ");
      exit(-1);
    }
  }

  for (currentThread = 0; currentThread < THREAD_NUM; ++currentThread) {
    r = pthread_join(threads[currentThread], (void **) &rh0);
    if (r != 0) {
      perror("\n-->pthread_join error: ");
      exit(-1);
    }
  }

  bool isSuccess = cv::imwrite(outputImageName, *outputImage);

  if (!isSuccess) {
    perror("Failed to save the image\n");
    exit(1);
  }

  cv::namedWindow("HI");
  cv::imshow("HI", cv::imread(outputImageName));
  cv::waitKey(0);
  cv::destroyWindow("HI");

  free(outputImage);

  printf("Image was successfully saved to a file\n");
  return 0;
}
