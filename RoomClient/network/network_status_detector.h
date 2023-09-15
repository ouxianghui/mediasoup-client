//#pragma once
//
//#include <memory>
//#include "network.hpp"
//#include <QNetworkInformation>
//#include <mutex>
//#include "Utils/singleton.h"
//
//namespace vi {
//
//class NetworkStatusDetector :
//        public QObject,
//        public INetworkStatusDetector,
//        public std::enable_shared_from_this<NetworkStatusDetector>,
//        public Singleton<NetworkStatusDetector>
//{
//    Q_OBJECT
//public:
//    ~NetworkStatusDetector() override;
//
//    void init();
//
//    void addListener(const std::shared_ptr<INetworkStatusListener>& listener) override;
//
//    bool isOnline() const;
//
//protected:
//    void onReachabilityChanged(QNetworkInformation::Reachability newReachability);
//
//private:
//    NetworkStatusDetector();
//
//    NetworkStatusDetector(const NetworkStatusDetector&);
//
//    NetworkStatusDetector& operator=(const NetworkStatusDetector&);
//
//private:
//    std::vector<std::weak_ptr<INetworkStatusListener>> _listeners;
//
//    std::mutex _mutex;
//
//    QNetworkInformation::Reachability _reachability = QNetworkInformation::Reachability::Unknown;
//
//    friend class Singleton<NetworkStatusDetector>;
//};
//
//}
