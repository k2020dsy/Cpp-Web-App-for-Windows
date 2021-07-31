#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include "cencode.c"
#include "cdecode.c"
 
using namespace std;
/**
 * Base64 编码/解码
 */
class Base64{
private:
    std::string _base64_table;
    static const char base64_pad = '=';
public:
    Base64()
    {
        _base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; /*这是Base64编码使用的标准字典*/
    }
    /**
     * 这里必须是unsigned类型，否则编码中文的时候出错
     */
    std::string Encode(const unsigned char * str,int bytes);
    std::string Decode(const char *str,int bytes);
};
 
std::string Base64::Encode(const unsigned char * str,int bytes) {
    int num = 0,bin = 0;
    std::string _encode_result;
    const unsigned char * current;
    current = str;
    while(bytes > 2) {
        _encode_result += _base64_table[current[0] >> 2];
        _encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
        _encode_result += _base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
        _encode_result += _base64_table[current[2] & 0x3f];
 
        current += 3;
        bytes -= 3;
    }
    if(bytes > 0)
    {
        _encode_result += _base64_table[current[0] >> 2];
        if(bytes%3 == 1) {
            _encode_result += _base64_table[(current[0] & 0x03) << 4];
            _encode_result += "==";
        } else if(bytes%3 == 2) {
            _encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
            _encode_result += _base64_table[(current[1] & 0x0f) << 2];
            _encode_result += "=";
        }
    }
    return _encode_result;
}
std::string Base64::Decode(const char *str,int length) {
       //解码表
    const char DecodeTable[] =
    {
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
        -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
        -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
    };
    int bin = 0,i=0,pos=0;
    std::string _decode_result;
    const char *current = str;
    char ch;
    while( (ch = *current++) != '\0' && length-- > 0 )
    {
        if (ch == base64_pad) { // 当前一个字符是“=”号
            /*
            先说明一个概念：在解码时，4个字符为一组进行一轮字符匹配。
            两个条件：
                1、如果某一轮匹配的第二个是“=”且第三个字符不是“=”，说明这个带解析字符串不合法，直接返回空
                2、如果当前“=”不是第二个字符，且后面的字符只包含空白符，则说明这个这个条件合法，可以继续。
            */
            if (*current != '=' && (i % 4) == 1) {
                return NULL;
            }
            continue;
        }
        ch = DecodeTable[ch];
        //这个很重要，用来过滤所有不合法的字符
        if (ch < 0 ) { /* a space or some other separator character, we simply skip over */
            continue;
        }
        switch(i % 4)
        {
            case 0:
                bin = ch << 2;
                break;
            case 1:
                bin |= ch >> 4;
                _decode_result += bin;
                bin = ( ch & 0x0f ) << 4;
                break;
            case 2:
                bin |= ch >> 2;
                _decode_result += bin;
                bin = ( ch & 0x03 ) << 6;
                break;
            case 3:
                bin |= ch;
                _decode_result += bin;
                break;
        }
        i++;
    }
    return _decode_result;
}
 
int getFile_size(string filename)
{
    FILE *fp=fopen(filename.data(), "r");
    if(!fp) return -1;
    fseek(fp,0L,SEEK_END);
    int size=ftell(fp);
    fclose(fp);
    return size;
}

/*方法2*/


static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";



bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

/* arbitrary buffer size */
#define B64BEFFERSIZE 5000000

char* encode(const char* input)
{
	/* set up a destination buffer large enough to hold the encoded data */
	char* output = (char*)malloc(B64BEFFERSIZE);
	/* keep track of our encoded position */
	char* c = output;
	/* store the number of bytes encoded by a single call */
	int cnt = 0;
	/* we need an encoder state */
	base64_encodestate s;

	/*---------- START ENCODING ----------*/
	/* initialise the encoder state */
	base64_init_encodestate(&s);
	/* gather data from the input and send it to the output */
	cnt = base64_encode_block(input, strlen(input), c, &s);
	c += cnt;
	/* since we have encoded the entire input string, we know that
	   there is no more input data; finalise the encoding */
	cnt = base64_encode_blockend(c, &s);
	c += cnt;
	/*---------- STOP ENCODING  ----------*/

	/* we want to print the encoded data, so null-terminate it: */
	*c = 0;

	return output;
}

char* decode(const char* input)
{
	/* set up a destination buffer large enough to hold the encoded data */
	char* output = (char*)malloc(B64BEFFERSIZE);
	/* keep track of our decoded position */
	char* c = output;
	/* store the number of bytes decoded by a single call */
	int cnt = 0;
	/* we need a decoder state */
	base64_decodestate s;

	/*---------- START DECODING ----------*/
	/* initialise the decoder state */
	base64_init_decodestate(&s);
	/* decode the input data */
	cnt = base64_decode_block(input, strlen(input), c, &s);
	c += cnt;
	/* note: there is no base64_decode_blockend! */
	/*---------- STOP DECODING  ----------*/

	/* we want to print the decoded data, so null-terminate it: */
	*c = 0;

	return output;
}
