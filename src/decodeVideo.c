#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

// 定义一个全局变量，用于记录解码的帧数
int frameCount = 0;

// 解码视频帧的函数
int decodeVideo(AVCodecContext *codecCtx, AVPacket *packet, FILE *dest_fp)
{
    // 将数据包发送到解码器
    int ret = avcodec_send_packet(codecCtx, packet);
    if (ret != 0)
    {
        // 如果发送失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not send packet:%s\n", av_err2str(ret));
        return -1;
    }

    // 分配一个AVFrame结构体，用于存储解码后的帧数据
    AVFrame *frame = av_frame_alloc();
    // 循环接收解码后的帧数据
    while (avcodec_receive_frame(codecCtx, frame) == 0)
    {
        // 将帧数据写入输出文件
        fwrite(frame->data[0], 1, codecCtx->width * codecCtx->height, dest_fp);
        fwrite(frame->data[1], 1, codecCtx->width * codecCtx->height / 4, dest_fp);
        fwrite(frame->data[2], 1, codecCtx->width * codecCtx->height / 4, dest_fp);
        // 增加帧计数
        frameCount++;
        // 记录当前帧数
        av_log(NULL, AV_LOG_INFO, "frameCount:%d\n", frameCount);
        // 输出宽高信息,linesize0 1 2
        av_log(NULL, AV_LOG_INFO, "width:%d,height:%d,linesize0:%d,linesize1:%d,linesize2:%d\n", codecCtx->width, codecCtx->height, frame->linesize[0], frame->linesize[1], frame->linesize[2]);
    }
    // 如果帧数据不为空，释放帧内存
    if (frame)
    {
        av_frame_free(&frame);
    }
    return 0;
}

int main(int argc, char **argv)
{
    // 设置日志级别为调试模式
    av_log_set_level(AV_LOG_DEBUG);
    // 检查命令行参数是否正确
    if (argc < 3)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input> <output>\n", argv[0]);
        return -1;
    }
    // 获取输入和输出文件名
    const char *inFileName = argv[1];
    const char *outFileName = argv[2];

    // 定义一个AVFormatContext结构体，用于存储输入文件的格式信息
    AVFormatContext *inFmtCtx = NULL;
    // 打开输入文件
    int ret = avformat_open_input(&inFmtCtx, inFileName, NULL, NULL);
    if (ret != 0)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open input file %s\n", inFileName);
        return -1;
    }

    // 获取输入文件的流信息
    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret < 0)
    {
        // 如果获取失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find stream information:%s\n", av_err2str(ret));
        goto fail;
    }

    // 查找最佳的视频流索引
    ret = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0)
    {
        // 如果查找失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find best stream index:%s\n", av_err2str(ret));
        goto fail;
    }

    // 获取视频流的索引
    int videoIndex = ret;

    // 分配一个AVCodecContext结构体，用于存储解码器上下文信息
    AVCodecContext *codecCtx = avcodec_alloc_context3(NULL);
    if (codecCtx == NULL)
    {
        // 如果分配失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not allocate codec context\n");
        ret = -1;
        goto fail;
    }

    // 将流参数复制到解码器上下文
    avcodec_parameters_to_context(codecCtx, inFmtCtx->streams[videoIndex]->codecpar);

    // 查找解码器
    const AVCodec *decoder = avcodec_find_decoder(codecCtx->codec_id);
    if (decoder == NULL)
    {
        // 如果查找失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not find codec\n");
        ret = -1;
        goto fail;
    }

    // 打开解码器
    ret = avcodec_open2(codecCtx, decoder, NULL);
    if (ret != 0)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open codec:%s\n", av_err2str(ret));
        goto fail;
    }

    // 打开输出文件
    FILE *dest_fp = fopen(outFileName, "wb+");
    if (dest_fp == NULL)
    {
        // 如果打开失败，记录错误信息
        av_log(NULL, AV_LOG_ERROR, "Could not open output file %s\n", outFileName);
        ret = -1;
        goto fail;
    }

    // 分配一个AVPacket结构体，用于存储数据包
    AVPacket *packet = av_packet_alloc();

    // 分配一个AVFrame结构体，用于存储解码后的帧数据
    AVFrame *frame = av_frame_alloc();
    // 循环读取输入文件中的数据包
    while (av_read_frame(inFmtCtx, packet) >= 0)
    {
        // 如果数据包属于视频流
        if (packet->stream_index == videoIndex)
        {
            // 解码视频帧
            if (decodeVideo(codecCtx, packet, dest_fp) == -1)
            {
                ret = -1;
                av_packet_unref(packet);
                goto fail;
            }

            // 释放数据包引用
            av_packet_unref(packet);
        }
    }
    // 刷新解码器，确保所有帧都被解码
    decodeVideo(codecCtx, NULL, dest_fp);
fail:
    // 如果输入文件格式上下文不为空，关闭输入文件
    if (inFmtCtx)
    {
        avformat_close_input(&inFmtCtx);
    }
    // 如果解码器上下文不为空，释放解码器上下文
    if (codecCtx)
    {
        avcodec_free_context(&codecCtx);
    }
    // 如果输出文件指针不为空，关闭输出文件
    if (dest_fp)
    {
        fclose(dest_fp);
    }
    return ret;
}