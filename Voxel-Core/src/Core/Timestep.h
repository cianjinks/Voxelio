#pragma once

namespace VoxelCore {

	class Timestep
	{
	private:
		static float s_LastFrame;
		static float s_DeltaTime;
	public:
		static float GetDeltaTime() { return s_DeltaTime; }
		static float GetDeltaTimeMS() { return s_DeltaTime * 1000.0f; }
		static void Update();
	};
}
