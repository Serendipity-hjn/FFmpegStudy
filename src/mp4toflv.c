#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

    int
    main(int argc, char **argv)
{
    av_log_set_level(AV_LOG_DEBUG);
    if (argc < 3)
    {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <infileName> <outfileName>\n", argv[0]);
        return -1;
    }
    const char *inFileName = argv[1];
    const char *outFileName = argv[2];

    AVFormatContext *inFmtCtx = NULL;
    int ret = avformat_open_input(&inFmtCtx, inFileName, NULL, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "open input format failed:%s\n", av_err2str(ret));
        return -1;
    }

    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "find input stream failed:%s\n", av_err2str(ret));
        goto fail;
    }

    AVFormatContext *outFmtCtx = NULL;

    // 分配输出格式上下文
    ret = avformat_alloc_output_context2(&outFmtCtx, NULL, NULL, outFileName);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "alloc output format failed:%s\n", av_err2str(ret));
        goto fail;
    }
    // 输入文件的流数量
    int streamCount = inFmtCtx->nb_streams;
    // 分配一个整数数组，用于存储输入流索引到输出流索引的映射关系，并将其初始化为零
    int *handleStreamIndexArray = av_malloc_array(streamCount, sizeof(int));
    if (handleStreamIndexArray == NULL)
    {
        av_log(NULL, AV_LOG_ERROR, "malloc handle stream index array failed\n");
        goto fail;
    }

    int streamIndex = 0;

    // 用于多媒体处理的循环，主要功能是将输入文件中的音视频流复制到输出文件中
    for (int i = 0; i < streamCount; i++)
    {
        // 获取输入文件的流
        AVStream *inStream = inFmtCtx->streams[i];
        // 判断流的类型（视频，音频或字幕）
        if (inStream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            inStream->codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            inStream->codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE)
        {
            // 不处理该流
            handleStreamIndexArray[i] = -1;
            continue;
        }
        handleStreamIndexArray[i] = streamIndex++;
        // 创建新的输出流
        AVStream *outStream = NULL;
        // 在输出文件中创建一个新的流
        outStream = avformat_new_stream(outFmtCtx, NULL);
        if (outStream == NULL)
        {
            ret = -1;
            av_log(NULL, AV_LOG_ERROR, "new output stream failed\n");
            goto fail;
        }
        // 复制编解码器参数
        avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);
        // 设置输出流的编解码器标签为0
        outStream->codecpar->codec_tag = 0;
    }

    // 判断outFmtCtx->oformat->flags是否包含AVFMT_NOFILE标志
    if (!(outFmtCtx->oformat->flags & AVFMT_NOFILE))
    {
        // 以写入模式打开
        ret = avio_open(&outFmtCtx->pb, outFileName, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "open output file failed:%s\n", outFileName);
            goto fail;
        }
    }

    // 将输出文件的头部信息写入到输出文件中
    ret = avformat_write_header(outFmtCtx, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "write header failed:%s\n", av_err2str(ret));
        goto fail;
    }
    AVPacket *packet = av_packet_alloc();

    // 读取输入文件的数据包
    while (av_read_frame(inFmtCtx, packet) == 0)
    {
        if (packet->stream_index >= streamCount ||
            handleStreamIndexArray[packet->stream_index == -1])
        {
            av_packet_unref(packet);
        }

        // 获取输入输出文件中对应流索引的流
        AVStream *inStream = inFmtCtx->streams[packet->stream_index];
        AVStream *outStream = outFmtCtx->streams[packet->stream_index];

        packet->stream_index = handleStreamIndexArray[packet->stream_index];
        packet->pts = av_rescale_q(packet->pts, inStream->time_base, outStream->time_base);
        packet->dts = av_rescale_q(packet->dts, inStream->time_base, outStream->time_base);
        packet->duration =
            av_rescale_q(packet->duration, inStream->time_base, outStream->time_base);
        // 将数据包的位置设置为-1
        packet->pos = -1;

        ret = av_interleaved_write_frame(outFmtCtx, packet);
        if (ret != 0)
        {
            av_log(NULL, AV_LOG_ERROR, "write interleaved failed:%s\n", av_err2str(ret));
            goto fail;
        }

        av_packet_unref(packet);
    }
    ret = av_write_trailer(outFmtCtx);
    if (ret != 0)
    {
        av_log(NULL, AV_LOG_ERROR, "write trailer failed :%s\n", av_err2str(ret));
    }
fail:
    if (inFmtCtx)
    {
        avformat_close_input(&inFmtCtx);
    }
    if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE))
    {
        avio_closep(&outFmtCtx->pb);
    }
    if (outFmtCtx)
    {
        avformat_free_context(outFmtCtx);
    }
    if (handleStreamIndexArray)
    {
        av_freep(&handleStreamIndexArray);
    }
    return ret;
}
