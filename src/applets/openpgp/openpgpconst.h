/*
  Copyright 2019 SoloKeys Developers

  Licensed under the Apache License, Version 2.0, <LICENSE-APACHE or
  http://apache.org/licenses/LICENSE-2.0> or the MIT license <LICENSE-MIT or
  http://opensource.org/licenses/MIT>, at your option. This file may not be
  copied, modified, or distributed except according to those terms.
 */

#ifndef SRC_APPLETS_OPENPGP_OPENPGPCONST_H_
#define SRC_APPLETS_OPENPGP_OPENPGPCONST_H_

namespace OpenPGP {

enum Password {
	PW1,
	PW3,
	RC
};

class PGPConst {
private:
	PGPConst(){};
public:
	static const uint8_t PW1MinLength = 6U;
	static const uint8_t PW3MinLength = 8U;
	static constexpr uint8_t PWMinLength(Password pwd) {
		if (pwd == Password::PW1)
			return PW1MinLength;
		else
			return PW3MinLength;
	}
	// look DO`c4`
	static const uint8_t RCMaxLength = 0x20U; // resetting code
	static const uint8_t PW1MaxLength = 0x20U;
	static const uint8_t PW3MaxLength = 0x20U;
	static constexpr uint8_t PWMaxLength(Password pwd) {
		if (pwd == Password::PW1)
			return PW1MaxLength;
		else
			return PW3MaxLength;
	}
	static const uint8_t DefaultPWResetCounter = 0x03U; // OpenPGP v 3.3.1 page 23
};

enum OpenPGPKeyType {
	Unknown          = 0x00,
	DigitalSignature = 0xb6,
	Confidentiality  = 0xb8,
	Authentication   = 0xa4,
};

// OpenPGP v3.3.1 page 38 and 78
enum LifeCycleState {
	NoInfo		= 0x00,
	Init		= 0x03,
	Operational = 0x05,
};

} // namespace OpenPGP

#endif /* SRC_APPLETS_OPENPGP_OPENPGPCONST_H_ */