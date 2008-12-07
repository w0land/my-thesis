#include "proxy.h"


#include <downloadmanager.h>
#include <enginemanager.h>
#include <optionscontainer.h>
#include <downloadengine.h>
#include <rslogger.h>
#include <QTime>
#include <cmath>

#include "settings.h"


Proxy * Proxy::proxy() 
{
	static std::auto_ptr<Proxy> pr ( new Proxy() ) ; 
	return pr.get() ;
}
Proxy::Proxy() : m_apSettings( new Settings() ) , m_pDownloadManager( new DownloadManager() ) 
{
};
Proxy::~Proxy()
{
    RSDM_LOG_FUNC;
}

DownloadManager * Proxy::downloadManager() 
{
    if ( proxy()->m_pDownloadManager== NULL ) 
        LOG(QString("Download Manager ptr is NULL " ) );
	return proxy()->m_pDownloadManager.get(); 
}
const EngineManager * Proxy::engineManager() 
{
	return proxy()->m_pDownloadManager->engineManager() ; 
}
Settings * Proxy::settings()
{
   return proxy()->m_apSettings.get() ; 
};
void Proxy::init()
{
    proxy()->m_apSettings->loadSettings();
    proxy()->m_pDownloadManager->init();
}

void Proxy::deinit()
{
    proxy()->m_apSettings.reset();
    proxy()->m_pDownloadManager.reset();
}