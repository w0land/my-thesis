 #include "qrapidsharedownload.h"
 
 
 QRapidshareDownload::QRapidshareDownload( /*const QString & _UrlFileAddress, const QString & _fileDest*/ ) 
 : m_apHttpObj( new QHttp() )
 , m_apHttpRequestHeader(new QHttpRequestHeader() )
 , m_apRSUser(NULL)
 , m_apFileUrl( new QUrl() )
 , m_apFile(new QFile() )
 , m_timerId(0)
 , m_readedBytes(0)
 , m_Logger(QString("qrapidsharedownload") + QString::number(qrand() ) )
 
 {
 	RSDM_LOG_FUNC ;
 	QObject::connect( m_apHttpObj.get(), SIGNAL( requestStarted( int ) ), this, SLOT( requestStarted( int ) ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( requestFinished( int,bool ) ), this, SLOT( requestFinished( int,bool ) ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( stateChanged( int ) ), this, SLOT( stateChanged( int ) ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( dataSendProgress( int,int ) ), this, SLOT(  dataSendProgress( int,int ) ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( responseHeaderReceived( const QHttpResponseHeader & ) ), this, SLOT(  responseHeaderReceived( const QHttpResponseHeader & ) ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( dataReadProgress( int,int ) ), this, SLOT(  dataReadProgress( int,int ) ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( done( bool ) ), this, SLOT(  done( bool ) ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( authenticationRequired(  const QString , quint16 , QAuthenticator *) ), this, SLOT(  authenticationRequired(  const QString , quint16 , QAuthenticator *)  ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( proxyAuthenticationRequired ( QNetworkProxy , QAuthenticator * ) ), this, SLOT(  proxyAuthenticationRequired ( QNetworkProxy , QAuthenticator * ) ) );
 	QObject::connect( m_apHttpObj.get(), SIGNAL( readyRead ( QHttpResponseHeader ) ), this, SLOT(  readyRead ( QHttpResponseHeader ) ) );	
 }

 QRapidshareDownload::~QRapidshareDownload()
 {
 	RSDM_LOG_FUNC ;
 	m_apHttpObj.get()->disconnect();
 }

 void QRapidshareDownload::start()
 {
 	RSDM_LOG_FUNC ;
    
    setUrlFileAddress( m_UrlAddress.c_str() );
    //invalid url set
    if( m_apFileUrl->isEmpty())
        return;

    m_ReferrerFileAddress = m_UrlAddress.c_str();

    QString fileName(m_FileDestination.c_str());
    if( fileName.isEmpty())
       return;

    m_apFile->setFileName( fileName + ".part");

 	if ( QFile::exists(m_fileDestination) )
 	{
 		 // resume downloading 
 	}
	//FIXME: add state machine with GET_FIRST etc.
 	m_rssmState = GET_FIRST;
    m_pDownloadInfo->m_State = DownloadState::INIT;
	//FIXME:
 	emit whatAmIDoing( m_pDownloadInfo->m_State );
 	m_apHttpRequestHeader->setRequest("GET", m_apFileUrl->path() );
 	m_apHttpRequestHeader->setValue("Host",  m_apFileUrl->host() );
 	m_apHttpRequestHeader->setValue("Connection", "Keep-Alive");
 	m_apHttpRequestHeader->setValue("Cookie", m_apRSUser->ComposeCookie() );
 	m_apHttpRequestHeader->setValue("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; ru) Opera 8.50");
 	m_apHttpRequestHeader->setValue("Referer", m_ReferrerFileAddress );
 	m_Logger << QString("First GET");
 	m_Logger << DebugUtils::httpReqToString(*m_apHttpRequestHeader) ;
 	m_apHttpObj->setHost(  m_apFileUrl->host() );
 	m_apHttpObj->request( *( m_apHttpRequestHeader ) );
 }

 void QRapidshareDownload::stop()
 {
 	RSDM_LOG_FUNC ;
 	if( m_pDownloadInfo->m_State  == DownloadState::STOPPED
 		|| m_pDownloadInfo->m_State  == DownloadState::DONE
 		|| m_pDownloadInfo->m_State  == DownloadState::FAILED)
 		return ; 
    m_pDownloadInfo->m_State  = DownloadState::PAUSED ;
 	m_apHttpObj->abort();
 	emit whatAmIDoing( m_pDownloadInfo->m_State );
 }

 void QRapidshareDownload::restart()
 {
 	RSDM_LOG_FUNC ;
 	//m_apHttpRequestHeader->removeValue();
 	m_apHttpRequestHeader->setRequest("GET", m_apFileUrl->path() );
 	m_apHttpRequestHeader->setValue("Host", m_DownloadServerHost );
 	m_apHttpRequestHeader->setValue("Connection", "Keep-Alive");
 	m_apHttpRequestHeader->setValue("Cookie", m_apRSUser->ComposeCookie() );
 	m_apHttpRequestHeader->setValue("Range", "bytes=" + QString::number(m_pDownloadInfo->m_BytesDownloaded)+ "-" );
 	m_apHttpRequestHeader->setValue("User-Agent", "Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)");
 	m_apHttpRequestHeader->setValue("Referer", m_ReferrerFileAddress);
 	m_Logger << QString("Resumed !!!!!!");
 	QString httpHeader = DebugUtils::httpReqToString(*m_apHttpRequestHeader) ;
 	m_Logger << httpHeader;
 	QString host = m_apFileUrl->host();
 	m_apHttpObj->setHost( m_DownloadServerHost );
 	m_apHttpObj->request( *( m_apHttpRequestHeader ) );
 }

 void QRapidshareDownload::setUrlFileAddress(const QString & _addr )
 {
 	RSDM_LOG_FUNC ;
 	DebugUtils::q_Log( QString(" _addr = ") + _addr);
 	if( ! _addr.isEmpty() )
 	{
 		m_apFileUrl.reset( new QUrl( _addr ) );
 	}
 }

/*
 void QRapidshareDownload::Download(const QString & _addr, const QString & _fileDest )
 {
 	RSDM_LOG_FUNC ;
 	SetUrlFileAddress( _addr );
 	if( !_addr.isEmpty() )
 		m_ReferrerFileAddress = _addr;
 	if( !_fileDest.isEmpty() )
 	{
 		m_fileDestination = _fileDest;
 	}
 	if( m_ReferrerFileAddress.isEmpty() || m_fileDestination.isEmpty() )
 		return ; 
	
 	m_apFile->setFileName(m_fileDestination + ".part");
 	//m_pDownloadInfo->m_FileInfo.m_FileName = m_fileDestination;
	
 	if ( QFile::exists(m_fileDestination) )
 	{
 		 // resume downloading 
 	}
	//FIXME: add state machine with GET_FIRST etc.
 	m_rssmState = GET_FIRST;
    m_pDownloadInfo->m_State = DownloadState::INIT;
	//FIXME:
 	emit whatAmIDoing( m_pDownloadInfo->m_State );
 	m_apHttpRequestHeader->setRequest("GET", m_apFileUrl->path() );
 	m_apHttpRequestHeader->setValue("Host",  m_apFileUrl->host() );
 	m_apHttpRequestHeader->setValue("Connection", "Keep-Alive");
 	m_apHttpRequestHeader->setValue("Cookie", m_apRSUser->ComposeCookie() );
 	m_apHttpRequestHeader->setValue("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; ru) Opera 8.50");
 	m_apHttpRequestHeader->setValue("Referer", m_ReferrerFileAddress );
 	m_Logger << QString("First GET");
 	m_Logger << DebugUtils::httpReqToString(*m_apHttpRequestHeader) ;
 	m_apHttpObj->setHost(  m_apFileUrl->host() );
 	m_apHttpObj->request( *( m_apHttpRequestHeader ) );
 };
 */
 
 
 /********** SLOTS **************/
 
 void QRapidshareDownload::requestStarted(const int & idReq)
 {
 	m_Logger << __FUNCTION_NAME__<< "idReq =  " << idReq ;
 }
 void QRapidshareDownload::requestFinished(const int & idReq, const bool & isFalse)
 {	
 	m_Logger << __FUNCTION_NAME__<< "idReq =  " << idReq << "isFalse=" << isFalse ;
 	m_apFile->close();
 	if( isFalse )
 	{
 		m_Logger << m_apHttpObj->errorString() ;
 		return ; 
 	}
 }
 void QRapidshareDownload::stateChanged(const int & iState)
 {	
 	m_Logger << __FUNCTION_NAME__<< "iState =  " << iState ;
 }
 void QRapidshareDownload::dataSendProgress(const int & done, const int & total)
 {	
 	m_Logger << __FUNCTION_NAME__<< "done =  " << done << "total=" << total ;
 }
 void QRapidshareDownload::dataReadProgress(const int & done, const int & total)
 {
 	m_Logger << __FUNCTION_NAME__<< "done =  " << done << "total=" << total ;
 	// support for Direct downloads
 	char *buff = NULL ; 
 	qint64 iBytes2 = 0;
 	qint64 bytes = m_apHttpObj->bytesAvailable();
 	m_readedBytes+=bytes;
 	if( m_rssmState == GET_THIRD || m_rssmState == POST_FIRST )
 	{
 		buff = new char[bytes];
 		iBytes2 = m_apHttpObj->read(buff, bytes);
 		if( -1 == iBytes2 )
 			m_Logger<<("ERROR while reading from Http stream ");
 		QString decive ( buff ) ;
 		if( decive.contains("<!DOCTYPE"))
 		{
 			// this is still html. just do nothing 
 			return ; 
 		}
        m_rssmState = DOWNLOADING;
        m_pDownloadInfo->m_State = DownloadState::DOWNLOADING;
 		emit whatAmIDoing( m_pDownloadInfo->m_State ) ; 
 		m_timerId = startTimer(1000);
 		m_readedBytes = 0;
 	}
    if( m_pDownloadInfo->m_State == DownloadState::PAUSED )
 	{
 		if(m_apFile.get() !=NULL)
 		{
 			if(!m_apFile->isOpen())
 			{
 				m_Logger << "Openning file to resume " ; 
 				if(!m_apFile->open(QIODevice::WriteOnly | QIODevice::Append ) )
 				{
 					m_Logger << "UnSuccessfull opening file " ; 
 					return ; 
 				}
 				m_pDownloadInfo->m_State = DownloadState::DOWNLOADING;
 				emit whatAmIDoing( m_pDownloadInfo->m_State );
 				m_timerId = startTimer(1000);
 				m_readedBytes = 0; 
 			}
 		}
 	}
 	if ( m_pDownloadInfo->m_State  == DOWNLOADING ) 
 	{
 		//m_pDownloadInfo->m_FileInfo.m_FileSize = total ; 
 		// just past it to the file ! 
 		int bytesDownloadedOverall = (m_pDownloadInfo->m_DownloadFileSize - total) > 0 ?  m_pDownloadInfo->m_DownloadFileSize - total : 0 ; 
 		m_pDownloadInfo->m_BytesDownloaded = done + bytesDownloadedOverall; 
 		double dDone = m_pDownloadInfo->m_BytesDownloaded;
 		double dTotal = m_pDownloadInfo->m_DownloadFileSize;
 		double dResTotal = dDone / dTotal;
 		dResTotal *= 100;
 		m_Progress = (int)dResTotal;
 		m_Logger << "emit :DownloadStatus("<< m_Progress <<")";  
 		emit downloadStatus(m_Progress) ;
 
 		m_pDownloadInfo->bytesReadPreviously =m_pDownloadInfo->bytesReadCurrent;
 		m_pDownloadInfo->bytesReadCurrent = done ; 
 		if ( buff == NULL ) 
 		{
 			buff = new char[bytes];
 			iBytes2 = m_apHttpObj->read(buff, bytes);
 		}
 		if ( -1 == iBytes2)
 			m_Logger<<("ERROR while reading from Http stream ");
 
 		else
 		{
 			m_Logger << "readed bytes " << iBytes2;
 			if( !m_apFile->isOpen())
 			{
 				m_Logger << ("isClosed, open it");
 				if( ! m_apFile->open(QIODevice::WriteOnly | QIODevice::Append) )
 				{
 					m_Logger<<("ERROR ..:::DDD124");
 					// message = bad
 					// 
 					return ;
 				}
 				else
 					m_Logger << "opened";
 			}
 
 			qint64 btmp = m_apFile->write(buff,iBytes2); 
 			if( -1 == btmp )
 			{
 				m_Logger<<("write failed");
 				return;
 			}
 			else
 				m_Logger << ("written successful") << btmp;
 		}
 		delete[] buff;
 	}
 
    if( done == total &&  m_rssmState ==  DOWNLOADING ) 
    {
        m_pDownloadInfo->m_State = DownloadState::DONE;
        m_rssmState = FINISHED;
    }
 }
 void QRapidshareDownload::authenticationRequired(const QString & hostname, quint16 port, QAuthenticator * authenticator)
 {
 	m_Logger << __FUNCTION_NAME__<< "hostname =  " << hostname << "port=" << port ;
 }
 void QRapidshareDownload::proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator)
 {
 	m_Logger << __FUNCTION_NAME__;
 }
 void QRapidshareDownload::readyRead ( const QHttpResponseHeader & resp )
 {
 	m_Logger << __FUNCTION_NAME__;
 }
 
 void QRapidshareDownload::responseHeaderReceived( const QHttpResponseHeader & resp)
 {
 	RSDM_LOG_FUNC ;
 	m_Logger << resp.reasonPhrase() ;
 	int iStatusCode = resp.statusCode(); 
 	if( iStatusCode == 200 || iStatusCode == 301 || iStatusCode == 302 || iStatusCode == 303 || iStatusCode == 307 )
 	{
 		;
 	}
 	else
 		m_Logger << "Error response:"<< iStatusCode;
 }
 void QRapidshareDownload::done(const bool & error)
 {
 	RSDM_LOG_FUNC ;
 	if( error )
 	{
 		m_Logger << m_apHttpObj->errorString();
 		return ;
 	}
 	if( m_rssmState == GET_FIRST )
 	{
 		//m_RSStateMachine = GET_SECOND;
 		QByteArray aa = m_apHttpObj->readAll();
 		DebugUtils::DumpReponseToFile(aa,"get_first");
 		m_apHttpRequestHeader->removeValue("Cookie");
 		m_apHttpRequestHeader->setRequest("GET", m_apFileUrl->path() );
 		m_apHttpRequestHeader->setValue("Host", m_apFileUrl->host() );
 		m_apHttpRequestHeader->setValue("Accept", "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, ap//plication/vnd.ms-powerpoint, application/msword, application/x-shockwave-flash, */*" );
 		m_apHttpRequestHeader->setValue("Referer", m_ReferrerFileAddress );
 		m_Logger << "Second GET";		
 		m_Logger << DebugUtils::httpReqToString(*m_apHttpRequestHeader) ;
 		m_apHttpObj->setHost( m_apFileUrl->host() );
 		m_apHttpObj->request( *( m_apHttpRequestHeader ));
  		m_rssmState = GET_SECOND ;
 	}
 	else if( m_rssmState == GET_SECOND )
 	{
 		QByteArray aa = m_apHttpObj->readAll();
 		DebugUtils::DumpReponseToFile(aa,"get_second");
 		QString newUrl = aa;
 		newUrl = parseResponseAndGetNewUrl( newUrl );
 		int iFileSize = parseResponseAndGetFileSize( aa ) ;
 		m_pDownloadInfo->m_DownloadFileSize = iFileSize ; 
 		if(newUrl.isEmpty() )
 		{
 			m_Logger<<("Could not find file on server");
            m_pDownloadInfo->m_State = DownloadState::States::FAILED;
 			emit whatAmIDoing( m_pDownloadInfo->m_State );
 			emit done();
 			return ;
 		}
 		setUrlFileAddress( newUrl );
 		emit whatAmIDoing( m_pDownloadInfo->m_State );
 		m_apHttpRequestHeader.reset(new QHttpRequestHeader() );
 		m_DownloadServerHost = QUrl(newUrl).host();
 		m_apHttpRequestHeader->setRequest("POST", m_apFileUrl->path() );
 		m_apHttpRequestHeader->setValue("Host", m_apFileUrl->host() );
 		m_apHttpRequestHeader->setValue("Connection", "Keep-Alive");
 		m_apHttpRequestHeader->setValue("Cookie", m_apRSUser->ComposeCookie() );
 		m_apHttpRequestHeader->setValue("User-Agent", "Mozilla/4.0 (compatible; Synapse)");
 		m_apHttpRequestHeader->setValue("Content-Type", "application/x-www-form-urlencoded");
 		m_apHttpRequestHeader->setValue("Content-Length", "16");
 		m_apHttpRequestHeader->setValue("Referrer", m_ReferrerFileAddress);
 		m_Logger << "First post"; 		
 		m_Logger << DebugUtils::httpReqToString(*m_apHttpRequestHeader) ;
 		m_apHttpObj->setHost( m_apFileUrl->host() );
 		m_apHttpObj->request( *( m_apHttpRequestHeader ), "dl.start=PREMIUM");
  		m_rssmState = POST_FIRST ;
  	}
 	else if( m_rssmState == POST_FIRST)
 	{
  		QByteArray aa = m_apHttpObj->readAll() ;
  		QString newUrlpath = parsePostReponseAndGetAddress( QString( aa ) );
  		DebugUtils::DumpReponseToFile(aa,"post_first");
  		
  		setUrlFileAddress(newUrlpath);
        //FIXME:
  		emit whatAmIDoing( m_pDownloadInfo->m_State );
  		m_apHttpRequestHeader.reset(new QHttpRequestHeader() );
 		m_apHttpRequestHeader->setRequest("GET", m_apFileUrl->path() );
 		m_apHttpRequestHeader->setValue("Host", m_apFileUrl->host() );
 		m_apHttpRequestHeader->setValue("Connection", "Keep-Alive");
 		m_apHttpRequestHeader->setValue("Cookie", m_apRSUser->ComposeCookie() );
 		m_apHttpRequestHeader->setValue("User-Agent", "Mozilla/4.0 (compatible; Synapse)");
 		m_apHttpRequestHeader->setValue("Referer", m_ReferrerFileAddress);
 		m_Logger << "First post"; 
 		m_rssmState = GET_THIRD; 
 		m_Logger << DebugUtils::httpReqToString(*m_apHttpRequestHeader) ;		
 		m_apHttpObj->setHost( m_apFileUrl->host() );
 		m_apHttpObj->request( *( m_apHttpRequestHeader ) );
 		m_timerId = startTimer(1000);
 		m_readedBytes = 0;
  	}
  	else if( m_pDownloadInfo->m_State == DownloadState::DONE )
  	{	
  		m_apFile->close();
  		renameFile();
  		emit done();
        emit whatAmIDoing( m_pDownloadInfo->m_State );
  		killTimer(m_timerId);
 	}
    else if( m_pDownloadInfo->m_State == DownloadState::PAUSED ) 
 	{
 		QByteArray aa = m_apHttpObj->readAll() ;
 		QString newUrlpath = parsePostReponseAndGetAddress( QString( aa ) );
 		DebugUtils::DumpReponseToFile(aa,"paused");
 	}
 };

 /*
 //fixme:
 void QRapidshareDownload::abort()
 {
 	RSDM_LOG_FUNC ;
    //FIXME:
 	//m_rssmState = DownloadState::STOPPED;
    m_pDownloadInfo->m_State = DownloadState::STOPPED;
 	m_apHttpObj->abort();
 	// do not emit, cause it will be removed from list. 
 }
 */

 QString QRapidshareDownload::parseResponseAndGetNewUrl(const QString & resp)
 {
 	RSDM_LOG_FUNC ;
 	QString line;
 	QString newUrl;
 	int z = 0;
 	z = resp.indexOf("form action");
 	if(z < 0 )
 	{
 		m_Logger << "Could not localize substring!" << resp ;
 		if( resp.indexOf("The file could not be found") >=0 )
 		{
 			
 		}
 		return "";
 	}
 	for(;;)
 	{
 		if( resp.at(z) == '\n')
 			break;
 		line +=resp.at(z++);
 	}
 	bool quot = false;
 	bool write = false;
 	for(z = 0;z< line.size();++z)
 	{
 		if( line.at(z) == '"' )
 		{
 			if( !quot )
 			{
 				write = true;
 			}
 			else
 				break;
 			quot = !quot;
 		}
 		if( write )
 			newUrl += line.at(z);
 	}
 	newUrl.remove('"');
 	return newUrl;
 }
 int QRapidshareDownload::parseResponseAndGetFileSize(const QString & resp)
 {
 	RSDM_LOG_FUNC ;
 	QString line;
 	QString newUrl;
 	int z = 0;
 	z = resp.indexOf("downloadlink");
 	if(z < 0 )
 	{
 		m_Logger <<"err: Hoho, response is zero!";
 		return -1;
 	}
 	for(;;)
 	{
 		if( resp.at(z) == '\n')
 			break;
 		line +=resp.at(z++);
 	}
 	bool stick = false;
 	for(z = 0;z< line.size();++z)
 	{
 		if( line.at(z) == '|' )
 		{
 			stick = true;
 		}
 		if( stick )
 		{
 			if( line.at(z) == 'K')
 				break;
 			newUrl += line.at(z);
 		}
 	}
 	newUrl.remove(' ');
 	newUrl.remove('|');
 	bool ok;
 	int ret = newUrl.toInt(&ok) * 1000 ;
 	if( !ok )
 		return -1;
 	return ret;
 }
 QString QRapidshareDownload::parsePostReponseAndGetAddress( const QString & resp )
 {
 	RSDM_LOG_FUNC ;
 	QString line;
 	QString newUrl;
 	int z = 0;
 	z = resp.indexOf("form name");
 	if(z < 0 )
 	{
 		m_Logger << " err : Could not localize substring!";
 		return "";
 	}
 	for(;;)
 	{
 		if( resp.at(z) == '\n')
 			break;
 		line +=resp.at(z++);
 	}
 	int iStartPos = line.indexOf("http://");
 	if( iStartPos < 0 )
 		return "";
 	for(int i=iStartPos;i < line.size() ;++i)
 	{
 		if(line.at(i) =='"')
 			break;
 		newUrl +=line.at(i);
 	}
 	m_Logger << newUrl;
 	return newUrl;
 }
 void QRapidshareDownload::setUser(const QRapidshareUser & rsUser)
 {
 	RSDM_LOG_FUNC ;
 	m_apRSUser.reset(new QRapidshareUser(rsUser));	
 }
 void QRapidshareDownload::setUser(const QString& rsName,  const QString& rsPass)
 {
 	RSDM_LOG_FUNC ;
 	setUser(QRapidshareUser(rsName,rsPass));
 }
/* RapidShareStateMachine QRapidshareDownload::GetState() const 
 {
 	RSDM_LOG_FUNC ;
 	return m_pDownloadInfo->m_State.m_rssmState;
 }
 */
 
 void QRapidshareDownload::timerEvent(QTimerEvent *event)
 {
 	RSDM_LOG_FUNC ;
 	emit downloadRate( QString("%1").arg( ((double) m_readedBytes / 1024),0, 'f',2) ); 
 	m_readedBytes = 0;
 };
 const QString QRapidshareDownload::getFullUrlFileAddress() const
 {
 	RSDM_LOG_FUNC ;
 	return m_ReferrerFileAddress ; 
 }
 const QString QRapidshareDownload::getFileDestination() const
 {
 	RSDM_LOG_FUNC ;
 	return m_fileDestination ;
 };
 
 /*
 unsigned int QRapidshareDownload::GetPercentage( ) const 
 {
 	RSDM_LOG_FUNC ;
 	return m_pDownloadInfo->m_State.m_bytesDownloaded ; 
 }
 */
 
 QRapidshareUser * QRapidshareDownload::getUser() const
 {
 	return m_apRSUser.get() ; 
 }
 const QString QRapidshareDownload::getDownloadHost() const
 {
 	return m_DownloadServerHost; 
 }
 
 void QRapidshareDownload::setDownloadHost( const QString & _host )
 {
 	m_DownloadServerHost = _host ;
 }
 
 void QRapidshareDownload::renameFile()
 {
 	if(m_apFile.get()!=NULL && !m_fileDestination.isEmpty())
 		QFile::rename(m_apFile->fileName(), m_fileDestination);
 }
 
 