/*
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 *
 * Copyright (C) 2003 Cordys R&D BV, All rights reserved.
 * Copyright (C) 2002-2022 Aleksey Sanin <aleksey@aleksey.com>. All Rights Reserved.
 * Copyright (c) 2005-2006 Cryptocom LTD (http://www.cryptocom.ru).
 */
/**
 * SECTION:crypto
 * @Short_description: Crypto transforms implementation for Microsoft Crypto API.
 * @Stability: Stable
 *
 */

#include "globals.h"

#include <string.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/keys.h>
#include <xmlsec/transforms.h>
#include <xmlsec/errors.h>
#include <xmlsec/dl.h>
#include <xmlsec/private.h>
#include <xmlsec/xmltree.h>

#include <xmlsec/mscrypto/app.h>
#include <xmlsec/mscrypto/crypto.h>
#include <xmlsec/mscrypto/x509.h>

#include "private.h"
#include "../cast_helpers.h"

#define XMLSEC_CONTAINER_NAME_A "xmlsec-key-container"
#define XMLSEC_CONTAINER_NAME_W L"xmlsec-key-container"
#ifdef UNICODE
#define XMLSEC_CONTAINER_NAME XMLSEC_CONTAINER_NAME_W
#else /* UNICODE */
#define XMLSEC_CONTAINER_NAME XMLSEC_CONTAINER_NAME_A
#endif /* UNICODE */


static xmlSecCryptoDLFunctionsPtr gXmlSecMSCryptoFunctions = NULL;

/**
 * xmlSecCryptoGetFunctions_mscrypto:
 *
 * Gets MSCrypto specific functions table.
 *
 * Returns: xmlsec-mscrypto functions table.
 */
xmlSecCryptoDLFunctionsPtr
xmlSecCryptoGetFunctions_mscrypto(void) {
    static xmlSecCryptoDLFunctions functions;

    if(gXmlSecMSCryptoFunctions != NULL) {
        return(gXmlSecMSCryptoFunctions);
    }

    memset(&functions, 0, sizeof(functions));
    gXmlSecMSCryptoFunctions = &functions;

    /********************************************************************
     *
     * Crypto Init/shutdown
     *
     ********************************************************************/
    gXmlSecMSCryptoFunctions->cryptoInit                        = xmlSecMSCryptoInit;
    gXmlSecMSCryptoFunctions->cryptoShutdown                    = xmlSecMSCryptoShutdown;
    gXmlSecMSCryptoFunctions->cryptoKeysMngrInit                = xmlSecMSCryptoKeysMngrInit;

    /********************************************************************
     *
     * Key data ids
     *
     ********************************************************************/
#ifndef XMLSEC_NO_DES
    gXmlSecMSCryptoFunctions->keyDataDesGetKlass                = xmlSecMSCryptoKeyDataDesGetKlass;
#endif /* XMLSEC_NO_DES */

#ifndef XMLSEC_NO_AES
    gXmlSecMSCryptoFunctions->keyDataAesGetKlass                = xmlSecMSCryptoKeyDataAesGetKlass;
#endif /* XMLSEC_NO_AES */

#ifndef XMLSEC_NO_RSA
    gXmlSecMSCryptoFunctions->keyDataRsaGetKlass                = xmlSecMSCryptoKeyDataRsaGetKlass;
#endif /* XMLSEC_NO_RSA */

#ifndef XMLSEC_NO_HMAC
    gXmlSecMSCryptoFunctions->keyDataHmacGetKlass               = xmlSecMSCryptoKeyDataHmacGetKlass;
#endif /* XMLSEC_NO_HMAC */

#ifndef XMLSEC_NO_DSA
    gXmlSecMSCryptoFunctions->keyDataDsaGetKlass                = xmlSecMSCryptoKeyDataDsaGetKlass;
#endif /* XMLSEC_NO_DSA */

#ifndef XMLSEC_NO_GOST
    gXmlSecMSCryptoFunctions->keyDataGost2001GetKlass           = xmlSecMSCryptoKeyDataGost2001GetKlass;
#endif /* XMLSEC_NO_GOST*/

#ifndef XMLSEC_NO_GOST2012
    gXmlSecMSCryptoFunctions->keyDataGostR3410_2012_256GetKlass = xmlSecMSCryptoKeyDataGost2012_256GetKlass;
    gXmlSecMSCryptoFunctions->keyDataGostR3410_2012_512GetKlass = xmlSecMSCryptoKeyDataGost2012_512GetKlass;
#endif /* XMLSEC_NO_GOST2012*/

#ifndef XMLSEC_NO_X509
    gXmlSecMSCryptoFunctions->keyDataX509GetKlass               = xmlSecMSCryptoKeyDataX509GetKlass;
    gXmlSecMSCryptoFunctions->keyDataRawX509CertGetKlass        = xmlSecMSCryptoKeyDataRawX509CertGetKlass;
#endif /* XMLSEC_NO_X509 */

    /********************************************************************
     *
     * Key data store ids
     *
     ********************************************************************/
#ifndef XMLSEC_NO_X509
    gXmlSecMSCryptoFunctions->x509StoreGetKlass                 = xmlSecMSCryptoX509StoreGetKlass;
#endif /* XMLSEC_NO_X509 */

    /********************************************************************
     *
     * Crypto transforms ids
     *
     ********************************************************************/

    /******************************* AES ********************************/
#ifndef XMLSEC_NO_AES
    gXmlSecMSCryptoFunctions->transformAes128CbcGetKlass        = xmlSecMSCryptoTransformAes128CbcGetKlass;
    gXmlSecMSCryptoFunctions->transformAes192CbcGetKlass        = xmlSecMSCryptoTransformAes192CbcGetKlass;
    gXmlSecMSCryptoFunctions->transformAes256CbcGetKlass        = xmlSecMSCryptoTransformAes256CbcGetKlass;
    gXmlSecMSCryptoFunctions->transformKWAes128GetKlass         = xmlSecMSCryptoTransformKWAes128GetKlass;
    gXmlSecMSCryptoFunctions->transformKWAes192GetKlass         = xmlSecMSCryptoTransformKWAes192GetKlass;
    gXmlSecMSCryptoFunctions->transformKWAes256GetKlass         = xmlSecMSCryptoTransformKWAes256GetKlass;
#endif /* XMLSEC_NO_AES */

    /******************************* DES ********************************/
#ifndef XMLSEC_NO_DES
    gXmlSecMSCryptoFunctions->transformDes3CbcGetKlass          = xmlSecMSCryptoTransformDes3CbcGetKlass;
    gXmlSecMSCryptoFunctions->transformKWDes3GetKlass           = xmlSecMSCryptoTransformKWDes3GetKlass;
#endif /* XMLSEC_NO_DES */

    /******************************* DSA ********************************/
#ifndef XMLSEC_NO_DSA
    gXmlSecMSCryptoFunctions->transformDsaSha1GetKlass          = xmlSecMSCryptoTransformDsaSha1GetKlass;
#endif /* XMLSEC_NO_DSA */

    /******************************* GOST ********************************/
#ifndef XMLSEC_NO_GOST
    gXmlSecMSCryptoFunctions->transformGost2001GostR3411_94GetKlass             = xmlSecMSCryptoTransformGost2001GostR3411_94GetKlass;
#endif /* XMLSEC_NO_GOST */

#ifndef XMLSEC_NO_GOST2012
    gXmlSecMSCryptoFunctions->transformGostR3411_2012_256GetKlass               = xmlSecMSCryptoTransformGostR3411_2012_256GetKlass;
    gXmlSecMSCryptoFunctions->transformGostR3410_2012GostR3411_2012_256GetKlass = xmlSecMSCryptoTransformGost2012_256GetKlass;

    gXmlSecMSCryptoFunctions->transformGostR3411_2012_512GetKlass               = xmlSecMSCryptoTransformGostR3411_2012_512GetKlass;
    gXmlSecMSCryptoFunctions->transformGostR3410_2012GostR3411_2012_512GetKlass = xmlSecMSCryptoTransformGost2012_512GetKlass;
#endif /* XMLSEC_NO_GOST2012 */

#ifndef XMLSEC_NO_GOST
    gXmlSecMSCryptoFunctions->transformGostR3411_94GetKlass             = xmlSecMSCryptoTransformGostR3411_94GetKlass;
#endif /* XMLSEC_NO_GOST */

    /******************************* HMAC ********************************/
#ifndef XMLSEC_NO_HMAC

#ifndef XMLSEC_NO_MD5
    gXmlSecMSCryptoFunctions->transformHmacMd5GetKlass         = xmlSecMSCryptoTransformHmacMd5GetKlass;
#endif /* XMLSEC_NO_MD5 */

#ifndef XMLSEC_NO_SHA1
    gXmlSecMSCryptoFunctions->transformHmacSha1GetKlass        = xmlSecMSCryptoTransformHmacSha1GetKlass;
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA256
    gXmlSecMSCryptoFunctions->transformHmacSha256GetKlass      = xmlSecMSCryptoTransformHmacSha256GetKlass;
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
    gXmlSecMSCryptoFunctions->transformHmacSha384GetKlass      = xmlSecMSCryptoTransformHmacSha384GetKlass;
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
    gXmlSecMSCryptoFunctions->transformHmacSha512GetKlass      = xmlSecMSCryptoTransformHmacSha512GetKlass;
#endif /* XMLSEC_NO_SHA512 */

#endif /* XMLSEC_NO_HMAC */

    /******************************* MD5 ********************************/
#ifndef XMLSEC_NO_MD5
    gXmlSecMSCryptoFunctions->transformMd5GetKlass             = xmlSecMSCryptoTransformMd5GetKlass;
#endif /* XMLSEC_NO_MD5 */

    /******************************* RSA ********************************/
#ifndef XMLSEC_NO_RSA

#ifndef XMLSEC_NO_MD5
    gXmlSecMSCryptoFunctions->transformRsaMd5GetKlass          = xmlSecMSCryptoTransformRsaMd5GetKlass;
#endif /* XMLSEC_NO_MD5 */

#ifndef XMLSEC_NO_SHA1
    gXmlSecMSCryptoFunctions->transformRsaSha1GetKlass          = xmlSecMSCryptoTransformRsaSha1GetKlass;
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA256
    gXmlSecMSCryptoFunctions->transformRsaSha256GetKlass       = xmlSecMSCryptoTransformRsaSha256GetKlass;
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
    gXmlSecMSCryptoFunctions->transformRsaSha384GetKlass       = xmlSecMSCryptoTransformRsaSha384GetKlass;
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
    gXmlSecMSCryptoFunctions->transformRsaSha512GetKlass       = xmlSecMSCryptoTransformRsaSha512GetKlass;
#endif /* XMLSEC_NO_SHA512 */

    gXmlSecMSCryptoFunctions->transformRsaPkcs1GetKlass         = xmlSecMSCryptoTransformRsaPkcs1GetKlass;

#ifndef XMLSEC_NO_SHA1
    gXmlSecMSCryptoFunctions->transformRsaOaepGetKlass          = xmlSecMSCryptoTransformRsaOaepGetKlass;
#endif /* XMLSEC_NO_SHA1 */

#endif /* XMLSEC_NO_RSA */

    /******************************* SHA ********************************/
#ifndef XMLSEC_NO_SHA1
    gXmlSecMSCryptoFunctions->transformSha1GetKlass             = xmlSecMSCryptoTransformSha1GetKlass;
#endif /* XMLSEC_NO_SHA1 */
#ifndef XMLSEC_NO_SHA256
    gXmlSecMSCryptoFunctions->transformSha256GetKlass          = xmlSecMSCryptoTransformSha256GetKlass;
#endif /* XMLSEC_NO_SHA256 */
#ifndef XMLSEC_NO_SHA384
    gXmlSecMSCryptoFunctions->transformSha384GetKlass          = xmlSecMSCryptoTransformSha384GetKlass;
#endif /* XMLSEC_NO_SHA384 */
#ifndef XMLSEC_NO_SHA512
    gXmlSecMSCryptoFunctions->transformSha512GetKlass          = xmlSecMSCryptoTransformSha512GetKlass;
#endif /* XMLSEC_NO_SHA512 */

    /********************************************************************
     *
     * High level routines form xmlsec command line utility
     *
     ********************************************************************/
    gXmlSecMSCryptoFunctions->cryptoAppInit                     = xmlSecMSCryptoAppInit;
    gXmlSecMSCryptoFunctions->cryptoAppShutdown                 = xmlSecMSCryptoAppShutdown;
    gXmlSecMSCryptoFunctions->cryptoAppDefaultKeysMngrInit      = xmlSecMSCryptoAppDefaultKeysMngrInit;
    gXmlSecMSCryptoFunctions->cryptoAppDefaultKeysMngrAdoptKey  = xmlSecMSCryptoAppDefaultKeysMngrAdoptKey;
    gXmlSecMSCryptoFunctions->cryptoAppDefaultKeysMngrLoad      = xmlSecMSCryptoAppDefaultKeysMngrLoad;
    gXmlSecMSCryptoFunctions->cryptoAppDefaultKeysMngrSave      = xmlSecMSCryptoAppDefaultKeysMngrSave;
#ifndef XMLSEC_NO_X509
    gXmlSecMSCryptoFunctions->cryptoAppKeysMngrCertLoad         = xmlSecMSCryptoAppKeysMngrCertLoad;
    gXmlSecMSCryptoFunctions->cryptoAppKeysMngrCertLoadMemory   = xmlSecMSCryptoAppKeysMngrCertLoadMemory;
    gXmlSecMSCryptoFunctions->cryptoAppPkcs12Load               = xmlSecMSCryptoAppPkcs12Load;
    gXmlSecMSCryptoFunctions->cryptoAppPkcs12LoadMemory         = xmlSecMSCryptoAppPkcs12LoadMemory;
    gXmlSecMSCryptoFunctions->cryptoAppKeyCertLoad              = xmlSecMSCryptoAppKeyCertLoad;
    gXmlSecMSCryptoFunctions->cryptoAppKeyCertLoadMemory        = xmlSecMSCryptoAppKeyCertLoadMemory;
#endif /* XMLSEC_NO_X509 */
    gXmlSecMSCryptoFunctions->cryptoAppKeyLoad                  = xmlSecMSCryptoAppKeyLoad;
    gXmlSecMSCryptoFunctions->cryptoAppKeyLoadMemory            = xmlSecMSCryptoAppKeyLoadMemory;
    gXmlSecMSCryptoFunctions->cryptoAppDefaultPwdCallback       = (void*)xmlSecMSCryptoAppGetDefaultPwdCallback();

    return(gXmlSecMSCryptoFunctions);
}

/**
 * xmlSecMSCryptoInit:
 *
 * XMLSec library specific crypto engine initialization.
 *
 * Returns: 0 on success or a negative value otherwise.
 */
int
xmlSecMSCryptoInit (void)  {
    /* Check loaded xmlsec library version */
    if(xmlSecCheckVersionExact() != 1) {
        xmlSecInternalError("xmlSecCheckVersionExact", NULL);
        return(-1);
    }

    /* set default errors callback for xmlsec to us */
    xmlSecErrorsSetCallback(xmlSecMSCryptoErrorsDefaultCallback);

    /* register our klasses */
    if(xmlSecCryptoDLFunctionsRegisterKeyDataAndTransforms(xmlSecCryptoGetFunctions_mscrypto()) < 0) {
        xmlSecInternalError("xmlSecCryptoDLFunctionsRegisterKeyDataAndTransforms", NULL);
        return(-1);
    }
    return(0);
}

/**
 * xmlSecMSCryptoShutdown:
 *
 * XMLSec library specific crypto engine shutdown.
 *
 * Returns: 0 on success or a negative value otherwise.
 */
int
xmlSecMSCryptoShutdown(void) {
    /* TODO: if necessary, do additional shutdown here */
    return(0);
}

/**
 * xmlSecMSCryptoKeysMngrInit:
 * @mngr:               the pointer to keys manager.
 *
 * Adds MSCrypto specific key data stores in keys manager.
 *
 * Returns: 0 on success or a negative value otherwise.
 */
int
xmlSecMSCryptoKeysMngrInit(xmlSecKeysMngrPtr mngr) {
    int ret;

    xmlSecAssert2(mngr != NULL, -1);

#ifndef XMLSEC_NO_X509
    /* create x509 store if needed */
    if(xmlSecKeysMngrGetDataStore(mngr, xmlSecMSCryptoX509StoreId) == NULL) {
        xmlSecKeyDataStorePtr x509Store;

        x509Store = xmlSecKeyDataStoreCreate(xmlSecMSCryptoX509StoreId);
        if(x509Store == NULL) {
            xmlSecInternalError("xmlSecKeyDataStoreCreate(xmlSecMSCryptoX509StoreId)", NULL);
            return(-1);
        }

        ret = xmlSecKeysMngrAdoptDataStore(mngr, x509Store);
        if(ret < 0) {
            xmlSecInternalError("xmlSecKeysMngrAdoptDataStore", NULL);
            xmlSecKeyDataStoreDestroy(x509Store);
            return(-1);
        }
    }
#endif /* XMLSEC_NO_X509 */

    return(0);
}

static xmlSecMSCryptoProviderInfo xmlSecMSCryptoProviderInfo_Random[] = {
    { MS_STRONG_PROV,               PROV_RSA_FULL },
    { MS_ENHANCED_PROV,             PROV_RSA_FULL },
    { NULL, 0 }
};

/**
 * xmlSecMSCryptoGenerateRandom:
 * @buffer:             the destination buffer.
 * @size:               the numer of bytes to generate.
 *
 * Generates @size random bytes and puts result in @buffer
 * (not implemented yet).
 *
 * Returns: 0 on success or a negative value otherwise.
 */
int
xmlSecMSCryptoGenerateRandom(xmlSecBufferPtr buffer, xmlSecSize size) {
    HCRYPTPROV hProv = 0;
    DWORD dwSize;
    int ret;
    int res = -1;

    xmlSecAssert2(buffer != NULL, -1);
    xmlSecAssert2(size > 0, -1);

    ret = xmlSecBufferSetSize(buffer, size);
    if(ret < 0) {
        xmlSecInternalError2("xmlSecBufferSetSize", NULL, "size=" XMLSEC_SIZE_FMT, size);
        goto done;
    }
    hProv = xmlSecMSCryptoFindProvider(xmlSecMSCryptoProviderInfo_Random, NULL, CRYPT_VERIFYCONTEXT, FALSE);
    if (0 == hProv) {
        xmlSecInternalError("xmlSecMSCryptoFindProvider", NULL);
        goto done;
    }

    XMLSEC_SAFE_CAST_SIZE_TO_ULONG(size, dwSize, goto done, NULL);
    if (FALSE == CryptGenRandom(hProv, dwSize, xmlSecBufferGetData(buffer))) {
        xmlSecMSCryptoError("CryptGenRandom", NULL);
        goto done;
    }

    /* success */
    res = 0;

done:
    /* cleanup */
    if (hProv != 0) {
        CryptReleaseContext(hProv, 0);
    }
    return(res);
}

/**
 * xmlSecMSCryptoGetErrorMessage:
 * @dwError:            the error code.
 * @out:                the output buffer.
 * $outSize:            the output buffer size.
 *
 * Returns the system error message for the give error code.
 */
void
xmlSecMSCryptoGetErrorMessage(DWORD dwError, xmlChar * out, int outLen) {
#ifndef UNICODE
    WCHAR errorTextW[XMLSEC_MSCRYPTO_ERROR_MSG_BUFFER_SIZE];
#endif /* UNICODE */
    LPTSTR errorText = NULL;
    DWORD dwRet;
    int ret;

    xmlSecAssert(out != NULL);
    xmlSecAssert(outLen > 0);
    out[0] = '\0';

    /* Use system message tables to retrieve error text, allocate buffer on local
       heap for error text, don't use any inserts/parameters */
    dwRet = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
                      | FORMAT_MESSAGE_ALLOCATE_BUFFER
                      | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      dwError,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
                      (LPTSTR)&errorText,
                      0,
                      NULL);
    if((dwRet <= 0) || (errorText == NULL)) {
        goto done;
    }

#ifdef UNICODE
    ret = WideCharToMultiByte(CP_UTF8, 0, errorText, -1, (LPSTR)out, outLen, NULL, NULL);
    if(ret <= 0) {
        goto done;
    }
#else /* UNICODE */
    ret = MultiByteToWideChar(CP_ACP, 0, errorText, -1, errorTextW, XMLSEC_MSCRYPTO_ERROR_MSG_BUFFER_SIZE);
    if(ret <= 0) {
        goto done;
    }
    ret = WideCharToMultiByte(CP_UTF8, 0, errorTextW, -1, (LPSTR)out, outLen, NULL, NULL);
    if(ret <= 0) {
        goto done;
    }
#endif /* UNICODE */

done:
    if(errorText != NULL) {
        LocalFree(errorText);
    }
    return;
}


/**
 * xmlSecMSCryptoErrorsDefaultCallback:
 * @file:               the error location file name (__FILE__ macro).
 * @line:               the error location line number (__LINE__ macro).
 * @func:               the error location function name (__FUNCTION__ macro).
 * @errorObject:        the error specific error object
 * @errorSubject:       the error specific error subject.
 * @reason:             the error code.
 * @msg:                the additional error message.
 *
 * The default errors reporting callback function. Just a pass through to the default callback.
 */
void
xmlSecMSCryptoErrorsDefaultCallback(const char* file, int line, const char* func,
                                const char* errorObject, const char* errorSubject,
                                int reason, const char* msg) {
    xmlSecErrorsDefaultCallback(file, line, func, errorObject, errorSubject, reason, msg);
}

/********************************************************************
 *
 * Crypto Providers
 *
 ********************************************************************/
/**
 * xmlSecMSCryptoFindProvider:
 * @providers:           the pointer to list of providers, last provider should have NULL for name.
 * @pszContainer:        the container name for CryptAcquireContext call
 * @dwFlags:             the flags for CryptAcquireContext call
 * @bUseXmlSecContainer: the flag to indicate whether we should try to use XmlSec container if default fails
 *
 * Finds the first provider from the list
 *
 * Returns: provider handle on success or NULL for error.
 */
HCRYPTPROV
xmlSecMSCryptoFindProvider(const xmlSecMSCryptoProviderInfo * providers,
                           LPCTSTR pszContainer,
                           DWORD dwFlags,
                           BOOL bUseXmlSecContainer)
{
    HCRYPTPROV res = 0;
    DWORD dwLastError;
    BOOL ret;
    int ii;

    xmlSecAssert2(providers != NULL, 0);

    for(ii = 0; (res == 0) && (providers[ii].providerName != NULL) && (providers[ii].providerType != 0); ++ii) {
        /* first try */
        ret = CryptAcquireContext(&res,
                    pszContainer,
                    providers[ii].providerName,
                    providers[ii].providerType,
                    dwFlags);
        if((ret == TRUE) && (res != 0)) {
            return (res);
        }

        /* check errors */
        dwLastError = GetLastError();
        switch(HRESULT_FROM_WIN32(dwLastError)) {
        case NTE_BAD_KEYSET:
            /* This error can indicate that a newly installed provider
             * does not have a usable key container yet. It needs to be
             * created, and then we have to try again CryptAcquireContext.
             * This is also referenced in
             * http://www.microsoft.com/mind/0697/crypto.asp (inituser)
             */
            ret = CryptAcquireContext(&res,
                        pszContainer,
                        providers[ii].providerName,
                        providers[ii].providerType,
                        CRYPT_NEWKEYSET | dwFlags);
            if((ret == TRUE) && (res != 0)) {
                return (res);
            }
            break;

        case NTE_EXISTS:
            /* If we can, try our container */
            if(bUseXmlSecContainer == TRUE) {
                ret = CryptAcquireContext(&res,
                            XMLSEC_CONTAINER_NAME,
                            providers[ii].providerName,
                            providers[ii].providerType,
                            CRYPT_NEWKEYSET | dwFlags);
                if((ret == TRUE) && (res != 0)) {
                    /* ALEKSEY TODO - NEED TO DELETE ALL THE TEMP CONTEXTS ON SHUTDOWN

                        CryptAcquireContext(&tmp, XMLSEC_CONTAINER_NAME,
                            providers[ii].providerName,
                            providers[ii].providerType,
                            CRYPT_DELETEKEYSET);

                     */
                    return (res);
                }
            }
            break;

        default:
            /* ignore */
            break;
        }
    }

    return (0);
}


/********************************************************************
 *
 * Utils
 *
 ********************************************************************/
int
ConvertEndian(const xmlSecByte * src, xmlSecByte * dst, xmlSecSize size) {
    xmlSecByte * p;

    xmlSecAssert2(src != NULL, -1);
    xmlSecAssert2(dst != NULL, -1);
    xmlSecAssert2(size > 0, -1);

    for(p = dst + size - 1; p >= dst; ++src, --p) {
        (*p) = (*src);
    }

    return (0);
}

int
ConvertEndianInPlace(xmlSecByte * buf, xmlSecSize size) {
    xmlSecByte * p;
    xmlSecByte ch;

    xmlSecAssert2(buf != NULL, -1);
    xmlSecAssert2(size > 0, -1);

    for(p = buf + size - 1; p >= buf; ++buf, --p) {
        ch = (*p);
        (*p) = (*buf);
        (*buf) = ch;
    }
    return (0);
}


