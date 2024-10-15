#include "libavutil/avutil.h"
#include "libavformat/avformat.h"

int main(int argc, char *argv[])
{
    // 设置日志级别
    av_log_set_level(AV_LOG_DEBUG);

    // 如果参数小于3，输出使用方法
    if (argc < 3)
    {
        // argv[0]是程序名
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input file> <output file>\n", argv[0]);
        return -1;
    }

    // 获取命令行的输入音频
    const char *inputName = argv[1];
    // 获取命令行的输出音频
    const char *outputName = argv[2];

    av_sdp_create;
    // 打开输入音频文件
    AVFormatContext *inFormatCtx = NULL;

    // 打开媒体文件，并获取流信息
    int ret = avformat_open_input(&inFormatCtx, inputName, NULL, NULL);
    // 如果打开输入文件失败，返回错误信息
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not open input file '%s'\n", inputName);
        return -1;
    }

    // 获取码流信息
    ret = avformat_find_stream_info(inFormatCtx, NULL);

    // 如果ret小于0，则打印错误信息
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find stream info failed:%s\n", av_err2str(ret));
        // 就算获取失败，也要关闭输入文件
        avformat_close_input(&inFormatCtx);
        return -1;
    }

    // 如果获取成功，则打印信息
    int audioIndex = av_find_best_stream(inFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioIndex < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not find audio stream in the input file\n");
        avformat_close_input(&inFormatCtx);
        return -1;
    }

    if (audioIndex < 0)
    {
        // 输出错误信息，表示找不到最佳音频流
        av_log(NULL, AV_LOG_ERROR, "find best stream failed, index is %d\n", audioIndex);
        avformat_close_input(&inFormatCtx);
        return -1;
    }

    // 打印音频信息
    av_log(NULL, AV_LOG_INFO, "the audio index is %d\n", audioIndex);

    // 初始化AVPacket结构体
    AVPacket *packet = av_packet_alloc();
    if (!packet)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate packet\n");
        avformat_close_input(&inFormatCtx);
        return -1;
    }

    // 存储音频流信息 输出文件
    FILE *dest_fp = fopen(outputName, "wb");

    if (dest_fp == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "open %s file failed\n", outputName);
        // 就算打不开文件也得关闭音频文件
        avformat_close_input(&inFormatCtx);
        // 释放分配的AVPacket
        av_packet_free(&packet);
        return -1;
    }

    // 有许多PC数据，所以需要循环读取
    while (av_read_frame(inFormatCtx, packet) == 0)
    {
        // 检查当前包是否属于音频流
        if (packet->stream_index == audioIndex)
        {
            // 将音频数据写入输出文件
            fwrite(packet->data, 1, packet->size, dest_fp);
            // 检查写入是否成功
            if (ret != packet->size)
            {
                // 如果写入的数据大小不等于包的大小，则输出错误信息
                av_log(NULL, AV_LOG_ERROR, "write data failed\n");
                // 关闭输出文件
                fclose(dest_fp);
                // 关闭输入文件
                avformat_close_input(&inFormatCtx);
                // 释放整个结构体
                av_packet_free(&packet);
                return -1;
            }
        }
        // 释放当前包的引用
        av_packet_unref(packet);
    }

    // 检查输入格式上下文是否已初始化
    if (inFormatCtx != NULL)
    {
        // 关闭输入文件
        avformat_close_input(&inFormatCtx);
    }
    // 检查输出文件指针是否已初始化
    if (dest_fp != NULL)
    {
        // 关闭输出文件
        fclose(dest_fp);
    }
    if (packet != NULL)
    {
        // 释放AVPacket结构体
        av_packet_free(&packet);
    }

    return 0;
}
