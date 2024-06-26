/*
 * HttpsServer.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#include "object.h"
#include "HttpsServer.h"
#include "ifs/http.h"

namespace fibjs {

result_t HttpsServer_base::_new(v8::Local<v8::Array> certs, int32_t port,
    Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new(certs, "", port, hdlr, retVal, This);
}

result_t HttpsServer_base::_new(v8::Local<v8::Array> certs, exlib::string addr, int32_t port,
    Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<HttpsServer> svr = new HttpsServer();
    svr->wrap(This);

    result_t hr = svr->create(certs, addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t HttpsServer_base::_new(X509Cert_base* crt, PKey_base* key, int32_t port,
    Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new(crt, key, "", port, hdlr, retVal, This);
}

result_t HttpsServer_base::_new(X509Cert_base* crt, PKey_base* key, exlib::string addr, int32_t port,
    Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<HttpsServer> svr = new HttpsServer();
    svr->wrap(This);

    result_t hr = svr->create(crt, key, addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t HttpsServer::create(X509Cert_base* crt, PKey_base* key, exlib::string addr, int32_t port,
    Handler_base* hdlr)
{
    result_t hr;
    obj_ptr<SslServer_base> _server;
    obj_ptr<HttpHandler_base> _handler;

    hr = HttpHandler_base::_new(hdlr, _handler);
    if (hr < 0)
        return hr;

    hr = SslServer_base::_new(crt, key, addr, port, _handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", _handler->wrap());
    m_hdlr = _handler;

    SetPrivate("server", _server->wrap());
    m_server = _server;

    return 0;
}

result_t HttpsServer::create(v8::Local<v8::Array> certs, exlib::string addr, int32_t port,
    Handler_base* hdlr)
{
    result_t hr;
    obj_ptr<SslServer_base> _server;
    obj_ptr<HttpHandler_base> _handler;

    hr = HttpHandler_base::_new(hdlr, _handler);
    if (hr < 0)
        return hr;

    hr = SslServer_base::_new(certs, addr, port, _handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", _handler->wrap());
    m_hdlr = _handler;

    SetPrivate("server", _server->wrap());
    m_server = _server;

    return 0;
}

result_t HttpsServer::start()
{
    return m_server->start();
}

result_t HttpsServer::stop(AsyncEvent* ac)
{
    return m_server->stop(ac);
}

result_t HttpsServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    return m_server->get_socket(retVal);
}

result_t HttpsServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    return m_hdlr->get_handler(retVal);
}

result_t HttpsServer::set_handler(Handler_base* newVal)
{
    return m_hdlr->set_handler(newVal);
}

result_t HttpsServer::enableCrossOrigin(exlib::string allowHeaders)
{
    return m_hdlr->enableCrossOrigin(allowHeaders);
}

result_t HttpsServer::get_maxHeadersCount(int32_t& retVal)
{
    return m_hdlr->get_maxHeadersCount(retVal);
}

result_t HttpsServer::set_maxHeadersCount(int32_t newVal)
{
    return m_hdlr->set_maxHeadersCount(newVal);
}

result_t HttpsServer::get_maxHeaderSize(int32_t& retVal)
{
    return m_hdlr->get_maxHeaderSize(retVal);
}

result_t HttpsServer::set_maxHeaderSize(int32_t newVal)
{
    return m_hdlr->set_maxHeaderSize(newVal);
}

result_t HttpsServer::get_maxBodySize(int32_t& retVal)
{
    return m_hdlr->get_maxBodySize(retVal);
}

result_t HttpsServer::set_maxBodySize(int32_t newVal)
{
    return m_hdlr->set_maxBodySize(newVal);
}

result_t HttpsServer::get_enableEncoding(bool& retVal)
{
    return m_hdlr->get_enableEncoding(retVal);
}

result_t HttpsServer::set_enableEncoding(bool newVal)
{
    return m_hdlr->set_enableEncoding(newVal);
}

result_t HttpsServer::get_serverName(exlib::string& retVal)
{
    return m_hdlr->get_serverName(retVal);
}

result_t HttpsServer::set_serverName(exlib::string newVal)
{
    return m_hdlr->set_serverName(newVal);
}

result_t HttpsServer::get_verification(int32_t& retVal)
{
    return m_server->get_verification(retVal);
}
result_t HttpsServer::set_verification(int32_t newVal)
{
    return m_server->set_verification(newVal);
}
result_t HttpsServer::get_ca(obj_ptr<X509Cert_base>& retVal)
{
    return m_server->get_ca(retVal);
}

} /* namespace fibjs */
