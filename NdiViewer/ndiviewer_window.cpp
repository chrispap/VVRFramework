#include "QtCore/qfuture.h"
#include <ui_ndiviewer_window.h>
#include <QPushButton>
#include <QMainWindow>
#include <QApplication>
#include <QVideoFrame>
#include <QVideoSink>
#include <QVideoWidget>
#include <QFuture>
#include <QtConcurrent>
#include <string>
#include <atomic>

#include <cstdio>
#include <iostream>
#include <chrono>
#include <Processing.NDI.Lib.h>

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x64.lib")
#else // _WIN64
#pragma comment(lib, "Processing.NDI.Lib.x86.lib")
#endif // _WIN64
#endif // _WIN32

std::atomic<int> running = 1;

template <typename T>
int ndiRxLoop(T delegate)
{
  printf("NDI Version: %s\n", NDIlib_version());
  int desired_num_sources = 1;

  if (!NDIlib_initialize())
    return 0;

  NDIlib_find_instance_t pNDI_find = NDIlib_find_create_v2();
  if (!pNDI_find)
    return 0;

  uint32_t num_sources = 0;
  const NDIlib_source_t* p_sources = NULL;
  while (running && num_sources < desired_num_sources) {
    // Wait until the sources on the network have changed
    printf("Looking for %d sources ...\n", desired_num_sources);
    NDIlib_find_wait_for_sources(pNDI_find, 1000/* One second */);
    p_sources = NDIlib_find_get_current_sources(pNDI_find, &num_sources);
    for (int i = 0; i < num_sources; i++) {
      printf("Source [%d/%d] -- Name: [%s] -- Address: [%s]\n",
        i + 1,
        num_sources,
        (p_sources + i)->p_ndi_name,
        (p_sources + i)->p_ip_address);
    }
  }

  printf("Will connect to source #%d...\n", desired_num_sources);

  p_sources = &p_sources[desired_num_sources - 1];

  NDIlib_recv_instance_t pNDI_recv = NDIlib_recv_create_v3();
  if (!pNDI_recv)
    return 0;

  NDIlib_recv_connect(pNDI_recv, p_sources);
  NDIlib_find_destroy(pNDI_find);

  using namespace std::chrono;
  while (running)
  {
    // The descriptors
    NDIlib_video_frame_v2_t video_frame;
    NDIlib_audio_frame_v2_t audio_frame;
    NDIlib_metadata_frame_t data_frame;

    switch (NDIlib_recv_capture_v2(pNDI_recv, &video_frame, &audio_frame, &data_frame, 2000))
    {
    case NDIlib_frame_type_none:
      printf("No data received.\n");
      break;
    case NDIlib_frame_type_video:
      delegate(video_frame);
      NDIlib_recv_free_video_v2(pNDI_recv, &video_frame);
      break;
    case NDIlib_frame_type_audio:
      printf("Audio data received (%d samples).\n", audio_frame.no_samples);
      NDIlib_recv_free_audio_v2(pNDI_recv, &audio_frame);
      break;
    case NDIlib_frame_type_metadata:
      printf("Received metadata: [%s]\n", data_frame.p_data);
      break;
    case NDIlib_frame_type_error:
      printf("Received error.\n");
      break;
    case NDIlib_frame_type_status_change:
      printf("Received status_change.\n");
      break;
    default:
      break;
    }
  }

  NDIlib_recv_destroy(pNDI_recv);
  NDIlib_destroy();
  return 0;
}

QVideoFrameFormat::PixelFormat ndiPixelFormatToPixelFormat(enum NDIlib_FourCC_video_type_e ndiFourCC)
{
  switch (ndiFourCC)
  {
  case NDIlib_FourCC_video_type_UYVY:
    return QVideoFrameFormat::PixelFormat::Format_UYVY;
  case NDIlib_FourCC_video_type_UYVA:
    return QVideoFrameFormat::PixelFormat::Format_UYVY;
    break;
  case NDIlib_FourCC_video_type_P216:
    return QVideoFrameFormat::PixelFormat::Format_P016;
  case NDIlib_FourCC_video_type_YV12:
    return QVideoFrameFormat::PixelFormat::Format_YV12;
  case NDIlib_FourCC_video_type_NV12:
    return QVideoFrameFormat::PixelFormat::Format_NV12;
  case NDIlib_FourCC_video_type_BGRA:
    return QVideoFrameFormat::PixelFormat::Format_BGRA8888;
  case NDIlib_FourCC_video_type_BGRX:
    return QVideoFrameFormat::PixelFormat::Format_BGRX8888;
  case NDIlib_FourCC_video_type_RGBA:
    return QVideoFrameFormat::PixelFormat::Format_RGBA8888;
  case NDIlib_FourCC_video_type_RGBX:
    return QVideoFrameFormat::PixelFormat::Format_RGBX8888;
  default:
    return QVideoFrameFormat::PixelFormat::Format_Invalid;
  }
}

class NdiViewerWindow : public QMainWindow
{
  Q_OBJECT
public:
  NdiViewerWindow();
  ~NdiViewerWindow();

private:
    Ui::NdiViewerWindow ui;
    QVideoWidget* videoWidget;
    QFuture<void> future;

private:
  void processVideo(NDIlib_video_frame_v2_t const *pNdiVideoFrame, QVideoSink *videoSink);

private:
  Ui::NdiViewerWindow ui;
  QVideoWidget *videoWidget;
};

void NdiViewerWindow::processVideo(NDIlib_video_frame_v2_t const *pNdiVideoFrame, QVideoSink *videoSink)
{
  auto ndiWidth = pNdiVideoFrame->xres;
  auto ndiHeight = pNdiVideoFrame->yres;
  auto ndiLineStrideInBytes = pNdiVideoFrame->line_stride_in_bytes;
  auto ndiPixelFormat = pNdiVideoFrame->FourCC;
  auto pixelFormat = ndiPixelFormatToPixelFormat(ndiPixelFormat);

  if (pixelFormat == QVideoFrameFormat::PixelFormat::Format_Invalid)
  {
    qDebug().nospace() << "Unsupported pNdiVideoFrame->FourCC";
    return;
  }

  QSize videoFrameSize(ndiWidth, ndiHeight);
  QVideoFrameFormat videoFrameFormat(videoFrameSize, pixelFormat);
  QVideoFrame videoFrame(videoFrameFormat);

  if (!videoFrame.map(QVideoFrame::WriteOnly))
  {
    qWarning() << "videoFrame.map(QVideoFrame::WriteOnly) failed; return;";
    return;
  }

  auto pDstY = videoFrame.bits(0);
  auto pSrcY = pNdiVideoFrame->p_data;
  auto pDstUV = videoFrame.bits(1);
  auto pSrcUV = pSrcY + (ndiLineStrideInBytes * ndiHeight);
  for (int line = 0; line < ndiHeight; ++line)
  {
    memcpy(pDstY, pSrcY, ndiLineStrideInBytes);
    pDstY += ndiLineStrideInBytes;
    pSrcY += ndiLineStrideInBytes;

    if (pDstUV)
    {
      // For now QVideoFrameFormat/QVideoFrame does not support P216. :(
      // I have started the conversation to have it added, but that may take awhile. :(
      // Until then, copying only every other UV line is a cheap way to downsample P216's 4:2:2 to P016's 4:2:0 chroma sampling.
      // There are still a few visible artifacts on the screen, but it is passable.
      if (line % 2)
      {
        memcpy(pDstUV, pSrcUV, ndiLineStrideInBytes);
        pDstUV += ndiLineStrideInBytes;
      }
      pSrcUV += ndiLineStrideInBytes;
    }
  }

  videoFrame.unmap();
  videoSink->setVideoFrame(videoFrame);
}

NdiViewerWindow::NdiViewerWindow()
{
  ui.setupUi(this);
  videoWidget = new QVideoWidget();
  ui.scrollArea->setWidget(videoWidget);

    future = QtConcurrent::run([this] {
    ndiRxLoop([this](NDIlib_video_frame_v2_t const &vf) {
      this->processVideo(&vf, this->videoWidget->videoSink());
    });
  });
}

NdiViewerWindow::~NdiViewerWindow()
{
  running = 0;
  future.waitForFinished();
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  NdiViewerWindow window;
  window.showMaximized();
  app.exec();
}

#include "ndiviewer_window.moc"
