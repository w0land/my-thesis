#ifndef QRAPIDSHAREDOWNLOAD_H
#define QRAPIDSHAREDOWNLOAD_H
//

//
#include <QString>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QUrl>
#include <QDebug>
#include <QObject>
#include <QFile>
#include "debugutils.h"
#include "qrapidshareuser.h"
#include "rslogger.h"

#define RSDM_LOG_FUNC 	m_Logger.Write(__PRETTY_FUNCTION__ ) 
enum RapidShareStateMachine
{
	STOPPED = 0,
	GET_FIRST,
 	GET_SECOND,
  	POST_FIRST,
   	GET_THIRD,
   	DONE,
   	FAILED
};
QString StateToString(const RapidShareStateMachine & rsMachineState);
struct DownloadInfo
{
	DownloadInfo()
	{
		bytesReadCurrent = 0;
		bytesReadPreviously = 0;
	}
	int bytesReadCurrent;
	int bytesReadPreviously;
	int Diff()
	{
		int ret  = bytesReadCurrent - bytesReadPreviously;
		return (ret > 0 ) ? ret : -1 ; 
	}
};

class QRapidshareDownload : public QObject
{
Q_OBJECT
public:
	QRapidshareDownload(const QString & _UrlFileAddress = QString(""), const QString & _fileDest = QString("") );
	virtual 							~QRapidshareDownload();
	void								SetUser(const QRapidshareUser & rsUser );
	void								SetUser(const QString& rsName,  const QString& pass);
	void 								Download(const QString & _addr = QString("") , const QString & _fileDest = QString(""));
	/// ???? implement or not ? 
	static	void						DownloadFile(const QString & _addr);
	void								stop();
	void								SetRapidshareUser(const QRapidshareUser & _usr ) ;
	RapidShareStateMachine				GetState();
	
private:
	void								timerEvent(QTimerEvent *event);	
	QString								ParseResponseAndGetNewUrl(const QString & resp);
	int									ParseResponseAndGetFileSize(const QString & resp);
	void								TranslateAnswer();
	QString 							ParsePostReponseAndGetAddress(const QString & resp);
	void								SetUrlFileAddress(const QString & _addr ) ;
	
private:
	QString								m_UrlFileAddress;
	QString								m_ReferrerFileAddress;
	QString 							m_fileDestination;
	std::auto_ptr<QHttp>				m_apHttpObj;
	std::auto_ptr<QHttpRequestHeader>	m_apHttpRequestHeader;
	std::auto_ptr<QRapidshareUser>		m_apRSUser;
	bool								m_bIsPrepared;
	std::auto_ptr<QUrl>					m_apFileUrl;
	std::auto_ptr<QFile>				m_apFile;
	RapidShareStateMachine				m_RSStateMachine;
	std::auto_ptr<DownloadInfo>			m_downloadInfo;
	QString								m_HostName;
	QString								m_PathOnServer;
	QString								m_RequestType;	
	QFile 								m_FileLog;
	int 								m_timerId;
	qint64								m_readedBytes;
	RSLogger							m_Logger;
	
private slots:
	void 								requestStarted(const int & idReq) ;
	void 								requestFinished(const int & idReq, const bool & isFalse) ;
	void 								stateChanged(const int & iState) ;
	void 								dataSendProgress(const int & done, const int & total) ;
	void 								dataReadProgress(const int & done, const int & total) ;
	void								responseHeaderReceived( const QHttpResponseHeader & resp) ;
	void								done(const bool & error);
	void 								authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator *authenticator );
	void 								proxyAuthenticationRequired ( const QNetworkProxy & proxy, QAuthenticator * authenticator );
	void 								readyRead ( const QHttpResponseHeader & resp );
signals:
	void								WhatAmIDoing(const RapidShareStateMachine & what);
	void								DownloadStatus(const int & istate );
	void								Done();
	void								downloadRate(const QString & dwnlRate);
};
#endif
