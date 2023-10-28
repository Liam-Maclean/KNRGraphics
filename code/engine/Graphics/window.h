#pragma once
namespace KNR
{
	//Come back and expand later incase the renderer actually needs to know more later
	struct WindowDesc
	{
		uint32_t width;
		uint32_t height;
		void* hwnd;
		void* instance;
	};
}