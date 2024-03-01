#include "VideoLayer.h"
#include "Application.h"
#include "imgui.h"
#include "Utils.h"
#include "TransForm.h"

extern bool show_video_layout;

void VideoLayer::OnAttach()
{
	m_Texture1 = std::make_unique<Texture>();
	m_VideoThread1 = std::make_unique<VideoThread>();
	m_VideoThread1->SetOnRenderDataCallback(std::bind(&VideoLayer::OnRenderData1, this, std::placeholders::_1));
	m_VideoThread1->start();
	m_dataBufferList1.clear();

	m_Texture2 = std::make_unique<Texture>();
	m_VideoThread2 = std::make_unique<VideoThread>();
	m_VideoThread2->SetOnRenderDataCallback(std::bind(&VideoLayer::OnRenderData2, this, std::placeholders::_1));
	m_VideoThread2->start();
	m_dataBufferList2.clear();

	m_Texture3 = std::make_unique<Texture>();
	m_VideoThread3 = std::make_unique<VideoThread>();
	m_VideoThread3->SetOnRenderDataCallback(std::bind(&VideoLayer::OnRenderData3, this, std::placeholders::_1));
	m_VideoThread3->start();
	m_dataBufferList3.clear();

	m_config = new INIReader("./configs/url.ini");
	m_url1 = m_config->Get("VIDEO", "URL_VIDEO1", "rtsp://192.168.2.113:8554/unicast");
	m_url2 = m_config->Get("VIDEO", "URL_VIDEO2", "rtsp://192.168.2.113:8554/unicast");
	m_url3 = m_config->Get("VIDEO", "URL_VIDEO3", "rtsp://192.168.2.113:8554/unicast");
}

void VideoLayer::OnUpdate(float ts)
{
	if (show_video_layout)
		Show_Video_Layout(&show_video_layout);
}

void VideoLayer::Show_Video_Layout(bool* p_open)
{
	{
		ImGuiContext& g = *GImGui;
		if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize))
			ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetFontSize() * 12.0f), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(u8"视频", p_open) || ImGui::GetCurrentWindow()->BeginCount > 1)
		{
			ImGui::End();
			return;
		}

		ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.2f);
		ImGui::AlignTextToFramePadding();

		ImGui::Text("\t\t%s", m_url1.c_str());
		m_VideoThread1->SetUrl((char *)m_url1.c_str());
		static int radio_video1 = 0;
		ImGui::RadioButton(u8"打开 前摄像头", &radio_video1, 1); ImGui::SameLine(0, 10);
		ImGui::RadioButton(u8"关闭 前摄像头", &radio_video1, 0); ImGui::SameLine(0, 20);

		if (m_VideoThread1->GetCaptureStatus()) {
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
		}
		else {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stop");
		}
		m_VideoThread1->SetStartStatus((int)radio_video1);

		ImGui::NewLine();
		ImGui::Text("\t\t%s", m_url2.c_str());
		m_VideoThread2->SetUrl((char *)m_url2.c_str());

		static int radio_video2 = 0;
		ImGui::RadioButton(u8"打开 对接摄像头", &radio_video2, 1); ImGui::SameLine(0, 10);
		ImGui::RadioButton(u8"关闭 对接摄像头", &radio_video2, 0); ImGui::SameLine(0, 20);

		if (m_VideoThread2->GetCaptureStatus()) {
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
		}
		else {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stop");
		}
		m_VideoThread2->SetStartStatus((int)radio_video2);

		ImGui::NewLine();
		ImGui::Text("\t\t%s", m_url3.c_str());
		m_VideoThread3->SetUrl((char *)m_url3.c_str());

		static int radio_video3 = 0;
		ImGui::RadioButton(u8"打开 取油摄像头", &radio_video3, 1); ImGui::SameLine(0, 10);
		ImGui::RadioButton(u8"关闭 取油摄像头", &radio_video3, 0); ImGui::SameLine(0, 20);

		if (m_VideoThread3->GetCaptureStatus()) {
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
		}
		else {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stop");
		}
		m_VideoThread3->SetStartStatus((int)radio_video3);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::TextColored(ImVec4(0.1f, 0.5f, 0.1f, 1.0f), "Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::PopItemWidth();
		ImGui::End();
	}
	{
		ImGui::Begin(u8"前摄像头");
		if (!m_dataBufferList1.empty() && m_VideoThread1->GetCaptureStatus())
		{
			bool a = m_dataBufferList1.empty();
			mutex_data1.lock();
			m_dataBuffer1 = m_dataBufferList1.back();
			m_dataBufferList1.pop_back();
			m_Texture1->bind(m_VideoThread1->GetWidth(), m_VideoThread1->GetHeight(), m_dataBuffer1.data());
			mutex_data1.unlock();
		}
		else {
		}
		OnRenderVideo1();
		ImGui::End();
	}
	{
		ImGui::Begin(u8"对接摄像头");
		if (!m_dataBufferList2.empty() && m_VideoThread2->GetCaptureStatus())
		{
			mutex_data2.lock();
			m_dataBuffer2 = m_dataBufferList2.back();
			m_dataBufferList2.pop_back();
			m_Texture2->bind(m_VideoThread2->GetWidth(), m_VideoThread2->GetHeight(), m_dataBuffer2.data());
			mutex_data2.unlock();
		}
		else {
		}
		OnRenderVideo2();
		ImGui::End();
	}
	{
		ImGui::Begin(u8"取油室摄像头");
		if (!m_dataBufferList3.empty() && m_VideoThread3->GetCaptureStatus())
		{
			mutex_data3.lock();
			m_dataBuffer3 = m_dataBufferList3.back();
			m_dataBufferList3.pop_back();
			m_Texture3->bind(m_VideoThread3->GetWidth(), m_VideoThread3->GetHeight(), m_dataBuffer3.data());
			mutex_data3.unlock();
		}
		else {
		}
		OnRenderVideo3();
		ImGui::End();
	}
}
void VideoLayer::OnDetach()
{
}

void VideoLayer::OnUIRender()
{
}

void VideoLayer::OnRenderData1(std::vector<uint8_t>&& data)
{
	mutex_data1.lock();
	m_dataBufferList1.emplace_back(std::move(data));
	if (m_dataBufferList1.size() > 3)
		m_dataBufferList1.pop_back();
	mutex_data1.unlock();
}


void VideoLayer::OnRenderVideo1()
{
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (m_VideoThread1->GetCaptureStatus()) {
		ImGui::Image((ImTextureID)(intptr_t)m_Texture1->getId(), ImVec2(viewportSize.x, viewportSize.y), ImVec2(0, 0), ImVec2(1, 1));
	}
	else {
	}
}

void VideoLayer::OnRenderData2(std::vector<uint8_t>&& data)
{
	mutex_data2.lock();
	m_dataBufferList2.emplace_back(std::move(data));
	if (m_dataBufferList2.size() > 3)
		m_dataBufferList2.pop_back();
	mutex_data2.unlock();
}


void VideoLayer::OnRenderVideo2()
{
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (m_VideoThread2->GetCaptureStatus()) {
		ImGui::Image((ImTextureID)(intptr_t)m_Texture2->getId(), ImVec2(viewportSize.x, viewportSize.y), ImVec2(0, 0), ImVec2(1, 1));
	}
	else {
	}
}

void VideoLayer::OnRenderData3(std::vector<uint8_t>&& data)
{
	mutex_data3.lock();
	m_dataBufferList3.emplace_back(std::move(data));
	if (m_dataBufferList3.size() > 3)
		m_dataBufferList3.pop_back();
	mutex_data3.unlock();
}



void VideoLayer::OnRenderVideo3()
{
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (m_VideoThread3->GetCaptureStatus()) {
		ImGui::Image((ImTextureID)(intptr_t)m_Texture3->getId(), ImVec2(viewportSize.x, viewportSize.y), ImVec2(0, 0), ImVec2(1, 1));
	}
	else {
	}
}