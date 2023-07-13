#include "test.h"
#include "autocompleteform.h"
#include "widgetutils.h"
#include "globalutils.h"
#include "sqllexer.h"

#include <QTime>

Test::Test()
{

}

#include "qt-secret/aes/qaesencryption.h"
#include <QCryptographicHash>
void Test::test1()
{
    qDebug()<<QDateTime::currentMSecsSinceEpoch();
    for(int i=0;i<100;i++){
        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CFB);

        QString inputStr("12凤飞飞3硕鼠硕鼠4大小非5反反复复6");
    }
    qDebug()<<QDateTime::currentMSecsSinceEpoch();
}

#include "qt-secret/rsa/qrsaencryption.h"

void Test::test2(){
    QTime time;
    time.start();

    QByteArray pub, priv;
    QRSAEncryption e(QRSAEncryption::Rsa::RSA_1024);
    e.generatePairKey(pub, priv); // or other rsa size

    qDebug()<<time.elapsed()/1000.0<<"s";

    time.restart();

    QByteArray msg = "test message test message test message test message test message test message test message test message test message test message test message test message test message ";

    auto encryptMessage = e.encode(msg, pub);
//    qDebug()<<"encryptMessage:"<<encryptMessage;
    auto decodeMessage = e.decode(encryptMessage, priv);
    qDebug()<<"decodeMessage:"<<decodeMessage;


    qDebug()<<time.elapsed()/1000.0<<"s";
}

void Test::test2_2(){
    QTime time;
    time.start();

    QByteArray pub, priv;
    QRSAEncryption e(QRSAEncryption::Rsa::RSA_1024);
    e.generatePairKey(pub, priv); // or other rsa size

    qDebug()<<"pubKey:"<<pub.toBase64();
    qDebug()<<"privKey:"<<priv.toBase64();

    qDebug()<<time.elapsed()/1000.0<<"s";

    time.restart();

    QByteArray msg = "test message test void GenerateRSAKey(uint8_t* pSeed, uint64_t u64SeedLen, uint8_t* pPrivateKey, uint64_t& u64PrivateKeyLen, uint8_t* pPublicKey, uint64_t& u64PublicKeyLenage test message ";

    auto encryptMessage = e.encode(msg, priv);
    qDebug()<<"encryptMessage:"<<encryptMessage.toBase64();
    auto decodeMessage = e.decode(encryptMessage, pub);
    qDebug()<<"decodeMessage:"<<decodeMessage;

    encryptMessage = e.encode(decodeMessage, pub);
    qDebug()<<"encryptMessage2:"<<encryptMessage.toBase64();
    decodeMessage = e.decode(encryptMessage, priv);
    qDebug()<<"decodeMessage2:"<<decodeMessage;


    qDebug()<<time.elapsed()/1000.0<<"s";
}

void Test::test2_3(){
    QTime time;
    time.start();

    qDebug() << "WindowsVersion: " << QSysInfo::WindowsVersion;
    qDebug() << "buildAbi: " << QSysInfo::buildAbi();
    qDebug() << "buildCpuArchitecture: " << QSysInfo::buildCpuArchitecture();
    qDebug() << "currentCpuArchitecture: " << QSysInfo::currentCpuArchitecture();
    qDebug() << "kernelType: " << QSysInfo::kernelType();     //内核类型
    qDebug() << "kernelVersion: " << QSysInfo::kernelVersion();    //内核版本
    qDebug() << "machineHostName: " << QSysInfo::machineHostName();   // 主机名称
    qDebug() << "prettyProductName: " << QSysInfo::prettyProductName();   //操作系统的产品名称
    qDebug() << "productType: " << QSysInfo::productType();
    qDebug() << "productVersion: " << QSysInfo::productVersion();  //产品版本
    qDebug() << "sysname:" << QSysInfo::prettyProductName()+" "+QSysInfo::currentCpuArchitecture();

    qDebug()<<time.elapsed()/1000.0<<"s";

    time.restart();

    qDebug()<<time.elapsed()/1000.0<<"s";
}

//#include <cryptopp/randpool.h>
//#include <cryptopp/files.h>
//#include <cryptopp/rsa.h>

//void GenerateRSAKey(uint8_t* pSeed, uint64_t u64SeedLen, uint8_t* pPrivateKey, uint64_t& u64PrivateKeyLen, uint8_t* pPublicKey, uint64_t& u64PublicKeyLen)
//{
//    CryptoPP::RandomPool randomPool;
//    randomPool.IncorporateEntropy(pSeed, (size_t)u64SeedLen);

//    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(randomPool, 4096);
//    CryptoPP::ArraySink decArr(pPrivateKey, (size_t)u64PrivateKeyLen);

//    decryptor.AccessMaterial().Save(decArr);
//    decArr.MessageEnd();
//    u64PrivateKeyLen = decArr.TotalPutLength();

//    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(decryptor);
//    CryptoPP::ArraySink encArr(pPublicKey, (size_t)u64PublicKeyLen);
//    encryptor.AccessMaterial().Save(encArr);
//    encArr.MessageEnd();
//    u64PublicKeyLen = encArr.TotalPutLength();
//}

//void RSAEncryptData(uint8_t* pSeed, uint64_t u64SeedLen, uint8_t* pPublicKey, uint64_t u64PublicKeyLen, uint8_t* pPlainData, uint64_t u64PlainDataLen, uint8_t* pCipherData, uint64_t& u64CipherDataLen)
//{
//    CryptoPP::ArraySource keyArr(pPublicKey, (size_t)u64PublicKeyLen, true);
//    CryptoPP::RSAES_OAEP_SHA_Encryptor enc;
//    enc.AccessKey().Load(keyArr);

//    CryptoPP::RandomPool randomPool;
//    randomPool.IncorporateEntropy(pSeed, (size_t)u64SeedLen);

//    uint64_t putLen = 0;
//    uint64_t fixedLen = enc.FixedMaxPlaintextLength();

//    for (uint64_t i = 0; i < u64PlainDataLen; i += fixedLen)
//    {
//        uint64_t len = fixedLen < (u64PlainDataLen - i) ? fixedLen : (u64PlainDataLen - i);
//        CryptoPP::ArraySink* dstArr = new CryptoPP::ArraySink(pCipherData + putLen, (size_t)(u64CipherDataLen - putLen));
//        CryptoPP::ArraySource source(pPlainData + i, (size_t)len, true, new CryptoPP::PK_EncryptorFilter(randomPool, enc, dstArr));
//        putLen += dstArr->TotalPutLength();
//    }

//    u64CipherDataLen = putLen;
//}

//void RSADecryptData(uint8_t* pSeed, uint64_t u64SeedLen, uint8_t* pPrivateKey, uint64_t u64PrivateKeyLen, uint8_t* pCipherData, uint64_t u64CipherDataLen, uint8_t* pPlainData, uint64_t& u64PlainDataLen)
//{
//    CryptoPP::ArraySource keyArr(pPrivateKey, (size_t)u64PrivateKeyLen, true);
//    CryptoPP::RSAES_OAEP_SHA_Decryptor dec;
//    dec.AccessKey().Load(keyArr);

//    CryptoPP::RandomPool randomPool;
//    randomPool.IncorporateEntropy(pSeed, (size_t)u64SeedLen);

//    uint64_t putLen = 0;
//    uint64_t fixedLen = dec.FixedCiphertextLength();

//    for (uint64_t i = 0; i < u64CipherDataLen; i += fixedLen)
//    {
//        uint64_t len = fixedLen < (u64CipherDataLen - i) ? fixedLen : (u64CipherDataLen - i);
//        CryptoPP::ArraySink* dstArr = new CryptoPP::ArraySink(pPlainData + putLen, (size_t)(u64PlainDataLen - putLen));
//        CryptoPP::ArraySource source(pCipherData + i, (size_t)len, true, new CryptoPP::PK_DecryptorFilter(randomPool, dec, dstArr));
//        putLen += dstArr->TotalPutLength();
//    }
//    u64PlainDataLen = putLen;
//}

void Test::test3()
{
//    QTime time;
//    time.start();

//    uint8_t seed[] = "jhsgfjwebnr";
//    uint8_t publicKey[4096] = { 0 };
//    uint64_t publicKeyLen = 4096;
//    uint8_t privateKey[4096] = { 0 };
//    uint64_t privateKeyLen = 4096;
//    GenerateRSAKey(seed, sizeof(seed), privateKey, privateKeyLen, publicKey, publicKeyLen);

//    qDebug()<<time.elapsed()/1000.0<<"s";
//    time.restart();

//    char str[] = "test message test message test message test message test message test message test message test message test message test message test message test message test message ";

//    uint8_t pOut[2048] = { 0 };
//    uint64_t OutLen = 2048;

//    RSAEncryptData(seed, sizeof(seed), publicKey, publicKeyLen, (uint8_t*)str, sizeof(str), pOut, OutLen);

//    RSADecryptData(seed, sizeof(seed), privateKey, privateKeyLen, pOut, OutLen, pOut, OutLen);

//    qDebug()<<"pOut:"<<QByteArray::fromRawData((char*)pOut,OutLen);
//    pOut[OutLen] = 0;
    //    qDebug()<<time.elapsed()/1000.0<<"s";
}

void Test::test11()
{
    qDebug()<<QDateTime::currentMSecsSinceEpoch();
    for(int i=0;i<1;i++){
        QString str=" ' f  ";
        qDebug() << StringUtils::trimKuoHao(str);
        qDebug()<<"matchid:"<<QSysInfo::machineUniqueId();

    }
    qDebug()<<QDateTime::currentMSecsSinceEpoch();
}
