#include <libavutil/log.h>
#include <stddef.h>

int main(int argc,char **argv)
{
	av_log_set_level(AV_LOG_ERROR);
	av_log(NULL,AV_LOG_ERROR,"this is error log level!\n");
	av_log(NULL,AV_LOG_INFO,"this is info log leverl,%d\n",argc);
	av_log(NULL,AV_LOG_WARNING,"this is warning log level,%s\n",argv[0]);
	av_log(NULL,AV_LOG_DEBUG,"this is debug log level!\n");

	return 0;
}
