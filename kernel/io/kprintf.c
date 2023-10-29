#include <io/stdio.h>
#include <string.h>

#define FLAGS_LEFT 		(1 << 0)
#define FLAGS_UPPERCASE (1 << 1)
#define FLAGS_HASH		(1 << 2)
#define FLAGS_PLUS		(1 << 3)
#define FLAGS_SPACE		(1 << 4)
#define FLAGS_ZERO		(1 << 5)

#define BUFLEN 1024

struct printf_state {
	char buf[BUFLEN];
	u64 index;
	u64 written;
};

static u64 out(struct printf_state *s, struct io_device *stream, char c) {
	if (c == '\0' || s->index >= BUFLEN) {
		stream->write(stream, (u8*) s->buf, s->index, 0);
		s->index = 0;
		u64 ret = 0;
		if (c == '\0') {
			ret = s->written;
			s->written = 0;
		}
		return ret;
	}

	s->buf[s->index] = c;
	s->index++;
	s->written++;
	return 0;
}

static bool is_digit(char c) {
	return c >= '0' && c <= '9';
}

static u64 atoi(const char **fmt) {
	u64 i = 0;
	while (is_digit(**fmt)) {
		i = i * 10 + (u64) (**fmt - '0');
		(*fmt)++;
	}
	return i;
}

static void ntoa(struct printf_state *s, struct io_device *stream, u64 value, bool negative, u64 base, u64 flags, u64 precision) {
	char tmp[64];
	u64 len = 0;
	do {
		char digit = (char) (value % base);
		tmp[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
		value /= base;
	} while (value);
	i64 last_index = len - 1;

	bool sign = false;
	if (negative) {
		out(s, stream, '-');
		sign = true;
	} else if (flags & FLAGS_PLUS) {
		out(s, stream, '+');
		sign = true;
	} else if (flags & FLAGS_SPACE) {
		out(s, stream, ' ');
		sign = true;
	}

	if (flags & FLAGS_HASH) {
		out(s, stream, '0');
		if (base == 16) {
			out(s, stream, flags & FLAGS_UPPERCASE ? 'X' : 'x');
		} else if (base == 2) {
			out(s, stream, 'b');
		}
	}

	if (precision && (flags & FLAGS_ZERO) && sign) {
		precision--;
	}
	while (len < precision) {
		out(s, stream, (flags & FLAGS_ZERO) ? '0' : ' ');
		len++;
	}
	while (last_index >= 0) {
		out(s, stream, tmp[last_index]);
		last_index--;
	}
}

u64 vfprintf(struct io_device *stream, const char *fmt, va_list args) {
	struct printf_state _s = {0};

	if (stream == NULL || fmt == NULL) {
		return 0;
	}

	u64 flags, precision, n;

	while (*fmt) {

		if (*fmt != '%') {
			out(&_s, stream, *fmt);
			fmt++;
			continue;
		}

		fmt++;

		flags = 0;

		do {
			switch (*fmt) {
				case '0': {
					flags |= FLAGS_ZERO;
					fmt++;
					n = 1;
					break;
				}
				case '-': {
					flags |= FLAGS_LEFT;
					fmt++;
					n = 1;
					break;
				}
				case '+': {
					flags |= FLAGS_PLUS;
					fmt++;
					n = 1;
					break;
				}
				case ' ': {
					flags |= FLAGS_SPACE;
					fmt++;
					n = 1;
					break;
				}
				case '#': {
					flags |= FLAGS_HASH;
					fmt++;
					n = 1;
					break;
				}
				default: {
					n = 0;
					break;
				}
			}
		} while (n);

		precision = 0;

		if (*fmt == '.') {
			fmt++;

			if (is_digit(*fmt)) {
				precision = atoi(&fmt);
			} else if (*fmt == '*') {
				i64 p = va_arg(args, i64);
				if (p < 0) {
					flags |= FLAGS_LEFT;
					precision = -p;
				} else {
					precision = p;
				}
				fmt++;
			}
		}

		switch (*fmt) {
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
			case 'b': {
				u64 base;

				if (*fmt == 'x' || *fmt == 'X') {
					base = 16;
				} else if (*fmt == 'o') {
					base = 8;
				} else if (*fmt == 'b') {
					base = 2;
				} else {
					base = 10;
					flags &= ~FLAGS_HASH;
				}

				if (*fmt == 'X') {
					flags |= FLAGS_UPPERCASE;
				}

				if (*fmt != 'i' && *fmt != 'd') {
					flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
				}

				if (*fmt == 'i' || *fmt == 'd') {
					i64 value = va_arg(args, i64);
					ntoa(&_s, stream, value < 0 ? -value : value, value < 0, base, flags, precision);
				} else {
					u64 value = va_arg(args, u64);
					ntoa(&_s, stream, value, false, base, flags, precision);
				}
				fmt++;
				break;
			}
			case 'c': {
				u64 l = 1;
				if (!(flags & FLAGS_LEFT)) {
					while (l < precision) {
						l++;
						out(&_s, stream, ' ');
					}
				}
				out(&_s, stream, (char) va_arg(args, i64));
				if (flags & FLAGS_LEFT) {
					while (l < precision) {
						l++;
						out(&_s, stream, ' ');
					}
				}
				fmt++;
				break;
			}
			case 's': {
				const char *s = va_arg(args, char*);
				u64 len = strlen(s);
				len = (precision && precision < len) ? precision : len;
				if (!(flags & FLAGS_LEFT)) {
					while (len < precision) {
						len++;
						out(&_s, stream, ' ');
					}
				}
				for (u64 i = 0; i < len; i++) {
					out(&_s, stream, *s);
					s++;
				}
				if (flags & FLAGS_LEFT) {
					while (len < precision) {
						len++;
						out(&_s, stream, ' ');
					}
				}
				fmt++;
				break;
			}
			case 'p': {
				flags |= FLAGS_ZERO | FLAGS_HASH;
				u64 value = (u64) va_arg(args, void*);
				ntoa(&_s, stream, value, false, 16, flags, precision);
				fmt++;
				break;
			}
			case '%': {
				out(&_s, stream, '%');
				fmt++;
				break;
			}
			default: {
				out(&_s, stream, *fmt);
				fmt++;
				break;
			}
		}
	}

	return out(&_s, stream, '\0');
}

struct string_io {
	struct io_device header;
	char *buf;
	u64 maxlen;
	u64 index;
};

static u64 _string_write(struct io_device *stream, u8 *buf, u64 size, __unused u64 offset) {
	struct string_io *_io = (struct string_io*) stream;
	u64 maxlen = _io->maxlen;
	char *sbuf = _io->buf;
	u64 i;
	for (i = 0; i < size && _io->index < maxlen; _io->index++, i++) {
		sbuf[_io->index] = buf[i];
	}
	return i;
}

u64 vsnprintf(char *buf, u64 maxlen, const char *fmt, va_list args) {
	struct string_io _io;
	_io.header.write = _string_write;
	_io.header.read  = NULL;
	_io.buf          = buf;
	_io.maxlen       = maxlen;
	_io.index        = 0;
	return vfprintf((struct io_device*) &_io, fmt, args);
}

u64 snprintf(char *buf, u64 maxlen, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	u64 len = vsnprintf(buf, maxlen, fmt, args);
	va_end(args);
	buf[len] = '\0';
	return len;
}

u64 printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	u64 len = vfprintf(stdio, fmt, args);
	va_end(args);
	return len;
}

u64 vprintf(const char *fmt, va_list args) {
	return vfprintf(stdio, fmt, args);
}

u64 fprintf(struct io_device *stream, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	u64 len = vfprintf(stream, fmt, args);
	va_end(args);
	return len;
}
