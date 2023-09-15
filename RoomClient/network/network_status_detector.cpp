//#include "network_status_detector.h"
//#include <assert.h>
//#include <QDebug>
//
//namespace vi {
//
//NetworkStatusDetector::NetworkStatusDetector()
//{
//    if (QNetworkInformation::load(QNetworkInformation::Feature::Reachability)) {
//        if (auto instance = QNetworkInformation::instance()) {
//            _reachability = instance->reachability();
//            connect(instance, &QNetworkInformation::reachabilityChanged, this, &NetworkStatusDetector::onReachabilityChanged);
//        }
//    }
//}
//
//NetworkStatusDetector::~NetworkStatusDetector()
//{
//
//}
//
//void NetworkStatusDetector::init()
//{
//    qDebug() << "available backends:";
//    auto list = QNetworkInformation::availableBackends();
//    for (std::size_t i = 0; i < list.count(); ++i) {
//        qDebug() << "index: " << i << list[i];
//    }
//}
//
//void NetworkStatusDetector::addListener(const std::shared_ptr<INetworkStatusListener>& listener)
//{
//    assert(listener != nullptr);
//    {
//        std::lock_guard<std::mutex> locker(_mutex);
//        _listeners.emplace_back(listener);
//    }
//}
//
//void NetworkStatusDetector::onReachabilityChanged(QNetworkInformation::Reachability newReachability)
//{
//    if (newReachability != _reachability) {
//        _reachability = newReachability;
//        bool isOnline = _reachability == QNetworkInformation::Reachability::Online;
//        std::vector<std::weak_ptr<INetworkStatusListener>> listeners;
//        {
//            std::lock_guard<std::mutex> locker(_mutex);
//            listeners = _listeners;
//        }
//        auto it = listeners.begin();
//        while (it != listeners.end()) {
//            if (auto listener = (*it).lock()) {
//                listener->onNetworkStatusChanged(isOnline);
//                ++it;
//            }
//            else {
//                it = listeners.erase(it);
//            }
//        }
//    }
//}
//
//bool NetworkStatusDetector::isOnline() const
//{
//    if (QNetworkInformation::instance()->supports(QNetworkInformation::Feature::Reachability)) {
//        return _reachability == QNetworkInformation::Reachability::Online;
//    }
//    else {
//        return true;
//    }
//}
//
//}
