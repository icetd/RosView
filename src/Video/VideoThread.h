#ifndef VIDEO_THREAD_H
#define VIDEO_THREAD_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "MThread.h"
#include "VideoCapture.h"

class VideoThread : public MThread
{
public:
	VideoThread();
	~VideoThread();

	void SetOnRenderDataCallback(std::function<void(std::vector<uint8_t>&&)> callback);
	void SetUrl(const char* url) { m_Url = url; }
	void SetStartStatus(bool is_start) { isStart = is_start; }
	bool GetStartStatus() { return isStart; }
	bool GetCaptureStatus() { return isStartCapture; }

	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

protected:
	virtual void run() override;

private:
	int m_Width;
	int m_Height;
	uint8_t* m_data;
	int64_t m_pts;

	std::unique_ptr <VideoCapture> m_VideoCapture;

	bool isStart;
	bool isStartCapture;
	std::string m_Url;

	std::function<void(std::vector<uint8_t>&&)> OnRenderDataCallback;
	void StartPlay();
	void StopPlay();
};

#endif