#include "libavutil/log.h"
#include "libavformat/avformat.h"

// 传入命令行参数个数
int main(int argc, char **argv)
{
	// 设置日志级别
	av_log_set_level(AV_LOG_DEBUG);
	// 设置日志输出函数

	// 检查参数个数
	if (argc < 2)
	{
		av_log(NULL, AV_LOG_DEBUG, "Usage:%s infileName.\n", argv[0]);
		return -1;
	}

	// 获取输入文件名
	const char *infileName = argv[1];

	// 初始化所有组件
	AVFormatContext *pFormatCtx = NULL;

	// 打开媒体文件
	int ret = avformat_open_input(&pFormatCtx, infileName, NULL, NULL);

	// av_err2str()函数返回错误信息
	if (ret != 0)
	{
		// av_err2str()函数返回错误信息
		av_log(NULL, AV_LOG_DEBUG, "open input file:%s failed: %s\n", infileName, av_err2str(ret));
		return -1;
	}

	// 获取媒体文件信息
	av_dump_format(pFormatCtx, 0, infileName, 0);

	// 关闭媒体文件
	avformat_close_input(&pFormatCtx);

	return 0;
}
