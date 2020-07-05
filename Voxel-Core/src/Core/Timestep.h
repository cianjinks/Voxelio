#pragma once

namespace VoxelCore {

	class Timestep
	{
	private:
		static float s_LastFrame;
	public:
		static float s_DeltaTime;

		static float GetSeconds() { return s_DeltaTime; }
		static float GetMilliseconds() { return s_DeltaTime * 1000.0f; }
		static void Update();
	};
}
