/*
 * PKey.h
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#pragma once

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "ifs/PKey.h"
#include <mbedtls/mbedtls/mbedtls_config.h>
#include <mbedtls/mbedtls/pk.h>

namespace fibjs {

class PKey : public PKey_base {
public:
    PKey();
    ~PKey();

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    // PKey_base
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_curve(exlib::string& retVal);
    virtual result_t get_keySize(int32_t& retVal);
    virtual result_t get_sigType(exlib::string& retVal);
    virtual result_t set_sigType(exlib::string newVal);
    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal);
    virtual result_t isPrivate(bool& retVal);
    virtual result_t clone(obj_ptr<PKey_base>& retVal);
    virtual result_t import(Buffer_base* DerKey, exlib::string password);
    virtual result_t import(exlib::string pemKey, exlib::string password);
    virtual result_t import(v8::Local<v8::Object> jsonKey);
    virtual result_t importFile(exlib::string filename, exlib::string password);
    virtual result_t pem(exlib::string& retVal);
    virtual result_t der(obj_ptr<Buffer_base>& retVal);
    virtual result_t json(v8::Local<v8::Object>& retVal);
    virtual result_t equal(PKey_base* key, bool& retVal);
    virtual result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t sign(Buffer_base* data, int32_t alg, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t sign(Buffer_base* data, PKey_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, int32_t alg, bool& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, PKey_base* key, bool& retVal, AsyncEvent* ac);
    virtual result_t computeSecret(PKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

public:
    result_t copy(const mbedtls_pk_context& key);
    result_t generateKey(int32_t size, AsyncEvent* ac);
    result_t generateKey(exlib::string curve, AsyncEvent* ac);

private:
    void clear();

public:
    mbedtls_pk_context m_key;
    bool m_sdsa = false;
};
}
