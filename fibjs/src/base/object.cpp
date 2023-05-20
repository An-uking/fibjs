/*
 * object.cpp
 *
 *  Created on: May 21, 2023
 *      Author: lion
 */

#include "object.h"

namespace fibjs {

v8::Local<v8::Object> object_base::wrap(v8::Local<v8::Object> o)
{
    Isolate* isolate = holder();
    v8::Isolate* v8_isolate = isolate->m_isolate;

    if (!(m_isJSObject & JSOBJECT_JSHANDLE)) {
        if (o.IsEmpty())
            o = Classinfo().CreateInstance(isolate);
        handle_.Reset(v8_isolate, o);

        if (!o->IsUint8Array())
            o->SetAlignedPointerInInternalField(0, this);

        v8_isolate->AdjustAmountOfExternalAllocatedMemory(m_nExtMemory);

        internalRef();
        handle_.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);

        m_isJSObject |= JSOBJECT_JSHANDLE;
    } else
        o = v8::Local<v8::Object>::New(v8_isolate, handle_);

    return o;
}

void* object_base::unwrap(v8::Local<v8::Value> o)
{
    if (o.IsEmpty() || !o->IsObject())
        return NULL;

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(o);

    if (obj->IsUint8Array())
        return fetch_store_data(obj.As<v8::Uint8Array>()->Buffer()->GetBackingStore(), StoreDeleter);

    if (obj->InternalFieldCount() != 1)
        return NULL;

    return obj->GetAlignedPointerFromInternalField(0);
}

}
