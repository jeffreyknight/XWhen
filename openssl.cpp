#include "openssl.h"

OpenSSL::OpenSSL(){

}

int OpenSSL::encrypt(const QByteArray plaintext, const QByteArray key, const QByteArray iv, QByteArray &ciphertext){
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    EVP_CIPHER_CTX *ctx;
    int len = 0;
    int ciphertext_len = 0;

    unsigned char *ciphertext_c = (unsigned char*)malloc(2*plaintext.size()+1);
    memset(ciphertext_c, '\0', 2*plaintext.size()+1);

    if(!(ctx = EVP_CIPHER_CTX_new())) return -1;
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                               reinterpret_cast<const unsigned char*>(key.constData()),
                               reinterpret_cast<const unsigned char*>(iv.constData())))
        return -1;
    if(1 != EVP_EncryptUpdate(ctx, ciphertext_c, &len,
                              reinterpret_cast<const unsigned char*>(plaintext.constData()), plaintext.size()))
        return -1;
    ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext_c + len, &len)) return -1;
    ciphertext_len += len;
    ciphertext = QByteArray((const char*)ciphertext_c, ciphertext_len);
    free(ciphertext_c);
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int OpenSSL::decrypt(const QByteArray ciphertext, const QByteArray key, const QByteArray iv, QByteArray &plaintext){
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
    //https://www.mitchr.me/SS/exampleCode/openssl/evp_decrypt.c.html
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    if(!(ctx = EVP_CIPHER_CTX_new())) return -1;
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                               reinterpret_cast<const unsigned char*>(key.constData()),
                               reinterpret_cast<const unsigned char*>(iv.constData())))
        return -1;
    unsigned char *plaintext_c = (unsigned char*)malloc(ciphertext.size()+1);
    memset(plaintext_c, '\0', ciphertext.size()+1);

    if(1 != EVP_DecryptUpdate(ctx, plaintext_c, &len,
                              reinterpret_cast<const unsigned char*>(ciphertext.data()), ciphertext.size()))
        return -1;
    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plaintext_c + len, &len))
        return -1;
    plaintext_len += len;
    plaintext = QByteArray((const char*)plaintext_c, plaintext_len);
    free(plaintext_c);
    EVP_CIPHER_CTX_free(ctx);
    EVP_cleanup();
    ERR_free_strings();
    return plaintext_len;
}

void OpenSSL::sha256(const QByteArray in, QByteArray &out, QByteArray salt){
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    const int bufSize = 32768;
    unsigned char* buffer = (unsigned char *)malloc(bufSize);
    memset(buffer, '\0', bufSize);
    int bytesRead = 0;
    if(!buffer)
        return;
    int index = 0;
    if(!salt.isNull())
        if(!SHA256_Update(&sha256, (unsigned char*)salt.data(), salt.length()))
            return;
    do{
        if(index >= bufSize){
            bytesRead = 0;
        }else if(in.size() - index < bufSize){
            buffer = (unsigned char *)in.mid(index, bufSize).data();
            bytesRead = in.size() - index;
            index += bytesRead;
            SHA256_Update(&sha256, buffer, bytesRead);
        }else{
            buffer = (unsigned char *)in.mid(index, bufSize).data();
            bytesRead = bufSize;
            index += bufSize;
            SHA256_Update(&sha256, buffer, bytesRead);
        }
    }while(bytesRead != 0);
    unsigned char hash[256];
    memset(hash, '\0', sizeof(hash));
    SHA256_Final(hash, &sha256);
    out = QByteArray(reinterpret_cast<char*>(hash));
    EVP_cleanup();
    ERR_free_strings();
}
