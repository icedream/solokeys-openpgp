// Copyright 2019 SoloKeys Developers
//
// Licensed under the Apache License, Version 2.0, <LICENSE-APACHE or
// http://apache.org/licenses/LICENSE-2.0> or the MIT license <LICENSE-MIT or
// http://opensource.org/licenses/MIT>, at your option. This file may not be
// copied, modified, or distributed except according to those terms.

#ifndef _UTIL_H
#define _UTIL_H

#include <cstdio>
#include <cstdint>
#include <string_view>
#include <cstring>

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef OPTIMIZATION_O2
#define OPTIMIZATION_O2 __attribute__((optimize("O2")))
#endif

namespace std {
	template<typename _CharT, typename _Traits = std::char_traits<_CharT>>
    class w_basic_string_view : public basic_string_view<_CharT, _Traits> {
    private:
		size_t _max_length;
    public:
		using basic_string_view<_CharT, _Traits>::basic_string_view;

		constexpr w_basic_string_view(const std::basic_string_view<_CharT, _Traits> sv, const size_t maxLength = 0)
							:basic_string_view<_CharT, _Traits>(sv) {
			_max_length = MAX(sv.length(), maxLength);
		}
		constexpr w_basic_string_view(const _CharT* __str, const size_t __len, const size_t maxLength = 0)
							:basic_string_view<_CharT, _Traits>(__str, __len), _max_length(MAX(__len, maxLength)) {}

		constexpr uint8_t *uint8Data() {
			return const_cast<uint8_t *>(this->data());
		}

		constexpr size_t max_length() {
			return _max_length;
		}

		constexpr size_t free_space() {
			if (_max_length > this->length())
				return _max_length - this->length();
			else
				return 0;
		}

		constexpr uint32_t get_uint_be(const size_t indx, const size_t size) {
			if (indx + size > this->length())
				return 0;

			uint32_t res = 0;
			for(uint8_t i = 0; i < size; i++) {
				res = res << 8;
				res += uint8Data()[indx + i];
			}
			return res;
		}

		constexpr uint32_t get_uint_le(const size_t indx, const size_t size) {
			if (indx + size > this->length())
				return 0;

			uint32_t res = 0;
			for(uint8_t i = 0; i < size; i++) {
				res += (uint8Data()[indx + i]) << (i * 8);
			}
			return res;
		}

		constexpr void set_uint_be(const size_t indx, const size_t size, const uint32_t value) {
			if (indx + size > this->length())
				return;

			for(uint8_t i = 0; i < size; i++) {
				uint8Data()[indx + i] = (value >> ((size - i - 1) * 8)) & 0xff;
			}
		}

		constexpr void clear() {
			set_length(0);
		}

		constexpr void set_length(const size_t len) {
			w_basic_string_view<_CharT, _Traits> newsv(this->data(), len, this->max_length());
			*this = newsv;
		}

		constexpr void append(const uint8_t *data, const size_t len) {
			memmove(uint8Data() + this->length(), data, len);
			set_length(this->length() + len);
		}

		constexpr void append(const uint8_t b) {
			append(&b, 1);
		}

		constexpr void appendAPDUres(const uint16_t w) {
			uint8_t b[2] = {
					static_cast<uint8_t>((w >> 8) & 0xff),
					static_cast<uint8_t>(w & 0xff)};
			append(b, 2);
		}

		constexpr void append(const std::basic_string_view<_CharT, _Traits> sv) {
			append(sv.data(), sv.length());
		}

		constexpr void set(const std::basic_string_view<_CharT, _Traits> sv) {
			clear();
			append(sv.data(), sv.length());
		}

		constexpr void set(const uint8_t *data, const size_t len) {
			clear();
			append(data, len);
		}

		constexpr void setAPDURes(const uint16_t w) {
			clear();
			appendAPDUres(w);
		}

		constexpr void del(const size_t begin, const size_t len) {
			if (begin + len > this->length()) {
				set_length(begin);
			} else {
				moveTail(begin + len, -len);
			}
		}

		constexpr void moveTail(size_t begin, int delta) {
			if (delta == 0) // || (this->length() + delta > _max_length))
				return;

			if (this->length() + delta < 0)
				delta = -this->length();

			if (begin + delta < 0)
				begin = -delta;

			uint8_t *data = const_cast<uint8_t*>(this->data());
			memmove(data + begin + delta, data + begin, this->length() - begin);
			set_length(this->length() + delta);
		}
   };
}

using bstr = std::w_basic_string_view<uint8_t>;
constexpr bstr operator "" _bstr(const char* data, const size_t len){
	return bstr(static_cast<const uint8_t *>(static_cast<const void*>(data)), len);
};

using KeyID_t = uint16_t;
using AppID_t = uint16_t;

constexpr void dump_hex(uint8_t * buf, int size, size_t maxlen = 0) {
	if (maxlen == 0)
		maxlen = size + 1;

    while(size-- && maxlen) {
        printf("%02x ", *buf++);
    	maxlen--;
    }

    if (maxlen == 0 && size != 0)
    	printf("...");

    printf("\n");
}

constexpr void dump_hex(bstr data, size_t maxlen = 0) {
	dump_hex(const_cast<uint8_t*>(&data.front()), data.length(), maxlen);
}

#endif
