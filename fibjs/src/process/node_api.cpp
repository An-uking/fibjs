/*
 * node_api.cpp
 *
 *  Created on: Oct 17, 2023
 *      Author: lion
 */

#include "addons/js_native_api_internal.h"

namespace v8impl {
namespace {
    class BufferFinalizer : private Finalizer {
    public:
        static BufferFinalizer* New(napi_env env,
            napi_finalize finalize_callback = nullptr,
            void* finalize_data = nullptr,
            void* finalize_hint = nullptr)
        {
            return new BufferFinalizer(
                env, finalize_callback, finalize_data, finalize_hint);
        }
        // node::Buffer::FreeCallback
        static void FinalizeBufferCallback(char* data, void* hint)
        {
            std::unique_ptr<BufferFinalizer, Deleter> finalizer {
                static_cast<BufferFinalizer*>(hint)
            };
            finalizer->finalize_data_ = data;

            // It is safe to call into JavaScript at this point.
            if (finalizer->finalize_callback_ == nullptr)
                return;
            finalizer->env_->CallFinalizer(finalizer->finalize_callback_,
                finalizer->finalize_data_,
                finalizer->finalize_hint_);
        }

        struct Deleter {
            void operator()(BufferFinalizer* finalizer) { delete finalizer; }
        };

    private:
        BufferFinalizer(napi_env env,
            napi_finalize finalize_callback,
            void* finalize_data,
            void* finalize_hint)
            : Finalizer(env, finalize_callback, finalize_data, finalize_hint)
        {
            env_->Ref();
        }

        ~BufferFinalizer() { env_->Unref(); }
    };

}
}

namespace {
namespace uvimpl {

    static napi_status ConvertUVErrorCode(int code)
    {
        switch (code) {
        case 0:
            return napi_ok;
        case UV_EINVAL:
            return napi_invalid_arg;
        case UV_ECANCELED:
            return napi_cancelled;
        default:
            return napi_generic_failure;
        }
    }

    // Wrapper around uv_work_t which calls user-provided callbacks.
    class Work : public node::AsyncResource, public node::ThreadPoolWork {
    private:
        explicit Work(node_napi_env env,
            v8::Local<v8::Object> async_resource,
            v8::Local<v8::String> async_resource_name,
            napi_async_execute_callback execute,
            napi_async_complete_callback complete = nullptr,
            void* data = nullptr)
            : AsyncResource(
                env->isolate,
                async_resource,
                *v8::String::Utf8Value(env->isolate, async_resource_name))
            , ThreadPoolWork(env->node_env(), "node_api")
            , _env(env)
            , _data(data)
            , _execute(execute)
            , _complete(complete)
        {
        }

        ~Work() override = default;

    public:
        static Work* New(node_napi_env env,
            v8::Local<v8::Object> async_resource,
            v8::Local<v8::String> async_resource_name,
            napi_async_execute_callback execute,
            napi_async_complete_callback complete,
            void* data)
        {
            return new Work(
                env, async_resource, async_resource_name, execute, complete, data);
        }

        static void Delete(Work* work) { delete work; }

        void DoThreadPoolWork() override { _execute(_env, _data); }

        void AfterThreadPoolWork(int status) override
        {
            if (_complete == nullptr)
                return;

            // Establish a handle scope here so that every callback doesn't have to.
            // Also it is needed for the exception-handling below.
            v8::HandleScope scope(_env->isolate);

            CallbackScope callback_scope(this);

            _env->CallbackIntoModule<true>([&](napi_env env) {
                _complete(env, ConvertUVErrorCode(status), _data);
            });

            // Note: Don't access `work` after this point because it was
            // likely deleted by the complete callback.
        }

    private:
        node_napi_env _env;
        void* _data;
        napi_async_execute_callback _execute;
        napi_async_complete_callback _complete;
    };

}

}

napi_status NAPI_CDECL napi_create_buffer(napi_env env,
    size_t length,
    void** data,
    napi_value* result)
{
    NAPI_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::MaybeLocal<v8::Object> maybe = node::Buffer::New(env->isolate, length);

    CHECK_MAYBE_EMPTY(env, maybe, napi_generic_failure);

    v8::Local<v8::Object> buffer = maybe.ToLocalChecked();

    *result = v8impl::JsValueFromV8LocalValue(buffer);

    if (data != nullptr) {
        *data = node::Buffer::Data(buffer);
    }

    return GET_RETURN_STATUS(env);
}

napi_status NAPI_CDECL napi_create_external_buffer(napi_env env,
    size_t length,
    void* data,
    napi_finalize finalize_cb,
    void* finalize_hint,
    napi_value* result)
{
    NAPI_PREAMBLE(env);
    CHECK_ARG(env, result);

#if defined(V8_ENABLE_SANDBOX)
    return napi_set_last_error(env, napi_no_external_buffers_allowed);
#endif

    v8::Isolate* isolate = env->isolate;

    // The finalizer object will delete itself after invoking the callback.
    v8impl::BufferFinalizer* finalizer = v8impl::BufferFinalizer::New(env, finalize_cb, nullptr, finalize_hint);

    v8::MaybeLocal<v8::Object> maybe = node::Buffer::New(isolate, static_cast<char*>(data),
        length, v8impl::BufferFinalizer::FinalizeBufferCallback, finalizer);

    CHECK_MAYBE_EMPTY(env, maybe, napi_generic_failure);

    *result = v8impl::JsValueFromV8LocalValue(maybe.ToLocalChecked());
    return GET_RETURN_STATUS(env);
    // Tell coverity that 'finalizer' should not be freed when we return
    // as it will be deleted when the buffer to which it is associated
    // is finalized.
    // coverity[leaked_storage]
}

napi_status NAPI_CDECL napi_create_buffer_copy(napi_env env,
    size_t length,
    const void* data,
    void** result_data,
    napi_value* result)
{
    NAPI_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::MaybeLocal<v8::Object> maybe = node::Buffer::Copy(env->isolate, static_cast<const char*>(data), length);

    CHECK_MAYBE_EMPTY(env, maybe, napi_generic_failure);

    v8::Local<v8::Object> buffer = maybe.ToLocalChecked();
    *result = v8impl::JsValueFromV8LocalValue(buffer);

    if (result_data != nullptr) {
        *result_data = node::Buffer::Data(buffer);
    }

    return GET_RETURN_STATUS(env);
}

napi_status NAPI_CDECL napi_is_buffer(napi_env env,
    napi_value value,
    bool* result)
{
    CHECK_ENV(env);
    env->CheckGCAccess();
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    *result = node::Buffer::HasInstance(v8impl::V8LocalValueFromJsValue(value));
    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_get_buffer_info(napi_env env,
    napi_value value,
    void** data,
    size_t* length)
{
    CHECK_ENV(env);
    env->CheckGCAccess();
    CHECK_ARG(env, value);

    v8::Local<v8::Value> buffer = v8impl::V8LocalValueFromJsValue(value);

    if (data != nullptr) {
        *data = node::Buffer::Data(buffer);
    }
    if (length != nullptr) {
        *length = node::Buffer::Length(buffer);
    }

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL
napi_create_async_work(napi_env env,
    napi_value async_resource,
    napi_value async_resource_name,
    napi_async_execute_callback execute,
    napi_async_complete_callback complete,
    void* data,
    napi_async_work* result)
{
    CHECK_ENV(env);
    env->CheckGCAccess();
    CHECK_ARG(env, execute);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Object> resource;
    if (async_resource != nullptr) {
        CHECK_TO_OBJECT(env, context, resource, async_resource);
    } else {
        resource = v8::Object::New(env->isolate);
    }

    v8::Local<v8::String> resource_name;
    CHECK_TO_STRING(env, context, resource_name, async_resource_name);

    uvimpl::Work* work = uvimpl::Work::New(reinterpret_cast<node_napi_env>(env),
        resource,
        resource_name,
        execute,
        complete,
        data);

    *result = reinterpret_cast<napi_async_work>(work);

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_delete_async_work(napi_env env,
    napi_async_work work)
{
    CHECK_ENV(env);
    env->CheckGCAccess();
    CHECK_ARG(env, work);

    uvimpl::Work::Delete(reinterpret_cast<uvimpl::Work*>(work));

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_queue_async_work(napi_env env,
    napi_async_work work)
{
    CHECK_ENV(env);
    CHECK_ARG(env, work);

    uvimpl::Work* w = reinterpret_cast<uvimpl::Work*>(work);

    w->ScheduleWork();

    return napi_clear_last_error(env);
}

#define CALL_UV(env, condition)                                  \
    do {                                                         \
        int result = (condition);                                \
        napi_status status = uvimpl::ConvertUVErrorCode(result); \
        if (status != napi_ok) {                                 \
            return napi_set_last_error(env, status, result);     \
        }                                                        \
    } while (0)

napi_status NAPI_CDECL napi_cancel_async_work(napi_env env,
    napi_async_work work)
{
    CHECK_ENV(env);
    CHECK_ARG(env, work);

    uvimpl::Work* w = reinterpret_cast<uvimpl::Work*>(work);

    CALL_UV(env, w->CancelWork());

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_get_uv_event_loop(napi_env env, uv_loop_t** loop) {
  CHECK_ENV(env);
  CHECK_ARG(env, loop);
  *loop = reinterpret_cast<node_napi_env>(env)->node_env()->event_loop();
  return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_get_node_version(napi_env env,
    const napi_node_version** result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);
    static const napi_node_version version = {
        NODE_MAJOR_VERSION, NODE_MINOR_VERSION, NODE_PATCH_VERSION, NODE_RELEASE
    };
    *result = &version;
    return napi_clear_last_error(env);
}

NAPI_NO_RETURN void NAPI_CDECL napi_fatal_error(const char* location,
    size_t location_len,
    const char* message,
    size_t message_len)
{
    std::string location_string;
    std::string message_string;

    if (location_len != NAPI_AUTO_LENGTH) {
        location_string.assign(const_cast<char*>(location), location_len);
    } else {
        location_string.assign(const_cast<char*>(location), strlen(location));
    }

    if (message_len != NAPI_AUTO_LENGTH) {
        message_string.assign(const_cast<char*>(message), message_len);
    } else {
        message_string.assign(const_cast<char*>(message), strlen(message));
    }

    node::OnFatalError(location_string.c_str(), message_string.c_str());
}

static void napi_module_register_cb(v8::Local<v8::Object> exports,
    v8::Local<v8::Value> module,
    v8::Local<v8::Context> context,
    void* priv)
{
    napi_module_register_by_symbol(
        exports,
        module,
        context,
        static_cast<const napi_module*>(priv)->nm_register_func);
}

void napi_module_register_by_symbol(v8::Local<v8::Object> exports, v8::Local<v8::Value> module,
    v8::Local<v8::Context> context, napi_addon_register_func init, int32_t module_api_version)
{
    node::Environment* node_env = node::Environment::GetCurrent(context);
    std::string module_filename = "";
    if (init == nullptr) {
        CHECK_NOT_NULL(node_env);
        node_env->ThrowError("Module has no declared entry point.");
        return;
    }

    // Create a new napi_env for this specific module.
    napi_env env = new napi_env__(context, module_api_version);

    napi_value _exports = nullptr;
    env->CallIntoModule([&](napi_env env) {
        _exports = init(env, v8impl::JsValueFromV8LocalValue(exports));
    });

    // If register function returned a non-null exports object different from
    // the exports object we passed it, set that as the "exports" property of
    // the module.
    if (_exports != nullptr && _exports != v8impl::JsValueFromV8LocalValue(exports)) {
        napi_value _module = v8impl::JsValueFromV8LocalValue(module);
        napi_set_named_property(env, _module, "exports", _exports);
    }
}

namespace node {
node_module napi_module_to_node_module(const napi_module* mod)
{
    return {
        -1,
        mod->nm_flags,
        nullptr,
        mod->nm_filename,
        nullptr,
        napi_module_register_cb,
        mod->nm_modname,
        const_cast<napi_module*>(mod), // priv
        nullptr,
    };
}
} // namespace node

// Registers a NAPI module.
void NAPI_CDECL napi_module_register(napi_module* mod)
{
    node::node_module* nm = new node::node_module(node::napi_module_to_node_module(mod));
    node::node_module_register(nm);
}
