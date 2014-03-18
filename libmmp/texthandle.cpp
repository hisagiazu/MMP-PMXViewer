#include "texthandle.h"

#include <iconv.h>
#include <sstream>
#include <string.h>

std::string sjisToUTF8(const char *sjis)
{
	char	inbuf[MAX_BUF+1] = { 0 };
	char	outbuf[MAX_BUF+1] = { 0 };
		
	char	*in = inbuf;
	char	*out = outbuf;
	size_t	in_size = (size_t)MAX_BUF;
	size_t	out_size = (size_t)MAX_BUF;
	iconv_t ic = iconv_open("UTF-8", "SJIS");

	memcpy( in, sjis, strlen(sjis) );

	iconv( ic, &in, &in_size, &out, &out_size );
	iconv_close(ic);

	int ret = memcmp( outbuf, sjis, sizeof(sjis) );
	//printf(ret == 0 ? "success\n" : "failed\n" );
	//printf("%s\n", outbuf );
	
	if(ret==0)
	{
		//printf("FATAL ERROR: SJIS to UTF8 conversion failed\n");
		//exit(EXIT_FAILURE);
	}
	
	//cout<<outbuf<<endl;
		
	std::stringstream ss;
	ss<<outbuf;
	return ss.str();
}

std::string UTF8ToSJIS(const char *utf8)
{
	//printf("DOING UTF8->SJIS\n");
	char	inbuf[MAX_BUF+1] = { 0 };
	char	outbuf[MAX_BUF+1] = { 0 };
		
	char	*in = inbuf;
	char	*out = outbuf;
	size_t	in_size = (size_t)MAX_BUF;
	size_t	out_size = (size_t)MAX_BUF;
	iconv_t ic = iconv_open("SJIS", "UTF-8");

	memcpy( in, utf8, strlen(utf8) );

	iconv( ic, &in, &in_size, &out, &out_size );
	iconv_close(ic);

	int ret = memcmp( outbuf, utf8, sizeof(utf8));
	//printf(ret == 0 ? "success\n" : "failed\n" );
	//printf("%s\n", outbuf );
	
	if(ret==0)
	{
		printf("FATAL ERROR: UTF8 to SJIS conversion failed\n");
		exit(EXIT_FAILURE);
	}
	
	//cout<<outbuf<<endl;
		
	std::stringstream ss;
	ss<<outbuf;
	return ss.str();
}
