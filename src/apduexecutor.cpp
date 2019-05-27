/*
 Copyright 2019 SoloKeys Developers

 Licensed under the Apache License, Version 2.0, <LICENSE-APACHE or
 http://apache.org/licenses/LICENSE-2.0> or the MIT license <LICENSE-MIT or
 http://opensource.org/licenses/MIT>, at your option. This file may not be
 copied, modified, or distributed except according to those terms.
 */

#include <apduexecutor.h>
#include "applets/apduconst.h"

namespace Applet {

void APDUExecutor::SetResultError(bstr& result, Util::Error error) {
	using Util::Error;
	switch (error) {
	case Error::NoError:
		result.appendAPDUres(APDUResponse::OK);
		break;
	case Error::AppletNotFound:
    	result.setAPDURes(APDUResponse::FileNotFound);
		break;
	case Error::WrongAPDUCLA:
    	result.setAPDURes(APDUResponse::CLAnotSupported);
		break;
	case Error::WrongAPDUINS:
    	result.setAPDURes(APDUResponse::INSnotSupported);
		break;
	case Error::WrongAPDUP1P2:
    	result.setAPDURes(APDUResponse::WrongParametersP1orP2);
		break;

	default:
    	result.setAPDURes(APDUResponse::InternalException);
	}
}

Util::Error APDUExecutor::Execute(bstr apdu, bstr& result) {
	result.clear();

	if (apdu.length() < 5) {
    	result.setAPDURes(APDUResponse::WrongLength);
		return Util::Error::WrongAPDUStructure;
	}

	if (apdu.length() != apdu[4] + 5U && apdu.length() != apdu[4] + 6U) {
    	result.setAPDURes(APDUResponse::WrongLength);
		return Util::Error::WrongAPDULength;
	}

	auto cla = apdu[0];
	auto ins = apdu[1];
	auto p1 = apdu[2];
	auto p2 = apdu[3];
	auto len = apdu[4];
	auto data = bstr(apdu.substr(5, len));
	if (ins == APDUcommands::Select) {
		if (cla != 0) {
    		result.appendAPDUres(APDUResponse::CLAnotSupported);
    		return Util::Error::WrongAPDUCLA;
		}
		if (p1 != 0x04 || p2 != 0x00) {
    		result.appendAPDUres(APDUResponse::WrongParametersP1orP2);
    		return Util::Error::WrongAPDUP1P2;
		}

		auto err = appletStorage.SelectApplet(data, result);
    	SetResultError(result, err);
		return err;
	}

    Applet *applet = appletStorage.GetSelectedApplet();
    if (applet != nullptr) {

    	Util::Error err = applet->APDUExchange(apdu, result);
    	SetResultError(result, err);
      	printf("appdu exchange result: %s\n", Util::GetStrError(err));
    } else {
    	printf("applet not selected.\n");
    	result.setAPDURes(APDUResponse::ConditionsUseNotSatisfied);
    }

    return Util::NoError;
}

}

