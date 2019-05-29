#include <crtdbg.h>
#include "dbgext.h"

_CrtMemState memOld, memNew, memDiff;

winrt::com_ptr<IDataModelManager> spDataModelManager;
winrt::com_ptr<IDebugHost> spDebugHost;

extern "C" {

__declspec(dllexport) HRESULT __stdcall DebugExtensionInitialize(PULONG /*pVersion*/, PULONG /*pFlags*/) {
  _RPTF0(_CRT_WARN, "Entered DebugExtensionInitialize\n");
  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  _CrtMemCheckpoint(&memOld);

  winrt::com_ptr<IDebugClient> spDebugClient;
  winrt::com_ptr<IHostDataModelAccess> spDataModelAccess;

  HRESULT hr = DebugCreate(__uuidof(IDebugClient), spDebugClient.put_void());
  if (FAILED(hr)) return E_FAIL;

  if (!spDebugClient.try_as(spDataModelAccess)) return E_FAIL;

  hr = spDataModelAccess->GetDataModel(spDataModelManager.put(),
                                       spDebugHost.put());
  if (FAILED(hr)) return E_FAIL;

  return CreateExtension() ? S_OK : E_FAIL;
  return S_OK;
}

__declspec(dllexport) void __stdcall DebugExtensionUninitialize() {
  _RPTF0(_CRT_WARN, "Entered DebugExtensionUninitialize\n");
  DestroyExtension();
  spDebugHost = nullptr;
  spDataModelManager = nullptr;

  _CrtMemCheckpoint(&memNew);
  if (_CrtMemDifference(&memDiff, &memOld, &memNew)) {
    _CrtMemDumpStatistics(&memDiff);
  }
}

__declspec(dllexport) HRESULT __stdcall DebugExtensionCanUnload(void) {
  _RPTF0(_CRT_WARN, "Entered DebugExtensionCanUnload\n");
  uint32_t lock_count = winrt::get_module_lock().load();
  _RPTF1(_CRT_WARN, "module_lock count in DebugExtensionCanUnload is %d\n", lock_count);
  return lock_count == 0 ? S_OK : S_FALSE;
}

__declspec(dllexport) void __stdcall DebugExtensionUnload() {
  _RPTF0(_CRT_WARN, "Entered DebugExtensionUnload\n");
  return;
}

} // extern "C"
