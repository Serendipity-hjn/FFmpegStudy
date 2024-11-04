#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

int main(int argc, char **argv)
{
    // 设置日志级别
    av_log_set_level(AV_LOG_INFO);
    if (argc < 2)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage:%s <infileName>\n", argv[0]);
    }
    const char *inFileName = argv[1];

    // 打开输入文件
    AVFormatContext *inFmtCtx = NULL; // 用于存储输入文件的格式信息
    avformat_open_input(&inFmtCtx, inFileName, NULL,
                        NULL); // 打开输入文件inFileName，并将格式信息存储在inFmtCtx中

    avformat_find_stream_info(inFmtCtx, NULL); // 查找输入文件的流信息，并将流信息存储在inFmtCtx中

    av_dump_format(inFmtCtx, 0, inFileName, 0); // 打印输入文件inFileName的格式信息

    av_log(NULL, AV_LOG_INFO, "input file duration:%ld us, %lf s \n", inFmtCtx->duration,
           inFmtCtx->duration *
               av_q2d(AV_TIME_BASE_Q)); // 打印输入文件的总时长，单位为微秒和秒
                                        // AV_TIME_BASE_Q是ffmpeg内部的时间基，值为{1,
                                        // AV_TIME_BASE}，AV_TIME_BASE的值为1000000，即1秒

    // AVRational是ffmpeg内部的时间基，值为{num, den}，num为分子，den为分母
    AVRational videoTimeBase;
    AVRational audioTimeBase;
    for (int i = 0; i < inFmtCtx->nb_streams; i++) // 遍历输入文件中的所有流
    {
        AVStream *inStream = inFmtCtx->streams[i]; // 获取输入文件中的第i个流
        // 分别判断是否为音频或视频流
        if (inStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoTimeBase = inStream->time_base;
            av_log(NULL, AV_LOG_INFO, "video timebase:num = %d,den = %d\n", videoTimeBase.num,
                   videoTimeBase.den);
        }
        else if (inStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioTimeBase = inStream->time_base;
            av_log(NULL, AV_LOG_INFO, "audio timebase:num = %d,den = %d\n", audioTimeBase.num,
                   audioTimeBase.den);
        }
    }
    AVPacket *packet = av_packet_alloc(); // 分配一个AVPacket结构体，用于存储解码后的数据
    while (av_read_frame(inFmtCtx, packet) >=
           0) // 循环读取输入文件中的每个数据包，并将数据包存储在packet中
    {
        AVStream *inStream = inFmtCtx->streams[packet->stream_index]; // 获取当前数据包所属的流
        av_log(
            NULL, AV_LOG_INFO, "streamIndex = %d,pts = %ld,ptsTime = %lf,dts = %ld,dtsTime = %lf\n",
            packet->stream_index, packet->pts, packet->pts * av_q2d(inStream->time_base),
            packet->dts,
            packet->dts *
                av_q2d(inStream->time_base)); // 打印当前数据包的流索引、pts、pts时间、dts、dts时间
    }
    return 0;
}