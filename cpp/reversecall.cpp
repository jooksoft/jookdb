#include "reversecall.h"
#include "utils.h"

ReverseCall::ReverseCall()
{

}

void ReverseCall::call(const QJsonObject &json)
{
    QJsonObject resultJson;

    int reverseCallFuncId=Utils::getInt(json,"RCFuncId");
    switch (reverseCallFuncId) {
    case DECODE_PWD_FROM_B64:{
        QString str=Utils::getString(json,"str");
        QByteArray ba=StringUtils::decodePwdFromB64(str);
        resultJson.insert("str",QString{ba});
    }
        break;
    }

    resultJson.insert("funcId", REVERSE_CALL);
    resultJson.insert("RCReqId",Utils::getString(json,"RCReqId"));

    auto connData=QSharedPointer<ConnData>{new ConnData()};
    connData->getResJson(resultJson,false);
}
