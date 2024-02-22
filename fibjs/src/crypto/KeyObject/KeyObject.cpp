/*
 * KeyObject.cpp
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#include "encoding.h"
#include "ifs/crypto.h"
#include "KeyObject.h"
#include "Buffer.h"

namespace fibjs {

result_t crypto_base::getCurves(v8::Local<v8::Array>& retVal)
{
    const size_t num_curves = EC_get_builtin_curves(nullptr, 0);
    std::vector<EC_builtin_curve> curves(num_curves);
    EC_get_builtin_curves(curves.data(), num_curves);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    std::sort(curves.begin(), curves.end(), [](const EC_builtin_curve& a, const EC_builtin_curve& b) {
        return a.nid < b.nid;
    });

    std::vector<const char*> curve_names(num_curves);
    for (size_t i = 0; i < num_curves; i++)
        curve_names[i] = OBJ_nid2sn(curves[i].nid);

    std::sort(curve_names.begin(), curve_names.end(), [](const char* a, const char* b) {
        return strcmp(a, b) < 0;
    });

    retVal = v8::Array::New(isolate->m_isolate, num_curves);
    for (size_t i = 0; i < num_curves; i++)
        retVal->Set(context, i, isolate->NewString(curve_names[i]));

    return 0;
}

static result_t _createSecretKey(const unsigned char* key, size_t size, obj_ptr<KeyObject_base>& retVal)
{
    obj_ptr<KeyObject> keyObj = new KeyObject();
    result_t hr = keyObj->createSecretKey(key, size);
    if (hr < 0)
        return hr;

    retVal = keyObj;
    return 0;
}

result_t crypto_base::createSecretKey(Buffer_base* key, exlib::string encoding, obj_ptr<KeyObject_base>& retVal)
{
    Buffer* key_buf = Buffer::Cast(key);
    return _createSecretKey((const unsigned char*)key_buf->data(), key_buf->length(), retVal);
}

result_t crypto_base::createSecretKey(exlib::string key, exlib::string encoding, obj_ptr<KeyObject_base>& retVal)
{
    result_t hr = commonDecode(encoding, key, key);
    if (hr < 0)
        return hr;
    return _createSecretKey((const unsigned char*)key.c_str(), key.length(), retVal);
}

result_t KeyObject::createSecretKey(const unsigned char* key, size_t size)
{
    m_keyType = kKeyTypeSecret;
    m_key.assign(key, key + size);

    return 0;
}

result_t KeyObject::get_asymmetricKeyType(exlib::string& retVal)
{
    if (m_keyType == kKeyTypeSecret)
        return CALL_RETURN_UNDEFINED;

    switch (EVP_PKEY_id(m_pkey)) {
    case EVP_PKEY_RSA:
        retVal = "rsa";
        break;
    case EVP_PKEY_RSA_PSS:
        retVal = "rsa-pss";
        break;
    case EVP_PKEY_DSA:
        retVal = "dsa";
        break;
    case EVP_PKEY_DH:
        retVal = "dh";
        break;
    case EVP_PKEY_EC:
        retVal = "ec";
        break;
    case EVP_PKEY_ED25519:
        retVal = "ed25519";
        break;
    case EVP_PKEY_ED448:
        retVal = "ed448";
        break;
    case EVP_PKEY_X25519:
        retVal = "x25519";
        break;
    case EVP_PKEY_X448:
        retVal = "x448";
        break;
    default:
        return CALL_RETURN_UNDEFINED;
    }

    return 0;
}

result_t KeyObject::get_symmetricKeySize(int32_t& retVal)
{
    if (m_keyType != kKeyTypeSecret)
        return CALL_RETURN_UNDEFINED;

    retVal = m_key.size();
    return 0;
}

result_t KeyObject::get_type(exlib::string& retVal)
{
    switch (m_keyType) {
    case kKeyTypeSecret:
        retVal = "secret";
        break;
    case kKeyTypePublic:
        retVal = "public";
        break;
    case kKeyTypePrivate:
        retVal = "private";
        break;
    }

    return 0;
}

result_t KeyObject::_export(v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string format = "buffer";
    hr = GetConfigValue(isolate, options, "format", format, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (format == "jwk") {
        if (options->HasOwnProperty(context, isolate->NewString("passphrase")).FromMaybe(false))
            return Runtime::setError("The property 'options.passphrase' is not supported for 'jwk' format.");
        return export_json(retVal);
    }

    switch (m_keyType) {
    case kKeyTypeSecret:
        if (format == "buffer") {
            obj_ptr<Buffer_base> buf = new Buffer(m_key.data(), m_key.size());
            retVal = buf->wrap(holder());
            return 0;
        }

        return CHECK_ERROR(Runtime::setError("The property 'options.format' must be one of: undefined, 'buffer', 'jwk'."));
    case kKeyTypePublic:
        return ExportPublicKey(options, retVal);
    case kKeyTypePrivate:
        return ExportPrivateKey(options, retVal);
    }

    return Runtime::setError("Invalid key type");
}

result_t KeyObject::createAsymmetricKey(v8::Local<v8::Object> key, KeyType type)
{
    result_t hr;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string format = "pem";
    hr = GetConfigValue(isolate, key, "format", format, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (format == "jwk")
        return ImportJWKAsymmetricKey(key, type);

    if (type == kKeyTypePublic)
        return ParsePublicKey(key);

    if (type == kKeyTypePrivate)
        return ParsePrivateKey(key);

    return CALL_E_INVALID_CALL;
}

result_t KeyObject::equals(KeyObject_base* otherKey, bool& retVal)
{
    KeyObject* other = (KeyObject*)otherKey;

    if (m_keyType != other->m_keyType)
        retVal = false;
    else if (m_key.size() != other->m_key.size())
        retVal = false;
    else
        retVal = memcmp(m_key.data(), other->m_key.data(), m_key.size()) == 0;

    return 0;
}

}