#ifndef VIDEO_LAYER_H
#define VIDEO_LAYER_H

#include "Layer.h"
#include "VideoThread.h"
#include "Texuture.h"
#include "INIReader.h"
#include <mutex>

class VideoLayer : public Layer
{
protected:
	virtual void OnAttach() override;
	virtual void OnUpdate(float ts) override;
	virtual void OnDetach() override;
	virtual void OnUIRender() override;

private:
	void Show_Video_Layout(bool* p_open);

	std::unique_ptr<VideoThread> m_VideoThread1;
	std::unique_ptr<Texture> m_Texture1;
	std::string m_url1;
	std::vector<std::vector<uint8_t>> m_dataBufferList1;
	std::vector<uint8_t> m_dataBuffer1;
	std::mutex mutex_data1;
	void OnRenderData1(std::vector<uint8_t>&& data);
	void OnRenderVideo1();

	std::unique_ptr<VideoThread> m_VideoThread2;
	std::unique_ptr<Texture> m_Texture2;
	std::string m_url2;
	std::vector<std::vector<uint8_t>> m_dataBufferList2;
	std::vector<uint8_t> m_dataBuffer2;
	std::mutex mutex_data2;
	void OnRenderData2(std::vector<uint8_t>&& data);
	void OnRenderVideo2();

	std::unique_ptr<VideoThread> m_VideoThread3;
	std::unique_ptr<Texture> m_Texture3;
	std::string m_url3;
	std::vector<std::vector<uint8_t>> m_dataBufferList3;
	std::vector<uint8_t> m_dataBuffer3;
	std::mutex mutex_data3;
	void OnRenderData3(std::vector<uint8_t>&& data);
	void OnRenderVideo3();

	INIReader *m_config;
};

#endif