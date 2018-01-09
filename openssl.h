#ifndef OPENSSL_H
#define OPENSSL_H
#include <QByteArray>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>

class OpenSSL
{
public:
    OpenSSL();
    static int encrypt(const QByteArray plaintext, const QByteArray key, const QByteArray iv, QByteArray &ciphertext);
    static int decrypt(const QByteArray ciphertext, const QByteArray key, const QByteArray iv, QByteArray &plaintext);
    static void sha256(const QByteArray in, QByteArray &out, QByteArray salt = NULL);
};

#endif // OPENSSL_H
