#ifndef INFLUXDATA_TRANSPORTS_HTTP_CLIENT_WRAPPER_H
#define INFLUXDATA_TRANSPORTS_HTTP_CLIENT_WRAPPER_H

#include <memory>
#include <string>
#include <Transport.h>
#include <HttpClient.hpp>

namespace influxdb
{
    namespace transports
    {
        //
        // implementation of Transport interface
        //
        class HttpClientWrapper : public Transport, HttpClient {
        protected:
            std::string hostname;
            std::string readUrl;
            std::string writeUrl;

        public:
            HttpClientWrapper(const std::string& hostname) {
                this->hostname = hostname;
                writeUrl = hostname;
                auto position = writeUrl.find("?");
                writeUrl.insert(position, (writeUrl.at(position - 1) != '/' ? "/write" : "write"));
                //writeUrl.append("&precision=ms");
                readUrl = hostname + "&q=";
                position = readUrl.find("?");
                readUrl.insert(position, (readUrl.at(position - 1) != '/' ? "/query" : "query"));
            }
            virtual ~HttpClientWrapper(void) {}

            virtual void send(std::string&& message) {
                std::string response, content;
                sendHttpPostRequest(writeUrl, message, response, content);
            }
            virtual std::string query(const std::string& query) {
                auto position = readUrl.find("&q=");
                std::string url = readUrl.insert(position + 3, query);
                std::string response, content;
                sendHttpGetRequest(url, response, content);
                return content;
            }
        };
    }
}
#endif

