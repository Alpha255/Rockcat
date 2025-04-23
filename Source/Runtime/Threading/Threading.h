#pragma once

namespace Threading
{
	bool IsInMainThread();
	bool IsInGameThread();
	bool IsInRenderThread();
	bool IsInWorkerThread();
}