#include "minimal_cs.h"

Mat image;
bool  selectObject = false;
SelectionStatus select_status = WAITING;
Point origin;
Rect  selection;
int   vmin = 10, vmax = 256, smin = 30;

static void onMouse(int event, int x, int y, int, void *)
{
  if (selectObject)
  {
    selection.x      = MIN(x, origin.x);
    selection.y      = MIN(y, origin.y);
    selection.width  = abs(x - origin.x);
    selection.height = abs(y - origin.y);
    selection &= Rect(0, 0, image.cols, image.rows);
  }

  switch (event)
  {
    case EVENT_LBUTTONDOWN:
      origin       = Point(x, y);
      selection    = Rect(x, y, 0, 0);
      selectObject = true;
      break;

    case EVENT_LBUTTONUP:
      selectObject = false;
      if ((selection.width > 0) && (selection.height > 0))
        select_status = SELECTED;
      break;
  }
}

void *camshift(void* params)
{
  int** prms = (int**)params;
  int* x = (int*)prms[0];
  int* y = (int*)prms[1];

  VideoCapture cap;
  Rect  tracked_object;
  int   hbins           = 16;
  float hranges[]       = { 0, 180 };
  const float *phranges = hranges;

  cap.open(0);
  if (!cap.isOpened()) {
    cout << "Camera not found\n";
    exit(0);
  }
  namedWindow("Tracker", 0);
  setMouseCallback("Tracker", onMouse, 0);

  Mat frame, hsv, hue, mask, hist, backproj;

  while (1) {
    cap >> frame;
    if (frame.empty()) break;
    frame.copyTo(image);
    cvtColor(image, hsv, COLOR_BGR2HSV);

    if (select_status != WAITING)
    {

      inRange(hsv, Scalar(0, smin, MIN(vmin, vmax)),
              Scalar(180, 256, MAX(vmin, vmax)), mask);
      int ch[] = { 0, 0 };
      hue.create(hsv.size(), hsv.depth());
      mixChannels(&hsv, 1, &hue, 1, ch, 1);

      // Initialize histogram for selected object
      if (select_status == SELECTED)
      {
        Mat roi(hue, selection), maskroi(mask, selection);
        calcHist(&roi, 1, 0, maskroi, hist, 1, &hbins, &phranges);
        normalize(hist, hist, 0, 255, NORM_MINMAX);

        tracked_object = selection;
        select_status = INITIALIZED;
      }

      // Perform CAMShift
      calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
      backproj &= mask;
      RotatedRect trackBox = CamShift(backproj, tracked_object,
                                      TermCriteria(TermCriteria::EPS |
                                                   TermCriteria::COUNT, 10, 1));

      ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);

      // Set detection coordinates
      *x = tracked_object.x;
      *y = tracked_object.y;
    }

    // Draw selection
    if (selectObject)
      rectangle(image, selection.tl(), selection.br(), Scalar(0,0,255), 5, 4);

    imshow("Tracker", image);
    char c = (char)waitKey(10);
    if (c == 27) break;
  }
}
