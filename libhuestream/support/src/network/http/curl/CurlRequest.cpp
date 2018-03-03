/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <curl/curl.h>

#include <cstddef>
#include <string>
#include <future>

#include "support/network/http/curl/CurlRequest.h"
#include "support/network/http/HttpRequestConst.h"
#include "support/network/http/HttpRequestParams.h"
#include "support/network/http/util/X509Certificate.h"
#include "support/network/NetworkConfiguration.h"
#include "support/util/StringUtil.h"

namespace support {

    static const char* FILE_UPLOAD_FORM_FIELD = "file";
    static const char* FILE_UPLOAD_FILE_NAME  = "file.fw";

    static const char  HEADER_FIELD_NAME_VALUE_SEPERATOR = ':';
    static const char* HEADER_FIELD_EMPTY                = "\r";

    CurlRequest::CurlRequest(const HttpRequestParams& data, HttpRequestCallback callback)
        : _dispatcher(global_dispatch_queue()) {
        _curl = curl_easy_init();
        _header_list = nullptr;
        _form_post = nullptr;
        _resolve_list = nullptr;
        _callback = callback;
        _verify_ssl = data.verify_ssl;

        _callback_posted_future = _callback_posted.get_future();
        _is_complete_future = _is_complete.get_future();

        setup_options(data);
        setup_tls(data);
        setup_tls_common_name(data);
        setup_proxy(data);
        setup_request_headers(data);
        setup_post_body(data);
    }

    CURLM* CurlRequest::get_handle() const {
        return _curl;
    }

    void CurlRequest::send_response(CURLcode curl_code) {
        long http_code = 0; // NOLINT
        curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &http_code);

        _response.set_status_code(static_cast<unsigned int>(http_code));
        _response.set_body(_body_buffer);
        _response.set_certificate_chain(_certificate_chain);
        process_response_headers();

        _error.set_message(std::string("request finished with curl code ") + to_string(curl_code));
        _error.set_code(parse_error_code(curl_code));

        cleanup();

        _dispatcher.post([this]{
            _callback(_error, _response);
        });

        _callback_posted.set_value();
    }

    void CurlRequest::wait_for_completion() {
        _is_complete_future.wait();
        _callback_posted_future.wait();

        _dispatcher.shutdown();
    }

    void CurlRequest::set_as_complete() {
        _is_complete.set_value();
    }

    // appends a buffer to a string in non-quadratic time
    static size_t write_callback(char *ptr, size_t memb_size, size_t nmemb, void* userdata) {
        std::string* str = static_cast<string*>(userdata);
        size_t size = memb_size * nmemb;
        if (str->capacity() < str->size() + size) {
            // double the size every time it needs growing
            str->reserve((str->size() + size) * 2);
        }
        str->append(ptr, size);
        return size;
    }

#if defined(VERBOSE_HTTP_LOGGING) || defined(SDK_DEBUG)
    static int log_callback(CURL*, curl_infotype type, char *data, size_t size, void*)  {
        std::string text = "CURL";

        switch (type) {
            case CURLINFO_HEADER_OUT:
            case CURLINFO_DATA_OUT:
            case CURLINFO_SSL_DATA_OUT:
                text += " > ";
                break;
            case CURLINFO_HEADER_IN:
            case CURLINFO_DATA_IN:
            case CURLINFO_SSL_DATA_IN:
                text += " < ";
                break;
            default:
                text += ": ";
                break;
        }

        HUE_LOG << HUE_NETWORK << HUE_DEBUG << text << std::string(data, size) << HUE_ENDL;

        return 0;
    }
#endif

    void CurlRequest::setup_options(const HttpRequestParams& data) {
        curl_easy_setopt(_curl, CURLOPT_URL, data.url.c_str());
        curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &_header_buffer);
        curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_body_buffer);
        curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, write_callback);
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, data.connect_timeout);
        curl_easy_setopt(_curl, CURLOPT_TIMEOUT, data.request_timeout);
        curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
#if defined(VERBOSE_HTTP_LOGGING) || defined(SDK_DEBUG)
        curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(_curl, CURLOPT_DEBUGFUNCTION, log_callback);
#endif
        if (!NetworkConfiguration::get_reuse_connections()) {
            curl_easy_setopt(_curl, CURLOPT_FORBID_REUSE, 1L);
        }
    }

    int CurlRequest::mbedtls_x509parse_verify(void* data, mbedtls_x509_crt* cert, int /*path_cnt*/, uint32_t* flags) {
/*
        char buf[1024];

        printf("\nVerifying certificate at depth %d:\n", path_cnt);
        mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "  ", cert);
        printf("%s", buf);

        if (*flags == 0)
            printf("No verification issue for this certificate\n");
        else {
            mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", *flags);
            printf("%s\n", buf);
        }
*/
        auto request = static_cast<CurlRequest*>(data);
        request->_certificate_chain.push_back(X509Certificate::der_to_pem(cert->raw.p, cert->raw.len));

        if (!request->_verify_ssl) {
            *flags = 0;
        }

        return 0;
    }

    CURLcode CurlRequest::curl_sslctx_function(CURL* /*curl*/, void* sslctx, void* data) {
        auto request = static_cast<CurlRequest*>(data);
        auto config = static_cast<mbedtls_ssl_config*>(sslctx);

        mbedtls_ssl_conf_ca_chain(config, &request->_crt, &request->_crl);

        mbedtls_ssl_conf_authmode(config, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_verify(config, CurlRequest::mbedtls_x509parse_verify, data);

        return CURLE_OK;
    }

    void CurlRequest::setup_tls(const HttpRequestParams& data) {
        // setup the certificates

        mbedtls_x509_crt_init(&_crt);
        mbedtls_x509_crl_init(&_crl);  // empty crl to be passed to mbedtls

        if (!data.trusted_certs.empty()) {
            for (auto&& trusted_cert : data.trusted_certs) {
                // the length must include the terminating null char
                auto cert_str = reinterpret_cast<const unsigned char *>(trusted_cert.c_str());
                auto cert_len = trusted_cert.length() + 1;

                if (mbedtls_x509_crt_parse(&_crt, cert_str, cert_len) != 0) {
                    HUE_LOG << HUE_NETWORK << HUE_DEBUG << "error parsing cert" << trusted_cert.c_str()
                            << HUE_ENDL;
                }
            }
        }

        curl_easy_setopt(_curl, CURLOPT_SSL_CTX_FUNCTION, CurlRequest::curl_sslctx_function);
        curl_easy_setopt(_curl, CURLOPT_SSL_CTX_DATA, this);

        curl_easy_setopt(_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

        // Prevent the client from using insecure ciphers
        // Note that the cipher list format is dependent on the ssl backend used by curl (e.g. mbedtls)
        curl_easy_setopt(_curl, CURLOPT_SSL_CIPHER_LIST, TLS_CIPHER_WHITELIST_OPENSSL);

        if (!data.verify_ssl) {
            curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }
    }

    void CurlRequest::setup_tls_common_name(const HttpRequestParams &data) {
        // this allows us to verify the common name in server certificates
        // while connecting to ip addresses (e.g. https://192.168.1.2/)

        if (!data.common_name.empty()) {
            static std::regex url_regex(R"(^https://(\d+\.\d+.\d+.\d+)(:\d+)?(.*))");
            std::smatch m;

            if (std::regex_search(data.url, m, url_regex)) {
                if (!data.proxy_address.empty()) {
                    // disable url rewriting when using proxies
                    // the common name won't be correctly verified
                    return;
                }

                auto ip_address = m[1].str();
                auto port = m[2].str().empty() ? ":443" : m[2].str();
                auto rest = m[3].str();

                // replace the address part of the url with the common name
                auto new_url = std::string("https://") + data.common_name + port + rest;
                curl_easy_setopt(_curl, CURLOPT_URL, new_url.c_str());

                // instruct curl to resolve the common name back to the address
                auto resolve_str = data.common_name + port + ":" + ip_address;
                _resolve_list = curl_slist_append(nullptr, resolve_str.c_str());
                curl_easy_setopt(_curl, CURLOPT_RESOLVE, _resolve_list);
            }
        }
    }

    void CurlRequest::setup_proxy(const HttpRequestParams& data) {
        if (!data.proxy_address.empty()) {
            curl_easy_setopt(_curl, CURLOPT_PROXY, data.proxy_address.c_str());
            curl_easy_setopt(_curl, CURLOPT_PROXYPORT, data.proxy_port);
        }
    }

    void CurlRequest::setup_request_headers(const HttpRequestParams& data) {
        for (auto iter : data.headers) {
            std::string header_str = iter.first + ": " + iter.second;
            _header_list = curl_slist_append(_header_list, header_str.c_str());
        }
        if (_header_list != nullptr) {
            curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _header_list);
        }
    }

    void CurlRequest::setup_post_body(const HttpRequestParams& data) {
        if (data.file == nullptr) {
            curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, data.method.c_str());

            if (!data.body.empty()) {
                curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE,  data.body.size());
                curl_easy_setopt(_curl, CURLOPT_COPYPOSTFIELDS, data.body.c_str());
            }
        } else {
            struct curl_httppost* last_item = nullptr;

            curl_formadd(&_form_post,
                         &last_item,
                         CURLFORM_COPYNAME,     FILE_UPLOAD_FORM_FIELD,
                         CURLFORM_BUFFER,       FILE_UPLOAD_FILE_NAME,
                         CURLFORM_BUFFERPTR,    data.file->data.data(),
                         CURLFORM_BUFFERLENGTH, data.file->size,
                         CURLFORM_END);

            curl_easy_setopt(_curl, CURLOPT_HTTPPOST, _form_post);
        }
    }

    HttpRequestError::ErrorCode CurlRequest::parse_error_code(CURLcode curl_code) {
        switch (curl_code) {
            case CURLE_OK:
                /**
                 * On HTTPS requests curl does not automatically detect server reset, at least on the proxy.
                 * In order to detect server reset just check whether the status code is 0 and the body is empty
                 */
                if (_response.get_status_code() == 0 && _body_buffer.empty()) {
                    return HttpRequestError::HTTP_REQUEST_ERROR_CODE_SERVER_RESET;
                }
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS;
            case CURLE_ABORTED_BY_CALLBACK:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_CANCELED;
            case CURLE_COULDNT_RESOLVE_HOST:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_CANNOT_RESOLVE_HOST;
            case CURLE_COULDNT_CONNECT:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_CANNOT_CONNECT;
            case CURLE_OPERATION_TIMEDOUT:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_TIMEOUT;
            case CURLE_GOT_NOTHING:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_SERVER_RESET;
            case CURLE_PEER_FAILED_VERIFICATION:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_INVALID_CERTIFICATE;
            case CURLE_SSL_CACERT_BADFILE:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_INVALID_CERTIFICATE;
            case CURLE_SSL_CONNECT_ERROR:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_INVALID_CERTIFICATE;
            default:
                return HttpRequestError::HTTP_REQUEST_ERROR_CODE_UNDEFINED;
        }
    }

    void CurlRequest::process_response_headers() {
        std::istringstream response_stream(_header_buffer);
        std::string field;

        while (std::getline(response_stream, field) && field != HEADER_FIELD_EMPTY) {
            auto seperator_pos = field.find(HEADER_FIELD_NAME_VALUE_SEPERATOR, 0);
            if (seperator_pos != std::string::npos) {
                auto key = field.substr(0, seperator_pos);
                auto value = support::trim(field.substr(seperator_pos + 1));
                _response.add_header_field(key.c_str(), value.c_str());
            }
        }
    }

    void CurlRequest::cleanup() {
        curl_easy_cleanup(_curl);
        _curl = nullptr;

        if (_form_post != nullptr) {
            curl_formfree(_form_post);
        }

        if (_header_list != nullptr) {
            curl_slist_free_all(_header_list);
        }

        if (_resolve_list != nullptr) {
            curl_slist_free_all(_resolve_list);
        }

        mbedtls_x509_crl_free(&_crl);
        mbedtls_x509_crt_free(&_crt);
    }

}  // namespace support
