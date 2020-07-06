#pragma once
#include "voxelpch.h"

namespace VoxelCore
{
	class Window
	{
	private:
		void* m_NativeWindow;
	public:
		~Window() = default;

		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
		virtual bool WindowShouldClose() = 0;

		virtual void* GetNativeWindow() = 0;

		static std::unique_ptr<Window> Create();
	};
}