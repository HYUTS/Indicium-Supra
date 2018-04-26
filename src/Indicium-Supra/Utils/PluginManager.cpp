#include "PluginManager.h"
#include "Misc.h"

#include <Shlwapi.h>
#include <algorithm>

#define POCO_NO_UNWINDOWS
#include <Poco/Logger.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Glob.h>

using Poco::Logger;
using Poco::Path;
using Poco::File;
using Poco::Glob;


PluginManager::PluginManager()
{
    GetModuleFileName(reinterpret_cast<HINSTANCE>(&__ImageBase), m_DllPath, _countof(m_DllPath));
    PathRemoveFileSpec(m_DllPath);
}

PluginManager::~PluginManager()
{
}

void PluginManager::load()
{
    auto& logger = Logger::get(LOG_REGION());

    ScopedLock<FastMutex> lock(mPlugins);

    //
    // Fetch libraries with names ending in ".Plugin.dll"
    // 
    std::set<std::string> files;
    const Path pluginDir(m_DllPath, "*.Plugin.dll");
    Glob::glob(pluginDir, files);

    std::set<std::string>::iterator it = files.begin();
    for (; it != files.end(); ++it)
    {
        std::cout << *it << std::endl;
    }

    /*
    File root(m_DllPath);
    std::vector<File> files;
    root.list(files);

    logger.information("Enumerating plugins...");

    for (auto it = files.begin(); it != files.end(); ++it)
    {
        if ((*it).isFile() && findStringIC((*it).path(), ".Plugin.dll"))
        {
            logger.information("Found plugin %s", it->path());

            auto plugin = new SharedLibrary();

            try
            {
                plugin->load(it->path());
            }
            catch (Poco::LibraryLoadException& lle)
            {
                logger.error("Couldn't load plugin %s: %s", it->path(), lle.displayText());
                delete plugin;
                continue;
            }

            for (const auto& symbol : exports)
            {
                if (plugin->hasSymbol(symbol))
                {
                    _fpMap[symbol].push_back(plugin->getSymbol(symbol));
                }
                else
                {
                    logger.warning("Missing export %s from plugin %s", symbol, plugin->getPath());
                }
            }

            if (!plugin->isLoaded())
            {
                delete plugin;
                continue;
            }

            plugins.push_back(plugin);
        }
    }
    */

    logger.information("Finished enumerating plugins");
}

void PluginManager::unload()
{
    auto& logger = Logger::get(LOG_REGION());

    logger.information("Unloading plugins...");

    ScopedLock<FastMutex> lock(mPlugins);

    // discard function pointers
    for (const auto& symbol : exports)
    {
        _fpMap[symbol].clear();
    }

    // unload libraries
    for (auto it = plugins.begin(); it != plugins.end();)
    {
        if ((*it)->isLoaded())
            (*it)->unload();
        delete *it;
        it = plugins.erase(it);
    }

    logger.information("Finished unloading plugins");
}

void PluginManager::present(IID guid, LPVOID unknown, Direct3DVersion version)
{
    static std::string key = "Present";

    for (auto& fp : _fpMap[key])
    {
        static_cast<VOID(__cdecl*)(IID, LPVOID, Direct3DVersion)>(fp)(guid, unknown, version);
    }
}

void PluginManager::reset(IID guid, LPVOID unknown, Direct3DVersion version)
{
    static std::string key = "Reset";

    for (auto& fp : _fpMap[key])
    {
        static_cast<VOID(__cdecl*)(IID, LPVOID, Direct3DVersion)>(fp)(guid, unknown, version);
    }
}

void PluginManager::endScene(IID guid, LPVOID unknown, Direct3DVersion version)
{
    static std::string key = "EndScene";

    for (auto& fp : _fpMap[key])
    {
        static_cast<VOID(__cdecl*)(IID, LPVOID, Direct3DVersion)>(fp)(guid, unknown, version);
    }
}

void PluginManager::resizeTarget(IID guid, LPVOID unknown, Direct3DVersion version)
{
    static std::string key = "ResizeTarget";

    for (auto& fp : _fpMap[key])
    {
        static_cast<VOID(__cdecl*)(IID, LPVOID, Direct3DVersion)>(fp)(guid, unknown, version);
    }
}

void PluginManager::load(Direct3DVersion version)
{
    //
    // Fetch libraries with names ending in ".Plugin.dll"
    // 
    std::set<std::string> files;
    const Path pluginDir(m_DllPath, "*.Plugin.dll");
    Glob::glob(pluginDir, files);

    std::set<std::string>::iterator it = files.begin();
    for (; it != files.end(); ++it)
    {
        std::cout << *it << std::endl;
    }
}

void PluginManager::d3d9_present(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect,
                                 HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
}

void PluginManager::d3d9_reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
}

void PluginManager::d3d9_endscene(LPDIRECT3DDEVICE9 pDevice)
{
}

void PluginManager::d3d9_presentex(LPDIRECT3DDEVICE9EX pDevice, const RECT* pSourceRect, const RECT* pDestRect,
                                   HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags)
{
}

void PluginManager::d3d9_resetex(LPDIRECT3DDEVICE9EX pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters,
                                 D3DDISPLAYMODEEX* pFullscreenDisplayMode)
{
}

void PluginManager::d3d10_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
}

void PluginManager::d3d10_resizetarget(IDXGISwapChain* pSwapChain, const DXGI_MODE_DESC* pNewTargetParameters)
{
}

void PluginManager::d3d11_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
}

void PluginManager::d3d11_resizetarget(IDXGISwapChain* pSwapChain, const DXGI_MODE_DESC* pNewTargetParameters)
{
}

void PluginManager::d3d12_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
}

void PluginManager::d3d12_resizetarget(IDXGISwapChain* pSwapChain, const DXGI_MODE_DESC* pNewTargetParameters)
{
}
