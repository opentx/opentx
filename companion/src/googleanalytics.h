#include <QtNetwork>

class GoogleAnalytics: public QObject
{
  Q_OBJECT

  public:
    GoogleAnalytics();
    ~GoogleAnalytics();
    void sendPageView(QString page);
    void sendEvent(QString category, QString action, QString label);

  protected slots:
    void slot_receive(QNetworkReply * reply);

  protected:
    int userId;
    QNetworkAccessManager * networkManager;
};

extern GoogleAnalytics ga;
