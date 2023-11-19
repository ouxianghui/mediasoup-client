/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <any>
#include "i_notification.h"

namespace vi {

    class KeyValueNotification : public INotification, public std::enable_shared_from_this<KeyValueNotification>
    {
    public:
        KeyValueNotification(const std::string& key, std::any value) : m_key(key), m_value(std::move(value)) {}

        const std::string& key() const { return m_key; }

        std::any& value() { return m_value; }

    private:
        std::string m_key;

        std::any m_value;
    };
}
