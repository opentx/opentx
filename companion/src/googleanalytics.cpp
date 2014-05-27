#include "googleanalytics.h"
#include <QtGui>

#define TRACKING_ID  "UA-40098569-1"

GoogleAnalytics ga;

GoogleAnalytics::GoogleAnalytics():
  userId(qrand()),
  networkManager(new QNetworkAccessManager(this))
{
}

GoogleAnalytics::~GoogleAnalytics()
{
  delete networkManager;
}

void GoogleAnalytics::sendPageView(QString title)
{
  // create request and set URL of receiver
  QNetworkRequest request;
  QUrl host("http://www.google-analytics.com/collect");
  request.setUrl(host);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

  // setup parameters of request
  QString requestParams;
  requestParams += "v=1"; // version of protocol
  requestParams += "&t=pageview"; // type of request
  requestParams += "&tid=" TRACKING_ID; // Google Analytics account
  requestParams += "&cid=" + QString::number(userId); // unique user identifier
  requestParams += "&dp=companion-" + title.toLower().replace(" ", "-"); // name of page
  requestParams += "&dt=" + title; // title of page
  requestParams += "&ul=" + QLocale::system().name(); // language

  // send request via post method
  networkManager->post(request, requestParams.toStdString().c_str());
}

void GoogleAnalytics::sendEvent(QString category, QString action, QString label)
{
  // create request and set URL of receiver
  QNetworkRequest request;
  QUrl host("http://www.google-analytics.com/collect");
  request.setUrl(host);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

  // setup parameters of request
  QString requestParams;
  requestParams += "v=1"; // version of protocol
  requestParams += "&t=event"; // type of request
  requestParams += "&ec=" + category;
  requestParams += "&ea=" + action;
  requestParams += "&el=" + label;
  requestParams += "&tid=" TRACKING_ID; // Google Analytics account
  requestParams += "&cid=" + QString::number(userId); // unique user identifier
  requestParams += "&dt=Companion"; // name of page (or app name)
  requestParams += "&ul=" + QLocale::system().name(); // language

  // send request via post method
  networkManager->post(request, requestParams.toStdString().c_str());
}

void GoogleAnalytics::slot_receive(QNetworkReply * reply)
{
  // output information about reply
  qDebug() << "RequestUrl:" << reply->request().url();
  qDebug() << "Status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  qDebug() << "Error:"<< reply->error();
  QByteArray bytes = reply->readAll();
  qDebug() << "Contents" << QString::fromUtf8(bytes.data(), bytes.size());
}
