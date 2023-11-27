#include "VideoThread.h"

VideoThread::VideoThread() :
    isStartCapture(false),
    isStart(false)
{
    m_VideoCapture = std::make_unique<VideoCapture>();
}

VideoThread::~VideoThread()
{
}


void VideoThread::SetOnRenderDataCallback(std::function<void(std::vector<uint8_t> &&)> callback)
{
    OnRenderDataCallback = std::move(callback);
}

void VideoThread::StartPlay()
{
    bool re = false;
    m_VideoCapture->init();
    re = m_VideoCapture->open(m_Url.c_str());
    if (!re) {
        isStart = false;
        return;
    }
    m_data = (uint8_t *)malloc(0);
    m_Width = m_VideoCapture->getWidth();
    m_Height = m_VideoCapture->getHeight();
    isStartCapture = true;
}

void VideoThread::StopPlay()
{
    isStartCapture = false;
    m_VideoCapture->close();
    m_data = nullptr;
}

void VideoThread::run()
{
    while(!this->isStoped()) {
        if (isStart == true && isStartCapture == false) {
            StartPlay();
        } else if (isStart == false && isStartCapture == true) {
            StopPlay();
        }

        if (isStartCapture) {
            int lenght = m_Width * m_Height * 4;
            m_data = (uint8_t*) realloc(m_data, lenght);
            m_pts = 1;
            bool isDecodeSucceed;
            isDecodeSucceed = m_VideoCapture->decode(m_data, &m_pts);
            if (isDecodeSucceed) {
               OnRenderDataCallback(std::vector<uint8_t>(m_data, m_data + lenght));
            }
            this->sleepMs(10);
        }
    }
}