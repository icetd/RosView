#include "VideoLayer.h"
#include "Application.h"
#include "imgui.h"
#include "Utils.h"

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

		ImGui::InputTextWithHint("video1", "set rtsp url here", m_url1, IM_ARRAYSIZE(m_url1));
		m_VideoThread1->SetUrl(m_url1);
		static int radio_video1 = 0;
		ImGui::RadioButton(u8"打开 video1", &radio_video1, 1); ImGui::SameLine(0, 0);
		ImGui::RadioButton(u8"关闭 video1", &radio_video1, 0); ImGui::SameLine(0, 20);

		if (m_VideoThread1->GetCaptureStatus()) {
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
		} else {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 0.0f), "Stop");
		}
		m_VideoThread1->SetStartStatus((int)radio_video1);

		ImGui::NewLine();
		ImGui::InputTextWithHint("video2", "set rtsp url here", m_url2, IM_ARRAYSIZE(m_url2));
		m_VideoThread2->SetUrl(m_url2);

		static int radio_video2 = 0;
		ImGui::RadioButton(u8"打开 video2", &radio_video2, 1); ImGui::SameLine(0, 0);
		ImGui::RadioButton(u8"关闭 video2", &radio_video2, 0); ImGui::SameLine(0, 20);
	
		if (m_VideoThread2->GetCaptureStatus()) {
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Start");
		} else {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stop");
		}
		m_VideoThread2->SetStartStatus((int)radio_video2);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::PopItemWidth();
		ImGui::End();
	}
	{
		ImGui::Begin("Video1");
		if (!m_dataBufferList1.empty())
		{
			mutex_data1.lock();
			m_dataBuffer1 = m_dataBufferList1.back();
			m_dataBufferList1.pop_back();
			m_Texture1->bind(m_VideoThread1->GetWidth(), m_VideoThread1->GetHeight(), m_dataBuffer1.data());
			mutex_data1.unlock();
		}
		OnRenderVideo1();
		ImGui::End();
	}
	{
		ImGui::Begin("Video2");
		if (!m_dataBufferList2.empty())
		{
			mutex_data2.lock();
			m_dataBuffer2 = m_dataBufferList2.back();
			m_dataBufferList2.pop_back();
			m_Texture2->bind(m_VideoThread2->GetWidth(), m_VideoThread2->GetHeight(), m_dataBuffer2.data());
			mutex_data2.unlock();
		}
		OnRenderVideo2();
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
	mutex_data1.unlock();

	if (m_dataBufferList1.size() > 3)
		m_dataBufferList1.pop_back();
}


void VideoLayer::OnRenderVideo1()
{
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (m_VideoThread1->GetCaptureStatus()) {
		ImGui::Image((ImTextureID)(intptr_t)m_Texture1->getId(), ImVec2(viewportSize.x, viewportSize.y), ImVec2(1, 0), ImVec2(0, 1));
	} else {
		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void VideoLayer::OnRenderData2(std::vector<uint8_t>&& data)
{
	mutex_data2.lock();
	m_dataBufferList2.emplace_back(std::move(data));
	mutex_data2.unlock();

	if (m_dataBufferList2.size() > 3)
		m_dataBufferList2.pop_back();
}


void VideoLayer::OnRenderVideo2()
{
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (m_VideoThread2->GetCaptureStatus()) {
		ImGui::Image((ImTextureID)(intptr_t)m_Texture2->getId(), ImVec2(viewportSize.x, viewportSize.y), ImVec2(1, 0), ImVec2(0, 1));
	}else {
	}
}